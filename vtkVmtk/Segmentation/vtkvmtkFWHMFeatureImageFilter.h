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
 * @class   vtkvmtkFWHMFeatureImageFilter
 * @brief   Wraps itk::FWHMFeatureImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkFWHMFeatureImageFilter builds a feature image for full-width-half-maximum (FWHM) level set
 * segmentation. For every pixel of the (internally float-cast, assumed 3D) input, a local maximum is
 * estimated by grayscale dilation with a ball structuring element of radius Radius, the pixel value
 * is compared to half that local maximum (offset by BackgroundValue), and the resulting difference is
 * passed through a sigmoid to produce a smooth output roughly in [-1, 1]: positive inside the
 * FWHM-bright region, negative outside. This is one of the feature image construction methods
 * ("fwhm") offered by the vmtkimagefeatures pype script, typically for use as the FeatureImage/
 * SpeedImage of a subsequent level set segmentation.
 *
 * @sa vtkvmtkGeodesicActiveContourLevelSetImageFilter, vtkvmtkCurvesLevelSetImageFilter
 */

#ifndef __vtkvmtkFWHMFeatureImageFilter_h
#define __vtkvmtkFWHMFeatureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFWHMFeatureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkFWHMFeatureImageFilter *New();
  vtkTypeMacro(vtkvmtkFWHMFeatureImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Toggle use of the image spacing (as opposed to an isotropic unit spacing) when computing the
   * structuring element used for local-maximum estimation. Default: on.
   */
  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);
  ///@}

  ///@{
  /**
   * Set/get the (x,y,z) radius, in voxels, of the ball structuring element used to estimate the
   * local maximum via grayscale dilation. Default: (1,1,1).
   */
  vtkGetVectorMacro(Radius,int,3);
  vtkSetVectorMacro(Radius,int,3);
  ///@}

  ///@{
  /**
   * Set/get the background value subtracted when computing the half-maximum threshold used to build
   * the feature image. Default: 0.0.
   */
  vtkGetMacro(BackgroundValue,double);
  vtkSetMacro(BackgroundValue,double);
  ///@}

protected:
  vtkvmtkFWHMFeatureImageFilter();
  ~vtkvmtkFWHMFeatureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkFWHMFeatureImageFilter(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.

  double BackgroundValue;
  int UseImageSpacing;
  int* Radius;
};

#endif
