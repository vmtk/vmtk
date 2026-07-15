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
 * @class   vtkvmtkPolyDataManifoldNeighborhood
 * @brief   Apply a neighborhood to the points of a manifold surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataManifoldNeighborhood computes the ordered one-ring neighborhood (the points
 * directly connected by an edge) of a point (DataSetPointId) on a manifold triangulated surface: it
 * walks around the point from cell to cell via shared edges (vtkPolyData::GetCellEdgeNeighbors),
 * collecting neighbor point ids in angular order, until it returns to the start (interior point,
 * IsBoundary false) or runs out of triangles on one side and must walk the remaining triangles from
 * the other side (point on an open mesh boundary, IsBoundary true, in which case the neighbor ids are
 * reordered so the two boundary neighbors end up first/last). This is the neighborhood used by
 * vtkvmtkPolyDataManifoldStencil::Build (and by vtkvmtkPolyDataManifoldExtendedNeighborhood, which
 * enlarges it further) to build discrete differential operators (Laplacian, gradient, curvature) on
 * a surface mesh.
 *
 * @sa vtkvmtkPolyDataManifoldExtendedNeighborhood, vtkvmtkPolyDataManifoldStencil
 */

#ifndef __vtkvmtkPolyDataManifoldNeighborhood_h
#define __vtkvmtkPolyDataManifoldNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkPolyData.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataManifoldNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkPolyDataManifoldNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataManifoldNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_POLYDATA_MANIFOLD_NEIGHBORHOOD;};

  /**
   * Build the neighborhood.
   */
  virtual void Build() override;

protected:
  vtkvmtkPolyDataManifoldNeighborhood() {};
  ~vtkvmtkPolyDataManifoldNeighborhood() {};

private:
  vtkvmtkPolyDataManifoldNeighborhood(const vtkvmtkPolyDataManifoldNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataManifoldNeighborhood&);  // Not implemented.
};

#endif

