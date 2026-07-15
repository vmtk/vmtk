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
 * @class   vtkvmtkPolyDataClampedSmoothingFilter
 * @brief   Apply a laplacian or curvature based smoothing filter to a surface with particular points fixed in place.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataClampedSmoothingFilter iteratively displaces the points of an input triangulated
 * surface, either along the discrete Laplacian (umbrella) direction (SmoothingType ==
 * LAPLACIAN) or along the surface normal scaled by the local mean curvature (SmoothingType ==
 * CURVATURE_DIFFUSION, using vtkCurvatures and vtkPolyDataNormals internally), for
 * NumberOfIterations iterations. At each iteration the (explicit-Euler) time step is computed from
 * the smallest triangle area in the mesh (see ComputeTimeStep) scaled by TimeStepFactor, so that the
 * scheme stays stable. Points whose value in the point data array named ClampArrayName exceeds
 * ClampThreshold are left untouched when Clamp is on, which lets the caller pin down selected
 * regions (e.g. inlet/outlet rings) while the rest of the surface is smoothed; points found to be on
 * a mesh boundary are always left untouched. This is a lower-level building block used where surface
 * smoothing must not move certain fixed points, unlike filters that smooth the whole surface
 * uniformly.
 *
 * @sa vtkvmtkPolyDataManifoldNeighborhood
 */

#ifndef __vtkvmtkPolyDataClampedSmoothingFilter_h
#define __vtkvmtkPolyDataClampedSmoothingFilter_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataClampedSmoothingFilter : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataClampedSmoothingFilter *New();
  vtkTypeMacro(vtkvmtkPolyDataClampedSmoothingFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the smoothing scheme applied at each iteration: LAPLACIAN (the default) displaces each
   * free point towards the unweighted average of its one-ring neighbors; CURVATURE_DIFFUSION
   * displaces each free point along its surface normal, driven by the local variation of the mean
   * curvature (a curvature-diffusion / mean-curvature-flow-like scheme).
   */
  vtkSetMacro(SmoothingType,int);
  vtkGetMacro(SmoothingType,int);
  void SetSmoothingTypeToLaplacian()
  { this->SmoothingType = LAPLACIAN; }
  void SetSmoothingTypeToCurvatureDiffusion()
  { this->SmoothingType = CURVATURE_DIFFUSION; }
  ///@}

  ///@{
  /**
   * Set/get the number of smoothing iterations to perform. Default: 100.
   */
  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Toggle whether points are clamped (excluded from smoothing) based on ClampArrayName /
   * ClampThreshold. When off (default), every non-boundary point is free to move.
   */
  vtkSetMacro(Clamp,int);
  vtkGetMacro(Clamp,int);
  vtkBooleanMacro(Clamp,int);
  ///@}

  ///@{
  /**
   * Set/get the threshold applied to the ClampArrayName point data array: points whose value in
   * that array is greater than ClampThreshold are held fixed when Clamp is on. Default: 0.0.
   */
  vtkSetMacro(ClampThreshold,double);
  vtkGetMacro(ClampThreshold,double);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data scalar array read to decide, together with ClampThreshold,
   * which points are held fixed while Clamp is on. Required (and must exist on the input) whenever
   * Clamp is on. Default: NULL.
   */
  vtkSetStringMacro(ClampArrayName);
  vtkGetStringMacro(ClampArrayName);
  ///@}

  ///@{
  /**
   * Set/get the factor multiplying the per-iteration time step (itself derived from the smallest
   * triangle area in the mesh, see ComputeTimeStep) used to advance point positions. Larger values
   * smooth faster but risk instability/mesh folding; smaller values are safer but slower. Default:
   * 1E-1.
   */
  vtkSetMacro(TimeStepFactor,double);
  vtkGetMacro(TimeStepFactor,double);
  ///@}

//BTX
  enum {
    LAPLACIAN,
    CURVATURE_DIFFUSION
  };
//ETX

protected:
  vtkvmtkPolyDataClampedSmoothingFilter();
  ~vtkvmtkPolyDataClampedSmoothingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  void LaplacianIteration(vtkPolyData* surface);
  void CurvatureDiffusionIteration(vtkPolyData* surface);
  double ComputeTimeStep(vtkPolyData* surface);

  int SmoothingType;
  int NumberOfIterations;
  int Clamp;
  double ClampThreshold;
  double TimeStepFactor;

  char* ClampArrayName;

private:
  vtkvmtkPolyDataClampedSmoothingFilter(const vtkvmtkPolyDataClampedSmoothingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataClampedSmoothingFilter&);  // Not implemented.
};

#endif

