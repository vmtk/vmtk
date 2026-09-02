/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSimpleCapPolyData.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkSimpleCapPolyData.h"
#include "vtkvmtkBoundaryLabels.h"
#include "vtkIdList.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkSimpleCapPolyData);

vtkvmtkSimpleCapPolyData::vtkvmtkSimpleCapPolyData()
{
  this->CellEntityIdsArrayName = NULL;
  this->BoundaryIds = NULL;
  this->CellEntityIdOffset = 1;
  this->BoundaryLabelsArrayName = NULL;
  this->BoundaryPointOrderArrayName = NULL;
  this->BoundaryCellEntityIds = NULL;
}

vtkvmtkSimpleCapPolyData::~vtkvmtkSimpleCapPolyData()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }

  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }

  this->SetBoundaryLabelsArrayName(NULL);
  this->SetBoundaryPointOrderArrayName(NULL);

  if (this->BoundaryCellEntityIds)
    {
    this->BoundaryCellEntityIds->Delete();
    this->BoundaryCellEntityIds = NULL;
    }
}

int vtkvmtkSimpleCapPolyData::RequestData(
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

  if ( input->GetNumberOfPoints() < 1 )
    {
    return 1;
    }

  bool markCells = this->CellEntityIdsArrayName && this->CellEntityIdsArrayName[0];

  input->BuildLinks();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());

  vtkCellArray* newPolys = vtkCellArray::New();
  newPolys->DeepCopy(input->GetPolys());

  vtkIdTypeArray* cellEntityIdsArray = NULL;

  if (markCells)
    {
    cellEntityIdsArray = vtkIdTypeArray::New();
    cellEntityIdsArray->SetName(this->CellEntityIdsArrayName);
    if (input->GetCellData()->GetArray(this->CellEntityIdsArrayName))
      {
      cellEntityIdsArray->DeepCopy(input->GetCellData()->GetArray(this->CellEntityIdsArrayName));
      }
    else
      {
      cellEntityIdsArray->SetNumberOfTuples(newPolys->GetNumberOfCells());
      cellEntityIdsArray->FillComponent(0,static_cast<double>(this->CellEntityIdOffset));
      }
    }

  // The boundaries either come from the labels the input already carries, which are the same
  // boundaries in the same order every other filter reading those labels sees, or they are
  // extracted here as they always were.
  vtkNew<vtkPolyData> boundaries;
  vtkNew<vtkIdList> boundaryLabels;
  bool useBoundaryLabels = vtkvmtkBoundaryLabels::GetOrExtractBoundaries(
    input,this->BoundaryLabelsArrayName,this->BoundaryPointOrderArrayName,boundaries,boundaryLabels,this);

  for (int i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    // A boundary is named by its label when the labels are in use, and by its position in the
    // extraction order otherwise, here as everywhere else in this filter.
    vtkIdType boundaryId = useBoundaryLabels ? boundaryLabels->GetId(i) : i;
    if (this->BoundaryIds)
      {
      if (this->BoundaryIds->IsId(boundaryId) == -1)
        {
        continue;
        }
      }
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));
    vtkIdType numberOfBoundaryPoints = boundary->GetNumberOfPoints();
    vtkIdList* boundaryPointIds = vtkIdList::New();
    boundaryPointIds->SetNumberOfIds(numberOfBoundaryPoints);
    for (int j=0; j<numberOfBoundaryPoints; j++)
      {
      boundaryPointIds->SetId(j,static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundary->GetPointId(j))));
      }
    newPolys->InsertNextCell(boundaryPointIds);
    if (markCells)
      {
      // An id the caller chose for this boundary is used as it stands; CellEntityIdOffset is
      // what moves the ids this filter derives itself out of the way of the input's, and has
      // no business shifting one that was picked deliberately.
      // With the labels in use the boundary's label is its name, so it is also the cap's id
      // unless the caller chose another; neither is shifted by CellEntityIdOffset.
      vtkIdType capCellEntityId = useBoundaryLabels ? boundaryId : i+1+this->CellEntityIdOffset;
      if (this->BoundaryCellEntityIds
          && boundaryId < this->BoundaryCellEntityIds->GetNumberOfTuples()
          && this->BoundaryCellEntityIds->GetValue(boundaryId) >= 0)
        {
        capCellEntityId = this->BoundaryCellEntityIds->GetValue(boundaryId);
        }
      cellEntityIdsArray->InsertNextValue(capCellEntityId);
      }
    boundaryPointIds->Delete();
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  output->GetPointData()->PassData(input->GetPointData());

  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    cellEntityIdsArray->Delete();
    }

  newPoints->Delete();
  newPolys->Delete();

  return 1;
}

void vtkvmtkSimpleCapPolyData::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
