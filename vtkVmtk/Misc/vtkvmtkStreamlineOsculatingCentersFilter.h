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
 * @class   vtkvmtkStreamlineOsculatingCentersFilter
 * @brief   Compute the osculating (center-of-curvature) points of a set of streamlines.
 * @ingroup Misc
 *
 * Resamples each input polyline (streamline) cell to a fixed point spacing, then, for every
 * interior point, computes the center of the osculating circle defined by it and its two
 * neighbors (i.e. the local center of curvature) and its radius, storing the result as vertex
 * points in the output GetOsculatingCenters() poly data (with "Label" and "Radius" point data
 * arrays). If a VoronoiDiagram is supplied, each osculating center is additionally tagged with the
 * id of the nearest Voronoi "sheet" (VoronoiSheetIdsArrayName), associating the local curvature
 * behavior of the flow with the underlying medial-surface structure of the vessel.
 */

#ifndef __vtkvmtkStreamlineOsculatingCentersFilter_h
#define __vtkvmtkStreamlineOsculatingCentersFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineOsculatingCentersFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineOsculatingCentersFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkStreamlineOsculatingCentersFilter *New();

  ///@{
  /**
   * Set/Get an optional Voronoi diagram (e.g. as computed during centerline extraction) used to tag
   * each osculating center with its nearest Voronoi "sheet" id. If not set, no sheet-id tagging is
   * performed.
   */
  vtkSetObjectMacro(VoronoiDiagram,vtkPolyData);
  vtkGetObjectMacro(VoronoiDiagram,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of VoronoiDiagram holding each cell's sheet id; also the
   * name used for the corresponding output point data array on GetOsculatingCenters().
   */
  vtkSetStringMacro(VoronoiSheetIdsArrayName);
  vtkGetStringMacro(VoronoiSheetIdsArrayName);
  ///@}

  /**
   * Get the computed osculating centers, one vertex point per interior point of every resampled
   * input streamline, with "Label" (source streamline id) and "Radius" (osculating circle radius)
   * point data arrays, plus VoronoiSheetIdsArrayName if VoronoiDiagram was supplied. Valid only
   * after Update() has been called.
   */
  vtkGetObjectMacro(OsculatingCenters,vtkPolyData);

  protected:
  vtkvmtkStreamlineOsculatingCentersFilter();
  ~vtkvmtkStreamlineOsculatingCentersFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* VoronoiDiagram;
  char* VoronoiSheetIdsArrayName;
  
  vtkPolyData* OsculatingCenters;

  private:
  vtkvmtkStreamlineOsculatingCentersFilter(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
};

#endif
