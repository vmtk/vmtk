/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataPotentialFit.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "vtkvmtkPolyDataPotentialFit.h"

#include "vtkvmtkNeighborhoods.h"
#include "vtkvmtkConstants.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkVoxel.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataPotentialFit, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkPolyDataPotentialFit);

vtkvmtkPolyDataPotentialFit::vtkvmtkPolyDataPotentialFit()
{
  this->PotentialImage = NULL;
  this->NumberOfIterations = VTK_VMTK_LARGE_INTEGER;
  this->TimeStep = 0.0;
  this->MaxDisplacementNorm = 0.0;
  this->MinEdgeLength = 0.0;
  this->Relaxation = 1.0;
  this->Displacements = NULL;
  this->MinPotentialSpacing = 0.0;
  this->Convergence = 1E-1;
  this->PotentialWeight = 0.0;
  this->StiffnessWeight = 0.0;
  this->PotentialMaxNorm = 0.0;

  this->MaxTimeStep = 1.0; // pixels per time step

  this->NumberOfStiffnessSubIterations = 5;

  this->Neighborhoods = NULL;

  VoxelOffsets[0][0] = 0;  VoxelOffsets[0][1] = 0;  VoxelOffsets[0][2] = 0;
  VoxelOffsets[1][0] = 1;  VoxelOffsets[1][1] = 0;  VoxelOffsets[1][2] = 0;
  VoxelOffsets[2][0] = 0;  VoxelOffsets[2][1] = 1;  VoxelOffsets[2][2] = 0;
  VoxelOffsets[3][0] = 1;  VoxelOffsets[3][1] = 1;  VoxelOffsets[3][2] = 0;
  VoxelOffsets[4][0] = 0;  VoxelOffsets[4][1] = 0;  VoxelOffsets[4][2] = 1;
  VoxelOffsets[5][0] = 1;  VoxelOffsets[5][1] = 0;  VoxelOffsets[5][2] = 1;
  VoxelOffsets[6][0] = 0;  VoxelOffsets[6][1] = 1;  VoxelOffsets[6][2] = 1;
  VoxelOffsets[7][0] = 1;  VoxelOffsets[7][1] = 1;  VoxelOffsets[7][2] = 1;
}

vtkvmtkPolyDataPotentialFit::~vtkvmtkPolyDataPotentialFit()
{
  if (this->PotentialImage)
    {
    this->PotentialImage->Delete();
    this->PotentialImage = NULL;
    }

  if (this->Displacements)
    {
    this->Displacements->Delete();
    this->Displacements = NULL;
    }

  if (this->Neighborhoods)
    {
    this->Neighborhoods->Delete();
    this->Neighborhoods = NULL;
    }
}

vtkCxxSetObjectMacro(vtkvmtkPolyDataPotentialFit,PotentialImage,vtkImageData);

void vtkvmtkPolyDataPotentialFit::EvaluateForce(double point[3], double force[3], bool normalize)
{
  vtkIdType ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  force[0] = force[1] = force[2] = 0.0;

  inBounds = this->PotentialImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds || !this->IsCellInExtent(this->PotentialImage->GetExtent(),ijk,0))
    {
    return;
    }

  vtkVoxel::InterpolationFunctions(pcoords,weights);

  for (i=0; i<8; i++)
    {
    double vectorValue[3];
    int offsetIjk[3];
    offsetIjk[0] = ijk[0]+VoxelOffsets[i][0];
    offsetIjk[1] = ijk[1]+VoxelOffsets[i][1];
    offsetIjk[2] = ijk[2]+VoxelOffsets[i][2];
    this->PotentialImage->GetPointData()->GetScalars()->GetTuple(this->PotentialImage->ComputePointId(offsetIjk),vectorValue);
    force[0] += weights[i] * vectorValue[0];
    force[1] += weights[i] * vectorValue[1];
    force[2] += weights[i] * vectorValue[2];
    }

  if (normalize && this->PotentialMaxNorm > VTK_VMTK_DOUBLE_TOL)
    {
    force[0] /= this->PotentialMaxNorm;
    force[1] /= this->PotentialMaxNorm;
    force[2] /= this->PotentialMaxNorm;
    }

  force[0] *= -1.0;
  force[1] *= -1.0;
  force[2] *= -1.0;
}

