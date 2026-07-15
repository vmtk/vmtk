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
 * @class   vtkvmtkPolyDataPatchingFilter
 * @brief   Cut a branched, longitudinally/angularly mapped surface into a grid of quadrilateral patches, averaging point data arrays over each patch.
 * @ingroup ComputationalGeometry
 *
 * Given a surface whose points already carry a longitudinal (abscissa) mapping and, optionally, a
 * circular (angular) mapping array -- typically computed per-branch by vtkvmtkPolyDataStretchMappingFilter
 * or an equivalent reference-system-based metric -- and a branch GroupIdsArrayName, this filter cuts
 * each branch into a regular grid of patches following iso-contours of these two mapping arrays.
 * Patch dimensions are set through PatchSize (longitudinal height and circular fraction of 2*pi).
 * All point data arrays are area-averaged over each patch and written both onto the output surface
 * (as cell data, one value per patch) and into PatchedData, a vtkImageData whose pixels correspond to
 * patches (one axis per branch/longitudinal slab, the other per circular sector), which is convenient
 * for aligning and statistically comparing this quantity across a population of vessels mapped onto
 * the same parametric grid. This is the filter behind the vmtkbranchpatching pype script.
 *
 * @sa
 * vtkvmtkPolyDataBranchUtilities, vtkvmtkPolyDataStretchMappingFilter
 */

#ifndef __vtkvmtkPolyDataPatchingFilter_h
#define __vtkvmtkPolyDataPatchingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkImageData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataPatchingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataPatchingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataPatchingFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the longitudinal (abscissa)
   * mapping used to cut patches along the branch direction. Required input.
   * Commonly named "AbscissaMetric".
   */
  vtkSetStringMacro(LongitudinalMappingArrayName);
  vtkGetStringMacro(LongitudinalMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the circular (angular, in
   * radians over (-pi, +pi]) mapping used to cut patches around the branch circumference. Required
   * input when CircularPatching is on.
   * Commonly named "AngularMetric".
   */
  vtkSetStringMacro(CircularMappingArrayName);
  vtkGetStringMacro(CircularMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the branch group id of each
   * point; patches are cut and their longitudinal bounds computed independently for each group.
   * Required input.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array (also written to PatchedData) where the
   * longitudinal patch (slab) index of each patch is stored.
   * Commonly named "Slab".
   */
  vtkSetStringMacro(LongitudinalPatchNumberArrayName);
  vtkGetStringMacro(LongitudinalPatchNumberArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array (also written to PatchedData) where the circular
   * patch (sector) index of each patch is stored.
   * Commonly named "Sector".
   */
  vtkSetStringMacro(CircularPatchNumberArrayName);
  vtkGetStringMacro(CircularPatchNumberArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array (also written to PatchedData) where the surface
   * area of each patch is stored.
   * Commonly named "PatchArea".
   */
  vtkSetStringMacro(PatchAreaArrayName);
  vtkGetStringMacro(PatchAreaArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the size of each patch: PatchSize[0] is the longitudinal extent (in the units of
   * LongitudinalMappingArrayName, e.g. mm of arc length), PatchSize[1] is the circular extent
   * expressed as a fraction of the full circle (2*pi); e.g. 1.0/N to cut N equal sectors. Default: (0,0).
   */
  vtkSetVector2Macro(PatchSize,double);
  vtkGetVectorMacro(PatchSize,double,2);
  ///@}

  ///@{
  /**
   * Set/Get the longitudinal and circular offsets (in the same units as PatchSize) added to the patch
   * grid origin, shifting where the first longitudinal/circular patch boundary falls. Default: (0,0).
   */
  vtkSetVector2Macro(PatchOffsets,double);
  vtkGetVectorMacro(PatchOffsets,double,2);
  ///@}

  ///@{
  /**
   * Set/Get the longitudinal mapping range, per branch, over which patches are cut. If left at the
   * default (0,0), the full range of LongitudinalMappingArrayName on each branch is used.
   */
  vtkSetVector2Macro(LongitudinalPatchBounds,double);
  vtkGetVectorMacro(LongitudinalPatchBounds,double,2);
  ///@}

  ///@{
  /**
   * Set/Get the circular mapping range (in radians) over which patches are cut, used when
   * CircularPatching is on. If left at the default (0,0), the full circle (-pi, +pi) is used.
   */
  vtkSetVector2Macro(CircularPatchBounds,double);
  vtkGetVectorMacro(CircularPatchBounds,double,2);
  ///@}

  ///@{
  /**
   * Set/Get the image data holding, one pixel per patch (longitudinal slabs stacked along one axis,
   * circular sectors along the other), the area-averaged value of every point data array of the input
   * surface, plus LongitudinalPatchNumberArrayName, CircularPatchNumberArrayName and
   * PatchAreaArrayName. Populated by this filter; not meant to be set directly by the caller.
   */
  vtkSetObjectMacro(PatchedData,vtkImageData);
  vtkGetObjectMacro(PatchedData,vtkImageData);
  ///@}

  ///@{
  /**
   * Toggle cutting patches in the circular direction as well as the longitudinal one. When off, each
   * branch is only cut into longitudinal slabs (one patch per slab) and CircularMappingArrayName is
   * not required. Default: on.
   */
  vtkSetMacro(CircularPatching,int);
  vtkGetMacro(CircularPatching,int);
  vtkBooleanMacro(CircularPatching,int);
  ///@}

  ///@{
  /**
   * Toggle restricting each cut patch to its largest connected region (via
   * vtkPolyDataConnectivityFilter) before averaging, discarding smaller disconnected fragments that
   * can result from clipping a non-convex cross-section. Default: on.
   */
  vtkSetMacro(UseConnectivity,int);
  vtkGetMacro(UseConnectivity,int);
  vtkBooleanMacro(UseConnectivity,int);
  ///@}

protected:
  vtkvmtkPolyDataPatchingFilter();
  ~vtkvmtkPolyDataPatchingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* LongitudinalMappingArrayName;
  char* CircularMappingArrayName;
  char* GroupIdsArrayName;

  char* LongitudinalPatchNumberArrayName;
  char* CircularPatchNumberArrayName;

  char* PatchAreaArrayName;

  double PatchSize[2];
  double PatchOffsets[2];
  double LongitudinalPatchBounds[2];
  double CircularPatchBounds[2];

  vtkImageData* PatchedData;

  int CircularPatching;
  int UseConnectivity;

private:
  vtkvmtkPolyDataPatchingFilter(const vtkvmtkPolyDataPatchingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataPatchingFilter&);  // Not implemented.
};

#endif

