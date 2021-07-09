/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFELaplaceBeltramiStencil.h,v $
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
// .NAME vtkvmtkPolyDataFELaplaceBeltramiStencil - Apply finite-element LB weighting to the neighborhood connections of a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataFELaplaceBeltramiStencil_h
#define __vtkvmtkPolyDataFELaplaceBeltramiStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataLaplaceBeltramiStencil.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFELaplaceBeltramiStencil : public vtkvmtkPolyDataLaplaceBeltramiStencil
{
public:

  static vtkvmtkPolyDataFELaplaceBeltramiStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataFELaplaceBeltramiStencil,vtkvmtkPolyDataLaplaceBeltramiStencil);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL;};

protected:
  vtkvmtkPolyDataFELaplaceBeltramiStencil();
  ~vtkvmtkPolyDataFELaplaceBeltramiStencil() {};

  void ScaleWithArea() override;

private:
  vtkvmtkPolyDataFELaplaceBeltramiStencil(const vtkvmtkPolyDataFELaplaceBeltramiStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFELaplaceBeltramiStencil&);  // Not implemented.
};

#endif