void vtkvmtkPolyDataPotentialFit::ComputePotentialDisplacement(vtkIdType pointId, double potentialDisplacement[3])
{
  double point[3];
  double force[3];
  this->GetOutput()->GetPoint(pointId, point);
  this->EvaluateForce(point, force);
  potentialDisplacement[0] = force[0];
  potentialDisplacement[1] = force[1];
  potentialDisplacement[2] = force[2];
}

void vtkvmtkPolyDataPotentialFit::ComputeStiffnessDisplacement(vtkIdType pointId, double stiffnessDisplacement[3])
{
  vtkIdType j;
  double point[3];
  double laplacianPoint[3], neighborhoodPoint[3];
  vtkIdType ijk[3];
  double pcoords[3];
  vtkIdType numberOfNeighborhoodPoints;
  vtkvmtkNeighborhood *neighborhood;
  vtkPolyData *output;

  output = this->GetOutput();
  neighborhood = this->Neighborhoods->GetNeighborhood(pointId);
  
  output->GetPoint(pointId,point);

  stiffnessDisplacement[0] = stiffnessDisplacement[1] = stiffnessDisplacement[2] = 0.0;

  this->PotentialImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (this->InplanePotential)
    {
    if ((pcoords[2] < this->InplaneTolerance) || (1.0 - pcoords[2] < this->InplaneTolerance))
      {
      return;
      }
    }

//   double weight;
//   double weightSum;
//   weightSum = 0.0;

  laplacianPoint[0] = laplacianPoint[1] = laplacianPoint[2] = 0.0;
  numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
  for (j=0; j<numberOfNeighborhoodPoints; j++)
    {
    output->GetPoint(neighborhood->GetPointId(j),neighborhoodPoint);

    laplacianPoint[0] += neighborhoodPoint[0];
    laplacianPoint[1] += neighborhoodPoint[1];
    laplacianPoint[2] += neighborhoodPoint[2];

//     weight = fabs(neighborhoodPoint[2] - point[2]);
//     laplacianPoint[0] += weight * neighborhoodPoint[0];
//     laplacianPoint[1] += weight * neighborhoodPoint[1];
//     laplacianPoint[2] += weight * neighborhoodPoint[2];
//     weightSum += weight;
    }
  laplacianPoint[0] /= (double)numberOfNeighborhoodPoints;
  laplacianPoint[1] /= (double)numberOfNeighborhoodPoints;
  laplacianPoint[2] /= (double)numberOfNeighborhoodPoints;
//   laplacianPoint[0] /= weightSum;
//   laplacianPoint[1] /= weightSum;
//   laplacianPoint[2] /= weightSum;
  
  stiffnessDisplacement[0] = laplacianPoint[0] - point[0];
  stiffnessDisplacement[1] = laplacianPoint[1] - point[1];
  stiffnessDisplacement[2] = laplacianPoint[2] - point[2];
}

void vtkvmtkPolyDataPotentialFit::ComputeDisplacements(bool potential, bool stiffness)
{
  vtkIdType i;
  vtkIdType numberOfPoints;
  vtkPoints *points;
  double displacement[3];
  double potentialDisplacement[3];
  double stiffnessDisplacement[3];
  double displacementNorm;

  numberOfPoints = this->GetOutput()->GetNumberOfPoints();

  this->MaxDisplacementNorm = 0.0;

  for (i=0; i<numberOfPoints; i++)
    {
    displacement[0] = displacement[1] = displacement[2] = 0.0;

    if (potential)
      {
      this->ComputePotentialDisplacement(i, potentialDisplacement);
      displacement[0] += this->PotentialWeight * potentialDisplacement[0];
      displacement[1] += this->PotentialWeight * potentialDisplacement[1];
      displacement[2] += this->PotentialWeight * potentialDisplacement[2];
      }
    
    if (stiffness)
      {
      this->ComputeStiffnessDisplacement(i, stiffnessDisplacement);
      displacement[0] += this->StiffnessWeight * stiffnessDisplacement[0];
      displacement[1] += this->StiffnessWeight * stiffnessDisplacement[1];
      displacement[2] += this->StiffnessWeight * stiffnessDisplacement[2];
      }

    this->Displacements->SetTuple(i, displacement);
    displacementNorm = vtkMath::Norm(displacement);

    if (displacementNorm > this->MaxDisplacementNorm)
      {
      this->MaxDisplacementNorm = displacementNorm;
      }
    }
}

