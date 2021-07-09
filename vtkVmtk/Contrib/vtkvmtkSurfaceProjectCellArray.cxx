/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfaceProjectCellArray.cxx,v $
Language:  C++
Date:      $$
Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
     
  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/

#include "vtkvmtkSurfaceProjectCellArray.h"

#include "vtkCellLocator.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"

#include <algorithm>

vtkStandardNewMacro(vtkvmtkSurfaceProjectCellArray);

vtkvmtkSurfaceProjectCellArray::vtkvmtkSurfaceProjectCellArray()
{
  this->ReferenceSurface = NULL;
  this->ProjectedArrayName = NULL;
  this->DistanceTolerance = VTK_VMTK_LARGE_DOUBLE;
  this->DefaultValue = 0.;
}

vtkvmtkSurfaceProjectCellArray::~vtkvmtkSurfaceProjectCellArray()
{
  if (this->ReferenceSurface)
    {
    this->ReferenceSurface->Delete();
    this->ReferenceSurface = NULL;
    }
  
  if (this->ProjectedArrayName)
    {
    delete[] this->ProjectedArrayName;
    this->ProjectedArrayName = NULL;
    }
}

int vtkvmtkSurfaceProjectCellArray::RequestData(
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
  
  
  vtkCellLocator *locator;
  vtkGenericCell *genericCell;

  if (!this->ReferenceSurface)
    {
    vtkErrorMacro(<<"No reference surface!");
    return 1;
    }

  vtkDataArray *referenceArray = this->ReferenceSurface->GetCellData()->GetArray(this->ProjectedArrayName);
  if (!referenceArray)
    {
    vtkErrorMacro(<<"No reference array!");
    return 1;
    }

  int numberOfCells = input->GetNumberOfCells();

  output->DeepCopy(input);
  
  vtkDataArray *projectedArray = output->GetCellData()->GetArray(this->ProjectedArrayName);
  
  if (!projectedArray)
    {
    projectedArray = vtkDoubleArray::New();
    projectedArray->SetName(this->ProjectedArrayName);
    projectedArray->SetNumberOfTuples(numberOfCells);
    projectedArray->SetNumberOfComponents(referenceArray->GetNumberOfComponents());
    output->GetCellData()->AddArray(projectedArray);
    }
    
  int numberOfComponents = std::min(referenceArray->GetNumberOfComponents(), projectedArray->GetNumberOfComponents());

  for (int i=0; i<numberOfComponents; i++)
    {
    projectedArray->FillComponent(i, DefaultValue);
    }

  
  this->ReferenceSurface->BuildCells();

  locator = vtkCellLocator::New();
  genericCell = vtkGenericCell::New();

  locator->SetDataSet(this->ReferenceSurface);
  locator->BuildLocator();

  vtkIdList *cellPtsIds = vtkIdList::New();
  
  double point[3], closestPoint[3];
  double distance2;
  double distanceTolerance2 = this->DistanceTolerance*this->DistanceTolerance;
  vtkIdType cellId;
  int subId;

  for (int i=0; i<numberOfCells; i++)
    {
    input->GetCellPoints(i,cellPtsIds);
    bool skipCell = false;
    double minDistance = VTK_VMTK_LARGE_DOUBLE;
    vtkIdType minId = 0;
    
    //Find the cell with the smallest minimum distance to the vertices
    for (int j=0; j<cellPtsIds->GetNumberOfIds(); j++)
      {
      input->GetPoint(cellPtsIds->GetId(j), point);
      locator->FindClosestPoint(point,closestPoint,genericCell,cellId,subId,distance2);
      if (distance2>distanceTolerance2)
        {
        skipCell = true;
        break;
        }
      if ((j==0) || (distance2 < minDistance))
        {
        minDistance = distance2;
        minId = cellId;
        }
      }
    
    if (!skipCell)
      {
      for (int j=0; j<numberOfComponents; j++)
        {
        projectedArray->SetComponent(i,j,referenceArray->GetComponent(minId,j));
        }
      }
    }

  locator->Delete();
  genericCell->Delete();
  cellPtsIds->Delete();

  return 1;
}

void vtkvmtkSurfaceProjectCellArray::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
