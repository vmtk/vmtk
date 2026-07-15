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
 * @class   vtkvmtkGradientMagnitudeImageFilter
 * @brief   Wraps itk::GradientMagnitudeImageFilter.
 * @ingroup Segmentation
 *
 * Computes the magnitude of the gradient of the input image at each pixel using simple
 * finite-difference derivatives (unlike vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter, no
 * Gaussian smoothing/scale parameter is involved). No configurable parameters beyond the input
 * image itself.
 *
 * @sa vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter
 */

#ifndef __vtkvmtkGradientMagnitudeImageFilter_h
#define __vtkvmtkGradientMagnitudeImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGradientMagnitudeImageFilter *New();
  vtkTypeMacro(vtkvmtkGradientMagnitudeImageFilter, vtkSimpleImageToImageFilter);

protected:
  vtkvmtkGradientMagnitudeImageFilter() {};
  ~vtkvmtkGradientMagnitudeImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkGradientMagnitudeImageFilter(const vtkvmtkGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeImageFilter&);  // Not implemented.
};

#endif




