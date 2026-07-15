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
 * @class   vtkvmtkSparseMatrix
 * @brief   Implements a row-based sparse matrix built from mesh neighborhoods or stencils,
 * supporting basic linear algebra operations.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkSparseMatrix stores one vtkvmtkSparseMatrixRow per matrix row, each row holding the
 * off-diagonal (column id, value) pairs plus a separate diagonal element. Rows can be allocated
 * from the point connectivity of a vtkDataSet (AllocateRowsFromDataSet) or from a
 * vtkvmtkNeighborhoods object (AllocateRowsFromNeighborhoods) -- both optionally replicated for
 * several independent variables per point (numberOfVariables) to build a block-structured system
 * -- or its row structure and weights can be copied directly from a set of finite-element/
 * Laplacian vtkvmtkStencils (CopyRowsFromStencils). It supports matrix-vector multiplication
 * (Multiply, TransposeMultiply) against a vtkvmtkDoubleVector and is the matrix type used by
 * vtkvmtkLinearSystem / vtkvmtkOpenNLLinearSystemSolver and the finite-element assemblers
 * (vtkvmtkFEAssembler subclasses) throughout vmtk's harmonic mapping, gradient, and vorticity
 * filters.
 *
 * @sa vtkvmtkSparseMatrixRow, vtkvmtkDoubleVector, vtkvmtkLinearSystem, vtkvmtkFEAssembler
 */

#ifndef __vtkvmtkSparseMatrix_h
#define __vtkvmtkSparseMatrix_h

#include "vtkObject.h"
#include "vtkvmtkSparseMatrixRow.h"
#include "vtkvmtkNeighborhoods.h"
#include "vtkvmtkStencils.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkDataSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkSparseMatrix : public vtkObject
{
public:

  static vtkvmtkSparseMatrix* New();
  vtkTypeMacro(vtkvmtkSparseMatrix,vtkObject);

  /**
   * Compute y = A*x, the matrix-vector product of this sparse matrix with x, storing the result
   * in y. Values are clamped to +/-VTK_VMTK_LARGE_DOUBLE and snapped to zero below
   * VTK_VMTK_PIVOTING_TOL to guard against overflow/underflow.
   */
  void Multiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);

  /**
   * Compute y = A^T*x, the matrix-vector product of this sparse matrix's transpose with x,
   * storing the result in y. Values are clamped/snapped the same way as Multiply().
   */
  void TransposeMultiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);

  /**
   * Get a row given a row id.
   */
  vtkvmtkSparseMatrixRow* GetRow(vtkIdType i) { return this->Array[i]; }

  /**
   * Get the number of rows currently allocated in the matrix. Use SetNumberOfRows() to change it.
   */
  vtkGetMacro(NumberOfRows,vtkIdType);

  /**
   * Initialize the matrix with one row per stencil in stencils, copying each stencil's neighbor
   * ids, weights, and center weight into the corresponding matrix row (see
   * vtkvmtkSparseMatrixRow::CopyStencil). Replaces any previously allocated rows.
   */
  void CopyRowsFromStencils(vtkvmtkStencils *stencils);

  /**
   * Initialize the matrix with numberOfVariables * neighborhoods->GetNumberOfNeighborhoods()
   * rows, one per (point, variable) pair, allocating each row's column structure (values left at
   * 0) from the corresponding neighborhood's point ids, replicated across all variable blocks so
   * the matrix can represent a coupled multi-variable system. Replaces any previously allocated
   * rows.
   */
  void AllocateRowsFromNeighborhoods(vtkvmtkNeighborhoods *neighborhoods, int numberOfVariables=1);

  /**
   * Initialize the matrix the same way as AllocateRowsFromNeighborhoods, but building the point
   * neighborhoods internally from the connectivity of dataSet (must be a vtkPolyData or
   * vtkUnstructuredGrid). Replaces any previously allocated rows.
   */
  void AllocateRowsFromDataSet(vtkDataSet *dataSet, int numberOfVariables=1);

  /**
   * Release all allocated rows and reset the matrix to zero rows.
   */
  void Initialize();

  /**
   * Set the number of rows in the matrix, (re)allocating a fresh vtkvmtkSparseMatrixRow for each
   * row (any previously allocated rows are released). Rows are created empty; use
   * GetRow()->SetNumberOfElements() or one of the Allocate.../CopyRowsFromStencils() helpers to
   * populate them.
   */
  void SetNumberOfRows(vtkIdType numberOfRows);

  /**
   * Get the value at row i, column j (the diagonal element when i == j, otherwise the
   * off-diagonal element identified by column id j, which must already exist in the row's
   * sparsity pattern).
   */
  double GetElement(vtkIdType i, vtkIdType j);

  /**
   * Set the value at row i, column j (the diagonal element when i == j, otherwise the
   * off-diagonal element identified by column id j, which must already exist in the row's
   * sparsity pattern).
   */
  void SetElement(vtkIdType i, vtkIdType j, double value);

  /**
   * Add value to the current value at row i, column j (equivalent to
   * SetElement(i,j,GetElement(i,j)+value)).
   */
  void AddElement(vtkIdType i, vtkIdType j, double value);

  /**
   * Standard DeepCopy method: copies the number of rows and the contents of every row from src.
   */
  void DeepCopy(vtkvmtkSparseMatrix *src);

protected:
  vtkvmtkSparseMatrix();
  ~vtkvmtkSparseMatrix();

  vtkvmtkSparseMatrixRow** Array;
  vtkIdType NumberOfRows;

private:
  vtkvmtkSparseMatrix(const vtkvmtkSparseMatrix&);  // Not implemented.
  void operator=(const vtkvmtkSparseMatrix&);  // Not implemented.
};

#endif

