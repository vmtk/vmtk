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
 * @class   vtkvmtkOpenNLLinearSystemSolver
 * @brief   Solve a linear system of equations using open NL.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkOpenNLLinearSystemSolver implements vtkvmtkLinearSystemSolver::Solve() on top of the
 * OpenNL (Open Numerical Library) sparse linear solver. It converts the vtkvmtkSparseMatrix /
 * vtkvmtkDoubleVector representation held by LinearSystem (see the base class) into an OpenNL
 * context, runs the requested iterative solver up to MaximumNumberOfIterations or
 * ConvergenceTolerance (both inherited from vtkvmtkLinearSystemSolver), and copies the result back
 * into the solution vector. It is used internally by finite-element/stencil based filters in this
 * module (e.g. vtkvmtkPolyDataGradientFilter, vtkvmtkPolyDataHarmonicMappingFilter) to solve the
 * sparse systems they assemble.
 *
 * There are a number of different solver types that can be used:
 *    - VTK_VMTK_OPENNL_SOLVER_BICGSTAB = Biconjugate gradient stabilized method
 *    - VTK_VMTK_OPENNL_SOLVER_GMRES = Generalized minimal residual method
 *    - VTK_VMTK_OPENNL_SOLVER_CG = Iterative conjugate gradient solver
 *
 * and a number of different preconditioner types:
 *    - VTK_VMTK_OPENNL_PRECONDITIONER_NONE = no preconditioning
 *    - VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI = Jacobi (diagonal) preconditioning
 *    - VTK_VMTK_OPENNL_PRECONDITIONER_SSOR = symmetric successive over-relaxation preconditioning
 *
 * @sa vtkvmtkLinearSystemSolver
 */

#ifndef __vtkvmtkOpenNLLinearSystemSolver_h
#define __vtkvmtkOpenNLLinearSystemSolver_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystemSolver.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkOpenNLLinearSystemSolver : public vtkvmtkLinearSystemSolver
{
public:
  static vtkvmtkOpenNLLinearSystemSolver* New();
  vtkTypeMacro(vtkvmtkOpenNLLinearSystemSolver,vtkvmtkLinearSystemSolver);

  /**
   * Assemble and solve the linear system currently set on LinearSystem (see
   * vtkvmtkLinearSystemSolver::SetLinearSystem) using OpenNL, with the solver and preconditioner
   * selected by SolverType and PreconditionerType. Returns 0 on success, -1 if no linear system
   * (or an incomplete one) has been set.
   */
  int Solve() override;

  ///@{
  /**
   * Set/get the OpenNL iterative solver algorithm used by Solve(): VTK_VMTK_OPENNL_SOLVER_CG
   * (conjugate gradient, the default), VTK_VMTK_OPENNL_SOLVER_GMRES (generalized minimal residual),
   * or VTK_VMTK_OPENNL_SOLVER_BICGSTAB (biconjugate gradient stabilized). CG requires the system
   * matrix to be symmetric positive definite; use GMRES or BiCGStab for non-symmetric systems.
   */
  vtkSetMacro(SolverType,int);
  vtkGetMacro(SolverType,int);
  void SetSolverTypeToCG()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_CG); }
  void SetSolverTypeToGMRES()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_GMRES); }
  void SetSolverTypeToBiCGStab()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_BICGSTAB); }
  ///@}

  ///@{
  /**
   * Set/get the preconditioner applied by Solve(): VTK_VMTK_OPENNL_PRECONDITIONER_NONE (the
   * default), VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI (diagonal scaling), or
   * VTK_VMTK_OPENNL_PRECONDITIONER_SSOR (symmetric successive over-relaxation, using Omega as the
   * relaxation factor). A suitable preconditioner can significantly reduce the number of iterations
   * needed to reach ConvergenceTolerance.
   */
  vtkSetMacro(PreconditionerType,int);
  vtkGetMacro(PreconditionerType,int);
  void SetPreconditionerTypeToNone()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_NONE); }
  void SetPreconditionerTypeToJacobi()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI); }
  void SetPreconditionerTypeToSSOR()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_SSOR); }
  ///@}

  ///@{
  /**
   * Set/get the relaxation factor used by the SSOR preconditioner (ignored for other
   * PreconditionerType values). Default: 0.0.
   */
  vtkSetMacro(Omega,double);
  vtkGetMacro(Omega,double);
  ///@}

  //BTX
  enum
    {
      VTK_VMTK_OPENNL_SOLVER_CG,
      VTK_VMTK_OPENNL_SOLVER_GMRES,
      VTK_VMTK_OPENNL_SOLVER_BICGSTAB
    };
  //ETX
  
  //BTX
  enum
    {
      VTK_VMTK_OPENNL_PRECONDITIONER_NONE,
      VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI,
      VTK_VMTK_OPENNL_PRECONDITIONER_SSOR
    };
  //ETX

protected:
  vtkvmtkOpenNLLinearSystemSolver();
  ~vtkvmtkOpenNLLinearSystemSolver() {};

  int SolverType;
  int PreconditionerType;
  double Omega;

private:
  vtkvmtkOpenNLLinearSystemSolver(const vtkvmtkOpenNLLinearSystemSolver&);  // Not implemented.
  void operator=(const vtkvmtkOpenNLLinearSystemSolver&);  // Not implemented.
};

#endif

