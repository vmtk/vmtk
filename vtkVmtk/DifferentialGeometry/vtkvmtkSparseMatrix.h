/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSparseMatrix.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkSparseMatrix - Class for constructing sparse matrices from a dataset and performing basic mathematical operations on it. 
// .SECTION Description
// ..

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

  void Multiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);
  void TransposeMultiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y);

  // Description:
  // Get a row given a row id.
  vtkvmtkSparseMatrixRow* GetRow(vtkIdType i) { return this->Array[i]; }

  vtkGetMacro(NumberOfRows,vtkIdType);
  void CopyRowsFromStencils(vtkvmtkStencils *stencils);
  void AllocateRowsFromNeighborhoods(vtkvmtkNeighborhoods *neighborhoods, int numberOfVariables=1);
  void AllocateRowsFromDataSet(vtkDataSet *dataSet, int numberOfVariables=1);
  
  void Initialize();
  void SetNumberOfRows(vtkIdType numberOfRows);

  double GetElement(vtkIdType i, vtkIdType j);
  void SetElement(vtkIdType i, vtkIdType j, double value);
  void AddElement(vtkIdType i, vtkIdType j, double value);

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

