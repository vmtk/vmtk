/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter - Assists in mapping and patching workflow by computing boundary metrics in the boundary reference system frame.
// .SECTION Description
// ...

#ifndef __vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter_h
#define __vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class vtkDataArray;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter,vtkPolyDataAlgorithm);

  vtkSetStringMacro(BoundaryMetricArrayName);
  vtkGetStringMacro(BoundaryMetricArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetStringMacro(CenterlineAbscissasArrayName);
  vtkGetStringMacro(CenterlineAbscissasArrayName);

  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);

  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);

  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);

  vtkSetStringMacro(ReferenceSystemGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemGroupIdsArrayName);

protected:
  vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter();
  ~vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* BoundaryMetricArrayName;

  char* GroupIdsArrayName;

  vtkPolyData* Centerlines;
  char* CenterlineAbscissasArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;

  vtkPolyData* ReferenceSystems;
  char* ReferenceSystemGroupIdsArrayName;

private:
  vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter(const vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter&);  // Not implemented.
};

#endif

