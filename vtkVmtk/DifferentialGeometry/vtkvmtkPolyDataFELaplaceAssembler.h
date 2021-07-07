/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFELaplaceAssembler.h,v $
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
// .NAME vtkvmtkPolyDataFELaplaceAssembler - construct a laplacian based finite element calculation on a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataFELaplaceAssembler_h
#define __vtkvmtkPolyDataFELaplaceAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFELaplaceAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkPolyDataFELaplaceAssembler* New();
  vtkTypeMacro(vtkvmtkPolyDataFELaplaceAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

protected:
  vtkvmtkPolyDataFELaplaceAssembler();
  ~vtkvmtkPolyDataFELaplaceAssembler();

private:
  vtkvmtkPolyDataFELaplaceAssembler(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
};

#endif