void vtkvmtkPolyDataPotentialFit::ComputeTimeStep()
{
  if (this->MaxDisplacementNorm > VTK_VMTK_DOUBLE_TOL)
    {
    this->TimeStep = this->MinPotentialSpacing / this->MaxDisplacementNorm;
    if (this->TimeStep > this->MaxTimeStep)
      {
      this->TimeStep = this->MaxTimeStep;
      }
    }
  else
    {
    this->TimeStep = this->MaxTimeStep;
    }
}

void vtkvmtkPolyDataPotentialFit::ApplyDisplacements()
{
  vtkIdType i;
  vtkIdType numberOfPoints;
  double point[3], displacement[3], newPoint[3];
  vtkPoints *points;

  points = this->GetOutput()->GetPoints();
  numberOfPoints = points->GetNumberOfPoints();

  for (i=0; i<numberOfPoints; i++)
    {
    points->GetPoint(i,point);
    this->Displacements->GetTuple(i,displacement);
    newPoint[0] = point[0] + this->Relaxation * this->TimeStep * displacement[0];
    newPoint[1] = point[1] + this->Relaxation * this->TimeStep * displacement[1];
    newPoint[2] = point[2] + this->Relaxation * this->TimeStep * displacement[2];
    points->SetPoint(i,newPoint);
    }
}

double vtkvmtkPolyDataPotentialFit::ComputeMinSpacing(double spacing[3])
{
  double minSpacing;
  minSpacing = (spacing[0] < spacing[1]) && (spacing[0] < spacing[2]) ? spacing[0] : (spacing[1] < spacing[2]) ? spacing[1] : spacing[2];
  return minSpacing;
}

int vtkvmtkPolyDataPotentialFit::TestConvergence()
{
  return this->TimeStep * this->MaxDisplacementNorm < this->Convergence ? 1 : 0;
}

void vtkvmtkPolyDataPotentialFit::Execute()
{
  bool potential, stiffness;
  vtkIdType n;
  vtkIdType numberOfPoints;
  vtkPoints *newPoints;
  vtkPolyData *input;
  vtkPolyData *output;

  input = this->GetInput();
  output = this->GetOutput();

  numberOfPoints = input->GetNumberOfPoints();

  newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());

  if (this->Displacements)
    {
    this->Displacements->Delete();
    this->Displacements = NULL;
    }

  this->Displacements = vtkDoubleArray::New();
  this->Displacements->SetNumberOfComponents(3);
  this->Displacements->SetNumberOfTuples(numberOfPoints);

  output->SetPoints(newPoints);
  output->SetVerts(input->GetVerts());
  output->SetLines(input->GetLines());
  output->SetPolys(input->GetPolys());
  output->SetStrips(input->GetStrips());
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  newPoints->Delete();

  if (this->Neighborhoods)
    {
    this->Neighborhoods->Delete();
    this->Neighborhoods = NULL;
    }

  this->Neighborhoods = vtkvmtkNeighborhoods::New();
  this->Neighborhoods->SetNeighborhoodTypeToPolyDataManifoldNeighborhood();
  this->Neighborhoods->SetDataSet(input);
  this->Neighborhoods->Build();

  this->MinPotentialSpacing = this->ComputeMinSpacing(this->PotentialImage->GetSpacing());

  this->PotentialMaxNorm = this->PotentialImage->GetPointData()->GetScalars()->GetMaxNorm();

  for (n=0; n<this->NumberOfIterations; n++)
    {
    stiffness = true;
    potential = n % this->NumberOfStiffnessSubIterations ? false : true;
    
    this->ComputeDisplacements(potential, stiffness);
    this->ComputeTimeStep();
    this->ApplyDisplacements();
    if (potential)
      {
//       cout<<n<<" "<<this->MaxDisplacementNorm<<" "<<this->TimeStep<<endl;
      if (this->TestConvergence())
        {
        break;
        }
      }
    }
}

void vtkvmtkPolyDataPotentialFit::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

