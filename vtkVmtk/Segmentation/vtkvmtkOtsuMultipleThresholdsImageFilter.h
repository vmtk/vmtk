/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkOtsuMultipleThresholdsImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

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

// .NAME vtkvmtkOtsuMultipleThresholdsImageFilter - Wrapper class around itk::OtsuMultipleThresholdsImageFilter
// .SECTION Description
// vtkvmtkOtsuMultipleThresholdsImageFilter


#ifndef __vtkvmtkOtsuMultipleThresholdsImageFilter_h
#define __vtkvmtkOtsuMultipleThresholdsImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkFloatArray;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkOtsuMultipleThresholdsImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkOtsuMultipleThresholdsImageFilter *New();
  vtkTypeMacro(vtkvmtkOtsuMultipleThresholdsImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(NumberOfHistogramBins,int);
  vtkSetMacro(NumberOfHistogramBins,int);

  vtkGetMacro(NumberOfThresholds,int);
  vtkSetMacro(NumberOfThresholds,int);

  vtkGetMacro(LabelOffset,int);
  vtkSetMacro(LabelOffset,int);

  vtkGetObjectMacro(Thresholds,vtkFloatArray);

protected:
  vtkvmtkOtsuMultipleThresholdsImageFilter();
  ~vtkvmtkOtsuMultipleThresholdsImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;
  virtual int RequestInformation(vtkInformation * vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;

private:
  vtkvmtkOtsuMultipleThresholdsImageFilter(const vtkvmtkOtsuMultipleThresholdsImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkOtsuMultipleThresholdsImageFilter&);  // Not implemented.

  int NumberOfHistogramBins;
  int NumberOfThresholds;
  int LabelOffset;
  vtkFloatArray* Thresholds;
};

#endif

