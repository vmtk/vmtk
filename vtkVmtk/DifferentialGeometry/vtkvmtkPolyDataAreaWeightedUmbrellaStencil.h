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
 * @class   vtkvmtkPolyDataAreaWeightedUmbrellaStencil
 * @brief   Apply umbrella (laplacian) weighting to the neighborhood connections of a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataAreaWeightedUmbrellaStencil builds, for each mesh point, a discrete Laplacian
 * (umbrella-operator) stencil over its one-ring neighborhood, in which every neighbor initially
 * receives an equal weight (1/N) and is then re-weighted by the area of the two triangles
 * incident to the edge connecting it to the center point (see vtkvmtkMath::TriangleArea), summed
 * into CenterWeight/Area, and finally scaled by ScaleWithArea and sign-flipped so it approximates
 * the surface Laplace operator. It is one of the stencil types selectable through vtkvmtkStencils
 * and vtkvmtkPolyDataMeanCurvature (VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL) for mesh smoothing and
 * curvature estimation.
 *
 * @sa vtkvmtkPolyDataManifoldStencil, vtkvmtkStencils, vtkvmtkPolyDataMeanCurvature
 */

#ifndef __vtkvmtkPolyDataAreaWeightedUmbrellaStencil_h
#define __vtkvmtkPolyDataAreaWeightedUmbrellaStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataAreaWeightedUmbrellaStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  static vtkvmtkPolyDataAreaWeightedUmbrellaStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataAreaWeightedUmbrellaStencil,vtkvmtkPolyDataManifoldStencil);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL;};

  void Build() override;

protected:
  vtkvmtkPolyDataAreaWeightedUmbrellaStencil();
  ~vtkvmtkPolyDataAreaWeightedUmbrellaStencil() {};

  void ScaleWithArea() override;

private:
  vtkvmtkPolyDataAreaWeightedUmbrellaStencil(const vtkvmtkPolyDataAreaWeightedUmbrellaStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataAreaWeightedUmbrellaStencil&);  // Not implemented.
};

#endif

