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
 * @class   vtkvmtkPolyDataLaplaceBeltramiStencil
 * @brief   Apply LB weighting to the neighborhood connections of a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataLaplaceBeltramiStencil builds, for each mesh point, the classic cotangent-weight
 * discrete Laplace-Beltrami stencil over its one-ring neighborhood: for every edge of the ring, the
 * cotangent of the angle opposite that edge (in each of the (up to) two triangles sharing it) is
 * accumulated as the weight of the corresponding neighbor (see vtkvmtkMath::Cotangent), and the
 * total is stored (with sign flipped) in CenterWeight after area-normalization (ComputeArea /
 * ScaleWithArea, overridden by subclasses to select finite-element or finite-volume normalization).
 * Special handling (BuildBoundaryWeights) is applied when the point lies on an open mesh boundary,
 * where the missing outer triangle is approximated using the boundary edge geometry. This is the
 * base class for vtkvmtkPolyDataFELaplaceBeltramiStencil and
 * vtkvmtkPolyDataFVFELaplaceBeltramiStencil, which only differ in how the normalizing area is
 * computed; it is used, e.g., by vtkvmtkPolyDataMeanCurvature to estimate mean curvature.
 *
 * @sa vtkvmtkPolyDataFELaplaceBeltramiStencil, vtkvmtkPolyDataFVFELaplaceBeltramiStencil,
 *     vtkvmtkPolyDataManifoldStencil, vtkvmtkPolyDataMeanCurvature
 */

#ifndef __vtkvmtkPolyDataLaplaceBeltramiStencil_h
#define __vtkvmtkPolyDataLaplaceBeltramiStencil_h

#include "vtkObject.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataLaplaceBeltramiStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  vtkTypeMacro(vtkvmtkPolyDataLaplaceBeltramiStencil,vtkvmtkPolyDataManifoldStencil);

  void Build() override;

  /**
   * Compute the pair of cotangent-like weights contributed by the single triangle formed by the
   * center point (DataSetPointId), boundaryPointId and boundaryNeighborPointId, used by Build() to
   * approximate the missing second triangle at an open mesh boundary edge. boundaryWeight and
   * boundaryNeighborWeight are output parameters, filled in with the weight to add to
   * boundaryPointId and boundaryNeighborPointId respectively. No-op if the stencil's center point is
   * not on a boundary (IsBoundary is false).
   */
  void BuildBoundaryWeights(vtkIdType boundaryPointId, vtkIdType boundaryNeighborPointId, double &boundaryWeight, double &boundaryNeighborWeight);

protected:
  vtkvmtkPolyDataLaplaceBeltramiStencil();
  ~vtkvmtkPolyDataLaplaceBeltramiStencil() {};

private:
  vtkvmtkPolyDataLaplaceBeltramiStencil(const vtkvmtkPolyDataLaplaceBeltramiStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataLaplaceBeltramiStencil&);  // Not implemented.
};

#endif

