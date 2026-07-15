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
 * @class   vtkvmtkUnstructuredGridGradientFilter
 * @brief   Compute the gradient of data stored within an unstructured grid mesh.
 * @ingroup DifferentialGeometry
 *
 * The vtkUnstructuredGrid counterpart of vtkvmtkPolyDataGradientFilter: for each component of the
 * point data array named InputArrayName, computes a smoothed nodal gradient using a finite-element
 * L2 projection (vtkvmtkUnstructuredGridFEGradientAssembler, Gauss quadrature order
 * QuadratureOrder), solved with vtkvmtkOpenNLLinearSystemSolver to the given ConvergenceTolerance,
 * writing the result into GradientArrayName.
 *
 * @sa vtkvmtkPolyDataGradientFilter, vtkvmtkUnstructuredGridFEGradientAssembler
 */

#ifndef __vtkvmtkUnstructuredGridGradientFilter_h
#define __vtkvmtkUnstructuredGridGradientFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridGradientFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridGradientFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridGradientFilter,vtkUnstructuredGridAlgorithm);

  ///@{
  /**
   * Set/Get the name of the point data array whose (per-component) gradient is computed. Must be
   * set, and the array must exist on the input, before Update() is called.
   */
  vtkSetStringMacro(InputArrayName);
  vtkGetStringMacro(InputArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array, with 3 components per component of InputArrayName,
   * that the computed gradient(s) are written into on the output. Must be set before Update() is
   * called.
   */
  vtkSetStringMacro(GradientArrayName);
  vtkGetStringMacro(GradientArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the convergence tolerance of the iterative linear solver (see
   * vtkvmtkOpenNLLinearSystemSolver) used to solve the finite-element gradient projection.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get the order of the Gauss quadrature rule used to integrate the finite-element matrices in
   * vtkvmtkUnstructuredGridFEGradientAssembler.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

  ///@{
  /**
   * Toggle assembling and solving each partial derivative of the gradient as a separate linear
   * system (one per coordinate direction), instead of assembling the full gradient in a single
   * system. Default: off.
   */
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);
  ///@}

protected:
  vtkvmtkUnstructuredGridGradientFilter();
  ~vtkvmtkUnstructuredGridGradientFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* InputArrayName;
  char* GradientArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;
  int ComputeIndividualPartialDerivatives;

private:
  vtkvmtkUnstructuredGridGradientFilter(const vtkvmtkUnstructuredGridGradientFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridGradientFilter&);  // Not implemented.
};

#endif

