/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBranchUtilities.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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
  // .NAME vtkvmtkPolyDataBranchUtilities - ...
  // .SECTION Description
  // .

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

  static void GetGroupsIdList(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdList* groupIds);
  static void ExtractGroup(vtkPolyData* surface, const char* groupIdsArrayName, vtkIdType groupId, bool cleanGroupSurface, vtkPolyData* groupSurface);
  
protected:
  vtkvmtkPolyDataBranchUtilities() {};
  ~vtkvmtkPolyDataBranchUtilities() {};

private:
  vtkvmtkPolyDataBranchUtilities(const vtkvmtkPolyDataBranchUtilities&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBranchUtilities&);  // Not implemented.
};

#endif
