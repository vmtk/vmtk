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
 * @class   vtkvmtkMeshWallShearRate
 * @brief   Calculates wall shear rate from velocity components in a mesh.
 * @ingroup Misc
 *
 * Extracts the wall surface of the input volumetric mesh, computes the velocity gradient tensor of
 * the 3-component point data array named VelocityArrayName (via vtkvmtkUnstructuredGridGradientFilter,
 * Gauss quadrature order QuadratureOrder, solved to ConvergenceTolerance), and derives the wall
 * shear rate vector at each wall point, storing it in the point data array named
 * WallShearRateArrayName. By default (UseFullStrainRateTensor off), the simple approximation
 * tau = -(grad u) . n is used; when on, the full symmetric strain-rate tensor
 * E = 0.5*(grad u + (grad u)^T) is used instead (tau = -2 * E.n projected tangent to the wall),
 * per Matyka et al. (doi:10.1016/j.compfluid.2012.12.018).
 *
 * @sa vtkvmtkUnstructuredGridGradientFilter, vtkvmtkMeshVorticity
 */

#ifndef __vtkvmtkMeshWallShearRate_h
#define __vtkvmtkMeshWallShearRate_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshWallShearRate : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshWallShearRate,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMeshWallShearRate *New();

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
   * Set/Get the name of the output 3-component point data array where the computed wall shear rate
   * is stored.
   * Commonly named "WallShearRate".
   */
  vtkSetStringMacro(WallShearRateArrayName);
  vtkGetStringMacro(WallShearRateArrayName);
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
   * Toggle using the full symmetric strain-rate tensor formulation (per Matyka et al.) instead of
   * the simple tau = -(grad u).n approximation. See the class description. Default: off.
   */
  vtkSetMacro(UseFullStrainRateTensor,int);
  vtkGetMacro(UseFullStrainRateTensor,int);
  vtkBooleanMacro(UseFullStrainRateTensor,int);
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
  vtkvmtkMeshWallShearRate();
  ~vtkvmtkMeshWallShearRate();  

  int FillInputPortInformation(int, vtkInformation *info) override;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* WallShearRateArrayName;

  int ComputeIndividualPartialDerivatives;

  double ConvergenceTolerance;
  int QuadratureOrder;
  int UseFullStrainRateTensor;

  private:
  vtkvmtkMeshWallShearRate(const vtkvmtkMeshWallShearRate&);  // Not implemented.
  void operator=(const vtkvmtkMeshWallShearRate&);  // Not implemented.
};

#endif
