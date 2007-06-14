/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSparseMatrix.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkSparseMatrix - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkSparseMatrix_h
#define __vtkvmtkSparseMatrix_h

#include "vtkObject.h"
#include "vtkvmtkItems.h"
#include "vtkvmtkSparseMatrixRow.h"
#include "vtkvmtkNeighborhoods.h"
#include "vtkvmtkStencils.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkDataSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkSparseMatrix : public vtkvmtkItems
{
public:

  static vtkvmtkSparseMatrix* New();
  vtkTypeRevisionMacro(vtkvmtkSparseMatrix,vtkvmtkItems);

  void Multiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);
  void TransposeMultiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);

  // Description:
  // Get a row given a row id.
  vtkvmtkSparseMatrixRow* GetRow(vtkIdType i) {return (vtkvmtkSparseMatrixRow*)this->Array[i];};

  void SetNumberOfRows(vtkIdType numberOfRows) { this->SetNumberOfItems(numberOfRows);};
  vtkIdType GetNumberOfRows() {return this->GetNumberOfItems();};

  void CopyRowsFromStencils(vtkvmtkStencils *stencils);
  void AllocateRowsFromNeighborhoods(vtkvmtkNeighborhoods *neighborhoods, int numberOfVariables=1);

  void AllocateRow(vtkIdType i) {this->AllocateItem(i,VTK_VMTK_SPARSE_MATRIX_ROW);};

  double GetElement(vtkIdType i, vtkIdType j);
  void SetElement(vtkIdType i, vtkIdType j, double value);
  void AddElement(vtkIdType i, vtkIdType j, double value);

protected:
  vtkvmtkSparseMatrix();
  ~vtkvmtkSparseMatrix() {};

  virtual vtkvmtkItem* InstantiateNewItem(int itemType);

private:
  vtkvmtkSparseMatrix(const vtkvmtkSparseMatrix&);  // Not implemented.
  void operator=(const vtkvmtkSparseMatrix&);  // Not implemented.
};

#endif

