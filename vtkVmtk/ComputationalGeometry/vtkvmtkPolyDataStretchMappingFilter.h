/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataStretchMappingFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataStretchMappingFilter - Constructs a harmonic function over each vascular segment which is used to map and stretch the longitudinal metric so as to correctly account for insertion regions present at bifurcations.
// .SECTION Description
// ...

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

  vtkSetStringMacro(StretchedMappingArrayName);
  vtkGetStringMacro(StretchedMappingArrayName);

  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);

  vtkSetStringMacro(MetricArrayName);
  vtkGetStringMacro(MetricArrayName);

  vtkSetStringMacro(BoundaryMetricArrayName);
  vtkGetStringMacro(BoundaryMetricArrayName);

  vtkSetMacro(UseBoundaryMetric,int);
  vtkGetMacro(UseBoundaryMetric,int);
  vtkBooleanMacro(UseBoundaryMetric,int);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetMacro(MetricBoundsGapFactor,double);
  vtkGetMacro(MetricBoundsGapFactor,double);

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

