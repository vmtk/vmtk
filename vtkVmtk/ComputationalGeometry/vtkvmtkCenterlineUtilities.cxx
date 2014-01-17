/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineUtilities.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
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

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkPolyLine.h"
#include "vtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineUtilities);

vtkIdType vtkvmtkCenterlineUtilities::GetMaxGroupId(vtkPolyData* centerlines, const char* groupIdsArrayName)
{
  vtkDataArray* groupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);

  int numberOfCells = centerlines->GetNumberOfCells();
  vtkIdType maxGroupId = -1;

  int i;
  for (i=0; i<numberOfCells; i++)
    {
    vtkIdType groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (groupId > maxGroupId)
      {
      maxGroupId = groupId;
      }
    }

  return maxGroupId;
}

void vtkvmtkCenterlineUtilities::GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, int blanked, vtkIdList* groupIds)
{
  vtkDataArray* groupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);

  vtkDataArray* blankingArray = NULL;
  
  if (blankingArrayName)
  {          
    blankingArray = centerlines->GetCellData()->GetArray(blankingArrayName);
  }
  
  int numberOfCells = centerlines->GetNumberOfCells();

  groupIds->Initialize();

  int i;
  for (i=0; i<numberOfCells; i++)
    {
    vtkIdType groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));

    if (blankingArray)
    {
      vtkIdType current_blanked = static_cast<int>(blankingArray->GetComponent(i,0));
      if (blanked != current_blanked)
      {
        continue;
      }
    }
    groupIds->InsertUniqueId(groupId);
    }
}
 
void vtkvmtkCenterlineUtilities::GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdList* groupIds)
{
  vtkvmtkCenterlineUtilities::GetGroupsIdList(centerlines,groupIdsArrayName,NULL,0,groupIds);
}

void vtkvmtkCenterlineUtilities::GetNonBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds)
{
  vtkvmtkCenterlineUtilities::GetGroupsIdList(centerlines,groupIdsArrayName,blankingArrayName,0,groupIds);
}
  
void vtkvmtkCenterlineUtilities::GetBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds)
{
  vtkvmtkCenterlineUtilities::GetGroupsIdList(centerlines,groupIdsArrayName,blankingArrayName,1,groupIds);
}

void vtkvmtkCenterlineUtilities::GetGroupCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds)
{
  vtkDataArray* groupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);
  int numberOfCells = centerlines->GetNumberOfCells();

  groupCellIds->Initialize();
  
  int i;
  for (i=0; i<numberOfCells; i++)
  {
    vtkIdType current_groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (current_groupId != groupId)
    {
      continue;
    }
    groupCellIds->InsertNextId(i);
  }
}

void vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds)
{
  vtkDataArray* groupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);
  int numberOfCells = centerlines->GetNumberOfCells();

  groupCellIds->Initialize();

  int i;
  for (i=0; i<numberOfCells; i++)
    {
    vtkIdType current_groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (current_groupId != groupId)
      {
      continue;
      }
    int currentNumberOfGroupCellIds = groupCellIds->GetNumberOfIds();
    bool duplicate = false;
    int j;
    for (j=0; j<currentNumberOfGroupCellIds; j++)
      {
      double currentCellFirstPoint[3], currentCellLastPoint[3];
      double cellFirstPoint[3], cellLastPoint[3];
      vtkCell* cell = centerlines->GetCell(groupCellIds->GetId(j));
      cell->GetPoints()->GetPoint(0,currentCellFirstPoint);
      cell->GetPoints()->GetPoint(cell->GetNumberOfPoints()-1,currentCellLastPoint);
      cell = centerlines->GetCell(i);
      cell->GetPoints()->GetPoint(0,cellFirstPoint);
      cell->GetPoints()->GetPoint(cell->GetNumberOfPoints()-1,cellLastPoint);
      if ((vtkMath::Distance2BetweenPoints(currentCellFirstPoint,cellFirstPoint) < VTK_VMTK_DOUBLE_TOL) &&
          (vtkMath::Distance2BetweenPoints(currentCellLastPoint,cellLastPoint) < VTK_VMTK_DOUBLE_TOL))
        {
        duplicate = true;
        break;
        }
      }
    if (duplicate)
      {
      continue;
      }
    groupCellIds->InsertNextId(i);
    }
}

