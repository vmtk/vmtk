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
 * @class   vtkvmtkLinearSystem
 * @brief   Serves as the base container holding sparse matrix objects that represent a linear system of equations Ax = B.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkLinearSystem is a plain container for the three pieces of a linear system A x = B: the
 * sparse system matrix A, the unknown vector X, and the right-hand side vector B. CheckSystem()
 * validates that all three are set and that their sizes are mutually consistent (number of rows of
 * A matches the number of elements of X and B). It carries no solution logic itself; it is passed
 * to a vtkvmtkLinearSystemSolver (which reads/writes A, X, B) and is the type of system that
 * vtkvmtkBoundaryConditions/vtkvmtkDirichletBoundaryConditions modify to impose boundary
 * conditions, and that vtkvmtkFEAssembler subclasses populate during assembly.
 *
 * @sa
 * vtkvmtkLinearSystemSolver, vtkvmtkFEAssembler, vtkvmtkBoundaryConditions
 */

#ifndef __vtkvmtkLinearSystem_h
#define __vtkvmtkLinearSystem_h

#include "vtkObject.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkDoubleVector.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkLinearSystem : public vtkObject
{
public:
  static vtkvmtkLinearSystem* New();
  vtkTypeMacro(vtkvmtkLinearSystem,vtkObject);

  ///@{
  /**
   * Set/get the sparse system matrix A.
   */
  vtkSetObjectMacro(A,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(A,vtkvmtkSparseMatrix);
  ///@}

  ///@{
  /**
   * Set/get the unknown vector X (i.e. the solution of A x = B, filled in by a
   * vtkvmtkLinearSystemSolver).
   */
  vtkSetObjectMacro(X,vtkvmtkDoubleVector);
  vtkGetObjectMacro(X,vtkvmtkDoubleVector);
  ///@}

  ///@{
  /**
   * Set/get the right-hand side vector B.
   */
  vtkSetObjectMacro(B,vtkvmtkDoubleVector);
  vtkGetObjectMacro(B,vtkvmtkDoubleVector);
  ///@}

  /**
   * Verify that A, X and B are all set and that their sizes are consistent (number of rows of A
   * equals the number of elements of both X and B). Returns 0 if the system is valid, -1 otherwise
   * (also emitting an error message describing the problem).
   */
  int CheckSystem();

protected:
  vtkvmtkLinearSystem();
  ~vtkvmtkLinearSystem();

  vtkvmtkSparseMatrix* A;
  vtkvmtkDoubleVector* X;
  vtkvmtkDoubleVector* B;

private:
  vtkvmtkLinearSystem(const vtkvmtkLinearSystem&);  // Not implemented.
  void operator=(const vtkvmtkLinearSystem&);  // Not implemented.
};

#endif

