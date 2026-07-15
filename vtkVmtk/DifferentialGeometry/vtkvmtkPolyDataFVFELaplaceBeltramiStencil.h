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
 * @class   vtkvmtkPolyDataFVFELaplaceBeltramiStencil
 * @brief   Apply finite-volume, finite-element LB weighting to the neighborhood connections of a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataFVFELaplaceBeltramiStencil is a specialization of
 * vtkvmtkPolyDataLaplaceBeltramiStencil (which builds the cotangent-weight discrete Laplace-Beltrami
 * stencil, see vtkvmtkPolyDataLaplaceBeltramiStencil::Build) that recomputes the normalizing area
 * (ComputeArea) as a proper Voronoi finite-volume mixed area: for each incident triangle it adds the
 * Voronoi sector area if the triangle is non-obtuse, or half/quarter of the triangle area if it is
 * obtuse at the center point / at another vertex respectively (see vtkvmtkMath::IsTriangleObtuse,
 * vtkvmtkMath::VoronoiSectorArea). ScaleWithArea then divides the cotangent weights by one third of
 * this finite-volume area. UseExtendedNeighborhood is enabled by default. It is one of the stencil
 * types selectable through vtkvmtkStencils and vtkvmtkPolyDataMeanCurvature
 * (VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL) for mesh smoothing and curvature estimation.
 *
 * @sa vtkvmtkPolyDataLaplaceBeltramiStencil, vtkvmtkPolyDataFELaplaceBeltramiStencil,
 *     vtkvmtkStencils, vtkvmtkPolyDataMeanCurvature
 */

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

