/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFEGradientAssembler.h,v $
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
// .NAME vtkvmtkPolyDataFEGradientAssembler - Construct a gradient based finite element calculation on a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataFEGradientAssembler_h
#define __vtkvmtkPolyDataFEGradientAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFEGradientAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkPolyDataFEGradientAssembler* New();
  vtkTypeMacro(vtkvmtkPolyDataFEGradientAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

  vtkSetStringMacro(ScalarsArrayName);
  vtkGetStringMacro(ScalarsArrayName);

  vtkSetMacro(ScalarsComponent,int);
  vtkGetMacro(ScalarsComponent,int);

protected:
  vtkvmtkPolyDataFEGradientAssembler();
  ~vtkvmtkPolyDataFEGradientAssembler();

  char* ScalarsArrayName;
  int ScalarsComponent;

private:
  vtkvmtkPolyDataFEGradientAssembler(const vtkvmtkPolyDataFEGradientAssembler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFEGradientAssembler&);  // Not implemented.
};

#endif