void vtkvmtkCenterlineUtilities::GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds)
{
  vtkDataArray* centerlineIdsArray = centerlines->GetCellData()->GetArray(centerlineIdsArrayName);
  int numberOfCells = centerlines->GetNumberOfCells();

  centerlineCellIds->Initialize();
  
  int i;
  for (i=0; i<numberOfCells; i++)
  {
    vtkIdType currentCenterlineId = static_cast<int>(centerlineIdsArray->GetComponent(i,0));
    if (currentCenterlineId != centerlineId)
    {
      continue;
    }
    centerlineCellIds->InsertNextId(i);
  }
}

void vtkvmtkCenterlineUtilities::GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds)
{
  vtkDataArray* centerlineIdsArray = centerlines->GetCellData()->GetArray(centerlineIdsArrayName);
  vtkDataArray* tractIdsArray = centerlines->GetCellData()->GetArray(tractIdsArrayName);
  int numberOfCells = centerlines->GetNumberOfCells();

  centerlineCellIds->Initialize();
  vtkIdList* centerlineTractIds = vtkIdList::New();
  
  int i;
  for (i=0; i<numberOfCells; i++)
  {
    vtkIdType currentCenterlineId = static_cast<int>(centerlineIdsArray->GetComponent(i,0));
    if (currentCenterlineId != centerlineId)
    {
      continue;
    }
    centerlineCellIds->InsertNextId(i);
    vtkIdType currectTractId = static_cast<int>(tractIdsArray->GetComponent(i,0));
    centerlineTractIds->InsertNextId(currectTractId);
  }

  int numberOfCenterlineCellIds = centerlineCellIds->GetNumberOfIds();
  
  bool done = false;
  while (!done)
    {
    done = true;
    for (i=0; i<numberOfCenterlineCellIds-1; i++)
      {
      if (centerlineTractIds->GetId(i) > centerlineTractIds->GetId(i+1))
        {
        vtkIdType tmp = centerlineTractIds->GetId(i+1);
        centerlineTractIds->SetId(i+1,centerlineTractIds->GetId(i));
        centerlineTractIds->SetId(i,tmp);
        tmp = centerlineCellIds->GetId(i+1);
        centerlineCellIds->SetId(i+1,centerlineCellIds->GetId(i));
        centerlineCellIds->SetId(i,tmp);
        done = false;
        }
      }
    }
  
  centerlineTractIds->Delete();
}

int vtkvmtkCenterlineUtilities::IsGroupBlanked(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdType groupId)
{
  vtkDataArray* groupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);
  int numberOfCells = centerlines->GetNumberOfCells();

  vtkIdType cellId = -1;
  int i;
  for (i=0; i<numberOfCells; i++)
  {
    vtkIdType current_groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (current_groupId != groupId)
    {
      continue;
    }
    cellId = i;
    break;
  }

  if (cellId == -1)
  {
    return -1;
  }
  
  return vtkvmtkCenterlineUtilities::IsCellBlanked(centerlines,blankingArrayName,cellId);
}
  
int vtkvmtkCenterlineUtilities::IsCellBlanked(vtkPolyData* centerlines, const char* blankingArrayName, vtkIdType cellId)
{
  vtkDataArray* blankingArray = centerlines->GetCellData()->GetArray(blankingArrayName);
  int blanking = static_cast<int>(blankingArray->GetComponent(cellId,0));
  int isBlanked = blanking == 1 ? 1 : 0;
  return isBlanked;
}
  
