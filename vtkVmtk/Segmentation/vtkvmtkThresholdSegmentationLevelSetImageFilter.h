/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkThresholdSegmentationLevelSetImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .NAME vtkvmtkThresholdSegmentationLevelSetImageFilter - Wrapper class around itk::ThresholdSegmentationLevelSetImageFilter
// .SECTION Description
// vtkvmtkThresholdSegmentationLevelSetImageFilter


#ifndef __vtkvmtkThresholdSegmentationLevelSetImageFilter_h
#define __vtkvmtkThresholdSegmentationLevelSetImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkThresholdSegmentationLevelSetImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkThresholdSegmentationLevelSetImageFilter *New();
  vtkTypeMacro(vtkvmtkThresholdSegmentationLevelSetImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(UpperThreshold,double);
  vtkSetMacro(UpperThreshold,double);

  vtkGetMacro(LowerThreshold,double);
  vtkSetMacro(LowerThreshold,double);

  vtkGetMacro(EdgeWeight,double);
  vtkSetMacro(EdgeWeight,double);

  vtkGetMacro(SmoothingIterations,int);
  vtkSetMacro(SmoothingIterations,int);

  vtkGetMacro(SmoothingTimeStep,double);
  vtkSetMacro(SmoothingTimeStep,double);

  vtkGetMacro(SmoothingConductance,double);
  vtkSetMacro(SmoothingConductance,double);

  vtkGetMacro(IsoSurfaceValue,double);
  vtkSetMacro(IsoSurfaceValue,double);

  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);

  vtkGetMacro(PropagationScaling,double);
  vtkSetMacro(PropagationScaling,double);

  vtkGetMacro(CurvatureScaling,double);
  vtkSetMacro(CurvatureScaling,double);

  vtkGetMacro(AdvectionScaling,double);
  vtkSetMacro(AdvectionScaling,double);

  vtkGetMacro(MaximumRMSError,double);
  vtkSetMacro(MaximumRMSError,double);

  vtkGetMacro(UseNegativeFeatures,int);
  vtkSetMacro(UseNegativeFeatures,int);
  vtkBooleanMacro(UseNegativeFeatures,int);

  vtkGetMacro(AutoGenerateSpeedAdvection,int);
  vtkSetMacro(AutoGenerateSpeedAdvection,int);
  vtkBooleanMacro(AutoGenerateSpeedAdvection,int);

  vtkGetMacro(InterpolateSurfaceLocation,int);
  vtkSetMacro(InterpolateSurfaceLocation,int);
  vtkBooleanMacro(InterpolateSurfaceLocation,int);

  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);

  vtkGetObjectMacro(FeatureImage,vtkImageData);
  vtkSetObjectMacro(FeatureImage,vtkImageData);

  vtkGetObjectMacro(SpeedImage,vtkImageData);
  vtkSetObjectMacro(SpeedImage,vtkImageData);

  vtkGetMacro(RMSChange,double);

  vtkGetMacro(ElapsedIterations,int);

protected:
  vtkvmtkThresholdSegmentationLevelSetImageFilter();
  ~vtkvmtkThresholdSegmentationLevelSetImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkThresholdSegmentationLevelSetImageFilter(const vtkvmtkThresholdSegmentationLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkThresholdSegmentationLevelSetImageFilter&); // Not implemented

  double UpperThreshold;
  double LowerThreshold;
  double EdgeWeight;
  int SmoothingIterations;
  double SmoothingTimeStep;
  double SmoothingConductance;
  double IsoSurfaceValue;
  int NumberOfIterations;
  double PropagationScaling;
  double CurvatureScaling;
  double AdvectionScaling;
  double MaximumRMSError;
  int UseNegativeFeatures;
  int AutoGenerateSpeedAdvection;
  int InterpolateSurfaceLocation;
  int UseImageSpacing;
  double RMSChange;
  int ElapsedIterations;
  vtkImageData* FeatureImage;
  vtkImageData* SpeedImage;
};

#endif




