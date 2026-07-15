/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkLevelSetSigmoidFilter
 * @brief   Apply a sigmoid function to every voxel of an image.
 * @ingroup Misc
 *
 * For every voxel whose value in LevelSetsImage (typically a signed distance / level set image
 * computed from a preliminary segmentation) is below a small fixed window (4 voxel-spacing units),
 * this filter adds a sigmoid-shaped correction to the corresponding input feature image value; the
 * correction is centered on the LevelSetsImage zero level set with width controlled by Sigma
 * (in voxel units) and height controlled by ScaleValue (or, if ComputeScaleValueFromInput is on,
 * by the mean of the input image). Voxels farther from the level set are left unchanged.
 *
 * This is primarily used in feature image correction (see http://www.vmtk.org/tutorials/ImageFeatureCorrection.html for details), where it locally boosts the speed/feature image
 * near a preliminary level set front so that a subsequent level set evolution is encouraged to
 * settle more precisely on the true boundary.
 */

#ifndef __vtkvmtkLevelSetSigmoidFilter_h
#define __vtkvmtkLevelSetSigmoidFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLevelSetSigmoidFilter : public vtkSimpleImageToImageFilter
{
  public: 
  vtkTypeMacro(vtkvmtkLevelSetSigmoidFilter,vtkSimpleImageToImageFilter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkLevelSetSigmoidFilter *New();

  ///@{
  /**
   * Set/Get the level set (signed distance) image whose zero level set defines the front around
   * which the sigmoid correction is applied to the input feature image. Required before
   * SimpleExecute is called.
   */
  vtkSetObjectMacro(LevelSetsImage,vtkImageData);
  vtkGetObjectMacro(LevelSetsImage,vtkImageData);
  ///@}

  ///@{
  /**
   * Set/Get the sigma value of the sigmoid function. Sigma determines the width of the sigmoid
   * function. The value is given in pixel (voxel spacing) units. Default: 1.0.
   */
  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);
  ///@}

  ///@{
  /**
   * Set/Get the Scalevalue of the sigmoind function. This determines the height of the sigmoid.
   */
  vtkSetMacro(ScaleValue,double);
  vtkGetMacro(ScaleValue,double);
  ///@}

  ///@{
  /**
   * Set/Get The the ComputeScaleValueFromInput option. When enabled, vmtkimagefeaturecorrection
   * computes the mean value from the featureimage and uses this value to set the height of the sigmoid
   * function instead of the value given to the option scalevalue.
   */
  vtkSetMacro(ComputeScaleValueFromInput,int);
  vtkGetMacro(ComputeScaleValueFromInput,int);
  vtkBooleanMacro(ComputeScaleValueFromInput,int);
  ///@}

  protected:
  vtkvmtkLevelSetSigmoidFilter();
  ~vtkvmtkLevelSetSigmoidFilter();  

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

  vtkImageData *LevelSetsImage;

  double Sigma;
  double ScaleValue;
  int ComputeScaleValueFromInput;

  private:
  vtkvmtkLevelSetSigmoidFilter(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
  void operator=(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
};

#endif