void vtkvmtkCenterlineUtilities::FindAdjacentCenterlineGroupIds(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType groupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds)
{
  upStreamGroupIds->Initialize();
  downStreamGroupIds->Initialize();

  vtkDataArray* centerlineGroupIdsArray = centerlines->GetCellData()->GetArray(groupIdsArrayName);
  vtkDataArray* centerlineIdsArray = centerlines->GetCellData()->GetArray(centerlineIdsArrayName);
  vtkDataArray* centerlineTractIdsArray = centerlines->GetCellData()->GetArray(tractIdsArrayName);

  int numberOfCenterlineCells = centerlines->GetNumberOfCells();

  int i;
  for (i=0; i<numberOfCenterlineCells; i++)
    {
    vtkIdType branchGroupId = static_cast<int>(centerlineGroupIdsArray->GetComponent(i,0));

    if (branchGroupId != groupId)
      {
      continue;
      }

    vtkCell* centerlineBranch = centerlines->GetCell(i);
    if (centerlineBranch->GetCellType() != VTK_LINE && centerlineBranch->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    vtkIdType branchCenterlineId = static_cast<int>(centerlineIdsArray->GetComponent(i,0));
    vtkIdType branchCenterlineTractId = static_cast<int>(centerlineTractIdsArray->GetComponent(i,0));

    int j;
    for (j=0; j<numberOfCenterlineCells; j++)
      {
      vtkIdType currentBranchCenterlineGroupId = static_cast<int>(centerlineGroupIdsArray->GetComponent(j,0));
      vtkIdType currentBranchCenterlineId = static_cast<int>(centerlineIdsArray->GetComponent(j,0));
      vtkIdType currentBranchCenterlineTractId = static_cast<int>(centerlineTractIdsArray->GetComponent(j,0));

      if (currentBranchCenterlineGroupId == branchGroupId)
        {
        continue;
        }

      if (currentBranchCenterlineId != branchCenterlineId)
        {
        continue;
        }

      //WARNING: this assumes that tract ids are adjacent
      if (currentBranchCenterlineTractId == branchCenterlineTractId - 1)
        {
        upStreamGroupIds->InsertUniqueId(currentBranchCenterlineGroupId);
        }

      if (currentBranchCenterlineTractId == branchCenterlineTractId + 1)
        {
        downStreamGroupIds->InsertUniqueId(currentBranchCenterlineGroupId);
        }
      }
    }
}

void vtkvmtkCenterlineUtilities::InterpolatePoint(vtkPolyData* centerlines, int cellId, int subId, double pcoord, double interpolatedPoint[3])
{
  double point0[3], point1[3];
  vtkPoints* centerlineCellPoints = centerlines->GetCell(cellId)->GetPoints();
  centerlineCellPoints->GetPoint(subId,point0);
  centerlineCellPoints->GetPoint(subId+1,point1);
      
  interpolatedPoint[0] = (1.0 - pcoord) * point0[0] + pcoord * point1[0];
  interpolatedPoint[1] = (1.0 - pcoord) * point0[1] + pcoord * point1[1];
  interpolatedPoint[2] = (1.0 - pcoord) * point0[2] + pcoord * point1[2];
}

void vtkvmtkCenterlineUtilities::InterpolateTuple(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double* interpolatedTuple)
{
  vtkDataArray* array = centerlines->GetPointData()->GetArray(arrayName);
  int numberOfComponents = array->GetNumberOfComponents();
  vtkIdType pointId0 = centerlines->GetCell(cellId)->GetPointId(subId);
  vtkIdType pointId1 = centerlines->GetCell(cellId)->GetPointId(subId+1);
  double* tuple0 = array->GetTuple(pointId0);
  double* tuple1 = array->GetTuple(pointId1);

  int i;
  for (i=0; i<numberOfComponents; i++)
  {
    interpolatedTuple[i] = (1.0 - pcoord) * tuple0[i] + pcoord * tuple1[i];
  }
}

void vtkvmtkCenterlineUtilities::FindMergingPoints(vtkPolyData* centerlines, vtkPoints* mergingPoints, double tolerance=1E-8)
{
  mergingPoints->Initialize();
  int numberOfCells = centerlines->GetNumberOfCells();
  int i, j, k;
  for (i=0; i<numberOfCells; i++)
    {
    vtkCell* centerline = centerlines->GetCell(i);
    int numberOfCenterlinePoints = centerline->GetNumberOfPoints();
    int previousMergeStatus = -1;
    int mergeStatus = -1;
    for (j=0; j<numberOfCenterlinePoints; j++)
      {
      double point[3];
      centerline->GetPoints()->GetPoint(j,point);
      for (k=i+1; k<numberOfCells; k++)
        {
        vtkCell* currentCenterline = centerlines->GetCell(k);
        int numberOfCurrentCenterlinePoints = currentCenterline->GetNumberOfPoints();
        double closestPoint[3];
        int subId;
        double pcoords[3];
        double dist2;
        double* weights = new double[numberOfCurrentCenterlinePoints];
        currentCenterline->EvaluatePosition(point,closestPoint,subId,pcoords,dist2,weights);
        if (dist2 < tolerance*tolerance)
          {
          mergeStatus = 1;
          }
        else
          {
          mergeStatus = 0;
          }
        if (j>0 && mergeStatus != previousMergeStatus)
          {
          mergingPoints->InsertNextPoint(point);
          previousMergeStatus = mergeStatus;
          }
        delete[] weights;
        }
      } 
    }
}

