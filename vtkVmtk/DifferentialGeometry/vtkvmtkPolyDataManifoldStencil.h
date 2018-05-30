/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataManifoldStencil.h,v $
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
// .NAME vtkvmtkPolyDataManifoldStencil - Base class for stencils applied to a manifold surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataManifoldStencil_h
#define __vtkvmtkPolyDataManifoldStencil_h

#include "vtkObject.h"
#include "vtkvmtkStencil.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataManifoldStencil : public vtkvmtkStencil 
{
public:

  vtkTypeMacro(vtkvmtkPolyDataManifoldStencil,vtkvmtkStencil);

  vtkGetMacro(Area,double);

  // Description:
  // Build the stencil.
  virtual void Build() VTK_OVERRIDE;

  virtual void ComputeArea();
  virtual void ScaleWithArea() = 0;

  void DeepCopy(vtkvmtkPolyDataManifoldStencil *src);

  vtkGetMacro(UseExtendedNeighborhood,int);
  vtkSetMacro(UseExtendedNeighborhood,int);
  vtkBooleanMacro(UseExtendedNeighborhood,int);
  
protected:
  vtkvmtkPolyDataManifoldStencil();
  ~vtkvmtkPolyDataManifoldStencil() {};

  void ScaleWithAreaFactor(double factor);

  double Area;

  int UseExtendedNeighborhood;
  
private:
  vtkvmtkPolyDataManifoldStencil(const vtkvmtkPolyDataManifoldStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataManifoldStencil&);  // Not implemented.
};

#endif

