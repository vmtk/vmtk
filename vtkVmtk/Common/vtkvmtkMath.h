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
 * @class   vtkvmtkMath
 * @brief   Provides static utility mathematical functions to avoid repeating code across classes.
 * @ingroup Common
 *
 * All arguments expect doubles.
 */

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
  
  /**
   * Compute the cotangent of the angle at point1 subtended by the segments point1-point0 and
   * point1-point2 (i.e. the angle at the vertex shared by the two segments). Degenerate (zero-area)
   * configurations return either 0.0 or a large sentinel value (see VTK_VMTK_LARGE_DOUBLE), depending
   * on whether the segments are also aligned (dot product near zero).
   */
  static double Cotangent(double point0[3], double point1[3], double point2[3]);

  /**
   * Compute the area of the triangle formed by point0, point1, point2, via the norm of the cross
   * product of two of its edge vectors.
   */
  static double TriangleArea(double point0[3], double point1[3], double point2[3]);

  /**
   * Return 1 if the angle at point1 subtended by point0 and point2 is obtuse (the dot product of the
   * two edge vectors from point1 is negative, within tolerance), 0 otherwise.
   */
  static int IsAngleObtuse(double point0[3], double point1[3], double point2[3]);

  /**
   * Classify the triangle (point0, point1, point2) according to which, if any, of its angles is
   * obtuse. Returns VTK_VMTK_OBTUSE_IN_POINT if the angle at point1 is obtuse, VTK_VMTK_OBTUSE_NOT_IN_POINT
   * if the angle at point0 or point2 is obtuse instead, or VTK_VMTK_NON_OBTUSE if the triangle is
   * non-obtuse. Used to select between the Voronoi and simple barycentric/mixed area formulas when
   * computing per-vertex mesh areas (e.g. in Voronoi/mixed-area curvature or stencil weighting
   * schemes).
   */
  static int IsTriangleObtuse(double point0[3], double point1[3], double point2[3]);

  /**
   * Compute the contribution of point1 to the (non-obtuse) Voronoi region area within the triangle
   * (point0, point1, point2), using the cotangent-weighted formula
   * 1/8 * (|point0-point1|^2 * cot(angle at point2) + |point2-point1|^2 * cot(angle at point0))
   * (Meyer et al.). Only meaningful when the triangle is non-obtuse; callers typically dispatch on
   * IsTriangleObtuse() first and fall back to a simpler area split for obtuse triangles.
   */
  static double VoronoiSectorArea(double point0[3], double point1[3], double point2[3]);

  /**
   * Compute the gradient, over the triangle (point0, point1, point2), of a scalar field linearly
   * interpolated from the per-vertex values scalar0, scalar1, scalar2. The gradient vector (constant
   * over the triangle) is stored in gradient and its magnitude is returned. If the triangle is
   * degenerate (zero area), returns 0.0 when the scalar values are all equal, or a large sentinel
   * value otherwise.
   */
  static double TriangleGradient(double point0[3], double point1[3], double point2[3], double scalar0, double scalar1, double scalar2, double gradient[3]);

  /**
   * Compute the center (origin) and axis (normal) of the circle formed by the intersection of two
   * spheres (center0, radius0) and (center1, radius1); origin lies on the line joining the two
   * centers. If the two centers coincide, origin is set to center1 and normal to the zero vector.
   */
  static void TwoSphereIntersection(double center0[3], double radius0, double center1[3], double radius1, double origin[3], double normal[3]);

  /**
   * Compute the angle, in radians, between two (not necessarily unit-length) normal vectors, using
   * the numerically stable formula 2*atan2(|normal0-normal1|, |normal0+normal1|) (equivalent to
   * acos of the normalized dot product, but well-conditioned for angles near 0 or pi).
   */
  static double AngleBetweenNormals(double normal0[3], double normal1[3]);

  /**
   * Evaluate the implicit sphere function of center/radius at point: the squared distance from point
   * to center minus radius squared. Negative inside the sphere, positive outside, zero on the sphere.
   */
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
