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
 * @class   vtkvmtkPolyDataCenterlineAbscissaMetricFilter
 * @brief   Project the abscissa metric of the nearest centerline point to every point of a surface.
 * @ingroup ComputationalGeometry
 *
 * Briefly, each segment of a vascular network is topologically equivalent to a cylinder and can consequently be mapped onto a rectangular parametric space that allows both easier investigations and comparisons between different models and datasets. The parameterization is performed longitudinally by means of the curvilinear abscissa computed over the model centerlines and circumferentially, by the angular position of each point on the surface mesh with respect to the centerlines.
 *
 * Requires Centerlines to already carry an abscissa point data array (see
 * vtkvmtkCenterlineAttributesFilter). This is the "longitudinal" half of the vmtkbranchmetrics pype
 * script.
 *
 * @sa
 * vtkvmtkPolyDataCenterlineMetricFilter, vtkvmtkPolyDataCenterlineAngularMetricFilter,
 * vtkvmtkCenterlineAttributesFilter
 */

#ifndef __vtkvmtkPolyDataCenterlineAbscissaMetricFilter_h
#define __vtkvmtkPolyDataCenterlineAbscissaMetricFilter_h

#include "vtkvmtkPolyDataCenterlineMetricFilter.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineAbscissaMetricFilter : public vtkvmtkPolyDataCenterlineMetricFilter
{
public:
  static vtkvmtkPolyDataCenterlineAbscissaMetricFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataCenterlineAbscissaMetricFilter,vtkvmtkPolyDataCenterlineMetricFilter);

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the (precomputed) centerline
   * abscissa to be projected onto the surface. Required input; see
   * vtkvmtkCenterlineAttributesFilter.
   * Commonly named "Abscissas".
   */
  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);
  ///@}

protected:
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter();
  ~vtkvmtkPolyDataCenterlineAbscissaMetricFilter();

  virtual void EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) override;

  char* AbscissasArrayName;

private:
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter(const vtkvmtkPolyDataCenterlineAbscissaMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineAbscissaMetricFilter&);  // Not implemented.
};

#endif

