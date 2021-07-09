/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridFEVorticityAssembler.h,v $
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
// .NAME vtkvmtkUnstructuredGridFEVorticityAssembler - Construct a vorticity based finite element calculation on a mesh.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridFEVorticityAssembler_h
#define __vtkvmtkUnstructuredGridFEVorticityAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFEVorticityAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFEVorticityAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFEVorticityAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);

  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);

protected:
  vtkvmtkUnstructuredGridFEVorticityAssembler();
  ~vtkvmtkUnstructuredGridFEVorticityAssembler();

  char* VelocityArrayName;
  int Direction;

private:
  vtkvmtkUnstructuredGridFEVorticityAssembler(const vtkvmtkUnstructuredGridFEVorticityAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFEVorticityAssembler&);  // Not implemented.
};

#endif
