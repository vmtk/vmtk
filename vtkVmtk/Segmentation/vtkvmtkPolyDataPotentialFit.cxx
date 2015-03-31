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
#include "vtkImageGradient.h"
#include "vtkDoubleArray.h"
#include "vtkVoxel.h"
#include "vtkPixel.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyDataNormals.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataPotentialFit);

vtkvmtkPolyDataPotentialFit::vtkvmtkPolyDataPotentialFit()
{
  this->PotentialImage = NULL;
  this->InflationImage = NULL;
  this->PotentialGradientImage = NULL;
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
  this->InflationWeight = 0.0;
  this->PotentialMaxNorm = 0.0;
  this->UsePotentialInInflation = 1;
  this->InflationThreshold = 0.0;

  this->MaxTimeStep = 1.0; // pixels per time step
  this->AdaptiveTimeStep = 1;
  
  this->FlipNormals = 0;
  this->Dimensionality = 3;

  this->NumberOfStiffnessSubIterations = 5;
  this->NumberOfInflationSubIterations = 0;

  this->Neighborhoods = NULL;
}

vtkvmtkPolyDataPotentialFit::~vtkvmtkPolyDataPotentialFit()
{
  if (this->PotentialImage)
    {
    this->PotentialImage->Delete();
    this->PotentialImage = NULL;
    }

  if (this->InflationImage)
    {
    this->InflationImage->Delete();
    this->InflationImage = NULL;
    }

  if (this->PotentialGradientImage)
    {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
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
vtkCxxSetObjectMacro(vtkvmtkPolyDataPotentialFit,InflationImage,vtkImageData);

void vtkvmtkPolyDataPotentialFit::EvaluateForce(double point[3], double force[3], bool normalize)
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  force[0] = force[1] = force[2] = 0.0;

  inBounds = this->PotentialGradientImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
//  if (!inBounds || !this->IsCellInExtent(this->PotentialGradientImage->GetExtent(),ijk,0))
    {
    vtkWarningMacro("Point out of extent.");
    return;
    }

  vtkCell* cell = this->PotentialGradientImage->GetCell(this->PotentialGradientImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else if (cell->GetCellType() == VTK_PIXEL)
  {
    vtkPixel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel or pixel cell found in PotentialImage");
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();

  vtkDataArray* gradientVectorsArray = this->PotentialGradientImage->GetPointData()->GetScalars();

  for (i=0; i<numberOfCellPoints; i++)
  {
    double vectorValue[3];
    gradientVectorsArray->GetTuple(cell->GetPointId(i),vectorValue);
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

double vtkvmtkPolyDataPotentialFit::EvaluatePotential(double point[3])
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  double potential = 0.0;

  inBounds = this->PotentialImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
//  if (!inBounds || !this->IsCellInExtent(this->PotentialImage->GetExtent(),ijk,0))
    {
    vtkWarningMacro("Point out of extent.");
    return 0.0;
    }

  vtkCell* cell = this->PotentialImage->GetCell(this->PotentialImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else if (cell->GetCellType() == VTK_PIXEL)
  {
    vtkPixel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel or pixel cell found in PotentialImage");
    return 0.0;
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();

  vtkDataArray* potentialArray = this->PotentialImage->GetPointData()->GetScalars();

  for (i=0; i<numberOfCellPoints; i++)
  {
    double value = potentialArray->GetTuple1(cell->GetPointId(i));
    potential += weights[i] * value;
  }

  return potential;
}

double vtkvmtkPolyDataPotentialFit::EvaluateInflation(double point[3])
{
  if (this->InflationImage == NULL) {
    return 1.0;
  }

  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  double inflation = 0.0;

  inBounds = this->InflationImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
//  if (!inBounds || !this->IsCellInExtent(this->PotentialImage->GetExtent(),ijk,0))
    {
    vtkWarningMacro("Point out of extent.");
    return 0.0;
    }

  vtkCell* cell = this->InflationImage->GetCell(this->InflationImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else if (cell->GetCellType() == VTK_PIXEL)
  {
    vtkPixel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel or pixel cell found in PotentialImage");
    return 0.0;
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();

  vtkDataArray* inflationArray = this->InflationImage->GetPointData()->GetScalars();

  for (i=0; i<numberOfCellPoints; i++)
  {
    double value = inflationArray->GetTuple1(cell->GetPointId(i));
    inflation += weights[i] * value;
  }

  return inflation - this->InflationThreshold;
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
  vtkIdType numberOfNeighborhoodPoints;
  vtkvmtkNeighborhood *neighborhood;
  vtkPolyData *output;

  output = this->GetOutput();
  neighborhood = this->Neighborhoods->GetNeighborhood(pointId);
  
  output->GetPoint(pointId,point);

  stiffnessDisplacement[0] = stiffnessDisplacement[1] = stiffnessDisplacement[2] = 0.0;

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

void vtkvmtkPolyDataPotentialFit::ComputeInflationDisplacement(vtkIdType pointId, double inflationDisplacement[3])
{
  vtkIdType j;
  double point[3], inputOutwardNormal[3];
  double neighborhoodVector[3], firstNeighborhoodVector[3];
  double cross[3], neighborhoodNormal[3];
  double neighborhoodPoint[3];
  double dot;
  vtkIdType numberOfNeighborhoodPoints;
  vtkvmtkNeighborhood *neighborhood;
  vtkPolyData *output;

  output = this->GetOutput();
  neighborhood = this->Neighborhoods->GetNeighborhood(pointId);
 
  if (neighborhood->GetNumberOfPoints() == 0)
    {
    inflationDisplacement[0] = 0.0;
    inflationDisplacement[1] = 0.0;
    inflationDisplacement[2] = 0.0;
    return;
    }

  output->GetPoint(pointId,point);
  this->Normals->GetTuple(pointId,inputOutwardNormal);

  double inflation = this->EvaluateInflation(point);
  double potential = 1.0;
  if (this->UsePotentialInInflation) {
    potential = this->EvaluatePotential(point);
  }

  neighborhoodNormal[0] = 0.0;
  neighborhoodNormal[1] = 0.0;
  neighborhoodNormal[2] = 0.0;

  output->GetPoint(neighborhood->GetPointId(0),neighborhoodPoint);
  firstNeighborhoodVector[0] = neighborhoodPoint[0] - point[0];
  firstNeighborhoodVector[1] = neighborhoodPoint[1] - point[1];
  firstNeighborhoodVector[2] = neighborhoodPoint[2] - point[2];

  numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
  for (j=1; j<numberOfNeighborhoodPoints; j++)
    {
    output->GetPoint(neighborhood->GetPointId(j),neighborhoodPoint);
    neighborhoodVector[0] = neighborhoodPoint[0] - point[0];
    neighborhoodVector[1] = neighborhoodPoint[1] - point[1];
    neighborhoodVector[2] = neighborhoodPoint[2] - point[2];
    vtkMath::Cross(firstNeighborhoodVector,neighborhoodVector,cross);
    neighborhoodNormal[0] += cross[0];
    neighborhoodNormal[1] += cross[1];
    neighborhoodNormal[2] += cross[2];
    }
  vtkMath::Normalize(neighborhoodNormal);

  dot = vtkMath::Dot(inputOutwardNormal,neighborhoodNormal);

  if (dot < 0.0)
  {
    neighborhoodNormal[0] *= -1.0;
    neighborhoodNormal[1] *= -1.0;
    neighborhoodNormal[2] *= -1.0;
  }

  inflationDisplacement[0] = inflation * potential * neighborhoodNormal[0];
  inflationDisplacement[1] = inflation * potential * neighborhoodNormal[1];
  inflationDisplacement[2] = inflation * potential * neighborhoodNormal[2];
}

void vtkvmtkPolyDataPotentialFit::ComputeDisplacements(bool potential, bool stiffness, bool inflation)
{
  vtkIdType i;
  vtkIdType numberOfPoints;
  double displacement[3];
  double potentialDisplacement[3];
  double stiffnessDisplacement[3];
  double inflationDisplacement[3];
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

    if (inflation)
      {
      this->ComputeInflationDisplacement(i, inflationDisplacement);
      displacement[0] += this->InflationWeight * inflationDisplacement[0];
      displacement[1] += this->InflationWeight * inflationDisplacement[1];
      displacement[2] += this->InflationWeight * inflationDisplacement[2];
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
  if (!this->AdaptiveTimeStep)
  {
    return;
  }

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

int vtkvmtkPolyDataPotentialFit::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  bool potential, stiffness, inflation;
  vtkIdType n, m;
  vtkIdType numberOfPoints;
  vtkPoints *newPoints;

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

  vtkImageGradient* gradientFilter = vtkImageGradient::New();
#if (VTK_MAJOR_VERSION <= 5)
  gradientFilter->SetInput(this->PotentialImage);
#else
  gradientFilter->SetInputData(this->PotentialImage);
#endif
  gradientFilter->SetDimensionality(this->Dimensionality);
  gradientFilter->Update();

  if (this->PotentialGradientImage)
  {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
  }

  this->PotentialGradientImage = vtkImageData::New();
  this->PotentialGradientImage->DeepCopy(gradientFilter->GetOutput());

  gradientFilter->Delete();

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

  vtkPolyDataNormals* surfaceNormals = vtkPolyDataNormals::New();
#if (VTK_MAJOR_VERSION <= 5)
  surfaceNormals->SetInput(input);
#else
  surfaceNormals->SetInputData(input);
#endif
  surfaceNormals->SplittingOff();
  surfaceNormals->AutoOrientNormalsOn();
  surfaceNormals->SetFlipNormals(this->FlipNormals);
  surfaceNormals->ComputePointNormalsOn();
  surfaceNormals->ConsistencyOn();
  surfaceNormals->Update();

  this->Normals = surfaceNormals->GetOutput()->GetPointData()->GetNormals();

  this->MinPotentialSpacing = this->ComputeMinSpacing(this->PotentialImage->GetSpacing());

  this->PotentialMaxNorm = this->PotentialGradientImage->GetPointData()->GetScalars()->GetMaxNorm();

  for (n=0; n<this->NumberOfIterations; n++)
  {
    for (m=0; m<this->NumberOfStiffnessSubIterations; m++)
    {
      potential = false;
      stiffness = true;
      inflation = false;
      this->ComputeDisplacements(potential, stiffness, inflation);
      this->ComputeTimeStep();
      this->ApplyDisplacements();
    }
    
    for (m=0; m<this->NumberOfInflationSubIterations; m++)
    {
      potential = false;
      stiffness = false;
      inflation = true;
      this->ComputeDisplacements(potential, stiffness, inflation);
      this->ComputeTimeStep();
      this->ApplyDisplacements();
    }

    potential = true;
    stiffness = true;
    inflation = true;
    this->ComputeDisplacements(potential, stiffness, inflation);
    this->ComputeTimeStep();
    this->ApplyDisplacements();

    if (this->TestConvergence())
    {
      break;
    }
  }

  surfaceNormals->Delete();
  this->Normals = NULL;

  return 1;
}

void vtkvmtkPolyDataPotentialFit::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

