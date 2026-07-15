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
 * @class   vtkvmtkLinearSystemSolver
 * @brief   Serves as the base class for linear-system-of-equations solver implementations.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkLinearSystemSolver defines the common interface and iteration control parameters
 * (MaximumNumberOfIterations, ConvergenceTolerance) for iterative solvers of a vtkvmtkLinearSystem.
 * This base class implementation of Solve() only validates that LinearSystem is set and internally
 * consistent (via vtkvmtkLinearSystem::CheckSystem()); concrete subclasses override Solve() to
 * actually iterate towards a solution, updating NumberOfIterations and Residual as they progress.
 *
 * @sa
 * vtkvmtkLinearSystem, vtkvmtkFEAssembler
 */

#ifndef __vtkvmtkLinearSystemSolver_h
#define __vtkvmtkLinearSystemSolver_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkLinearSystemSolver : public vtkObject
{
public:
  static vtkvmtkLinearSystemSolver* New();
  vtkTypeMacro(vtkvmtkLinearSystemSolver,vtkObject);

  ///@{
  /**
   * Set/get the linear system to be solved.
   */
  vtkSetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  vtkGetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  ///@}

  ///@{
  /**
   * Set/get the maximum number of iterations an iterative solver subclass is allowed to perform
   * before giving up. Default: a very large integer (effectively unlimited).
   */
  vtkSetMacro(MaximumNumberOfIterations,int);
  vtkGetMacro(MaximumNumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the residual tolerance at which an iterative solver subclass considers the system to
   * have converged. Default: 1E-4.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  /**
   * Solve LinearSystem. This base class implementation only checks that LinearSystem is set and
   * consistent (returning -1 and emitting an error if not); subclasses override this method to
   * perform the actual solve, storing the number of iterations and residual reached in
   * NumberOfIterations / Residual, and returning 0 on success or -1 on failure.
   */
  virtual int Solve();

protected:
  vtkvmtkLinearSystemSolver();
  ~vtkvmtkLinearSystemSolver();

  vtkvmtkLinearSystem* LinearSystem;

  int MaximumNumberOfIterations;
  double ConvergenceTolerance;

  int NumberOfIterations;
  double Residual;

private:
  vtkvmtkLinearSystemSolver(const vtkvmtkLinearSystemSolver&);  // Not implemented.
  void operator=(const vtkvmtkLinearSystemSolver&);  // Not implemented.
};

#endif

