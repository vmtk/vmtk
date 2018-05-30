/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkLinearSystem.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkLinearSystem - Base container to hold sparse matrice objects representing a linear system of equations Ax = B.
// .SECTION Description
// ..

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

  vtkSetObjectMacro(A,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(A,vtkvmtkSparseMatrix);

  vtkSetObjectMacro(X,vtkvmtkDoubleVector);
  vtkGetObjectMacro(X,vtkvmtkDoubleVector);

  vtkSetObjectMacro(B,vtkvmtkDoubleVector);
  vtkGetObjectMacro(B,vtkvmtkDoubleVector);

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

