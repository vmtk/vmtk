/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 * @class   vtkvmtkConstrainedLaplacianPolyDataFilter
 * @brief   Smooth a surface mesh by constraining the motion of selected point ids.
 * @ingroup Segmentation
 *
 * vtkvmtkConstrainedLaplacianPolyDataFilter performs Laplacian smoothing of a manifold triangulated
 * surface (vtkPolyData), moving each free point toward the centroid of its 1-ring neighborhood, while
 * keeping the points listed in ConstrainedPointIds (and, unless BoundarySmoothing is on, all boundary
 * points) fixed. Each iteration is a two-step (shrink, then partially compensating unshrink) update
 * similar to Taubin smoothing, intended to reduce volume shrinkage relative to plain Laplacian
 * smoothing. Iteration stops after NumberOfIterations passes or as soon as the largest point
 * displacement observed in a pass falls below Convergence, whichever comes first.
 */

#ifndef __vtkvmtkConstrainedLaplacianPolyDataFilter_h
#define __vtkvmtkConstrainedLaplacianPolyDataFilter_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkIdList;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkConstrainedLaplacianPolyDataFilter : public vtkPolyDataToPolyDataFilter
{
  public: 
  vtkTypeMacro(vtkvmtkConstrainedLaplacianPolyDataFilter,vtkPolyDataToPolyDataFilter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkConstrainedLaplacianPolyDataFilter *New();

  ///@{
  /**
   * Set/get the convergence criterion: iteration stops early, before NumberOfIterations is reached,
   * as soon as the largest point displacement observed during a smoothing pass drops below this
   * value. Default: 0.0 (disabled, so smoothing always runs the full NumberOfIterations passes).
   */
  vtkSetMacro(Convergence, double);
  vtkGetMacro(Convergence, double);
  ///@}

  ///@{
  /**
   * Set/get the relaxation factor controlling the fraction of the Laplacian displacement applied at
   * each of the two per-iteration update steps (shrink and partial unshrink). Default: 0.0 (no
   * motion); typical values are small positive fractions.
   */
  vtkSetMacro(RelaxationFactor, double);
  vtkGetMacro(RelaxationFactor, double);
  ///@}

  ///@{
  /**
   * Set/get the maximum number of Laplacian smoothing iterations to perform. Default: a very large
   * integer (VTK_VMTK_LARGE_INTEGER), so in practice Convergence (if set) determines when smoothing
   * stops.
   */
  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);
  ///@}

  ///@{
  /**
   * Toggle smoothing of boundary points. When off (default), points on a free edge of the surface are
   * left untouched regardless of ConstrainedPointIds.
   */
  vtkSetMacro(BoundarySmoothing, int);
  vtkGetMacro(BoundarySmoothing, int);
  vtkBooleanMacro(BoundarySmoothing, int);
  ///@}

  ///@{
  /**
   * Set/get the ids of the points that must remain fixed (not be moved by smoothing), in addition to
   * boundary points when BoundarySmoothing is off. If not set (default), only boundary points (if
   * any) are constrained.
   */
  virtual void SetConstrainedPointIds(vtkIdList *);
  vtkGetObjectMacro(ConstrainedPointIds, vtkIdList);
  ///@}

  protected:
  vtkvmtkConstrainedLaplacianPolyDataFilter();
  ~vtkvmtkConstrainedLaplacianPolyDataFilter();  

  void Execute();

  vtkIdList *ConstrainedPointIds;
  
  double Convergence;
  double RelaxationFactor;
  int NumberOfIterations;
  int BoundarySmoothing;

  private:
  vtkvmtkConstrainedLaplacianPolyDataFilter(const vtkvmtkConstrainedLaplacianPolyDataFilter&);  // Not implemented.
  void operator=(const vtkvmtkConstrainedLaplacianPolyDataFilter&);  // Not implemented.
};

#endif
