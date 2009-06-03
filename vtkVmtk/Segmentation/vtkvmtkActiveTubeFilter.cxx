/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkActiveTubeFilter.cxx,v $
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


#include "vtkvmtkActiveTubeFilter.h"

#include "vtkvmtkConstants.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkImageGradient.h"
#include "vtkDoubleArray.h"

#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyLine.h"

#include "vtkVoxel.h"

#include "vtkPointData.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkActiveTubeFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkActiveTubeFilter);

vtkvmtkActiveTubeFilter::vtkvmtkActiveTubeFilter()
{
  this->RadiusArrayName = NULL;

  this->PotentialImage = NULL;
  this->PotentialGradientImage = NULL;
  this->NumberOfIterations = VTK_VMTK_LARGE_INTEGER;
  this->TimeStep = 0.0;
  this->Convergence = 1E-1;
  this->PotentialWeight = 0.0;
  this->StiffnessWeight = 0.0;

  this->PotentialMaxNorm = 0.0;
  this->MinimumRadius = 0.0;
}

vtkvmtkActiveTubeFilter::~vtkvmtkActiveTubeFilter()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->PotentialImage)
    {
    this->PotentialImage->Delete();
    this->PotentialImage = NULL;
    }

  if (this->PotentialGradientImage)
    {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
    }
}

vtkCxxSetObjectMacro(vtkvmtkActiveTubeFilter,PotentialImage,vtkImageData);

void vtkvmtkActiveTubeFilter::EvaluateForce(double point[3], double force[3], bool normalize)
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  force[0] = force[1] = force[2] = 0.0;

  inBounds = this->PotentialGradientImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
    {
    //vtkWarningMacro("Point out of extent");
    return;
    }

  vtkCell* cell = this->PotentialGradientImage->GetCell(this->PotentialGradientImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel cell found in PotentialImage");
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

//  force[0] *= -1.0;
//  force[1] *= -1.0;
//  force[2] *= -1.0;
}

