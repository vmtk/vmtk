/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineUtilities.h,v $
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
  // .NAME vtkvmtkCenterlineUtilities - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkCenterlineUtilities_h
#define __vtkvmtkCenterlineUtilities_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkPoints;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineUtilities : public vtkObject
{
public: 
  vtkTypeMacro(vtkvmtkCenterlineUtilities,vtkObject);
  static vtkvmtkCenterlineUtilities* New(); 

  static vtkIdType GetMaxGroupId(vtkPolyData* centerlines, const char* groupIdsArrayName);
  
  static void GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdList* groupIds);
  
  static void GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, int blanked, vtkIdList* groupIds);
  
  static void GetNonBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds);
  
  static void GetBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds);

  static void GetGroupCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds);
  
  static void GetGroupUniqueCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds);

  static void GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds);
  
  static void GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds);

  static int IsGroupBlanked(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdType groupId);
  
  static int IsCellBlanked(vtkPolyData* centerlines, const char* blankingArrayName, vtkIdType cellId);
  
  static void FindAdjacentCenterlineGroupIds(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType groupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds);

  static void InterpolatePoint(vtkPolyData* centerlines, int cellId, int subId, double pcoord, double interpolatedPoint[3]);

  static void InterpolateTuple(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double* interpolatedTuple);

  static void InterpolateTuple1(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double& interpolatedTuple1)
  {
    InterpolateTuple(centerlines,arrayName,cellId,subId,pcoord,&interpolatedTuple1);
  }
 
  static void InterpolateTuple3(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double interpolatedTuple3[3])
  {
    InterpolateTuple(centerlines,arrayName,cellId,subId,pcoord,interpolatedTuple3);
  }
 
  static void FindMergingPoints(vtkPolyData* centerlines, vtkPoints* mergingPoints, double tolerance);

protected:
  vtkvmtkCenterlineUtilities() {};
  ~vtkvmtkCenterlineUtilities() {};

private:
  vtkvmtkCenterlineUtilities(const vtkvmtkCenterlineUtilities&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineUtilities&);  // Not implemented.
};

#endif
