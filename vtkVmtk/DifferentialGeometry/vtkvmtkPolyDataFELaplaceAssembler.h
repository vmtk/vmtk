/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFELaplaceAssembler.h,v $
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
// .NAME vtkvmtkPolyDataFELaplaceAssembler - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataFELaplaceAssembler_h
#define __vtkvmtkPolyDataFELaplaceAssembler_h

#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFELaplaceAssembler : public vtkObject
{
public:

  static vtkvmtkPolyDataFELaplaceAssembler* New();
  vtkTypeRevisionMacro(vtkvmtkPolyDataFELaplaceAssembler,vtkObject);

  vtkSetObjectMacro(DataSet,vtkPolyData);
  vtkGetObjectMacro(DataSet,vtkPolyData);

  vtkSetObjectMacro(Matrix,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(Matrix,vtkvmtkSparseMatrix);

  vtkSetMacro(UseAbsoluteJacobians,int);
  vtkGetMacro(UseAbsoluteJacobians,int);
  vtkBooleanMacro(UseAbsoluteJacobians,int);

  void Build();

  void DeepCopy(vtkvmtkPolyDataFELaplaceAssembler *src);

  void ShallowCopy(vtkvmtkPolyDataFELaplaceAssembler *src);

protected:
  vtkvmtkPolyDataFELaplaceAssembler();
  ~vtkvmtkPolyDataFELaplaceAssembler();

  vtkPolyData* DataSet;
  vtkvmtkSparseMatrix* Matrix;

  int QuadratureOrder;

  int UseAbsoluteJacobians;

private:
  vtkvmtkPolyDataFELaplaceAssembler(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
};

#endif
