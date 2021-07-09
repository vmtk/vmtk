/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridFELaplaceAssembler.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkUnstructuredGridFELaplaceAssembler - Construct a laplacian based finite element calculation on a mesh.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridFELaplaceAssembler_h
#define __vtkvmtkUnstructuredGridFELaplaceAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFELaplaceAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFELaplaceAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFELaplaceAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

protected:
  vtkvmtkUnstructuredGridFELaplaceAssembler();
  ~vtkvmtkUnstructuredGridFELaplaceAssembler();

private:
  vtkvmtkUnstructuredGridFELaplaceAssembler(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
};

#endif
