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
 * @class   vtkvmtkPolyDataGradientFilter
 * @brief   Compute the gradient of data stored within an polydata surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataGradientFilter computes, for each component of the point data array named
 * InputArrayName, a smoothed nodal surface gradient using a finite-element L2 projection: for every
 * component it assembles a mass matrix and right-hand side with vtkvmtkPolyDataFEGradientAssembler
 * (Gauss quadrature order QuadratureOrder), solves the resulting sparse linear system with
 * vtkvmtkOpenNLLinearSystemSolver (conjugate gradient with Jacobi preconditioning, tolerance
 * ConvergenceTolerance), and writes the 3-component gradient vector for that input component into
 * consecutive component triples of a new point data array named GradientArrayName (so a scalar input
 * yields a 3-component output, a vector input yields a 3*N-component output). The output poly data is
 * a copy of the input with this array added.
 *
 * @sa vtkvmtkPolyDataFEGradientAssembler, vtkvmtkOpenNLLinearSystemSolver
 */

#ifndef __vtkvmtkPolyDataGradientFilter_h
#define __vtkvmtkPolyDataGradientFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPolyDataAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataGradientFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataGradientFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataGradientFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the name of the point data array whose (per-component) gradient is computed. Must be
   * set, and the array must exist on the input, before Update() is called.
   */
  vtkSetStringMacro(InputArrayName);
  vtkGetStringMacro(InputArrayName);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data array, with 3 components per component of InputArrayName,
   * that the computed gradient(s) are written into on the output. Must be set before Update() is
   * called.
   */
  vtkSetStringMacro(GradientArrayName);
  vtkGetStringMacro(GradientArrayName);
  ///@}

  ///@{
  /**
   * Set/get the convergence tolerance of the iterative linear solver (see
   * vtkvmtkOpenNLLinearSystemSolver) used to solve the finite-element gradient projection for each
   * component. Default: 1E-6.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/get the order of the Gauss quadrature rule used to integrate the finite-element matrices in
   * vtkvmtkPolyDataFEGradientAssembler. Default: 3.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

protected:
  vtkvmtkPolyDataGradientFilter();
  ~vtkvmtkPolyDataGradientFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* InputArrayName;
  char* GradientArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkPolyDataGradientFilter(const vtkvmtkPolyDataGradientFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataGradientFilter&);  // Not implemented.
};

#endif

