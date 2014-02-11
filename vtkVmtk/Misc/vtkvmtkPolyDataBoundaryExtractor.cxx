/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBoundaryExtractor.cxx,v $
Language:  C++
Date:      $Date: 2006/07/27 08:27:40 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkGenericCell.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataBoundaryExtractor);

vtkvmtkPolyDataBoundaryExtractor::vtkvmtkPolyDataBoundaryExtractor()
{
}

int vtkvmtkPolyDataBoundaryExtractor::RequestData(
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

  // Declare
  vtkIdList *boundary, *boundaryIds, *cellEdgeNeighbors, *newCell;
  vtkIdType i, j, currentId, firstId, id, newId, id0, id1;
  vtkCell* cell;
  bool foundAny, foundNeighbor, done;
  vtkPoints* newPoints;
  vtkCellArray* newLines;
  vtkIdTypeArray* newScalars;

  newId = -1;

  // Initialize
  if ( ((input->GetNumberOfPoints()) < 1) )
    {
    return 1;
    }
  input->BuildLinks();

  // Allocate
  boundary = vtkIdList::New();
  boundaryIds = vtkIdList::New();
  cellEdgeNeighbors = vtkIdList::New();
  newCell = vtkIdList::New();
  newPoints = vtkPoints::New();
  newLines = vtkCellArray::New();
  newScalars = vtkIdTypeArray::New();

  // Execute  
  for (i=0; i<input->GetNumberOfCells(); i++)
    {
    cell = input->GetCell(i);
    for (j=0; j<3; j++)
      {
      cellEdgeNeighbors->Initialize();
      id0 = cell->GetEdge(j)->GetPointIds()->GetId(0);
      id1 = cell->GetEdge(j)->GetPointIds()->GetId(1);
      input->GetCellEdgeNeighbors(i,id0,id1,cellEdgeNeighbors);
      if (cellEdgeNeighbors->GetNumberOfIds()==0)
        {
        boundaryIds->InsertUniqueId(id0);
        boundaryIds->InsertUniqueId(id1);
        }
      }
    }

  foundAny = false;
  foundNeighbor = false;
  done = false;
  currentId = -1;

  while (!done)
    {
    foundAny = false;
    foundNeighbor = false;
                
    for (i=0; i<boundaryIds->GetNumberOfIds(); i++)
      {
      id = boundaryIds->GetId(i);
      if (id!=-1)
        {
        foundAny = true;
        if ((currentId==-1)||(input->IsEdge(currentId,id)))
          {
          foundNeighbor = true;
          }

        if (foundNeighbor)
          {
          currentId = id;
          boundary->InsertNextId(currentId);
          boundaryIds->SetId(i,-1);
          break;
          }
        }
      }
        
    if ( (((!foundNeighbor)&&(foundAny))||(!foundAny)) && (boundary->GetNumberOfIds() > 2))
      {
      newCell->Initialize();
      newCell->SetNumberOfIds(boundary->GetNumberOfIds());
      firstId = newPoints->GetNumberOfPoints();
      for (j=0; j<boundary->GetNumberOfIds(); j++)
        {
        id = boundary->GetId(j);
        newId = newPoints->InsertNextPoint(input->GetPoint(id));
        newCell->SetId(j,newId);
        newScalars->InsertNextValue(id);
        }

      if (input->IsEdge(newId,firstId))
        {
        newCell->InsertNextId(firstId);
        }

      newLines->InsertNextCell(newCell);

      currentId = -1;
      boundary->Initialize();
      }
        
    if (!foundAny)
      {
      done = true;
      }
    }

  output->SetPoints(newPoints);
  output->SetLines(newLines);
  output->GetPointData()->SetScalars(newScalars);

  // Destroy
  newPoints->Delete();
  newLines->Delete();
  newScalars->Delete();

  boundary->Delete();
  boundaryIds->Delete();
  cellEdgeNeighbors->Delete();
  newCell->Delete();

  return 1;
}

void vtkvmtkPolyDataBoundaryExtractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
