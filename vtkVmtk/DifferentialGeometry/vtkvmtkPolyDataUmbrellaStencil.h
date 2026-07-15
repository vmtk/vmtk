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
 * @class   vtkvmtkPolyDataUmbrellaStencil
 * @brief   Implements a uniform (unweighted) Laplacian stencil for the one-ring neighborhood of a surface mesh.
 * point.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataUmbrellaStencil builds, for each mesh point, the simplest discrete Laplacian
 * ("umbrella") stencil: every neighbor in the point's one-ring receives an equal weight of 1/N
 * (N being the number of neighbors), and the center point receives a weight of -1 (subject to the
 * sign convention set by NegateWeights), so that applying the stencil yields the displacement
 * towards the unweighted average of the neighbors. This is the cheapest and most common stencil
 * used for surface smoothing / mesh relaxation (see vtkvmtkPolyDataStencilFlowFilter,
 * vtkvmtkStencils).
 *
 * @sa vtkvmtkPolyDataManifoldStencil, vtkvmtkStencils, vtkvmtkPolyDataStencilFlowFilter
 */

#ifndef __vtkvmtkPolyDataUmbrellaStencil_h
#define __vtkvmtkPolyDataUmbrellaStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataUmbrellaStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  static vtkvmtkPolyDataUmbrellaStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataUmbrellaStencil,vtkvmtkPolyDataManifoldStencil);

  /**
   * Return the stencil item type identifier (VTK_VMTK_UMBRELLA_STENCIL), used by
   * vtkvmtkStencils/vtkvmtkItems factories to instantiate the correct stencil subclass.
   */
  virtual vtkIdType GetItemType() override {return VTK_VMTK_UMBRELLA_STENCIL;};

  /**
   * Build the umbrella stencil at the current point: populate the one-ring neighbor weights with
   * 1/N each and the center weight with -1 (before sign/area scaling), where N is the number of
   * one-ring neighbors.
   */
  void Build() override;

protected:
  vtkvmtkPolyDataUmbrellaStencil();
  ~vtkvmtkPolyDataUmbrellaStencil() {};

  void ScaleWithArea() override {};

private:
  vtkvmtkPolyDataUmbrellaStencil(const vtkvmtkPolyDataUmbrellaStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataUmbrellaStencil&);  // Not implemented.
};

#endif

