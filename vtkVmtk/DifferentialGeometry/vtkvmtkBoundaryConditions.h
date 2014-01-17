/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkBoundaryConditions.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkBoundaryConditions - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkBoundaryConditions_h
#define __vtkvmtkBoundaryConditions_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkBoundaryConditions : public vtkObject
{
public:

  vtkTypeMacro(vtkvmtkBoundaryConditions,vtkObject);

  vtkSetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  vtkGetObjectMacro(LinearSystem,vtkvmtkLinearSystem);

  vtkSetObjectMacro(BoundaryNodes,vtkIdList);
  vtkGetObjectMacro(BoundaryNodes,vtkIdList);

  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);

  virtual void Apply();

protected:
  vtkvmtkBoundaryConditions();
  ~vtkvmtkBoundaryConditions();

  vtkvmtkLinearSystem* LinearSystem;

  vtkIdList* BoundaryNodes;
  vtkDoubleArray* BoundaryValues;

private:
  vtkvmtkBoundaryConditions(const vtkvmtkBoundaryConditions&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryConditions&);  // Not implemented.
};

#endif

