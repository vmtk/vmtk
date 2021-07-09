/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataGradientStencil.h,v $
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
// .NAME vtkvmtkPolyDataGradientStencil - Apply gradient weighting to the neighborhood connections of a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataGradientStencil_h
#define __vtkvmtkPolyDataGradientStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataGradientStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  static vtkvmtkPolyDataGradientStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataGradientStencil,vtkvmtkPolyDataManifoldStencil);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_GRADIENT_STENCIL;};
  
  void Build() override;

protected:
  vtkvmtkPolyDataGradientStencil();
  ~vtkvmtkPolyDataGradientStencil() {};

  void ScaleWithArea() override;

  void Gamma(double p0[3], double p1[3], double p2[3], double gamma[3]);
  
private:
  vtkvmtkPolyDataGradientStencil(const vtkvmtkPolyDataGradientStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataGradientStencil&);  // Not implemented.
};

#endif

