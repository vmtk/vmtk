/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineAngularMetricFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataCenterlineAngularMetricFilter - ..
// .SECTION Description
// ..

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

  virtual void EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) VTK_OVERRIDE;

  char* CenterlineNormalsArrayName;

private:
  vtkvmtkPolyDataCenterlineAngularMetricFilter(const vtkvmtkPolyDataCenterlineAngularMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineAngularMetricFilter&);  // Not implemented.
};

#endif

