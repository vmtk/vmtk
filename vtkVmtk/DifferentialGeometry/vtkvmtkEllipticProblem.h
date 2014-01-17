/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkEllipticProblem.h,v $
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
// .NAME vtkvmtkEllipticProblem - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkEllipticProblem_h
#define __vtkvmtkEllipticProblem_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkEllipticProblem : public vtkObject
{
public:
  static vtkvmtkEllipticProblem* New();
  vtkTypeMacro(vtkvmtkEllipticProblem,vtkObject);

protected:
  vtkvmtkEllipticProblem();
  ~vtkvmtkEllipticProblem();

  vtkvmtkLinearSystem* LinearSystem;
  vtkvmtkDirichletBoundaryConditions* DirichletBoundaryConditions;

private:
  vtkvmtkEllipticProblem(const vtkvmtkEllipticProblem&);  // Not implemented.
  void operator=(const vtkvmtkEllipticProblem&);  // Not implemented.
};

#endif

