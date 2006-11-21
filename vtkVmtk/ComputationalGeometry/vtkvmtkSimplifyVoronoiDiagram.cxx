/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSimplifyVoronoiDiagram.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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

#include "vtkvmtkSimplifyVoronoiDiagram.h"
#include "vtkCellArray.h"
#include "vtkCellTypes.h"
#include "vtkCellLinks.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkSimplifyVoronoiDiagram, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkSimplifyVoronoiDiagram);

vtkvmtkSimplifyVoronoiDiagram::vtkvmtkSimplifyVoronoiDiagram()
{
  this->UnremovablePointIds = NULL;
  this->UnremovableCellIds = NULL;
  this->Simplification = VTK_VMTK_REMOVE_BOUNDARY_POINTS;
  this->IncludeUnremovable = 1;
}

vtkvmtkSimplifyVoronoiDiagram::~vtkvmtkSimplifyVoronoiDiagram()
{
  if (this->UnremovablePointIds)
    {
    this->UnremovablePointIds->Delete();
    this->UnremovablePointIds = NULL;
    }

  if (this->UnremovableCellIds)
    {
    this->UnremovableCellIds->Delete();
    this->UnremovableCellIds = NULL;
    }
}

vtkIdType vtkvmtkSimplifyVoronoiDiagram::IsBoundaryEdge(vtkCellLinks* links, vtkIdType* edge)
{
  vtkIdType j, k;
  vtkIdType cellId, ncells0, *cells0, ncells1, *cells1;

  ncells0 = links->GetNcells(edge[0]);
  cells0 = links->GetCells(edge[0]);
  ncells1 = links->GetNcells(edge[1]);
  cells1 = links->GetCells(edge[1]);

  cellId = -1;

  for (j=0; j<ncells0; j++)
    {
    for (k=0; k<ncells1; k++)
      {
      if (cells0[j]==cells1[k])
        {
        if (cellId==-1)
          {
          cellId = cells0[j];
          }
        else if (cells0[j]!=cellId)
          {
          return 0;
          }
        }
      }
    }

  if (cellId==-1)
    return -1;

  return 1;
}

int vtkvmtkSimplifyVoronoiDiagram::RequestData(
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
  bool anyRemoved, removeCell, considerPoint;
  bool* isUnremovable;
  vtkIdType i, j, id;
  vtkIdType n;
  vtkIdType npts, *pts, ncells;
  vtkIdType edge[2];
  vtkCellArray *newPolys, *currentPolys;
  vtkCellLinks* currentLinks;
  vtkIdList* newCell;
  vtkIdType newCellId;
  vtkCellArray* inputPolys = input->GetPolys();

  // Allocate

  newPolys = vtkCellArray::New();
  currentPolys = vtkCellArray::New();
  currentLinks = vtkCellLinks::New();
  newCell = vtkIdList::New();

  n = 0;
  if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_POINTS)
    {
    n = input->GetNumberOfPoints();
    }
  else if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_CELLS)
    {
    n = inputPolys->GetNumberOfCells();
    }

  isUnremovable = new bool[n];

  // Execute 

  for (i=0; i<n; i++)
    {
    isUnremovable[i] = false;
    }

  if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_POINTS)
    {
    if (this->UnremovablePointIds)
      {
      for (i=0; i<this->UnremovablePointIds->GetNumberOfIds(); i++)
        {
        id = this->UnremovablePointIds->GetId(i);
        if ((id >= 0) && (id < n))
          isUnremovable[id] = true;
        else 
          vtkErrorMacro(<< "Out of range id found among UnremovablePointIds!");
        }
      }
    }
  else if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_CELLS)
    {
    if (this->UnremovableCellIds)
      {
      for (i=0; i<this->UnremovableCellIds->GetNumberOfIds(); i++)
        {
        id = this->UnremovableCellIds->GetId(i);
        if ((id >= 0) && (id < n))
          isUnremovable[id] = true;
        else if (id<0)
          vtkErrorMacro(<< "Out of range id found among UnremovableCellIds!");
        }
      }
    }

  currentPolys->DeepCopy(inputPolys);

  currentLinks->Allocate(input->GetNumberOfPoints());
  currentLinks->BuildLinks(input,currentPolys);

  anyRemoved = true;
  while (anyRemoved)
    {
    anyRemoved = false;
    newPolys->Reset();
    currentPolys->InitTraversal();
    for (i=0; i<currentPolys->GetNumberOfCells(); i++)
      {
      currentPolys->GetNextCell(npts,pts);
      
      if (npts==0)
        {
        newPolys->InsertNextCell(npts,pts);
        continue;
        }

      newCell->Initialize();
      if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_POINTS)
        {
        for (j=0; j<npts; j++)
          {
          considerPoint = false;
          ncells = currentLinks->GetNcells(pts[j]);
        
          if (ncells==1)
            considerPoint = true;

          if (considerPoint)
            {
            if (isUnremovable[pts[j]])
              newCell->InsertNextId(pts[j]);
            else
              anyRemoved = true;
            }
          else
            {
            newCell->InsertNextId(pts[j]);
            }
          }

        if (newCell->GetNumberOfIds()<3)
          newCell->Initialize();
        }
      else if (this->Simplification==VTK_VMTK_REMOVE_BOUNDARY_CELLS)
        {
        removeCell = false;
        if (!isUnremovable[i])
          {
          for (j=0; j<npts; j++)
            {
            edge[0] = pts[j];
            edge[1] = pts[(j+1)%npts];

            if (this->IsBoundaryEdge(currentLinks,edge)>0)
              {
              removeCell = true;
              break;
              }
            }
          }

        if (removeCell)
          {
          anyRemoved = true;
          }
        else
          {
          for (j=0; j<npts; j++)
            newCell->InsertNextId(pts[j]);
          }
        }
      
      newCellId = newPolys->InsertNextCell(newCell);
      }

    currentPolys->DeepCopy(newPolys);
    currentLinks->Delete();
    currentLinks = vtkCellLinks::New();
    currentLinks->Allocate(input->GetNumberOfPoints());
    currentLinks->BuildLinks(input,currentPolys);
    }

  if (!this->IncludeUnremovable)
    {
    currentPolys->DeepCopy(newPolys);
    newPolys->Reset();
    currentPolys->InitTraversal();
    for (i=0; i<currentPolys->GetNumberOfCells(); i++)
      {
      currentPolys->GetNextCell(npts,pts);
      newCell->Initialize();
      if (npts==0)
        {
        newPolys->InsertNextCell(npts,pts);
        continue;
        }
      if (!isUnremovable[i])
        {
        for (j=0; j<npts; j++)
          newCell->InsertNextId(pts[j]);
        }
      newPolys->InsertNextCell(newCell);
      }
    currentPolys->DeepCopy(newPolys);
    }

  // simply passes points and point data (eventually vtkCleanPolyData)
  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());

  // WARNING: cell data are thrown away in the current version
  output->SetPolys(currentPolys);

  // Destroy
  newPolys->Delete();
  currentLinks->Delete();
  currentPolys->Delete();
  newCell->Delete();
  delete[] isUnremovable;

  return 1;
}

void vtkvmtkSimplifyVoronoiDiagram::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
