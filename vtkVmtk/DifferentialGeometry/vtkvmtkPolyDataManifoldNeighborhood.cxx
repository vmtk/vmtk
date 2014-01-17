/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataManifoldNeighborhood.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:44 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataManifoldNeighborhood.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkMath.h"

  
vtkStandardNewMacro(vtkvmtkPolyDataManifoldNeighborhood);

void vtkvmtkPolyDataManifoldNeighborhood::Build()
{
  vtkIdType i, j, id;
  vtkIdType numCellsInStencil;
  vtkIdType startCell, nextCell;
  vtkIdType p1, p2;
  vtkIdType bp1, bp2;
  vtkIdType K;
  vtkIdType pointId;
  vtkIdList *cellIds, *ptIds, *stencilIds;
  vtkCell* cell;
  vtkPolyData* pdata = vtkPolyData::SafeDownCast(this->DataSet);

  if (pdata==NULL)
    {
    vtkErrorMacro(<< "Input data NULL or not poly data");
    }

  pointId = this->DataSetPointId;

  this->NPoints = 0;

  cellIds = vtkIdList::New();
  ptIds = vtkIdList::New();
  stencilIds = vtkIdList::New();

  pdata->GetPointCells (pointId, cellIds);
  numCellsInStencil = cellIds->GetNumberOfIds();
  if (numCellsInStencil < 1)
    {
    //          vtkWarningMacro("numCellsInStencil < 1: " << numCellsInStencil);
    cellIds->Delete();
    ptIds->Delete();
    stencilIds->Delete();
    return;
    }

  pdata->GetCellPoints(cellIds->GetId(0),ptIds);
  p2 = ptIds->GetId(0);
  i = 1;
  while (pointId == p2)
    {
    p2 = ptIds->GetId(i++);
    }

  pdata->GetCellEdgeNeighbors (-1,pointId,p2,cellIds);

  nextCell = cellIds->GetId(0);
  bp2 = -1;
  bp1 = p2;
  if (cellIds->GetNumberOfIds() == 1)
    {
    startCell = -1;
    }
  else
    {
    startCell = cellIds->GetId(1);
    }

  stencilIds->InsertNextId(p2);

  // walk around the stencil counter-clockwise and get cells
  for (j=0; j<numCellsInStencil; j++)
    {
    cell = pdata->GetCell(nextCell);
    p1 = -1;
    for (i = 0; i < 3; i++)
      {
      if ((p1 = cell->GetPointId(i)) != pointId && cell->GetPointId(i) != p2)
        {
        break;
        }
      }

    p2 = p1;
    stencilIds->InsertNextId (p2);
    pdata->GetCellEdgeNeighbors (nextCell, pointId, p2, cellIds);
    if (cellIds->GetNumberOfIds() != 1)
      {
      bp2 = p2;
      j++;
      break;
      }
    nextCell = cellIds->GetId(0);
    }

  // now walk around the other way. this will only happen if there
  // is a boundary cell left that we have not visited
  nextCell = startCell;
  p2 = bp1;
  for (; j<numCellsInStencil && startCell!=-1; j++)
    {
    cell = pdata->GetCell(nextCell);
    p1 = -1;
    for (i=0; i<3; i++)
      {
      if ((p1=cell->GetPointId(i))!=pointId && cell->GetPointId(i)!=p2)
        {
        break;
        }
      }
    p2 = p1;
    stencilIds->InsertNextId(p2);
    pdata->GetCellEdgeNeighbors (nextCell, pointId, p2, cellIds);
    if (cellIds->GetNumberOfIds() != 1)
      {
      bp1 = p2;
      break;
      }
    nextCell = cellIds->GetId(0);
    }

  if (bp2 != -1) // boundary edge
    {
    this->IsBoundary = true;
    id = stencilIds->IsId(bp2);
    for (i=0; i<=id/2; i++)
      {
      p1 = stencilIds->GetId(i);
      p2 = stencilIds->GetId(id-i);
      stencilIds->SetId(i,p2);
      stencilIds->SetId(id-i,p1);
      }
    }
  else
    {
    this->IsBoundary = false;
    K = stencilIds->GetNumberOfIds();
    // Remove last id. It's a duplicate of the first
    stencilIds->SetNumberOfIds(--K);
    }

  this->NPoints = stencilIds->GetNumberOfIds();
  if (this->PointIds!=NULL)
    {
    delete[] this->PointIds;
    this->PointIds = NULL;
    }
  this->PointIds = new vtkIdType[this->NPoints];
  memcpy(this->PointIds,stencilIds->GetPointer(0),this->NPoints*sizeof(vtkIdType));

  cellIds->Delete();
  ptIds->Delete();
  stencilIds->Delete();
}

