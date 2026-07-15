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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkRBFInterpolation2
 * @brief   Implicit function that interpolates scalar point data with a
 *          radial basis function.
 * @ingroup Contrib
 *
 * vtkvmtkRBFInterpolation2 solves for a set of per-point coefficients such
 * that a sum of radial basis functions (RBFs) centered at the points of
 * Source exactly reproduces the scalar point data of Source, then evaluates
 * that sum as an implicit function at arbitrary query points. It is a
 * variant of vtkvmtkRBFInterpolation. The coefficients are found by solving
 * a dense linear system (one equation per source point) the first time
 * EvaluateFunction() is called, or explicitly via ComputeCoefficients().
 * EvaluateGradient() is not implemented.
 *
 * @sa vtkvmtkRBFInterpolation
 */

#ifndef __vtkvmtkRBFInterpolation2_h
#define __vtkvmtkRBFInterpolation2_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkRBFInterpolation2 : public vtkImplicitFunction
{
  public:
  vtkTypeMacro(vtkvmtkRBFInterpolation2,vtkImplicitFunction);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkRBFInterpolation2 *New();

  ///@{
  /**
   * Evaluate polyball.
   */
  double EvaluateFunction(double x[3]) override;
  double EvaluateFunction(double x, double y, double z) override
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;
  ///@}

  /**
   * Evaluate polyball gradient. Not implemented; issues a warning and
   * leaves n untouched.
   */
  void EvaluateGradient(double x[3], double n[3]) override;

  /**
   * Solve the dense linear system that determines the per-point RBF
   * coefficients required to interpolate the scalar point data of Source.
   * Called automatically by EvaluateFunction() the first time it is
   * invoked after Source (or its data) changes; may also be called
   * explicitly to force recomputation.
   */
  void ComputeCoefficients();

  ///@{
  /**
   * Set/get the source poly data whose points and point scalars define the
   * interpolation problem: the RBFs are centered at the Source points and
   * their coefficients are solved for so that the interpolant reproduces
   * the Source point scalars exactly.
   */
  vtkSetObjectMacro(Source,vtkPolyData);
  vtkGetObjectMacro(Source,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/get the type of radial basis function used, one of
   * THIN_PLATE_SPLINE (r^2 log r, the default), BIHARMONIC (r), or
   * TRIHARMONIC (r^3), where r is the distance to an RBF center.
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

  /**
   * Returns this object's MTime, taking into account the MTime of Source.
   */
  vtkMTimeType GetMTime() override;

  protected:
  vtkvmtkRBFInterpolation2();
  ~vtkvmtkRBFInterpolation2();

  double EvaluateRBF(double c[3], double x[3]);

  vtkPolyData* Source;
  int RBFType;

  vtkDoubleArray* Coefficients;
  
  private:
  vtkvmtkRBFInterpolation2(const vtkvmtkRBFInterpolation2&);  // Not implemented.
  void operator=(const vtkvmtkRBFInterpolation2&);  // Not implemented.
};

#endif


