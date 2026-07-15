/*=========================================================================

Program:   VMTK

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

/**
 * @class   vtkvmtkOtsuMultipleThresholdsImageFilter
 * @brief   Wraps itk::OtsuMultipleThresholdsImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkOtsuMultipleThresholdsImageFilter automatically labels an image into NumberOfThresholds+1
 * intensity classes by computing NumberOfThresholds thresholds that maximize the between-class
 * variance of the image histogram (Otsu's method, generalized to multiple thresholds), producing an
 * unsigned short label image and the vector of computed threshold values (retrievable with
 * GetThresholds). It is the filter behind the vmtkimageotsuthresholds pype script, typically used
 * for coarse, unsupervised segmentation/labeling of an image prior to further refinement (e.g. as
 * an initialization for level set segmentation). Like the other ITK wrappers in this module, it is
 * a thin vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to a float itk::Image,
 * runs itk::OtsuMultipleThresholdsImageFilter, and converts the unsigned short label output back to
 * vtkImageData; RequestInformation sets the output scalar type to VTK_UNSIGNED_SHORT.
 */

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

  ///@{
  /**
   * Set/get the number of bins used to build the image intensity histogram from which the Otsu
   * thresholds are computed. Default: 128.
   */
  vtkGetMacro(NumberOfHistogramBins,int);
  vtkSetMacro(NumberOfHistogramBins,int);
  ///@}

  ///@{
  /**
   * Set/get the number of intensity thresholds to compute; the output label image will have
   * NumberOfThresholds+1 distinct label values. Default: 1.
   */
  vtkGetMacro(NumberOfThresholds,int);
  vtkSetMacro(NumberOfThresholds,int);
  ///@}

  ///@{
  /**
   * Set/get the lowest label value used when generating the output labeled image (label values are
   * consecutive integers starting at LabelOffset). Default: 0.
   */
  vtkGetMacro(LabelOffset,int);
  vtkSetMacro(LabelOffset,int);
  ///@}

  /**
   * Get the array of NumberOfThresholds intensity threshold values computed by the most recent
   * Update(), in increasing order.
   */
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

