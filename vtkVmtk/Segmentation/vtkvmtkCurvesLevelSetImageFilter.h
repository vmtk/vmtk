/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCurvesLevelSetImageFilter.h,v $
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

// .NAME vtkvmtkCurvesLevelSetImageFilter - Wrapper class around itk::CurvesLevelSetImageFilter
// .SECTION Description
// vtkvmtkCurvesLevelSetImageFilter


#ifndef __vtkvmtkCurvesLevelSetImageFilter_h
#define __vtkvmtkCurvesLevelSetImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCurvesLevelSetImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkCurvesLevelSetImageFilter *New();
  vtkTypeMacro(vtkvmtkCurvesLevelSetImageFilter, vtkSimpleImageToImageFilter);

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

  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);

  vtkGetMacro(AutoGenerateSpeedAdvection,int);
  vtkSetMacro(AutoGenerateSpeedAdvection,int);
  vtkBooleanMacro(AutoGenerateSpeedAdvection,int);

  vtkGetMacro(InterpolateSurfaceLocation,int);
  vtkSetMacro(InterpolateSurfaceLocation,int);
  vtkBooleanMacro(InterpolateSurfaceLocation,int);

  vtkGetMacro(DerivativeSigma,double);
  vtkSetMacro(DerivativeSigma,double);

  vtkGetObjectMacro(FeatureImage,vtkImageData);
  vtkSetObjectMacro(FeatureImage,vtkImageData);

  vtkGetObjectMacro(SpeedImage,vtkImageData);
  vtkSetObjectMacro(SpeedImage,vtkImageData);

  vtkGetMacro(RMSChange,double);

  vtkGetMacro(ElapsedIterations,int);

protected:

  vtkvmtkCurvesLevelSetImageFilter();
  ~vtkvmtkCurvesLevelSetImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkCurvesLevelSetImageFilter(const vtkvmtkCurvesLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCurvesLevelSetImageFilter&);  //
                                                                          // Not implemented

  double IsoSurfaceValue;
  int NumberOfIterations;
  double PropagationScaling;
  double CurvatureScaling;
  double AdvectionScaling;
  double MaximumRMSError;
  int UseNegativeFeatures;
  int UseImageSpacing;
  int AutoGenerateSpeedAdvection;
  int InterpolateSurfaceLocation;
  double DerivativeSigma;
  double RMSChange;
  int ElapsedIterations;

  vtkImageData* FeatureImage;
  vtkImageData* SpeedImage;
};

#endif




