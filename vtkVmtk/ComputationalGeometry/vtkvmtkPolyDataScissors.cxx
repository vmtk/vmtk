/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataScissors.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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

#include "vtkvmtkPolyDataScissors.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIntArray.h"
#include "vtkPolyLine.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataScissors);

vtkvmtkPolyDataScissors::vtkvmtkPolyDataScissors()
{
  this->CutLine = NULL;
  this->CutLinePointIdsArrayName = NULL;
}

vtkvmtkPolyDataScissors::~vtkvmtkPolyDataScissors()
{
  if (this->CutLine)
    {
    this->CutLine->Delete();
    this->CutLine = NULL;
    }
}

int vtkvmtkPolyDataScissors::IsEdgeInCell(vtkPolyData *input, vtkIdType edgePointId0, vtkIdType edgePointId1, vtkIdType cellId)
{
  vtkIdType npts, *pts;
  vtkIdType i;

  input->GetCellPoints(cellId,npts,pts);

  for (i=0; i<npts; i++)
    {
    if ((pts[i]==edgePointId0 && pts[(i+1)%npts]==edgePointId1) || (pts[i]==edgePointId1 && pts[(i+1)%npts]==edgePointId0))
      {
      return 1;
      }
    }

  return 0;
}

int vtkvmtkPolyDataScissors::GetCellsOnSameSide(vtkPolyData* input, vtkIdType targetCellId0, vtkIdType targetCellId1, vtkIdType referenceCellId, vtkIdType linePointId0, vtkIdType linePointId1, vtkIdType linePointId2, vtkIdList *cellsOnSameSide)
{
  vtkIdType i, j;
  unsigned short ncells;
  vtkIdType *cells;
  vtkIdType previousCellId;
  vtkIdType npts, *pts;
  vtkIdType edge[2];
  bool done;

  if (cellsOnSameSide->GetNumberOfIds()>0)
    {
    cellsOnSameSide->Initialize();
    }
    
  input->GetPointCells(linePointId1,ncells,cells);

  edge[0] = linePointId1;
  
  input->GetCellPoints(referenceCellId,npts,pts);

  edge[1] = -1;

  for (i=0; i<npts; i++)
    {
    if (pts[i]!=linePointId1 && pts[i]!=linePointId0)
      {
      edge[1] = pts[i];
      break;
      }
    }

  if (edge[1]==-1)
    {
    vtkErrorMacro(<<"Bad request for CellsOnSameSide of CutLine. Not an edge.");
    return -1;
    }

  previousCellId = referenceCellId;

  done = false;

  while(!done)
    {
    done = true;
    for (i=0; i<ncells; i++)
      {
      if (cells[i]==previousCellId)
        continue;

      if (this->IsEdgeInCell(input,edge[0],edge[1],cells[i]))
        {
        if ((cells[i]==targetCellId0)||(cells[i]==targetCellId1))
          {
          cellsOnSameSide->InsertNextId(cells[i]);
          return 1;
          }

        done = false;

        cellsOnSameSide->InsertNextId(cells[i]);
        previousCellId = cells[i];

        input->GetCellPoints(cells[i],npts,pts);
        for (j=0; j<npts; j++)
          {
          if (pts[j]!=edge[0] && pts[j]!=edge[1])
            {
            edge[1] = pts[j];
            break;
            }
          }

        break;
        }
      }
    }

  return 0;
}

