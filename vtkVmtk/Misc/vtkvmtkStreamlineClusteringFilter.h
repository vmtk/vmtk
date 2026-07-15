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
 * @class   vtkvmtkStreamlineClusteringFilter
 * @brief   Cluster streamlines based on Mahalanobis distance metric and K-Means clustering.
 * @ingroup Misc
 *
 * Groups the input polyline cells (streamlines, e.g. as produced by particle tracing on a CFD
 * velocity field) into clusters of similar trajectories using K-Means clustering with a
 * Mahalanobis distance metric between resampled streamline point sequences. The output is a copy
 * of the input with an added cell data array identifying each streamline's cluster; the
 * representative center streamline of each cluster is available via GetClusterCenters().
 */

#ifndef __vtkvmtkStreamlineClusteringFilter_h
#define __vtkvmtkStreamlineClusteringFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineClusteringFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineClusteringFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkStreamlineClusteringFilter *New();
  
  /**
   * Get the representative center streamline of each cluster, one polyline cell per cluster, with
   * the cluster id stored in a "Label" cell data array. Valid only after Update() has been called.
   */
  vtkGetObjectMacro(ClusterCenters,vtkPolyData);

  protected:
  vtkvmtkStreamlineClusteringFilter();
  ~vtkvmtkStreamlineClusteringFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* ClusterCenters;

  private:
  vtkvmtkStreamlineClusteringFilter(const vtkvmtkStreamlineClusteringFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineClusteringFilter&);  // Not implemented.
};

#endif
