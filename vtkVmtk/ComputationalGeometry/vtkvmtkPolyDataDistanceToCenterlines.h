/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDistanceToCenterlines.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataDistanceToCenterlines - calculate the minimum euclidian from surface points to a centerline.
// .SECTION Description
// This function has three distinct ways of working:
// 1) UseRadiusInformation: 1 (default=1) -> Compute the euclidian distance between a surface point and the closest centerline point (based on polyball association).
// 2) EvaluateTubeFunction: 1 (default=0) -> Compute the euclidian distance between a surface point and the center of the tube function (based on polyball line).
// 3) EvaluateCenterlineRadius: 1 (default=0) -> Find the centerline point which is closest to a surface point (similar to method 1), and set distance at that surface point to the radius of the sphere associated with the closest centerline point id.
// By setting ProjectPointArrays: 1 (default=0) -> Project point data from the centerline onto every surface point by linearly interpolating the relative position of the surface point on the line formed by the two closest centerline points.

#ifndef __vtkvmtkPolyDataDistanceToCenterlines_h
#define __vtkvmtkPolyDataDistanceToCenterlines_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataDistanceToCenterlines : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataDistanceToCenterlines* New();
  vtkTypeMacro(vtkvmtkPolyDataDistanceToCenterlines,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  vtkSetMacro(EvaluateTubeFunction,int);
  vtkGetMacro(EvaluateTubeFunction,int);
  vtkBooleanMacro(EvaluateTubeFunction,int);

  vtkSetMacro(EvaluateCenterlineRadius,int);
  vtkGetMacro(EvaluateCenterlineRadius,int);
  vtkBooleanMacro(EvaluateCenterlineRadius,int);

  vtkSetStringMacro(DistanceToCenterlinesArrayName);
  vtkGetStringMacro(DistanceToCenterlinesArrayName);

  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);

  vtkSetMacro(ProjectPointArrays,int);
  vtkGetMacro(ProjectPointArrays,int);
  vtkBooleanMacro(ProjectPointArrays,int);

protected:
  vtkvmtkPolyDataDistanceToCenterlines();
  ~vtkvmtkPolyDataDistanceToCenterlines();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* DistanceToCenterlinesArrayName;
  char* CenterlineRadiusArrayName;

  vtkPolyData* Centerlines;

  int UseRadiusInformation;
  int EvaluateTubeFunction;
  int EvaluateCenterlineRadius;
  int ProjectPointArrays;

private:
  vtkvmtkPolyDataDistanceToCenterlines(const vtkvmtkPolyDataDistanceToCenterlines&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDistanceToCenterlines&);  // Not implemented.
};

#endif

