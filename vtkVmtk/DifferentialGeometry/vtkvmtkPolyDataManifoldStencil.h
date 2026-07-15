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
 * @class   vtkvmtkPolyDataManifoldStencil
 * @brief   Serves as the base class for stencils applied to a manifold surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataManifoldStencil is the common base class for the discrete differential-operator
 * stencils in this module (umbrella/Laplacian, Laplace-Beltrami, gradient, ...) that operate on a
 * point of a manifold triangulated surface (vtkPolyData). Build() constructs the underlying one-ring
 * neighborhood (vtkvmtkPolyDataManifoldNeighborhood, or vtkvmtkPolyDataManifoldExtendedNeighborhood
 * when UseExtendedNeighborhood is on) for DataSetPointId and allocates the Weights /
 * CenterWeight arrays (see vtkvmtkStencil) that subclasses fill in with operator-specific values;
 * ComputeArea computes the barycentric mixed area of the one-ring (stored in Area, retrievable with
 * GetArea) used by subclasses (via the protected ScaleWithAreaFactor helper, exposed as the pure
 * virtual ScaleWithArea) to normalize their weights.
 *
 * @sa vtkvmtkPolyDataManifoldNeighborhood, vtkvmtkPolyDataManifoldExtendedNeighborhood,
 *     vtkvmtkPolyDataLaplaceBeltramiStencil, vtkvmtkPolyDataGradientStencil,
 *     vtkvmtkPolyDataAreaWeightedUmbrellaStencil
 */

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

  /**
   * Get the barycentric mixed area of the point's one-ring neighborhood, as computed by the most
   * recent call to ComputeArea() (normally invoked internally by Build()).
   */
  vtkGetMacro(Area,double);

  /**
   * Build the stencil.
   */
  virtual void Build() override;

  /**
   * Compute the barycentric mixed area of the point's one-ring neighborhood (sum of the incident
   * triangle areas) and store it in Area. Called internally by Build(); subclasses that need a
   * different area normalization (e.g. a proper Voronoi mixed area) override this method.
   */
  virtual void ComputeArea();

  /**
   * Normalize the stencil's Weights/CenterWeight by the area computed by ComputeArea(), typically
   * via the protected ScaleWithAreaFactor helper. Must be implemented by every concrete stencil
   * subclass; called internally at the end of Build().
   */
  virtual void ScaleWithArea() = 0;

  /**
   * Deep copy the state (including Area) of src into this stencil, in addition to the base class
   * state copied by vtkvmtkStencil::DeepCopy.
   */
  void DeepCopy(vtkvmtkPolyDataManifoldStencil *src);

  ///@{
  /**
   * Set/get whether Build() uses the extended (second-ring, see
   * vtkvmtkPolyDataManifoldExtendedNeighborhood) rather than plain one-ring neighborhood to
   * construct the stencil. Default: on.
   */
  vtkGetMacro(UseExtendedNeighborhood,int);
  vtkSetMacro(UseExtendedNeighborhood,int);
  vtkBooleanMacro(UseExtendedNeighborhood,int);
  ///@}
  
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

