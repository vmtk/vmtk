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
 * @class   vtkvmtkFastMarchingDirectionalFreezeImageFilter
 * @brief   Wraps itk::FastMarchingDirectionalFreezeImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkFastMarchingDirectionalFreezeImageFilter runs a variant of fast marching front propagation
 * (itk::FastMarchingDirectionalFreezeImageFilter, a subclass of
 * itk::FastMarchingUpwindGradientImageFilter) on the (internally float-cast, assumed 3D) input speed
 * image, starting from the point ids listed in Seeds, that additionally freezes points based on the
 * relative direction of the propagating front and the gradient of the speed image -- points where the
 * front is moving against the speed gradient are frozen rather than accepted, preventing the front
 * from propagating back into regions it has already effectively passed. As with the base fast
 * marching filter, propagation can be stopped early once one/all of the point ids listed in Targets
 * are reached (TargetReachedMode), and the arrival time at the reached target is retrievable via
 * GetTargetValue after Update().
 *
 * @sa vtkvmtkFastMarchingUpwindGradientImageFilter
 */

#ifndef __vtkvmtkFastMarchingDirectionalFreezeImageFilter_h
#define __vtkvmtkFastMarchingDirectionalFreezeImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFastMarchingDirectionalFreezeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkFastMarchingDirectionalFreezeImageFilter *New();
  vtkTypeMacro(vtkvmtkFastMarchingDirectionalFreezeImageFilter, vtkSimpleImageToImageFilter);

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
   * Set/get the criterion used to decide when front propagation towards Targets should stop: either
   * ONE_TARGET (stop as soon as any target is reached) or ALL_TARGETS (stop only once every target
   * has been reached). Use SetTargetReachedModeToOneTarget/ToAllTargets for the equivalent named
   * setters. Default: ONE_TARGET.
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

  enum
  {
    ONE_TARGET,
    ALL_TARGETS
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
  vtkvmtkFastMarchingDirectionalFreezeImageFilter();
  ~vtkvmtkFastMarchingDirectionalFreezeImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkFastMarchingDirectionalFreezeImageFilter(const vtkvmtkFastMarchingDirectionalFreezeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFastMarchingDirectionalFreezeImageFilter&);  // Not implemented.

  int GenerateGradientImage;
  int TargetReachedMode;
  double TargetValue;
  double TargetOffset;

  vtkIdList* Seeds;
  vtkIdList* Targets;
};

#endif
