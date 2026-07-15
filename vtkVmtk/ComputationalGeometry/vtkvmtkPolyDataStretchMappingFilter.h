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
 * @class   vtkvmtkPolyDataStretchMappingFilter
 * @brief   Reparametrize a harmonic longitudinal mapping (0 to 1 per branch) into a "stretched" mapping proportional to a physical longitudinal metric (e.g. arc length), correcting the distortion the harmonic map introduces near bifurcation insertion regions.
 * @ingroup ComputationalGeometry
 *
 * A harmonic longitudinal mapping (as produced e.g. by
 * vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter, in HarmonicMappingArrayName) increases
 * monotonically from 0 to 1 along each branch, but not linearly with physical distance: near a
 * bifurcation, where a branch's cross-section is not a simple circle (the "insertion region"), the
 * harmonic potential varies slowly, compressing that part of the branch in mapping space. This filter
 * corrects for that by walking a sequence of harmonic-mapping iso-contours along each branch,
 * averaging a physical longitudinal metric (MetricArrayName, e.g. centerline abscissa mapped onto the
 * surface) over each contour, and using the resulting (harmonic value, physical metric) pairs to build
 * a piecewise stretch function that is then evaluated at every surface point to produce
 * StretchedMappingArrayName -- a longitudinal coordinate that tracks the physical metric more closely
 * than the raw harmonic mapping, while remaining well-behaved across the whole branch including its
 * two boundaries. This is the filter behind the stretching step of the vmtkbranchmapping pype script.
 *
 * @sa
 * vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter, vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter
 */

#ifndef __vtkvmtkPolyDataStretchMappingFilter_h
#define __vtkvmtkPolyDataStretchMappingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataStretchMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataStretchMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataStretchMappingFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/Get the name of the output point data array where the stretched longitudinal mapping is
   * stored.
   * Commonly named "StretchedMapping".
   */
  vtkSetStringMacro(StretchedMappingArrayName);
  vtkGetStringMacro(StretchedMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input point data array holding the harmonic longitudinal mapping (in the
   * 0 to 1 range, per branch) to be stretched. Required input.
   * Commonly named "HarmonicMapping".
   */
  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input point data array holding the physical longitudinal metric (e.g.
   * centerline abscissa mapped onto the surface) that the stretched mapping is built to track along
   * harmonic-mapping iso-contours. Required input.
   * Commonly named "AbscissaMetric".
   */
  vtkSetStringMacro(MetricArrayName);
  vtkGetStringMacro(MetricArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input point data array holding the physical metric value to enforce at
   * each of a branch's two open boundaries, used when UseBoundaryMetric is on (typically the output
   * of vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter).
   * Commonly named "BoundaryMetric".
   */
  vtkSetStringMacro(BoundaryMetricArrayName);
  vtkGetStringMacro(BoundaryMetricArrayName);
  ///@}

  ///@{
  /**
   * Toggle using BoundaryMetricArrayName to fix the physical metric value at each branch boundary
   * (on), instead of the extremal value of MetricArrayName found on the boundary itself (off).
   * Default: off.
   */
  vtkSetMacro(UseBoundaryMetric,int);
  vtkGetMacro(UseBoundaryMetric,int);
  vtkBooleanMacro(UseBoundaryMetric,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the branch group id of each
   * point; the stretch function is built and applied independently for each branch. Required input.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the fraction, of the physical metric range spanned near each branch boundary, that is
   * excluded from the range of contour samples used to build the stretch function. This discards
   * samples too close to a bifurcation's insertion region, where the metric-vs-harmonic-mapping
   * relationship can be non-monotonic. Default: 2.0.
   */
  vtkSetMacro(MetricBoundsGapFactor,double);
  vtkGetMacro(MetricBoundsGapFactor,double);
  ///@}

protected:
  vtkvmtkPolyDataStretchMappingFilter();
  ~vtkvmtkPolyDataStretchMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* StretchedMappingArrayName;

  char* HarmonicMappingArrayName;
  char* GroupIdsArrayName;

  char* MetricArrayName;
  char* BoundaryMetricArrayName;

  int UseBoundaryMetric;

  double MetricBoundsGapFactor;

private:
  vtkvmtkPolyDataStretchMappingFilter(const vtkvmtkPolyDataStretchMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataStretchMappingFilter&);  // Not implemented.
};

#endif

