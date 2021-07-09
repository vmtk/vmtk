/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFVFELaplaceBeltramiStencil.h,v $
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
// .NAME vtkvmtkPolyDataFVFELaplaceBeltramiStencil - Apply finite-volume, finite-element LB weighting to the neighborhood connections of a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataFVFELaplaceBeltramiStencil_h
#define __vtkvmtkPolyDataFVFELaplaceBeltramiStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataLaplaceBeltramiStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFVFELaplaceBeltramiStencil : public vtkvmtkPolyDataLaplaceBeltramiStencil
{
public:

  static vtkvmtkPolyDataFVFELaplaceBeltramiStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataFVFELaplaceBeltramiStencil,vtkvmtkPolyDataLaplaceBeltramiStencil);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL;};

protected:
  vtkvmtkPolyDataFVFELaplaceBeltramiStencil();
  ~vtkvmtkPolyDataFVFELaplaceBeltramiStencil() {};

  void ComputeArea(vtkPolyData *data, vtkIdType pointId);
  void ScaleWithArea() override;

private:
  vtkvmtkPolyDataFVFELaplaceBeltramiStencil(const vtkvmtkPolyDataFVFELaplaceBeltramiStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFVFELaplaceBeltramiStencil&);  // Not implemented.
};

#endif

