/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDirichletBoundaryConditions.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga 

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkDirichletBoundaryConditions - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkDirichletBoundaryConditions_h
#define __vtkvmtkDirichletBoundaryConditions_h

#include "vtkObject.h"
#include "vtkvmtkBoundaryConditions.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDirichletBoundaryConditions : public vtkvmtkBoundaryConditions
{
public:
  static vtkvmtkDirichletBoundaryConditions* New();
  vtkTypeMacro(vtkvmtkDirichletBoundaryConditions,vtkvmtkBoundaryConditions);

  void Apply() VTK_OVERRIDE;

protected:
  vtkvmtkDirichletBoundaryConditions() {};
  ~vtkvmtkDirichletBoundaryConditions() {};

private:
  vtkvmtkDirichletBoundaryConditions(const vtkvmtkDirichletBoundaryConditions&);  // Not implemented.
  void operator=(const vtkvmtkDirichletBoundaryConditions&);  // Not implemented.
};

#endif

