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
 * @class   vtkvmtkPolyDataStencilFlowFilter
 * @brief   Displace points of a surface with an iterative algorithm based on stencil weighting.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataStencilFlowFilter repeatedly rebuilds, at every point of the input surface, a
 * local stencil (see StencilType) that expresses each point's position as a weighted combination
 * of its neighbors, and moves the point towards that weighted average, scaled by
 * RelaxationFactor, for NumberOfIterations iterations. Depending on the chosen stencil this
 * implements Laplacian ("umbrella") smoothing, area-weighted smoothing, or finite-element/
 * finite-volume Laplace-Beltrami flows. Displacement per iteration can be capped
 * (MaximumDisplacement), boundary points can be left untouched or processed (ProcessBoundary),
 * and the moved points can optionally be reprojected onto the original surface
 * (ConstrainOnSurface) to keep the flow tangential rather than deforming the shape. This
 * underlies mesh-relaxation / smoothing steps used internally by higher-level vmtk surface
 * processing (no direct pype script wraps it).
 *
 * @sa vtkvmtkStencils, vtkvmtkStencil, vtkvmtkPolyDataUmbrellaStencil
 */

#ifndef __vtkvmtkPolyDataStencilFlowFilter_h
#define __vtkvmtkPolyDataStencilFlowFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkConstants.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkvmtkStencils;

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataStencilFlowFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataStencilFlowFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataStencilFlowFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the type of stencil used to compute, at each point, the weighted neighbor combination
   * that drives the point displacement. One of the VTK_VMTK_*_STENCIL constants (see
   * vtkvmtkConstants.h); convenience setters SetStencilTypeToUmbrellaStencil(),
   * SetStencilTypeToAreaWeightedUmbrellaStencil(), SetStencilTypeToFELaplaceBeltramiStencil() and
   * SetStencilTypeToFVFELaplaceBeltramiStencil() are provided. Default: VTK_VMTK_UMBRELLA_STENCIL.
   */
  vtkSetMacro(StencilType,int);
  vtkGetMacro(StencilType,int);
  ///@}
  void SetStencilTypeToUmbrellaStencil()
    {this->SetStencilType(VTK_VMTK_UMBRELLA_STENCIL);};
  void SetStencilTypeToAreaWeightedUmbrellaStencil() 
    {this->SetStencilType(VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL);};
  void SetStencilTypeToFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL);};
  void SetStencilTypeToFVFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL);};

  ///@{
  /**
   * Set/get the number of relaxation iterations applied. At each iteration the stencil is rebuilt
   * on the current (already displaced) point positions. Default: 100.
   */
  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the fraction of the stencil-weighted displacement applied to each point at every
   * iteration; smaller values relax more gently and are more stable but converge more slowly.
   * Default: 0.1.
   */
  vtkSetMacro(RelaxationFactor,double);
  vtkGetMacro(RelaxationFactor,double);
  ///@}

  ///@{
  /**
   * Toggle whether points lying on a boundary of the surface are displaced. When off (default),
   * boundary points are left untouched so the outline of the surface is preserved; when on, they
   * are moved like any other point using their (open) stencil.
   */
  vtkSetMacro(ProcessBoundary,int);
  vtkGetMacro(ProcessBoundary,int);
  vtkBooleanMacro(ProcessBoundary,int);
  ///@}

  ///@{
  /**
   * Toggle projection of each displaced point back onto the closest point of the original input
   * surface (using a cell locator), so that the flow moves points tangentially along the surface
   * rather than deforming its shape. Default: off.
   */
  vtkSetMacro(ConstrainOnSurface,int);
  vtkGetMacro(ConstrainOnSurface,int);
  vtkBooleanMacro(ConstrainOnSurface,int);
  ///@}

  ///@{
  /**
   * Set/get the maximum length of the per-iteration displacement vector applied to any point;
   * larger displacements are clamped (direction preserved, magnitude capped) before being
   * applied. Default: VTK_VMTK_LARGE_DOUBLE (effectively unbounded).
   */
  vtkSetMacro(MaximumDisplacement,double);
  vtkGetMacro(MaximumDisplacement,double);
  ///@}

protected:
  vtkvmtkPolyDataStencilFlowFilter();
  ~vtkvmtkPolyDataStencilFlowFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ReleaseStencils();
  
  int StencilType;
  vtkvmtkStencils* Stencils;

  int NumberOfIterations;
  double RelaxationFactor;

  double MaximumDisplacement;

  int ProcessBoundary;
  int ConstrainOnSurface;
  
private:
  vtkvmtkPolyDataStencilFlowFilter(const vtkvmtkPolyDataStencilFlowFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataStencilFlowFilter&);  // Not implemented.
};

#endif

