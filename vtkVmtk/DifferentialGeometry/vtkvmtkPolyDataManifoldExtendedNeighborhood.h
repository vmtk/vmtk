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
 * @class   vtkvmtkPolyDataManifoldExtendedNeighborhood
 * @brief   Apply a an extended neighborhood to the points of a manifold surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataManifoldExtendedNeighborhood extends vtkvmtkPolyDataManifoldNeighborhood by
 * enlarging the interior (non-boundary) one-ring neighborhood of a point when the ring only has 3 or
 * 4 points: for each edge of the ring whose two incident triangles both lie on the "outward" side of
 * the edge relative to the center point (tested via a dot-product sign check on the edge and the two
 * candidate outer points), the extra point across that edge is appended to the neighborhood. This
 * produces a wider (extended) stencil support, which is used, e.g., by
 * vtkvmtkPolyDataFELaplaceBeltramiStencil / vtkvmtkPolyDataFVFELaplaceBeltramiStencil (via
 * vtkvmtkPolyDataManifoldStencil::UseExtendedNeighborhood) to improve the accuracy of
 * Laplace-Beltrami weight computation on coarse or nearly-degenerate meshes.
 *
 * @sa vtkvmtkPolyDataManifoldNeighborhood, vtkvmtkPolyDataManifoldStencil
 */

#ifndef __vtkvmtkPolyDataManifoldExtendedNeighborhood_h
#define __vtkvmtkPolyDataManifoldExtendedNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"
#include "vtkPolyData.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataManifoldExtendedNeighborhood : public vtkvmtkPolyDataManifoldNeighborhood
{
public:

  static vtkvmtkPolyDataManifoldExtendedNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataManifoldExtendedNeighborhood,vtkvmtkPolyDataManifoldNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_POLYDATA_MANIFOLD_EXTENDED_NEIGHBORHOOD;};

  /**
   * Build the neighborhood.
   */
  virtual void Build() override;

protected:
  vtkvmtkPolyDataManifoldExtendedNeighborhood() {};
  ~vtkvmtkPolyDataManifoldExtendedNeighborhood() {};

private:
  vtkvmtkPolyDataManifoldExtendedNeighborhood(const vtkvmtkPolyDataManifoldExtendedNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataManifoldExtendedNeighborhood&);  // Not implemented.
};

#endif

