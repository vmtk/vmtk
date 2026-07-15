/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 * @class   vtkvmtkCardinalSpline
 * @brief   Extends vtkCardinalSpline with methods to evaluate the spline's first derivative, second derivative, and value-plus-derivatives at a parametric coordinate.
 * @ingroup Segmentation
 *
 * vtkvmtkCardinalSpline extends vtkCardinalSpline, which only evaluates the spline's value at a
 * parametric coordinate, with methods to also evaluate its first and second derivatives (and all
 * three at once) at a given parametric coordinate t, using the same cubic piecewise coefficients
 * computed by the base class's Compute(). This is used, for instance, by vtkvmtkActiveTubeFilter to
 * obtain tangent and curvature-like information (needed for internal stiffness forces) along a
 * spline fit through a centerline's points and radii.
 *
 * @sa vtkvmtkActiveTubeFilter
 */

#ifndef __vtkvmtkCardinalSpline_h
#define __vtkvmtkCardinalSpline_h

#include "vtkCardinalSpline.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCardinalSpline : public vtkCardinalSpline
{
public:
  static vtkvmtkCardinalSpline *New();

  vtkTypeMacro(vtkvmtkCardinalSpline,vtkCardinalSpline);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  /**
   * Evaluate the first derivative of the spline at parametric coordinate t. Recomputes the spline
   * coefficients first if the spline's control points have changed since the last computation. t is
   * clamped to the spline's parametric range; returns 0.0 if fewer than 2 points have been added.
   */
  virtual double EvaluateDerivative(double t);

  /**
   * Evaluate the second derivative of the spline at parametric coordinate t. Recomputes the spline
   * coefficients first if the spline's control points have changed since the last computation. t is
   * clamped to the spline's parametric range; returns 0.0 if fewer than 2 points have been added.
   */
  virtual double EvaluateSecondDerivative(double t);

  /**
   * Evaluate the spline's value, first derivative, and second derivative at parametric coordinate t
   * in a single call, storing them respectively in valueAndDerivatives[0], [1], and [2]. Equivalent
   * to (but more efficient than) calling Evaluate(), EvaluateDerivative(), and
   * EvaluateSecondDerivative() separately.
   */
  virtual void EvaluateValueAndDerivatives(double t, double valueAndDerivatives[3]);

protected:
  vtkvmtkCardinalSpline();
  ~vtkvmtkCardinalSpline() {}

private:
  vtkvmtkCardinalSpline(const vtkvmtkCardinalSpline&);  // Not implemented.
  void operator=(const vtkvmtkCardinalSpline&);  // Not implemented.
};

#endif