double vtkvmtkActiveTubeFilter::EvaluatePotential(double point[3])
{
  int ijk[3];
  double pcoords[3];
  double weights[8];
  int inBounds;
  int i;

  double potential = 0.0;

  inBounds = this->PotentialImage->ComputeStructuredCoordinates(point,ijk,pcoords);

  if (!inBounds)
    {
    //vtkWarningMacro("Point out of extent");
    return 0.0;
    }

  vtkCell* cell = this->PotentialImage->GetCell(this->PotentialImage->ComputeCellId(ijk));

  if (cell->GetCellType() == VTK_VOXEL)
  {
    vtkVoxel::InterpolationFunctions(pcoords,weights);
  }
  else
  {
    vtkErrorMacro("Non voxel cell found in PotentialImage");
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

int vtkvmtkActiveTubeFilter::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->DeepCopy(input);

  vtkDataArray* radiusArray = output->GetPointData()->GetArray(this->RadiusArrayName);

//  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
//  tube->SetInput(input);
//  tube->SetInputCellId(0);
//  tube->SetPolyBallRadiusArrayName(this->RadiusArrayName);
//  tube->UseRadiusInformationOn();

  vtkImageGradient* gradientFilter = vtkImageGradient::New();
  gradientFilter->SetInput(this->PotentialImage);
  gradientFilter->SetDimensionality(3);
  gradientFilter->Update();

  if (this->PotentialGradientImage)
    {
    this->PotentialGradientImage->Delete();
    this->PotentialGradientImage = NULL;
    }

  this->PotentialGradientImage = vtkImageData::New();
  this->PotentialGradientImage->DeepCopy(gradientFilter->GetOutput());

  this->PotentialMaxNorm = this->PotentialGradientImage->GetPointData()->GetScalars()->GetMaxNorm();

  int numberOfCells = output->GetNumberOfCells();  

  int i;
  for (i=0; i<this->NumberOfIterations; i++)
    {
 
    int c;
    for (c=0; c<numberOfCells; c++)
      {
      vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(output->GetCell(c));
      
      int numberOfPoints = polyLine->GetNumberOfPoints();
      int numberOfSubIds = numberOfPoints - 1;
  
      // TODO: make these parameters
      int numberOfPcoords = 10;
      int numberOfAngularPositions = 16;
    
      // TODO: whip strategy
    
      vtkDoubleArray* coordinatesChangeArray = vtkDoubleArray::New();
      coordinatesChangeArray->SetNumberOfComponents(3);
      coordinatesChangeArray->SetNumberOfTuples(numberOfPoints);
      coordinatesChangeArray->FillComponent(0,0.0);
      coordinatesChangeArray->FillComponent(1,0.0);
      coordinatesChangeArray->FillComponent(2,0.0);
    
      vtkDoubleArray* radiusChangeArray = vtkDoubleArray::New();
      radiusChangeArray->SetNumberOfTuples(numberOfPoints);
      radiusChangeArray->FillComponent(0,0.0);
    
      vtkIntArray* changeCountArray = vtkIntArray::New();
      changeCountArray->SetNumberOfTuples(numberOfPoints);
      changeCountArray->FillComponent(0,0.0);
    
      vtkIdType pointId0, pointId1;
      double point0[3], point1[3];
      double radius0, radius1;
      double tangent[3], normal[3];
      int j;
      for (j=0; j<numberOfSubIds; j++)
        {
        pointId0 = polyLine->GetPointId(j);
        pointId1 = polyLine->GetPointId(j+1);
        output->GetPoint(pointId0,point0);
        output->GetPoint(pointId1,point1);
        radius0 = radiusArray->GetTuple1(pointId0);
        radius1 = radiusArray->GetTuple1(pointId1);
        tangent[0] = point1[0] - point0[0];
        tangent[1] = point1[1] - point0[1];
        tangent[2] = point1[2] - point0[2];
        double length = vtkMath::Normalize(tangent);
        double pcoord;
        double point[3];
        double radius;
    
        if (length < 1E-20)
          {
          vtkWarningMacro("Cannot handle segments with zero length. Skipping.");
          continue;
          }
    
        double tubeNormSquared = vtkMath::Distance2BetweenPoints(point0,point1) - (radius1-radius0)*(radius1-radius0);
    
        if (tubeNormSquared < 0.0)
          {
          vtkWarningMacro("Segment has negative tubeNormSquared. Skipping.");
          continue;
          }
        
        double tubeNorm = sqrt(tubeNormSquared);
    
        vtkDoubleArray* probedForces = vtkDoubleArray::New();
        probedForces->SetNumberOfComponents(3);
        probedForces->SetNumberOfTuples(numberOfAngularPositions);
    
        vtkDoubleArray* tubeNormals = vtkDoubleArray::New();
        tubeNormals->SetNumberOfComponents(3);
        tubeNormals->SetNumberOfTuples(numberOfAngularPositions);
    
        int k; 
        for (k=0; k<numberOfPcoords; k++)
          {
          pcoord = (k+1) / (numberOfPcoords+1);
          point[0] = point0[0] + pcoord * (point1[0] - point0[0]);
          point[1] = point0[1] + pcoord * (point1[1] - point0[1]);
          point[2] = point0[2] + pcoord * (point1[2] - point0[2]);
          radius = radius0 + pcoord * (radius1 - radius0);
         
          double isotropicForce = 0.0;
          double anisotropicForce[3];
          anisotropicForce[0] = anisotropicForce[1] = anisotropicForce[2] = 0.0;
    
          double tubeNormal[3], probePoint[3], probedForce[3];
          double theta;
          int z;
          for (z=0; z<numberOfAngularPositions; z++)
            {
            theta = z * 2.0 * vtkMath::Pi() / numberOfAngularPositions;
    
            vtkMath::Perpendiculars(tangent,normal,NULL,theta);
    
            tubeNormal[0] = - tangent[0] * (radius1 - radius0) / length + normal[0] * tubeNorm / length;
            tubeNormal[1] = - tangent[1] * (radius1 - radius0) / length + normal[1] * tubeNorm / length;
            tubeNormal[2] = - tangent[2] * (radius1 - radius0) / length + normal[2] * tubeNorm / length;
    
            probePoint[0] = point[0] + tubeNormal[0] * radius;
            probePoint[1] = point[1] + tubeNormal[1] * radius;
            probePoint[2] = point[2] + tubeNormal[2] * radius;
    
            //double probedPotential = this->EvaluatePotential(probePoint);
    
            this->EvaluateForce(probePoint,probedForce,false);
    
            probedForces->SetTuple(z,probedForce);
            tubeNormals->SetTuple(z,tubeNormal);
    
            isotropicForce += vtkMath::Dot(probedForce,tubeNormal);
            }
    
          isotropicForce /= numberOfAngularPositions;
    
          for (z=0; z<numberOfAngularPositions; z++)
            {
            probedForces->GetTuple(z,probedForce);
            tubeNormals->GetTuple(z,tubeNormal);
            anisotropicForce[0] += probedForce[0] - tubeNormal[0] * isotropicForce;
            anisotropicForce[1] += probedForce[1] - tubeNormal[1] * isotropicForce;
            anisotropicForce[2] += probedForce[2] - tubeNormal[2] * isotropicForce;
            }
    
          anisotropicForce[0] /= numberOfAngularPositions;
          anisotropicForce[1] /= numberOfAngularPositions;
          anisotropicForce[2] /= numberOfAngularPositions;
    
          //anisotropicForce[0] = anisotropicForce[0] - vtkMath::Dot(tangent,anisotropicForce) * tangent[0];
          //anisotropicForce[1] = anisotropicForce[1] - vtkMath::Dot(tangent,anisotropicForce) * tangent[1];
          //anisotropicForce[2] = anisotropicForce[2] - vtkMath::Dot(tangent,anisotropicForce) * tangent[2];
    
          double coordinatesChange0[3], coordinatesChange1[3], radiusChange0, radiusChange1;
          coordinatesChange0[0] = anisotropicForce[0] * (1.0 - pcoord);
          coordinatesChange0[1] = anisotropicForce[1] * (1.0 - pcoord);
          coordinatesChange0[2] = anisotropicForce[2] * (1.0 - pcoord);
          coordinatesChange1[0] = anisotropicForce[0] * pcoord;
          coordinatesChange1[1] = anisotropicForce[1] * pcoord;
          coordinatesChange1[2] = anisotropicForce[2] * pcoord;
          radiusChange0 = isotropicForce * (1.0 - pcoord);
          radiusChange1 = isotropicForce * pcoord;
    
          double currentCoordinatesChange[3], currentRadiusChange;
          coordinatesChangeArray->GetTuple(j,currentCoordinatesChange);
          currentRadiusChange = radiusChangeArray->GetValue(j);
          currentCoordinatesChange[0] += coordinatesChange0[0];
          currentCoordinatesChange[1] += coordinatesChange0[1];
          currentCoordinatesChange[2] += coordinatesChange0[2];
          currentRadiusChange += radiusChange0;
          coordinatesChangeArray->SetTuple(j,currentCoordinatesChange);
          radiusChangeArray->SetValue(j,currentRadiusChange);
          changeCountArray->SetValue(j,changeCountArray->GetValue(j)+1);
    
          coordinatesChangeArray->GetTuple(j+1,currentCoordinatesChange);
          currentRadiusChange = radiusChangeArray->GetValue(j+1);
          currentCoordinatesChange[0] += coordinatesChange1[0];
          currentCoordinatesChange[1] += coordinatesChange1[1];
          currentCoordinatesChange[2] += coordinatesChange1[2];
          currentRadiusChange += radiusChange1;
          coordinatesChangeArray->SetTuple(j+1,currentCoordinatesChange);
          radiusChangeArray->SetValue(j+1,currentRadiusChange);
          changeCountArray->SetValue(j+1,changeCountArray->GetValue(j+1)+1);
    
          }
    
        probedForces->Delete();
        tubeNormals->Delete();
        }
    
      double coordinatesChange[3], radiusChange;
      for (j=0; j<numberOfPoints; j++)
        {
        int changeCount = changeCountArray->GetValue(j);
        if (changeCount == 0)
          {
          continue;
          }
        coordinatesChangeArray->GetTuple(j,coordinatesChange);
        radiusChange = radiusChangeArray->GetValue(j);
        coordinatesChange[0] /= changeCount;
        coordinatesChange[1] /= changeCount;
        coordinatesChange[2] /= changeCount;
        radiusChange /= changeCount;
        coordinatesChangeArray->SetTuple(j,coordinatesChange);
        radiusChangeArray->SetValue(j,radiusChange);
        }
    
      vtkIdType pointId;
      double point[3], radius;
      for (j=0; j<numberOfPoints; j++)
        {
        pointId = polyLine->GetPointId(j);
        output->GetPoint(pointId,point);
        radius = radiusArray->GetTuple1(pointId);
        coordinatesChangeArray->GetTuple(j,coordinatesChange);
        radiusChange = radiusChangeArray->GetValue(j);
        point[0] += coordinatesChange[0] * this->TimeStep;
        point[1] += coordinatesChange[1] * this->TimeStep;
        point[2] += coordinatesChange[2] * this->TimeStep;
        radius += radiusChange * this->TimeStep;
        if (radius < this->MinimumRadius)
          {
          radius = this->MinimumRadius;
          }
        output->GetPoints()->SetPoint(pointId,point);
        radiusArray->SetTuple1(pointId,radius);
        }
    #if 0
      for (j=1; j<numberOfPoints-1; j++)
        {
        pointId0 = polyLine->GetPointId(j-1);
        pointId = polyLine->GetPointId(j);
        pointId1 = polyLine->GetPointId(j+1);
        output->GetPoint(pointId0,point0);
        output->GetPoint(pointId,point);
        output->GetPoint(pointId1,point1);
        radius0 = radiusArray->GetTuple1(pointId0);
        radius = radiusArray->GetTuple1(pointId);
        radius1 = radiusArray->GetTuple1(pointId1);
    
        point[0] += 0.5 * (point1[0] - point0[0]) * 10 * this->TimeStep;
        point[1] += 0.5 * (point1[1] - point0[1]) * 10 * this->TimeStep;
        point[2] += 0.5 * (point1[2] - point0[2]) * 10 * this->TimeStep;
        radius += 0.5 * (radius1 - radius0) * 10 * this->TimeStep;
        output->GetPoints()->SetPoint(pointId,point);
        radiusArray->SetTuple1(pointId,radius);
        }
    #endif
      coordinatesChangeArray->Delete();
      radiusChangeArray->Delete();
      changeCountArray->Delete();
      }
    }

  gradientFilter->Delete();
//  tube->Delete();

  return 1;
}

void vtkvmtkActiveTubeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

