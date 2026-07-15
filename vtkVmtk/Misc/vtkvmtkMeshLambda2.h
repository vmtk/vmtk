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
 * @class   vtkvmtkMeshLambda2
 * @brief   Compute the Lambda2 surface for a given flow field which allow for the visualization of fluid vortex cores.
 * @ingroup Misc
 *
 * This filter implements the lambda2 vortex-core identification criterion (Jeong & Hussain): for
 * every point of an input volumetric mesh, it computes the velocity gradient tensor (via
 * vtkvmtkUnstructuredGridGradientFilter on the array named VelocityArrayName) and splits it into
 * its symmetric (strain-rate, S) and antisymmetric (spin/vorticity, Omega) parts, forms the tensor
 * S^2 + Omega^2, and stores its second-largest eigenvalue as a new point data array
 * (Lambda2ArrayName). Points where this value is negative lie inside a vortex core, so
 * thresholding/isosurfacing the output array below zero visualizes the vortex structures of the
 * flow.
 *
 * This is the filter behind the vmtkmeshlambda2 pype script, typically run on CFD velocity fields
 * to extract and visualize coherent vortical structures.
 */

#ifndef __vtkvmtkMeshLambda2_h
#define __vtkvmtkMeshLambda2_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshLambda2 : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshLambda2,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMeshLambda2 *New();

  ///@{
  /**
   * Set/Get the name of the input point data array holding the (3-component) velocity field.
   * Required before Update() is called.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the computed lambda2 values are stored.
   * If not set, defaults to "Lambda2".
   */
  vtkSetStringMacro(Lambda2ArrayName);
  vtkGetStringMacro(Lambda2ArrayName);
  ///@}

  ///@{
  /**
   * Toggle computing the individual partial derivatives of the velocity gradient in
   * vtkvmtkUnstructuredGridGradientFilter, rather than only the quantities strictly needed for the
   * gradient tensor. Passed through to the internal gradient filter. Default: off.
   */
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);
  ///@}

  ///@{
  /**
   * Set/Get the convergence tolerance used by the internal vtkvmtkUnstructuredGridGradientFilter
   * when recovering the point-wise velocity gradient tensor. Default: 1E-6.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get the Gauss quadrature order used by the internal vtkvmtkUnstructuredGridGradientFilter
   * when recovering the point-wise velocity gradient tensor. Default: 3.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

  ///@{
  /**
   * Toggle forcing the lambda2 value at mesh boundary points to be negative, to suppress spurious
   * vortex-core detections caused by the (typically fixed/no-slip) boundary conditions of the
   * underlying flow simulation. Default: off.
   */
  vtkSetMacro(ForceBoundaryToNegative,int);
  vtkGetMacro(ForceBoundaryToNegative,int);
  vtkBooleanMacro(ForceBoundaryToNegative,int);
  ///@}

  protected:
  vtkvmtkMeshLambda2();
  ~vtkvmtkMeshLambda2();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* Lambda2ArrayName;

  int ComputeIndividualPartialDerivatives;
  double ConvergenceTolerance;
  int QuadratureOrder;
  int ForceBoundaryToNegative;

  private:
  vtkvmtkMeshLambda2(const vtkvmtkMeshLambda2&);  // Not implemented.
  void operator=(const vtkvmtkMeshLambda2&);  // Not implemented.
};

#endif
