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
 * @class   vtkvmtkPolyDataCenterlineMetricFilter
 * @brief   Serves as the base class for evaluating metrics over a centerline onto a surface.
 * @ingroup ComputationalGeometry
 *
 * For each point of the input surface (already split into branches), finds the nearest point on the
 * corresponding branch of Centerlines and evaluates a per-point scalar metric there, storing the
 * result in the output point data array named MetricArrayName. The actual metric is defined by
 * subclasses overriding EvaluateMetric -- see vtkvmtkPolyDataCenterlineAbscissaMetricFilter (arc
 * length along the centerline) and vtkvmtkPolyDataCenterlineAngularMetricFilter (circumferential
 * angle around the centerline). Used by the vmtkbranchmetrics pype script to build the longitudinal/
 * circumferential parameterization used for centerline-based surface mapping and patching.
 *
 * @sa
 * vtkvmtkPolyDataCenterlineAbscissaMetricFilter, vtkvmtkPolyDataCenterlineAngularMetricFilter,
 * vtkvmtkPolyDataStretchMappingFilter, vtkvmtkPolyDataPatchingFilter
 */

#ifndef __vtkvmtkPolyDataCenterlineMetricFilter_h
#define __vtkvmtkPolyDataCenterlineMetricFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class vtkDataArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineMetricFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkvmtkPolyDataCenterlineMetricFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/Get the name of the output point data array where the evaluated metric is stored.
   * Commonly named "AbscissaMetric".
   */
  vtkSetStringMacro(MetricArrayName);
  vtkGetStringMacro(MetricArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input surface holding the branch group id of each
   * cell. Required input.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the split, grouped centerlines corresponding to the input surface. Required input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Toggle using the maximum inscribed sphere radius (RadiusArrayName) when locating the nearest
   * centerline point to each surface point, so that the search accounts for local vessel size rather
   * than pure Euclidean distance. Default: on.
   */
  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point. Used when UseRadiusInformation is on.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the branch group id of each cell.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the id of the original, unsplit
   * centerline that each cell belongs to.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the tract id of each cell.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding, for each cell, whether it is a
   * "blanked" (redundant, overlapping) tract.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Toggle including blanked (bifurcation-region) centerline tracts when locating the nearest
   * centerline point to each surface point. Default: on.
   */
  vtkSetMacro(IncludeBifurcations,int);
  vtkGetMacro(IncludeBifurcations,int);
  vtkBooleanMacro(IncludeBifurcations,int);
  ///@}

protected:
  vtkvmtkPolyDataCenterlineMetricFilter();
  ~vtkvmtkPolyDataCenterlineMetricFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  virtual void EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) = 0;

  char* MetricArrayName;
  char* BlankingArrayName;

  char* GroupIdsArrayName;

  vtkPolyData* Centerlines;

  char* RadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;

  int UseRadiusInformation;
  int IncludeBifurcations;

private:
  vtkvmtkPolyDataCenterlineMetricFilter(const vtkvmtkPolyDataCenterlineMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineMetricFilter&);  // Not implemented.
};

#endif

