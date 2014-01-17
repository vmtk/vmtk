/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkTMath.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkMath - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkMath_h
#define __vtkvmtkMath_h

#include "vtkObject.h"
#include "vtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkWin32Header.h"

#define VTK_VMTK_NON_OBTUSE 0
#define VTK_VMTK_OBTUSE_IN_POINT 1
#define VTK_VMTK_OBTUSE_NOT_IN_POINT 2

class VTK_VMTK_COMMON_EXPORT vtkvmtkMath : public vtkObject
{
public:

  vtkTypeMacro(vtkvmtkMath,vtkObject);
//  vtkTypeMacro(vtkvmtkMath,vtkObject);
  static vtkvmtkMath* New();
  
  static double Cotangent(double point0[3], double point1[3], double point2[3]);
  static double TriangleArea(double point0[3], double point1[3], double point2[3]);
  static int IsAngleObtuse(double point0[3], double point1[3], double point2[3]);
  static int IsTriangleObtuse(double point0[3], double point1[3], double point2[3]);
  static double VoronoiSectorArea(double point0[3], double point1[3], double point2[3]);
  static double TriangleGradient(double point0[3], double point1[3], double point2[3], double scalar0, double scalar1, double scalar2, double gradient[3]);
  static void TwoSphereIntersection(double center0[3], double radius0, double center1[3], double radius1, double origin[3], double normal[3]);
  static double AngleBetweenNormals(double normal0[3], double normal1[3]);
  static double EvaluateSphereFunction(double center[3], double radius, double point[3]);

protected:
  vtkvmtkMath() {};
  ~vtkvmtkMath() {};

  static double GetTolerance(float cookie)
  { 
    return VTK_VMTK_FLOAT_TOL; 
  }

  static double GetTolerance(double cookie)
  { 
    return VTK_VMTK_DOUBLE_TOL; 
  }

  static double GetLarge(float cookie)
  { 
    return VTK_VMTK_LARGE_FLOAT; 
  }

  static double GetLarge(double cookie)
  { 
    return VTK_VMTK_LARGE_DOUBLE; 
  }

private:
  vtkvmtkMath(const vtkvmtkMath&);  // Not implemented.
  void operator=(const vtkvmtkMath&);  // Not implemented.
};

#endif
