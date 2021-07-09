/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineMetricFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.8 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataCenterlineMetricFilter - Base class for evaluating metrics over a centerline onto a surface. 
// .SECTION Description
// Pure virtual functions.

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

  vtkSetStringMacro(MetricArrayName);
  vtkGetStringMacro(MetricArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetMacro(IncludeBifurcations,int);
  vtkGetMacro(IncludeBifurcations,int);
  vtkBooleanMacro(IncludeBifurcations,int);

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

