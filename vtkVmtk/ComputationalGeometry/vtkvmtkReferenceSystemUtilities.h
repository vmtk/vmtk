/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkReferenceSystemUtilities.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkReferenceSystemUtilities - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkReferenceSystemUtilities_h
#define __vtkvmtkReferenceSystemUtilities_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkReferenceSystemUtilities : public vtkObject
{
public: 
  vtkTypeMacro(vtkvmtkReferenceSystemUtilities,vtkObject);
  static vtkvmtkReferenceSystemUtilities* New();

  static vtkIdType GetReferenceSystemPointId(vtkPolyData* referenceSystems, const char* groupIdsArrayName, vtkIdType groupId);
  
protected:
  vtkvmtkReferenceSystemUtilities() {};
  ~vtkvmtkReferenceSystemUtilities() {};

private:
  vtkvmtkReferenceSystemUtilities(const vtkvmtkReferenceSystemUtilities&);  // Not implemented.
  void operator=(const vtkvmtkReferenceSystemUtilities&);  // Not implemented.
};

#endif
