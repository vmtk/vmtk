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
 * @class   vtkvmtkSparseMatrixRow
 * @brief   Represents a single row of a vtkvmtkSparseMatrix: a sparse list of (column id, value)
 * pairs plus a separate diagonal element.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkSparseMatrixRow stores the off-diagonal, non-zero entries of one matrix row as two
 * parallel arrays -- element (column) ids and their values -- together with a distinct diagonal
 * value stored separately for fast access. Rows can be populated directly (SetElementId/
 * SetElement) or built from the neighbor list and weights of a vtkvmtkStencil (CopyStencil) or the
 * point ids of a vtkvmtkNeighborhood (CopyNeighborhood). It is the building block used by
 * vtkvmtkSparseMatrix.
 *
 * @sa vtkvmtkSparseMatrix, vtkvmtkStencil, vtkvmtkNeighborhood
 */

#ifndef __vtkvmtkSparseMatrixRow_h
#define __vtkvmtkSparseMatrixRow_h

#include "vtkObject.h"
#include "vtkvmtkStencil.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkSparseMatrixRow : public vtkObject 
{
public:

  static vtkvmtkSparseMatrixRow *New();
  vtkTypeMacro(vtkvmtkSparseMatrixRow,vtkObject);

  ///@{
  /**
   * Get/set the column id stored at position i (0-based index into this row's element arrays, not
   * a matrix column lookup -- see GetElementIndex() to find the position for a given column id).
   */
  vtkIdType GetElementId(vtkIdType i) { return this->ElementIds[i]; }
  void SetElementId(vtkIdType i, vtkIdType id) { this->ElementIds[i] = id; }
  ///@}

  ///@{
  /**
   * Get/set the off-diagonal value stored at position i (0-based index into this row's element
   * arrays; see GetElementIndex() to find the position for a given column id).
   */
  double GetElement(vtkIdType i) { return this->Elements[i]; }
  void SetElement(vtkIdType i, double element) { this->Elements[i] = element; }
  ///@}

  /**
   * Look up the 0-based position within this row's element arrays that holds column id id, by
   * linear search. Returns -1 (and raises an error) if id is not present in the row.
   */
  vtkIdType GetElementIndex(vtkIdType id);

  ///@{
  /**
   * Get/set the number of off-diagonal (column id, value) entries stored in this row. Setting the
   * count (re)allocates the ElementIds/Elements arrays, releasing any previous contents and
   * filling the new entries with id 0 / value 0.0; the diagonal element is also reset to 0.0.
   */
  vtkIdType GetNumberOfElements() { return this->NElements; }
  void SetNumberOfElements(vtkIdType numberOfElements);
  ///@}

  ///@{
  /**
   * Set/get the diagonal value of this matrix row, stored separately from the off-diagonal
   * entries. Default: 0.0.
   */
  vtkSetMacro(DiagonalElement,double);
  vtkGetMacro(DiagonalElement,double);
  ///@}

  /**
   * Release the row's element arrays, reset the number of elements to zero, and reset the
   * diagonal element to 0.0.
   */
  void Initialize();

  /**
   * Replace this row's contents with the neighbor point ids and weights of stencil: one
   * off-diagonal entry per stencil neighbor (id = stencil point id, value = stencil weight) and
   * the diagonal element set to the stencil's center weight.
   */
  void CopyStencil(vtkvmtkStencil* stencil);

  /**
   * Replace this row's off-diagonal column ids with the point ids of neighborhood (one entry per
   * neighbor, values left at 0.0); the diagonal element is left unchanged.
   */
  void CopyNeighborhood(vtkvmtkNeighborhood* neighborhood);

  /**
   * Standard DeepCopy method.  Since this object contains no reference to other objects, there is no
   * ShallowCopy.
   */
  void DeepCopy(vtkvmtkSparseMatrixRow *src);

protected:
  vtkvmtkSparseMatrixRow();
  ~vtkvmtkSparseMatrixRow();

  vtkIdType* ElementIds;
  double* Elements;
  double DiagonalElement;
  vtkIdType NElements;

private:
  vtkvmtkSparseMatrixRow(const vtkvmtkSparseMatrixRow&);  // Not implemented.
  void operator=(const vtkvmtkSparseMatrixRow&);  // Not implemented.
};

#endif

