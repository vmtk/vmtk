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
 * @class   vtkvmtkUpwindGradientMagnitudeImageFilter
 * @brief   Wraps itk::UpwindGradientMagnitudeImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkUpwindGradientMagnitudeImageFilter computes the gradient magnitude of the input image
 * using an upwind (one-sided) finite difference scheme rather than central differences, which
 * produces a sharper, less smoothed edge response than vtkvmtkGradientMagnitudeImageFilter /
 * vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter. It is one of the feature image types
 * selectable (as "upwind") by the vmtkimagefeatures pype script (in turn used by
 * vmtklevelsetsegmentation), whose gradient magnitude output is typically remapped (with
 * vtkvmtkSigmoidImageFilter or vtkvmtkBoundedReciprocalImageFilter) into a level set speed/feature
 * image. Like the other ITK wrappers in this module, it is a thin vtkSimpleImageToImageFilter:
 * SimpleExecute() converts the VTK input to a float itk::Image, runs
 * itk::UpwindGradientMagnitudeImageFilter, and converts the result back to vtkImageData.
 *
 * @sa vtkvmtkGradientMagnitudeImageFilter, vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter
 */

#ifndef __vtkvmtkUpwindGradientMagnitudeImageFilter_h
#define __vtkvmtkUpwindGradientMagnitudeImageFilter_h


#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkUpwindGradientMagnitudeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkUpwindGradientMagnitudeImageFilter *New();
  vtkTypeMacro(vtkvmtkUpwindGradientMagnitudeImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the blend between upwind and centered finite differences used to estimate the
   * gradient: 1.0 is full upwind (one-sided) differencing, 0.0 is centered differencing, -1.0 is
   * full downwind differencing. Default: 1.0.
   */
  vtkGetMacro(UpwindFactor,double);
  vtkSetMacro(UpwindFactor,double);
  ///@}

protected:
  vtkvmtkUpwindGradientMagnitudeImageFilter();
  ~vtkvmtkUpwindGradientMagnitudeImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkUpwindGradientMagnitudeImageFilter(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.

  double UpwindFactor;
};

#endif




