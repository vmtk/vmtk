/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridFEGradientAssembler.h,v $
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
// .NAME vtkvmtkUnstructuredGridFEGradientAssembler - Construct a gradient based finite element calculation on a mesh.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridFEGradientAssembler_h
#define __vtkvmtkUnstructuredGridFEGradientAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFEGradientAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFEGradientAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFEGradientAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

  vtkSetStringMacro(ScalarsArrayName);
  vtkGetStringMacro(ScalarsArrayName);

  vtkSetMacro(ScalarsComponent,int);
  vtkGetMacro(ScalarsComponent,int);

  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);

  vtkSetMacro(AssemblyMode,int);
  vtkGetMacro(AssemblyMode,int);
  void SetAssemblyModeToGradient()
  { this->SetAssemblyMode(VTKVMTK_GRADIENTASSEMBLY); }
  void SetAssemblyModeToPartialDerivative()
  { this->SetAssemblyMode(VTKVMTK_PARTIALDERIVATIVEASSEMBLY); }

//BTX
  enum {
    VTKVMTK_GRADIENTASSEMBLY,
    VTKVMTK_PARTIALDERIVATIVEASSEMBLY
  };
//ETX

protected:
  vtkvmtkUnstructuredGridFEGradientAssembler();
  ~vtkvmtkUnstructuredGridFEGradientAssembler();

  void BuildGradient();
  void BuildPartialDerivative();

  char* ScalarsArrayName;
  int ScalarsComponent;
  int AssemblyMode;
  int Direction;

private:
  vtkvmtkUnstructuredGridFEGradientAssembler(const vtkvmtkUnstructuredGridFEGradientAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFEGradientAssembler&);  // Not implemented.
};

#endif
