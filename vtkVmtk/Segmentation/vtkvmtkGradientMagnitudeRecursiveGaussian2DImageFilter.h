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
 * @class   vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter
 * @brief   Wraps itk::GradientMagnitudeRecursiveGaussian2DImageFilter.
 * @ingroup Segmentation
 *
 * The 2D counterpart of vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter: computes the
 * magnitude of the gradient of a single 2D image slice after Gaussian smoothing at scale Sigma,
 * using ITK's recursive Gaussian derivative implementation.
 *
 * @sa vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter
 */

#ifndef __vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter_h
#define __vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter *New();
  vtkTypeMacro(vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter, vtkSimpleImageToImageFilter);

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
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter();
  ~vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter(const vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter&);  // Not implemented.

  double Sigma;
  int NormalizeAcrossScale;
};

#endif

