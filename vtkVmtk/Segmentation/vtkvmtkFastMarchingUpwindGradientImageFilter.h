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
 * @class   vtkvmtkFastMarchingUpwindGradientImageFilter
 * @brief   Wraps itk::FastMarchingUpwindGradientImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkFastMarchingUpwindGradientImageFilter runs a fast marching front propagation, on the
 * (internally float-cast, assumed 3D) input speed image, starting from the point ids listed in Seeds
 * and outputs the front arrival time at every voxel (and, if GenerateGradientImage is on, the
 * gradient of the arrival time field). Front propagation can be stopped early once one/all of the
 * point ids listed in Targets are reached, according to TargetReachedMode; after Update(), the
 * arrival time at the (first reached) target is available via GetTargetValue. This is used by the
 * vmtkimageinitialization pype script's fast-marching-based initialization method to build an
 * initial level set: the arrival time image, shifted by (minus) the target value, has its zero level
 * set passing through the target, giving a front that has grown outward from the seeds to reach it.
 *
 * @sa vtkvmtkFastMarchingDirectionalFreezeImageFilter, vtkvmtkCollidingFrontsImageFilter
 */

#ifndef __vtkvmtkFastMarchingUpwindGradientImageFilter_h
#define __vtkvmtkFastMarchingUpwindGradientImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFastMarchingUpwindGradientImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkFastMarchingUpwindGradientImageFilter *New();
  vtkTypeMacro(vtkvmtkFastMarchingUpwindGradientImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Toggle computation of the gradient of the arrival time image, in addition to the arrival time
   * image itself. Default: off.
   */
  vtkGetMacro(GenerateGradientImage,int);
  vtkSetMacro(GenerateGradientImage,int);
  vtkBooleanMacro(GenerateGradientImage,int);
  ///@}

  ///@{
  /**
   * Set/get the criterion used to decide when front propagation towards Targets should stop: one of
   * ONE_TARGET (stop as soon as any target is reached), ALL_TARGETS (stop only once every target has
   * been reached), or NO_TARGETS (ignore Targets and propagate over the whole image). Use
   * SetTargetReachedModeToOneTarget/ToAllTargets/ToNoTargets for the equivalent named setters.
   * Default: ONE_TARGET.
   */
  vtkGetMacro(TargetReachedMode,int);
  vtkSetMacro(TargetReachedMode,int);
  ///@}

  /**
   * Set TargetReachedMode to ONE_TARGET: propagation stops as soon as any one of the Targets is
   * reached.
   */
  void SetTargetReachedModeToOneTarget()
  {
    this->SetTargetReachedMode(ONE_TARGET);
  }

  /**
   * Set TargetReachedMode to ALL_TARGETS: propagation stops only once every point in Targets has
   * been reached.
   */
  void SetTargetReachedModeToAllTargets()
  {
    this->SetTargetReachedMode(ALL_TARGETS);
  }

  /**
   * Set TargetReachedMode to NO_TARGETS: Targets is ignored and the front propagates over the whole
   * image.
   */
  void SetTargetReachedModeToNoTargets()
  {
    this->SetTargetReachedMode(NO_TARGETS);
  }

  enum
  {
    ONE_TARGET,
    ALL_TARGETS,
    NO_TARGETS
  };

  /**
   * Get the front arrival time at the target reached according to TargetReachedMode. Valid only
   * after Update() has been called.
   */
  vtkGetMacro(TargetValue,double);

  ///@{
  /**
   * Set/get an offset added to (subtracted from, per ITK's convention) the stopping value used when
   * checking whether Targets have been reached, allowing the front to overshoot the target slightly.
   * Default: 0.0.
   */
  vtkGetMacro(TargetOffset,double);
  vtkSetMacro(TargetOffset,double);
  ///@}

  ///@{
  /**
   * Set/get the point ids (into the input image's points) of the seeds from which the front starts
   * propagating (fast marching trial points).
   */
  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the point ids (into the input image's points) of the target points used, together with
   * TargetReachedMode, to decide when front propagation stops.
   */
  vtkSetObjectMacro(Targets,vtkIdList);
  vtkGetObjectMacro(Targets,vtkIdList);
  ///@}

protected:
  vtkvmtkFastMarchingUpwindGradientImageFilter();
  ~vtkvmtkFastMarchingUpwindGradientImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkFastMarchingUpwindGradientImageFilter(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.

  int GenerateGradientImage;
  int TargetReachedMode;
  double TargetValue;
  double TargetOffset;

  vtkIdList* Seeds;
  vtkIdList* Targets;
};

#endif
