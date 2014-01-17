/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataNeighborhood.cxx,v $
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

#include "vtkvmtkPolyDataNeighborhood.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"


vtkStandardNewMacro(vtkvmtkPolyDataNeighborhood);

void vtkvmtkPolyDataNeighborhood::Build()
{
  vtkIdType i, j;
  vtkIdType numCellsInStencil;
  vtkIdList *cellIds, *ptIds, *stencilIds;
  vtkPolyData* pdata = vtkPolyData::SafeDownCast(this->DataSet);

  if (pdata==NULL)
    {
    vtkErrorMacro(<< "Input data NULL or not poly data");
    }

  vtkIdType pointId = this->DataSetPointId;

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

  vtkIdType cellPointId;
  for (i=0; i<numCellsInStencil; i++)
    {
    pdata->GetCellPoints(cellIds->GetId(i),ptIds);
    for (j=0; j<ptIds->GetNumberOfIds(); j++)
      {
      cellPointId = ptIds->GetId(j);
      if (cellPointId != pointId)
        {
        stencilIds->InsertUniqueId(ptIds->GetId(j));
        }
      }
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

