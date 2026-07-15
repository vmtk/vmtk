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
 * @class   vtkvmtkPolyDataFELaplaceBeltramiStencil
 * @brief   Apply finite-element LB weighting to the neighborhood connections of a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataFELaplaceBeltramiStencil is a thin specialization of
 * vtkvmtkPolyDataLaplaceBeltramiStencil (which builds the cotangent-weight discrete Laplace-Beltrami
 * stencil, see vtkvmtkPolyDataLaplaceBeltramiStencil::Build) that overrides the area-normalization
 * step (ScaleWithArea) to divide the cotangent weights by one third of the barycentric mixed area
 * (Area/3), matching the mass-lumped finite-element (FE) discretization of the Laplace-Beltrami
 * operator. UseExtendedNeighborhood is enabled by default so that the underlying neighborhood used
 * to build the stencil includes second-ring points where needed. It is one of the stencil types
 * selectable through vtkvmtkStencils and vtkvmtkPolyDataMeanCurvature
 * (VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL) for mesh smoothing and curvature estimation.
 *
 * @sa vtkvmtkPolyDataLaplaceBeltramiStencil, vtkvmtkPolyDataFVFELaplaceBeltramiStencil,
 *     vtkvmtkStencils, vtkvmtkPolyDataMeanCurvature
 */

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

