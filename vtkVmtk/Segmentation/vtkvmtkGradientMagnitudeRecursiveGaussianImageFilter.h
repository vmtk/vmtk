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
 * @class   vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter
 * @brief   Wraps itk::GradientMagnitudeRecursiveGaussianImageFilter.
 * @ingroup Segmentation
 *
 * Computes the magnitude of the gradient of the input image after Gaussian smoothing at scale
 * Sigma, using ITK's efficient IIR (recursive) Gaussian derivative implementation rather than a
 * discrete convolution kernel. Commonly used to build a speed/feature image for level-set
 * segmentation (see vtkvmtkGeodesicActiveContourLevelSetImageFilter).
 *
 * @sa vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter, vtkvmtkGradientMagnitudeImageFilter
 */

#ifndef __vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter_h
#define __vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter *New();
  vtkTypeMacro(vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/Get the standard deviation of the Gaussian used to smooth the image before differentiating,
   * in the same physical units as the image spacing.
   */
  vtkGetMacro(Sigma,double);
  vtkSetMacro(Sigma,double);
  ///@}

  ///@{
  /**
   * Toggle normalizing the derivative across scale space (ITK's SetNormalizeAcrossScale), which
   * keeps gradient magnitudes comparable across different Sigma values. Default: off.
   */
  vtkGetMacro(NormalizeAcrossScale,int);
  vtkSetMacro(NormalizeAcrossScale,int);
  vtkBooleanMacro(NormalizeAcrossScale,int);
  ///@}

protected:
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter();
  ~vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter(const vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter&);  // Not implemented.

  double Sigma;
  int NormalizeAcrossScale;
};

#endif

