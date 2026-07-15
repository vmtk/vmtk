/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkPolyDataBranchUtilities
 * @brief   Utility functions to ease working with branches and surfaces. 
 * @ingroup ComputationalGeometry
 *
 * - ExtractGroup: Extract a single surface branch group from a surface which has already been grouped.
 * - GetGroupIdsList: get the group ids which are contained within a grouped surface as a vtkIdList.
 */

#ifndef __vtkvmtkPolyDataBranchUtilities_h
#define __vtkvmtkPolyDataBranchUtilities_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBranchUtilities : public vtkObject
{
public: 
  vtkTypeMacro(vtkvmtkPolyDataBranchUtilities,vtkObject);
  static vtkvmtkPolyDataBranchUtilities* New();

  /**
   * Fill groupIds with the list of all distinct group ids found in the groupIdsArrayName cell data
   * array of surface.
   */
  static void GetGroupsIdList(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdList* groupIds);

  /**
   * Extract into groupSurface the subset of cells of surface belonging to groupId (as identified by
   * the groupIdsArrayName cell data array). If cleanGroupSurface is true, unused points are removed
   * (vtkCleanPolyData) from the extracted group.
   */
  static void ExtractGroup(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdType groupId, bool cleanGroupSurface, vtkPolyData* groupSurface);
  
protected:
  vtkvmtkPolyDataBranchUtilities() {};
  ~vtkvmtkPolyDataBranchUtilities() {};

private:
  vtkvmtkPolyDataBranchUtilities(const vtkvmtkPolyDataBranchUtilities&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBranchUtilities&);  // Not implemented.
};

#endif
