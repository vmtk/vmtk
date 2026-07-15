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
 * @class   vtkvmtkPolyDataDistanceToCenterlines
 * @brief   Calculate the minimum Euclidean distance from surface points to a centerline.
 * @ingroup ComputationalGeometry
 *
 * This function has three distinct ways of working:
 * 1) UseRadiusInformation: 1 (default=1) -> Compute the euclidian distance between a surface point and the closest centerline point (based on polyball association).
 * 2) EvaluateTubeFunction: 1 (default=0) -> Compute the euclidian distance between a surface point and the center of the tube function (based on polyball line).
 * 3) EvaluateCenterlineRadius: 1 (default=0) -> Find the centerline point which is closest to a surface point (similar to method 1), and set distance at that surface point to the radius of the sphere associated with the closest centerline point id.
 * By setting ProjectPointArrays: 1 (default=0) -> Project point data from the centerline onto every surface point by linearly interpolating the relative position of the surface point on the line formed by the two closest centerline points.
 */

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

  ///@{
  /**
   * Set/Get the centerlines to measure distance to. Required input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Toggle using the per-point sphere radii (CenterlineRadiusArrayName) when locating the closest
   * centerline point via polyball association, so the search accounts for local vessel size rather
   * than pure Euclidean distance. Default: on. See the class description for how this interacts with
   * EvaluateTubeFunction and EvaluateCenterlineRadius.
   */
  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);
  ///@}

  ///@{
  /**
   * Toggle computing the distance from each surface point to the center of the polyball-line tube
   * function (i.e. the interpolated centerline medial axis) rather than to the nearest discrete
   * centerline point. Default: off.
   */
  vtkSetMacro(EvaluateTubeFunction,int);
  vtkGetMacro(EvaluateTubeFunction,int);
  vtkBooleanMacro(EvaluateTubeFunction,int);
  ///@}

  ///@{
  /**
   * Toggle outputting, at each surface point, the maximum inscribed sphere radius of the closest
   * centerline point instead of the actual distance to it. Default: off.
   */
  vtkSetMacro(EvaluateCenterlineRadius,int);
  vtkGetMacro(EvaluateCenterlineRadius,int);
  vtkBooleanMacro(EvaluateCenterlineRadius,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the computed distance (or radius, see
   * EvaluateCenterlineRadius) is stored.
   * Commonly named "DistanceToCenterlines".
   */
  vtkSetStringMacro(DistanceToCenterlinesArrayName);
  vtkGetStringMacro(DistanceToCenterlinesArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point. Used when UseRadiusInformation or EvaluateCenterlineRadius is on.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);
  ///@}

  ///@{
  /**
   * Toggle projecting every other point data array present on Centerlines onto the surface, linearly
   * interpolated between the two closest centerline points, in addition to computing
   * DistanceToCenterlinesArrayName. Default: off.
   */
  vtkSetMacro(ProjectPointArrays,int);
  vtkGetMacro(ProjectPointArrays,int);
  vtkBooleanMacro(ProjectPointArrays,int);
  ///@}

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

