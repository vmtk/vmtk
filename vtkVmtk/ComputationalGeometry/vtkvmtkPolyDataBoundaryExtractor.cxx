/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBoundaryExtractor.cxx,v $
Language:  C++
Date:      $Date: 2006/07/27 08:27:26 $
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

  vtkIdList *boundary, *boundaryIds, *cellEdgeNeighbors, *newCell;
  vtkIdType i, j, currentId, id, id0, id1;
  vtkCell* cell;
  bool foundAny, foundNeighbor, done;
  vtkPoints* newPoints;
  vtkCellArray* newLines;
  vtkIdTypeArray* newScalars;

  // The topology queries below need cell links, but they must not be built on the input: a
  // filter does not own its input, and since VTK 9.4 the links hold a reference back to the
  // dataset they were built for. That reference cycle is normally reclaimed by VTK's garbage
  // collector, but when it is created while another filter's Update() is executing - this
  // extractor runs inside vtkvmtkPolyDataSurfaceRemeshing, among others - the collector never
  // reclaims it, and the caller's surface is leaked together with the links, its cell map and
  // its cell instances. The links are built instead on a shallow structural copy, which shares
  // the points and cell arrays (so every point and cell id below means the same thing) but
  // keeps the acceleration structures, and their reference cycle, to itself.
  vtkPolyData* working = vtkPolyData::New();
  working->CopyStructure(input);
  working->BuildCells();
  working->BuildLinks();

  // Allocate
  boundary = vtkIdList::New();
  boundaryIds = vtkIdList::New();
  cellEdgeNeighbors = vtkIdList::New();
  newCell = vtkIdList::New();
  newPoints = vtkPoints::New();
  newLines = vtkCellArray::New();
  newScalars = vtkIdTypeArray::New();

  // Execute  
  for (i=0; i<working->GetNumberOfCells(); i++)
    {
    cell = working->GetCell(i);
    for (j=0; j<3; j++)
      {
      cellEdgeNeighbors->Initialize();
      auto edge = cell->GetEdge(j);
      if (!edge) continue;
      auto pointIds = edge->GetPointIds();
      id0 = pointIds->GetId(0);
      id1 = pointIds->GetId(1);
      working->GetCellEdgeNeighbors(i,id0,id1,cellEdgeNeighbors);
      if (cellEdgeNeighbors->GetNumberOfIds()==0)
        {
        boundaryIds->InsertUniqueId(id0);
        boundaryIds->InsertUniqueId(id1);
        }
      }
    }

  if (boundaryIds->GetNumberOfIds() == 0)
    {
    output->SetPoints(newPoints);
    output->SetLines(newLines);
    output->GetPointData()->SetScalars(newScalars);
  
    newPoints->Delete();
    newLines->Delete();
    newScalars->Delete();
  
    boundary->Delete();
    boundaryIds->Delete();
    cellEdgeNeighbors->Delete();
    newCell->Delete();

    // Dropping the links first breaks their reference cycle with the working copy by plain
    // reference counting, so its destruction cannot depend on the garbage collector either.
    working->Initialize();
    working->Delete();

    return 1;
    }

  foundAny = false;
  foundNeighbor = false;
  done = false;
  currentId = -1;
  int loopCount = 0;
  bool isBoundaryEdge;

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
        isBoundaryEdge = false;
        if (currentId!=-1 && working->IsEdge(currentId,id))
          {
          cellEdgeNeighbors->Initialize();
          working->GetCellEdgeNeighbors(-1,currentId,id,cellEdgeNeighbors);
          if (cellEdgeNeighbors->GetNumberOfIds() == 1)
            {
            isBoundaryEdge = true;
            }
          }
        if ((currentId==-1) || isBoundaryEdge)
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
      // Each point of the boundary is listed once. The polyline is a ring, and the caller closes
      // it, which is what every caller in vmtk does: it takes the number of points of the cell
      // and steps round with (j+1) modulo that. Repeating the first point at the end to mark the
      // ring as closed would put a duplicate in the middle of that arithmetic, which biases the
      // barycenter, the mean radius and the normal computed from these points, and leaves a
      // degenerate triangle in the fan of a cap and in the first layer of a flow extension.
      newCell->Initialize();
      newCell->SetNumberOfIds(boundary->GetNumberOfIds());
      for (j=0; j<boundary->GetNumberOfIds(); j++)
        {
        id = boundary->GetId(j);
        newCell->SetId(j,newPoints->InsertNextPoint(working->GetPoint(id)));
        newScalars->InsertNextValue(id);
        }

      newLines->InsertNextCell(newCell);

      currentId = -1;
      boundary->Initialize();
      }
        
    if (!foundAny)
      {
      done = true;
      }

    loopCount++;
    if (loopCount > 2*boundaryIds->GetNumberOfIds())
      {
      int missing = 0;
      for (int n=0; n<boundaryIds->GetNumberOfIds(); n++)
        {
        if (boundaryIds->GetId(n) != -1)
          {
          missing++;
          }
        }
      vtkErrorMacro(<<"Can't find adjacent point. Bailing out."); 
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

  // Dropping the links first breaks their reference cycle with the working copy by plain
  // reference counting, so its destruction cannot depend on the garbage collector either.
  working->Initialize();
  working->Delete();

  return 1;
}

void vtkvmtkPolyDataBoundaryExtractor::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
