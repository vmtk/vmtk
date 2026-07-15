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
 * @class   vtkvmtkRBFInterpolation
 * @brief   Implements an implicit function that interpolates a scalar value at a scattered set of
 * points using radial basis functions.
 * @ingroup Misc
 *
 * vtkvmtkRBFInterpolation builds a radial basis function (RBF) interpolant that takes the value
 * RBFInterpolationValue at every point of Source, and evaluates that interpolant as an implicit
 * function anywhere in space. The interpolation weights are found by solving the dense linear
 * system obtained by requiring the RBF sum to match RBFInterpolationValue at each source point
 * (see ComputeCoefficients); RBFType selects the radial kernel (thin-plate spline, biharmonic,
 * or triharmonic). Since all source points share the same target value, this does not directly
 * reconstruct a signed-distance-like implicit surface from on/off-surface samples; instead it is
 * used to build a smooth scalar field seeded at a set of points, e.g. by vmtkrbfinterpolation to
 * resample a set of seed points onto a regular image grid via vtkSampleFunction.
 */

#ifndef __vtkvmtkRBFInterpolation_h
#define __vtkvmtkRBFInterpolation_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkWin32Header.h"
#include "vtkVersion.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkRBFInterpolation : public vtkImplicitFunction
{
  public:
  vtkTypeMacro(vtkvmtkRBFInterpolation,vtkImplicitFunction);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkRBFInterpolation *New();

  ///@{
  /**
   * Evaluate polyball.
   */
  double EvaluateFunction(double x[3]) override;
  double EvaluateFunction(double x, double y, double z) override
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;
  ///@}

  /**
   * Evaluate polyball gradient.
   */
  void EvaluateGradient(double x[3], double n[3]) override;

  /**
   * Solve for the RBF interpolation weights (Coefficients) so that the interpolant evaluates
   * to RBFInterpolationValue at every point of Source, using the kernel selected by RBFType.
   * Called automatically by EvaluateFunction the first time it runs if Coefficients has not
   * been computed yet; can also be called explicitly beforehand.
   */
  void ComputeCoefficients();

  ///@{
  /**
   * Set / get source poly data.
   */
  vtkSetObjectMacro(Source,vtkPolyData);
  vtkGetObjectMacro(Source,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the radial basis kernel used to build the interpolant, one of the THIN_PLATE_SPLINE,
   * BIHARMONIC, or TRIHARMONIC enum values (equivalently set through the SetRBFTypeTo... methods
   * below). Default: THIN_PLATE_SPLINE.
   */
  vtkSetMacro(RBFType,int);
  vtkGetMacro(RBFType,int);
  void SetRBFTypeToThinPlateSpline()
  { this->SetRBFType(THIN_PLATE_SPLINE); }
  void SetRBFTypeToBiharmonic()
  { this->SetRBFType(BIHARMONIC); }
  void SetRBFTypeToTriharmonic()
  { this->SetRBFType(TRIHARMONIC); }
  ///@}

//BTX
  enum 
  {
    THIN_PLATE_SPLINE,
    BIHARMONIC,
    TRIHARMONIC
  };
//ETX

  vtkMTimeType GetMTime() override;

  protected:
  vtkvmtkRBFInterpolation();
  ~vtkvmtkRBFInterpolation();

  double EvaluateRBF(double c[3], double x[3]);

  vtkPolyData* Source;
  int RBFType;

  vtkDoubleArray* Coefficients;
  double RBFInterpolationValue;

  private:
  vtkvmtkRBFInterpolation(const vtkvmtkRBFInterpolation&);  // Not implemented.
  void operator=(const vtkvmtkRBFInterpolation&);  // Not implemented.
};

#endif


