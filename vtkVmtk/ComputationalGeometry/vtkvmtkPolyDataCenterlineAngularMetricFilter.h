/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineAngularMetricFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataCenterlineAngularMetricFilter - create an angular metric array on each branch of a split surface representing he periodic circumferential coordinate of mesh points around the centerlines.
// .SECTION Description
// Briefly, each segment of a vascular network is topologically equivalent to a cylinder and can consequently be mapped onto a rectangular parametric space that allows both easier investigations and comparisons between different models and datasets. The parameterization is performed longitudinally by means of the curvilinear abscissa computed over the model centerlines and circumferentially, by the angular position of each point on the surface mesh with respect to the centerlines.

#ifndef __vtkvmtkPolyDataCenterlineAngularMetricFilter_h
#define __vtkvmtkPolyDataCenterlineAngularMetricFilter_h

#include "vtkvmtkPolyDataCenterlineMetricFilter.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineAngularMetricFilter : public vtkvmtkPolyDataCenterlineMetricFilter
{
public:
  static vtkvmtkPolyDataCenterlineAngularMetricFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataCenterlineAngularMetricFilter,vtkvmtkPolyDataCenterlineMetricFilter);

  vtkSetStringMacro(CenterlineNormalsArrayName);
  vtkGetStringMacro(CenterlineNormalsArrayName);

protected:
  vtkvmtkPolyDataCenterlineAngularMetricFilter();
  ~vtkvmtkPolyDataCenterlineAngularMetricFilter();

  virtual void EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) override;

  char* CenterlineNormalsArrayName;

private:
  vtkvmtkPolyDataCenterlineAngularMetricFilter(const vtkvmtkPolyDataCenterlineAngularMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineAngularMetricFilter&);  // Not implemented.
};

#endif

