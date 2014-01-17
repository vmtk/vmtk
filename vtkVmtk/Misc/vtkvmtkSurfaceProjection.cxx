/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfaceProjection.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkSurfaceProjection.h"

#include "vtkCellLocator.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkSurfaceProjection);

vtkvmtkSurfaceProjection::vtkvmtkSurfaceProjection()
{
  this->ReferenceSurface = NULL;
}

vtkvmtkSurfaceProjection::~vtkvmtkSurfaceProjection()
{
  if (this->ReferenceSurface)
    {
    this->ReferenceSurface->Delete();
    this->ReferenceSurface = NULL;
    }
}

int vtkvmtkSurfaceProjection::RequestData(
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

  vtkIdType i;
  vtkIdType cellId;
  int subId;
  double point[3], closestPoint[3];
  double pcoords[3];
  double distance2;
  vtkCellLocator *locator;
  vtkGenericCell *genericCell;

  if (!this->ReferenceSurface)
    {
    vtkErrorMacro(<<"No reference surface!");
    return 1;
    }

  int numberOfPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkPointData* outputPointData = output->GetPointData();
  vtkPointData* referencePointData = this->ReferenceSurface->GetPointData();

  outputPointData->InterpolateAllocate(referencePointData,numberOfPoints);
  
  this->ReferenceSurface->BuildCells();

  locator = vtkCellLocator::New();
  genericCell = vtkGenericCell::New();

  locator->SetDataSet(this->ReferenceSurface);
  locator->BuildLocator();

  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPoint(i,point);
    locator->FindClosestPoint(point,closestPoint,genericCell,cellId,subId,distance2);
    if (this->ReferenceSurface->GetCellType(cellId) != VTK_POLY_LINE)
      {
      double* weights = new double[genericCell->GetNumberOfPoints()];
      genericCell->EvaluatePosition(closestPoint,NULL,subId,pcoords,distance2,weights);
      outputPointData->InterpolatePoint(referencePointData,i,genericCell->GetPointIds(),weights);
      delete[] weights;
      }
    else
      {
      double testPoint0[3], testPoint1[3];
      vtkIdType pointId = genericCell->GetPointId(subId);
      input->GetPoint(genericCell->GetPointId(subId),testPoint0);
      input->GetPoint(genericCell->GetPointId(subId+1),testPoint1);
      if (vtkMath::Distance2BetweenPoints(closestPoint,testPoint1) < vtkMath::Distance2BetweenPoints(closestPoint,testPoint0))
        {
        pointId = genericCell->GetPointId(subId+1);
        }
      outputPointData->CopyData(referencePointData,pointId,i);
      }
    }

  locator->Delete();
  genericCell->Delete();

  return 1;
}

void vtkvmtkSurfaceProjection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
