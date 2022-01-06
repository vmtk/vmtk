/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBranchUtilities.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataBranchUtilities.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataBranchUtilities);

void vtkvmtkPolyDataBranchUtilities::GetGroupsIdList(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdList* groupIds)
{
  vtkDataArray* groupIdsArray = surface->GetPointData()->GetArray(groupIdsArrayName);

  int numberOfPoints = surface->GetNumberOfPoints();

  groupIds->Initialize();

  int maxGroupId = 0;
  int i;
  for (i=0; i<numberOfPoints; i++)
    {
    vtkIdType groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (groupId > maxGroupId)
      {
      maxGroupId = groupId;
      }
    }

  vtkIdList* isGroupList = vtkIdList::New();
  isGroupList->SetNumberOfIds(maxGroupId+1);

  for (i=0; i<maxGroupId+1; i++)
  {
    isGroupList->SetId(i,0);
  }

  for (i=0; i<numberOfPoints; i++)
    {
    vtkIdType groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    isGroupList->SetId(groupId,1);
    }
  
  for (i=0; i<maxGroupId+1; i++)
  {
    int isGroup = isGroupList->GetId(i);
    if (isGroup == 1)
      {
      groupIds->InsertNextId(i);
      }
  }

  isGroupList->Delete();
}

void vtkvmtkPolyDataBranchUtilities::ExtractGroup(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdType groupId, bool cleanGroupSurface, vtkPolyData* groupSurface)
{
  groupSurface->DeepCopy(surface);

  int numberOfCells = surface->GetPolys()->GetNumberOfCells();

  vtkDataArray* groupIdsArray = surface->GetPointData()->GetArray(groupIdsArrayName);

  surface->BuildCells();

  vtkCellArray* polys = groupSurface->GetPolys();
  polys->Reset();
  vtkIdType npts;
  const vtkIdType *pts;
  for (int j=0; j<numberOfCells; j++)
    {
    surface->GetCellPoints(j,npts,pts);
    bool insertCell = true;
    for (int k=0; k<npts; k++)
      {
      if (static_cast<int>(groupIdsArray->GetComponent(pts[k],0) != groupId))
        {
        insertCell = false;
        break;
        }
      }
    if (!insertCell)
      {
      continue;
      }
    polys->InsertNextCell(npts,pts);
    }
  polys->Squeeze();
  
  if (cleanGroupSurface)
    {
    vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
    cleaner->SetInputData(groupSurface);
    cleaner->Update();

    groupSurface->DeepCopy(cleaner->GetOutput());

    cleaner->Delete();
    }
}
