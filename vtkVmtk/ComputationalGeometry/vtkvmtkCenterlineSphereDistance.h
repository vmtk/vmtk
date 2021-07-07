/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSphereDistance.h,v $
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
// .NAME vtkvmtkCenterlineSphereDistance - Collection of functions which compute point ids which are n-spheres away from a reference point.
// .SECTION Description
// Allows us to find points which are n-touching spheres upstream or downstream from a reference point. In this description, "touching sphere" refers to the the points on the centerline (one upstream, one downstream) which lie closest to ("touch") the surface of a sphere whose barycenter is located at the reference points location. In practice, this means that we use the centerline's MaximumInscribedSphereRadius point data as the "sphere" radius/surface. 
//
// This metric is used because it is a well behaved way to normalize the comparison of distance as vessel radius scales. 

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 
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
