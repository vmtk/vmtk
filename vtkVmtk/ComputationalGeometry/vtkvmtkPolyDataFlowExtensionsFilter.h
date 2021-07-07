/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataFlowExtensionsFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/07 10:46:19 $
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
// .NAME vtkvmtkPolyDataFlowExtensionsFilter - remove the end caps of a surface at flow inlets/outlets and append a cylinder with matching radius that follows the centerline path.  
// .SECTION Description
//  Various options are explained below:
//  - ExtensionMode: method for computing the normal for extension, can be "centerlinedirection" or "boundarynormal"
//  - InterpolationMode: method for computing interpolation from the model section to a circular section, can be "linear" or "thinplatespline"

#ifndef __vtkvmtkPolyDataFlowExtensionsFilter_h
#define __vtkvmtkPolyDataFlowExtensionsFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataFlowExtensionsFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataFlowExtensionsFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataFlowExtensionsFilter *New();
  
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetMacro(ExtensionRatio,double);
  vtkGetMacro(ExtensionRatio,double);

  vtkSetMacro(ExtensionLength,double);
  vtkGetMacro(ExtensionLength,double);

  vtkSetMacro(ExtensionRadius,double);
  vtkGetMacro(ExtensionRadius,double);

  vtkSetMacro(TransitionRatio,double);
  vtkGetMacro(TransitionRatio,double);

  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);

  vtkSetMacro(CenterlineNormalEstimationDistanceRatio,double);
  vtkGetMacro(CenterlineNormalEstimationDistanceRatio,double);

  vtkSetMacro(AdaptiveExtensionLength,int);
  vtkGetMacro(AdaptiveExtensionLength,int);
  vtkBooleanMacro(AdaptiveExtensionLength,int);

  vtkSetMacro(AdaptiveExtensionRadius,int);
  vtkGetMacro(AdaptiveExtensionRadius,int);
  vtkBooleanMacro(AdaptiveExtensionRadius,int);

  vtkSetMacro(NumberOfBoundaryPoints,int);
  vtkGetMacro(NumberOfBoundaryPoints,int);

  vtkSetMacro(AdaptiveNumberOfBoundaryPoints,int);
  vtkGetMacro(AdaptiveNumberOfBoundaryPoints,int);
  vtkBooleanMacro(AdaptiveNumberOfBoundaryPoints,int);

  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);

  vtkSetMacro(ExtensionMode,int);
  vtkGetMacro(ExtensionMode,int);
  void SetExtensionModeToUseNormalToBoundary()
  { this->SetExtensionMode(USE_NORMAL_TO_BOUNDARY); }
  void SetExtensionModeToUseCenterlineDirection()
  { this->SetExtensionMode(USE_CENTERLINE_DIRECTION); }

  vtkSetMacro(InterpolationMode,int);
  vtkGetMacro(InterpolationMode,int);
  void SetInterpolationModeToLinear()
  { this->SetInterpolationMode(USE_LINEAR_INTERPOLATION); }
  void SetInterpolationModeToThinPlateSpline()
  { this->SetInterpolationMode(USE_THIN_PLATE_SPLINE_INTERPOLATION); }

//BTX
  enum {
    USE_NORMAL_TO_BOUNDARY = 0,
    USE_CENTERLINE_DIRECTION
  };

  enum {
    USE_LINEAR_INTERPOLATION = 0,
    USE_THIN_PLATE_SPLINE_INTERPOLATION
  };
//ETX

  protected:
  vtkvmtkPolyDataFlowExtensionsFilter();
  ~vtkvmtkPolyDataFlowExtensionsFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* Centerlines;

  double ExtensionRatio;
  double ExtensionLength;
  double ExtensionRadius;

  double TransitionRatio;
  double Sigma;

  double CenterlineNormalEstimationDistanceRatio;

  int AdaptiveExtensionLength;
  int AdaptiveExtensionRadius;

  int NumberOfBoundaryPoints;
  int AdaptiveNumberOfBoundaryPoints;

  int ExtensionMode;
  int InterpolationMode;

  vtkIdList* BoundaryIds;

  private:
  vtkvmtkPolyDataFlowExtensionsFilter(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
};

#endif
