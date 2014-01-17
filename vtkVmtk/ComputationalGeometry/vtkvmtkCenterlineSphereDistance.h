/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSphereDistance.h,v $
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
  // .NAME vtkvmtkCenterlineSphereDistance - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineSphereDistance_h
#define __vtkvmtkCenterlineSphereDistance_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSphereDistance : public vtkObject
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSphereDistance,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent); 
  static vtkvmtkCenterlineSphereDistance* New();

  static void FindTouchingSphereCenter(vtkPolyData* centerlines, const char* radiusArrayName, vtkIdType cellId, vtkIdType subId, double pcoord, vtkIdType& touchingSubId, double& touchingPCoord, bool forward=true);

  static void FindNTouchingSphereCenter(vtkPolyData* centerlines, const char* radiusArrayName, vtkIdType cellId, vtkIdType subId, double pcoord, int numberOfTouchingSpheres, vtkIdType& touchingSubId, double& touchingPCoord, bool forward=true);

  protected:
  vtkvmtkCenterlineSphereDistance() {};
  ~vtkvmtkCenterlineSphereDistance() {};  

  private:
  vtkvmtkCenterlineSphereDistance(const vtkvmtkCenterlineSphereDistance&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSphereDistance&);  // Not implemented.
};

#endif