int vtkvmtkPolyDataScissors::RequestData(
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

  vtkIdType k, i , j;
  vtkIdType surfacePointId, newPointId;
  vtkIdType id0, id1, previousId;
  vtkIdType duplicatePointLocation;
  vtkIdType numberOfLines, numberOfLinePoints, numberOfInputPoints;
  vtkIdType cellId, previousCellToEdit;
  vtkIdType npts, *pts;
  vtkDataArray *cutLinePointIdsArray;
  vtkCell *polyLine;
  vtkPoints *newPoints;
  vtkCellArray *newPolys;
  vtkCellArray *editedCells;
  vtkIdList *duplicatePointIds;
  vtkIdList *duplicatedPointIds;
  vtkIdList *cellsToEdit, *editedCellIds;
  vtkIdList *cellEdgeNeighbors;
  vtkIdList *cellsOnSameSide;
  vtkPointData *inputPD = input->GetPointData();
  vtkPointData *outputPD = output->GetPointData();

  previousCellToEdit = -1;
  
  if (this->CutLine==NULL)
    {
    vtkErrorMacro(<<"CutLine not specified!");
    return 1;
    }

  if (this->CutLinePointIdsArrayName==NULL)
    {
    vtkErrorMacro(<<"CutLinePointIdsArrayName not specified!");
    return 1;
    }

  cutLinePointIdsArray = this->CutLine->GetPointData()->GetArray(this->CutLinePointIdsArrayName);

  if (cutLinePointIdsArray==NULL)
    {
    vtkErrorMacro(<<"CutLinePointIdsArray with name specified does not exist!");
    return 1;
    }

  newPoints = vtkPoints::New();
  newPolys = vtkCellArray::New();

  editedCells = vtkCellArray::New();

  duplicatePointIds = vtkIdList::New();
  duplicatedPointIds = vtkIdList::New();
  cellsToEdit = vtkIdList::New();
  editedCellIds = vtkIdList::New();
  cellEdgeNeighbors = vtkIdList::New();
  cellsOnSameSide = vtkIdList::New();

  numberOfInputPoints = input->GetNumberOfPoints();
  newPoints->DeepCopy(input->GetPoints());

  numberOfLinePoints = this->CutLine->GetNumberOfPoints();

  numberOfLines = this->CutLine->GetNumberOfCells();

  input->BuildCells();
  input->BuildLinks();
  this->CutLine->BuildCells();

  for (k=0; k<numberOfLines; k++)
    {
    polyLine = this->CutLine->GetCell(k);
    if (polyLine->GetCellType() != VTK_LINE && polyLine->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    numberOfLinePoints = polyLine->GetNumberOfPoints();

    duplicatedPointIds->Initialize();
    duplicatePointIds->Initialize();
    cellsToEdit->Initialize();

    for (i=0; i<numberOfLinePoints; i++)
      {
      surfacePointId = static_cast<vtkIdType>(cutLinePointIdsArray->GetComponent(polyLine->GetPointId(i),0));
      newPointId = newPoints->InsertNextPoint(input->GetPoint(surfacePointId));
      duplicatedPointIds->InsertNextId(surfacePointId);
      duplicatePointIds->InsertNextId(newPointId);
      }

    for (i=0; i<numberOfLinePoints-1; i++)
      {
      id0 = static_cast<vtkIdType>(cutLinePointIdsArray->GetComponent(polyLine->GetPointId(i),0));
      id1 = static_cast<vtkIdType>(cutLinePointIdsArray->GetComponent(polyLine->GetPointId(i+1),0));

      if (id0==id1)
        {
        vtkErrorMacro(<<"Coincident points found on CutLine! ");
        return 1;
        }

      if (!input->IsEdge(id0,id1))
        {
        vtkErrorMacro(<<"CutLine is not entirely defined on mesh edges! ");
        return 1;
        }

      cellEdgeNeighbors->Initialize();
      input->GetCellEdgeNeighbors(-1,id0,id1,cellEdgeNeighbors);

      if (cellEdgeNeighbors->GetNumberOfIds()<2)
        {
        vtkWarningMacro(<<"Surface boundary or hole found.");
        continue;
        }

      if (cellEdgeNeighbors->GetNumberOfIds()>2)
        {
        vtkErrorMacro(<<"Surface is non-manifold! ");
        return 1;
        }

      // try to choose the first cell in order to maintain consistency in the orientation, if at all possible

      // this is a bit too naive (it basically doesn't work most of the times). TODO: special treatment of joints + consistency.
      if (i==0)
        {
        if (k==0)
          {
          cellsToEdit->InsertUniqueId(cellEdgeNeighbors->GetId(0));
          previousCellToEdit = cellEdgeNeighbors->GetId(0);
          }
        else
          {
          if (cellsToEdit->IsId(cellEdgeNeighbors->GetId(0))!=-1)
            {
            cellsToEdit->InsertUniqueId(cellEdgeNeighbors->GetId(0));
            previousCellToEdit = cellEdgeNeighbors->GetId(0);
            }
          else
            {
            cellsToEdit->InsertUniqueId(cellEdgeNeighbors->GetId(1));
            previousCellToEdit = cellEdgeNeighbors->GetId(1);
            }
          }
        continue;
        }

      previousId = static_cast<vtkIdType>(cutLinePointIdsArray->GetComponent(polyLine->GetPointId(i-1),0));

      // select the cells lying on the same side of the cut line.

      cellsOnSameSide->Initialize();
      if (this->GetCellsOnSameSide(input,cellEdgeNeighbors->GetId(0),cellEdgeNeighbors->GetId(1),previousCellToEdit,previousId,id0,id1,cellsOnSameSide))
        {
        for (j=0; j<cellsOnSameSide->GetNumberOfIds(); j++)
          {
          cellsToEdit->InsertUniqueId(cellsOnSameSide->GetId(j));
          }
        previousCellToEdit = cellsOnSameSide->GetId(cellsOnSameSide->GetNumberOfIds()-1);
        }
      else
        {
        // well, we could try to, going the other way and getting the complementary cells, but there's no time now (this is a TODO)
        vtkErrorMacro(<<"Hole in the surface found. Can't determine line orientation.");
        return 1;
        }
      }

    for (i=0; i<cellsToEdit->GetNumberOfIds(); i++)
      {
      cellId = cellsToEdit->GetId(i);
      input->GetCellPoints(cellId,npts,pts);

      editedCellIds->InsertUniqueId(cellId);
      editedCells->InsertNextCell(npts);
      for (j=0; j<npts; j++)
        {
        duplicatePointLocation = duplicatedPointIds->IsId(pts[j]);
        if (duplicatePointLocation!=-1)
          {
          editedCells->InsertCellPoint(duplicatePointIds->GetId(duplicatePointLocation));
          }
        else
          {
          editedCells->InsertCellPoint(pts[j]);
          }
        }
      }
    }

  input->GetPolys()->InitTraversal();
  for (i=0; i<input->GetPolys()->GetNumberOfCells(); i++)
    {
    input->GetPolys()->GetNextCell(npts,pts);
    if (editedCellIds->IsId(i)==-1)
      {
      newPolys->InsertNextCell(npts,pts);
      }
    }

  editedCells->InitTraversal();
  for (i=0; i<editedCells->GetNumberOfCells(); i++)
    {
    editedCells->GetNextCell(npts,pts);
    newPolys->InsertNextCell(npts,pts);
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  outputPD->CopyAllocate(inputPD,numberOfInputPoints+numberOfLinePoints);

  for (i=0; i<numberOfInputPoints; i++)
    {
    outputPD->CopyData(inputPD,i,i);
    }

  for (i=0; i<duplicatedPointIds->GetNumberOfIds(); i++)
    {
    outputPD->CopyData(inputPD,duplicatedPointIds->GetId(i),duplicatePointIds->GetId(i));
    }

  duplicatePointIds->Delete();
  duplicatedPointIds->Delete();
  cellsToEdit->Delete();
  cellEdgeNeighbors->Delete();
  cellsOnSameSide->Delete();
  editedCells->Delete();
  editedCellIds->Delete();
  newPoints->Delete();
  newPolys->Delete();

  return 1;
}

void vtkvmtkPolyDataScissors::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
