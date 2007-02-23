/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridFELaplaceAssembler.h,v $
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
// .NAME vtkvmtkUnstructuredGridFELaplaceAssembler - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridFELaplaceAssembler_h
#define __vtkvmtkUnstructuredGridFELaplaceAssembler_h

#include "vtkObject.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFELaplaceAssembler : public vtkObject
{
public:

  static vtkvmtkUnstructuredGridFELaplaceAssembler* New();
  vtkTypeRevisionMacro(vtkvmtkUnstructuredGridFELaplaceAssembler,vtkObject);

  vtkSetObjectMacro(DataSet,vtkUnstructuredGrid);
  vtkGetObjectMacro(DataSet,vtkUnstructuredGrid);

  vtkSetObjectMacro(Matrix,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(Matrix,vtkvmtkSparseMatrix);

  void Build();

  void DeepCopy(vtkvmtkUnstructuredGridFELaplaceAssembler *src);

  void ShallowCopy(vtkvmtkUnstructuredGridFELaplaceAssembler *src);

protected:
  vtkvmtkUnstructuredGridFELaplaceAssembler();
  ~vtkvmtkUnstructuredGridFELaplaceAssembler();

  vtkUnstructuredGrid* DataSet;
  vtkvmtkSparseMatrix* Matrix;

  int QuadratureOrder;

private:
  vtkvmtkUnstructuredGridFELaplaceAssembler(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
};

#endif
