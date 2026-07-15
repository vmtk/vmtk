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
 * @class   vtkvmtkMeshVorticity
 * @brief   Calculates vorticity from velocity gradients in a mesh.
 * @ingroup Misc
 *
 * Computes the full 3x3 velocity gradient tensor of the 3-component point data array named
 * VelocityArrayName using vtkvmtkUnstructuredGridGradientFilter (finite-element L2 projection,
 * Gauss quadrature order QuadratureOrder, solved to ConvergenceTolerance), then derives the
 * vorticity (curl) from the antisymmetric part of that tensor and stores it in the point data array
 * named VorticityArrayName. Unlike vtkvmtkUnstructuredGridVorticityFilter (which assembles the curl
 * directly, one component at a time, via vtkvmtkUnstructuredGridFEVorticityAssembler), this class
 * goes through the full gradient tensor.
 *
 * @sa vtkvmtkUnstructuredGridGradientFilter, vtkvmtkUnstructuredGridVorticityFilter
 */

#ifndef __vtkvmtkMeshVorticity_h
#define __vtkvmtkMeshVorticity_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshVorticity : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshVorticity,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMeshVorticity *New();

  ///@{
  /**
   * Set/Get the name of the 3-component point data array holding the velocity field. Required
   * input.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output 3-component point data array where the computed vorticity is
   * stored. If not set, "Vorticity" is used.
   */
  vtkSetStringMacro(VorticityArrayName);
  vtkGetStringMacro(VorticityArrayName);
  ///@}

  ///@{
  /**
   * Toggle assembling and solving each partial derivative of the velocity gradient as a separate
   * linear system, instead of assembling the full gradient in a single system. See
   * vtkvmtkUnstructuredGridGradientFilter::ComputeIndividualPartialDerivatives. Default: off.
   */
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);
  ///@}

  ///@{
  /**
   * Set/Get the convergence tolerance of the iterative linear solver used to solve the
   * finite-element gradient projection.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get the order of the Gauss quadrature rule used to integrate the finite-element gradient
   * matrices.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

  protected:
  vtkvmtkMeshVorticity();
  ~vtkvmtkMeshVorticity();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* VorticityArrayName;

  int ComputeIndividualPartialDerivatives;

  double ConvergenceTolerance;
  int QuadratureOrder;

  private:
  vtkvmtkMeshVorticity(const vtkvmtkMeshVorticity&);  // Not implemented.
  void operator=(const vtkvmtkMeshVorticity&);  // Not implemented.
};

#endif
