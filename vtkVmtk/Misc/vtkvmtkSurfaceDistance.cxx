/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfaceDistance.cxx,v $
Language:  C++
Date:      $Date: 2005/03/31 15:49:05 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkSurfaceDistance.h"

#include "vtkCellLocator.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkSurfaceDistance);

vtkvmtkSurfaceDistance::vtkvmtkSurfaceDistance()
{
  this->DistanceArrayName = NULL;
  this->DistanceVectorsArrayName = NULL;
  this->SignedDistanceArrayName = NULL;
  this->ReferenceSurface = NULL;
}

vtkvmtkSurfaceDistance::~vtkvmtkSurfaceDistance()
{
  if (this->ReferenceSurface)
    {
    this->ReferenceSurface->Delete();
    this->ReferenceSurface = NULL;
    }

  if (this->DistanceArrayName)
    {
    delete[] this->DistanceArrayName;
    this->DistanceArrayName = NULL;
    }

  if (this->DistanceVectorsArrayName)
    {
    delete[] this->DistanceVectorsArrayName;
    this->DistanceVectorsArrayName = NULL;
    }

  if (this->SignedDistanceArrayName)
    {
    delete[] this->SignedDistanceArrayName;
    this->SignedDistanceArrayName = NULL;
    }
}

int vtkvmtkSurfaceDistance::RequestData(
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

  vtkIdType i, j;
  vtkIdType numberOfPoints, numberOfCellPoints;
  vtkIdType cellId, pointId;
  int subId;
  double point[3], closestPoint[3];
  double distance2, distance, signedDistance;
  double pcoords[3];
  double pointNormal[3], *normal;
  double distanceVector[3];
  double *weights;
  double dot;
  bool computeDistance, computeDistanceVectors, computeSignedDistance;
  vtkDoubleArray *distanceArray, *distanceVectorsArray, *signedDistanceArray;
  vtkDataArray *normals;
  vtkCellLocator *locator;
  vtkGenericCell *genericCell;
  vtkCell *cell;

  if (!this->ReferenceSurface)
    {
    vtkErrorMacro(<<"No reference surface!");
    return 1;
    }

  if ((!this->DistanceArrayName)&&(!this->DistanceVectorsArrayName)&&(!this->SignedDistanceArrayName))
    {
    vtkErrorMacro(<<"No array names set!");
    return 1;
    }

  distanceArray = vtkDoubleArray::New();
  distanceVectorsArray = vtkDoubleArray::New();
  signedDistanceArray = vtkDoubleArray::New();
  normals = NULL;

  locator = vtkCellLocator::New();
  genericCell = vtkGenericCell::New();

  numberOfPoints = input->GetNumberOfPoints();

  computeDistance = false;
  computeDistanceVectors = false;
  computeSignedDistance = false;

  if (this->DistanceArrayName)
    {
    computeDistance = true;
    distanceArray->SetName(this->DistanceArrayName);
    distanceArray->SetNumberOfComponents(1);
    distanceArray->SetNumberOfTuples(numberOfPoints);
    }

  if (this->DistanceVectorsArrayName)
    {
    computeDistanceVectors = true;
    distanceVectorsArray->SetName(this->DistanceVectorsArrayName);
    distanceVectorsArray->SetNumberOfComponents(3);
    distanceVectorsArray->SetNumberOfTuples(numberOfPoints);
    }

  if (this->SignedDistanceArrayName)
    {
    computeSignedDistance = true;
    signedDistanceArray->SetName(this->SignedDistanceArrayName);
    signedDistanceArray->SetNumberOfComponents(1);
    signedDistanceArray->SetNumberOfTuples(numberOfPoints);
    normals = this->ReferenceSurface->GetPointData()->GetNormals();
    if (!normals)
      {
      vtkErrorMacro(<<"Signed distance requires point normals to be defined over ReferenceSurface!");
      return 1;
      }
    }

  locator->SetDataSet(this->ReferenceSurface);
  locator->BuildLocator();

  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    locator->FindClosestPoint(point,closestPoint,genericCell,cellId,subId,distance2);
    distanceVector[0] = point[0] - closestPoint[0];
    distanceVector[1] = point[1] - closestPoint[1];
    distanceVector[2] = point[2] - closestPoint[2];
    distance = sqrt(distance2);
    
    if (computeDistance)
      {
      distanceArray->SetTuple1(i,distance);
      }

    if (computeDistanceVectors)
      {
      distanceVectorsArray->SetTuple3(i,-distanceVector[0],-distanceVector[1],-distanceVector[2]);
      }

    if (computeSignedDistance)
      {
      cell = this->ReferenceSurface->GetCell(cellId);
      numberOfCellPoints = cell->GetNumberOfPoints();
      weights = new double[numberOfCellPoints];
      cell->EvaluatePosition(point,NULL,subId,pcoords,distance2,weights);
      pointNormal[0] = 0.0;
      pointNormal[1] = 0.0;
      pointNormal[2] = 0.0;
      for (j=0; j<numberOfCellPoints; j++)
        {
        pointId = cell->GetPointId(j);
        normal = normals->GetTuple3(pointId);
        pointNormal[0] += weights[j] * normal[0];
        pointNormal[1] += weights[j] * normal[1];
        pointNormal[2] += weights[j] * normal[2];
        }
      dot = vtkMath::Dot(distanceVector,pointNormal);
      signedDistance = distance;
      // distance is positive if distanceVector and normal have negative dot
      if (dot>0.0)
        {
        signedDistance *= -1.0;
        }
      signedDistanceArray->SetTuple1(i,signedDistance);
      delete[] weights;
      }
    }

  output->DeepCopy(input);

  if (computeDistance)
    {
    output->GetPointData()->AddArray(distanceArray);
    }

  if (computeDistanceVectors)
    {
    output->GetPointData()->AddArray(distanceVectorsArray);
    }

  if (computeSignedDistance)
    {
    output->GetPointData()->AddArray(signedDistanceArray);
    }

  distanceArray->Delete();
  distanceVectorsArray->Delete();
  signedDistanceArray->Delete();
  locator->Delete();
  genericCell->Delete();

  return 1;
}

void vtkvmtkSurfaceDistance::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
