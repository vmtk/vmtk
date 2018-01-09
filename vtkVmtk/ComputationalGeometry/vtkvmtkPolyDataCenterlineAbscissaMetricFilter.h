/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineAbscissaMetricFilter.h,v $
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
// .NAME vtkvmtkPolyDataCenterlineAbscissaMetricFilter - ..
// .SECTION Description
// ..

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

  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);

protected:
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter();
  ~vtkvmtkPolyDataCenterlineAbscissaMetricFilter();

  virtual void EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) VTK_OVERRIDE;

  char* AbscissasArrayName;

private:
  vtkvmtkPolyDataCenterlineAbscissaMetricFilter(const vtkvmtkPolyDataCenterlineAbscissaMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineAbscissaMetricFilter&);  // Not implemented.
};

#endif

