/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkFEAssembler.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkFEAssembler - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkFEAssembler_h
#define __vtkvmtkFEAssembler_h

#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkFEAssembler : public vtkObject
{
public:

  vtkTypeMacro(vtkvmtkFEAssembler,vtkObject);

  vtkSetObjectMacro(DataSet,vtkDataSet);
  vtkGetObjectMacro(DataSet,vtkDataSet);

  vtkSetObjectMacro(Matrix,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(Matrix,vtkvmtkSparseMatrix);

  vtkSetObjectMacro(RHSVector,vtkvmtkDoubleVector);
  vtkGetObjectMacro(RHSVector,vtkvmtkDoubleVector);

  vtkSetObjectMacro(SolutionVector,vtkvmtkDoubleVector);
  vtkGetObjectMacro(SolutionVector,vtkvmtkDoubleVector);

  vtkGetMacro(NumberOfVariables,int);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  virtual void Build() = 0;

  void DeepCopy(vtkvmtkFEAssembler *src);
  void ShallowCopy(vtkvmtkFEAssembler *src);

protected:
  vtkvmtkFEAssembler();
  ~vtkvmtkFEAssembler();

  void Initialize(int numberOfVariables);

  vtkDataSet* DataSet;
  vtkvmtkSparseMatrix* Matrix;
  vtkvmtkDoubleVector* RHSVector;
  vtkvmtkDoubleVector* SolutionVector;

  int NumberOfVariables;
  int QuadratureOrder;

private:
  vtkvmtkFEAssembler(const vtkvmtkFEAssembler&);  // Not implemented.
  void operator=(const vtkvmtkFEAssembler&);  // Not implemented.
};

#endif
