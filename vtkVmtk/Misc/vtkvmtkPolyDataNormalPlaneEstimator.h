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
 * @class   vtkvmtkPolyDataNormalPlaneEstimator
 * @brief   Estimate the plane that locally cuts a tubular surface transversally at a given
 * seed point.
 * @ingroup Misc
 *
 * Given a seed point (OriginPointId) on a tubular surface (e.g. a vessel wall), this filter
 * estimates the normal of the plane that is locally perpendicular to the tube's axis at that
 * point, together with the point's coordinates (Origin). It works purely from the surface
 * point normals: starting from the local average edge length at the seed, it grows a search
 * radius around Origin and, for every neighboring point whose normal is sufficiently different
 * from (not nearly parallel or nearly antiparallel to) the normal at Origin, accumulates the
 * cross product of the two normals into a running estimate of the plane normal; the estimate
 * for the smallest radius that gathers at least MinimumNeighborhoodSize neighbors and produces
 * a consistent (non-conflicting) cross-product direction is kept. If the input already has
 * point normals they are reused, otherwise they are computed internally with
 * vtkPolyDataNormals.
 *
 * This is used by the vmtksurfaceendclipper pype script to compute, at a manually or
 * automatically picked seed point on a vessel end, the origin and normal of the plane fed to
 * vtkvmtkTopologicalSeamFilter and vtkClipPolyData in order to cut the vessel end
 * perpendicularly to its axis.
 *
 * @sa
 * vtkvmtkTopologicalSeamFilter
 */

#ifndef __vtkvmtkPolyDataNormalPlaneEstimator_h
#define __vtkvmtkPolyDataNormalPlaneEstimator_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataNormalPlaneEstimator : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkPolyDataNormalPlaneEstimator,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataNormalPlaneEstimator *New();

  ///@{
  /**
   * Set/Get the id, in the input poly data, of the point at which the transversal plane is
   * estimated. Must be set before Update() is called; default is -1 (unset), which causes
   * RequestData to report an error and return without computing Origin/Normal.
   */
  vtkSetMacro(OriginPointId,vtkIdType);
  vtkGetMacro(OriginPointId,vtkIdType);
  ///@}

  /**
   * Get the coordinates of OriginPointId, copied directly from the input poly data. Valid only
   * after Update() has been called.
   */
  vtkGetVectorMacro(Origin,double,3);

  /**
   * Get the estimated unit normal of the plane that locally cuts the tubular surface
   * transversally at Origin, obtained by accumulating cross products of surface point normals
   * over a neighborhood grown around Origin. Valid only after Update() has been called.
   */
  vtkGetVectorMacro(Normal,double,3);

  ///@{
  /**
   * Toggle whether the neighborhood search used to estimate the plane normal is restricted to
   * points belonging to the same connected region as OriginPointId (as determined by
   * vtkPolyDataConnectivityFilter). When on, points from other, disconnected parts of the input
   * are ignored even if they fall within the current search radius. Default: on.
   */
  vtkSetMacro(UseConnectivity,int);
  vtkGetMacro(UseConnectivity,int);
  vtkBooleanMacro(UseConnectivity,int);
  ///@}

  ///@{
  /**
   * Set/Get the minimum number of neighboring points that must fall within the current search
   * radius (grown iteratively in steps of the local average edge length at OriginPointId)
   * before that radius is accepted for estimating the plane normal. Default: 10.
   */
  vtkSetMacro(MinimumNeighborhoodSize,int);
  vtkGetMacro(MinimumNeighborhoodSize,int);
  ///@}
 
  protected:
  vtkvmtkPolyDataNormalPlaneEstimator();
  ~vtkvmtkPolyDataNormalPlaneEstimator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdType OriginPointId;

  double Origin[3];
  double Normal[3];

  int UseConnectivity;
  int MinimumNeighborhoodSize;

  private:
  vtkvmtkPolyDataNormalPlaneEstimator(const vtkvmtkPolyDataNormalPlaneEstimator&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataNormalPlaneEstimator&);  // Not implemented.
};

#endif
