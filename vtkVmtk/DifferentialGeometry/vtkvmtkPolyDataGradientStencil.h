/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkPolyDataGradientStencil
 * @brief   Apply gradient weighting to the neighborhood connections of a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataGradientStencil builds, for each mesh point, a 3-component-per-neighbor stencil
 * (NumberOfComponents == 3) whose weights, when dotted with the scalar values at the center point and
 * its one-ring neighbors, approximate the 3D surface gradient of that scalar field at the center
 * point. For every triangle incident to an edge of the one-ring, the (area-weighted) contribution of
 * each of its three vertices to the gradient is computed via the internal Gamma() helper --
 * essentially the standard piecewise-linear finite-element gradient formula, weighted by 1/(4*triangle
 * area) -- and accumulated into CenterWeight (contribution of the center point) and the neighbor
 * Weights. It is one of the stencil types built by vtkvmtkStencils and consumed, e.g., by gradient
 * estimation on a surface (see also the finite-element based vtkvmtkPolyDataGradientFilter).
 *
 * @sa vtkvmtkPolyDataManifoldStencil, vtkvmtkStencils, vtkvmtkPolyDataGradientFilter
 */

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

