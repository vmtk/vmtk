/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshProjection.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkMeshProjection.h"

#include "vtkCellLocator.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkMeshProjection);

vtkvmtkMeshProjection::vtkvmtkMeshProjection()
{
  this->ReferenceMesh = NULL;
  this->Tolerance = 1E-6;
}

vtkvmtkMeshProjection::~vtkvmtkMeshProjection()
{
  if (this->ReferenceMesh)
    {
    this->ReferenceMesh->Delete();
    this->ReferenceMesh = NULL;
    }
}

int vtkvmtkMeshProjection::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType i;
  vtkIdType cellId;
  int subId;
  double point[3], closestPoint[3];
  double pcoords[3];
  double distance2;
  vtkCellLocator *locator;
  vtkGenericCell *genericCell;

  if (!this->ReferenceMesh)
    {
    vtkErrorMacro(<<"No reference mesh!");
    return 1;
    }

  int numberOfPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkPointData* outputPointData = output->GetPointData();
  vtkPointData* referencePointData = this->ReferenceMesh->GetPointData();

  outputPointData->InterpolateAllocate(referencePointData,numberOfPoints);
  
  locator = vtkCellLocator::New();
  genericCell = vtkGenericCell::New();

  locator->SetDataSet(this->ReferenceMesh);
  locator->BuildLocator();
  locator->SetTolerance(this->Tolerance);

  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    locator->FindClosestPoint(point,closestPoint,genericCell,cellId,subId,distance2);
    double* weights = new double[genericCell->GetNumberOfPoints()];
    genericCell->EvaluatePosition(closestPoint,NULL,subId,pcoords,distance2,weights);

    outputPointData->InterpolatePoint(referencePointData,i,genericCell->GetPointIds(),weights);

    delete[] weights;
    }

  locator->Delete();
  genericCell->Delete();

  return 1;
}

void vtkvmtkMeshProjection::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
