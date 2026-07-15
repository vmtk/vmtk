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
 * @class   vtkvmtkCollidingFrontsImageFilter
 * @brief   Wraps itk::CollidingFrontsImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkCollidingFrontsImageFilter propagates two fast marching fronts across a (float-cast,
 * assumed 3D) speed image, one from each of the two seed point sets Seeds1 and Seeds2, and outputs an
 * image encoding where the fronts collide -- points close to the collision surface are near zero,
 * with sign indicating which front arrived first/last. This is used by the vmtkimageinitialization
 * pype script's "collidingfronts" method to seed a level set segmentation with a surface connecting
 * two points/regions through the vessel (e.g. between an inlet and outlet), typically followed by
 * an offset/threshold step to build an initial level set.
 *
 * @sa vtkvmtkFastMarchingUpwindGradientImageFilter, vtkvmtkFastMarchingDirectionalFreezeImageFilter
 */

#ifndef __vtkvmtkCollidingFrontsImageFilter_h
#define __vtkvmtkCollidingFrontsImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCollidingFrontsImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkCollidingFrontsImageFilter *New();
  vtkTypeMacro(vtkvmtkCollidingFrontsImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Toggle enforcing connectivity of each front to its originating seed set while marching, passed
   * through to itk::CollidingFrontsImageFilter::SetApplyConnectivity. Default: off.
   */
  vtkGetMacro(ApplyConnectivity,int);
  vtkSetMacro(ApplyConnectivity,int);
  vtkBooleanMacro(ApplyConnectivity,int);
  ///@}

  ///@{
  /**
   * Set/get the small negative value used to mark voxels visited by the front that reaches them
   * first (front 1) versus the other (front 2); a larger negative magnitude leaves more headroom for
   * downstream shift/threshold operations that build an initial level set from the output. Default:
   * -1E-6.
   */
  vtkGetMacro(NegativeEpsilon,double);
  vtkSetMacro(NegativeEpsilon,double);
  ///@}

  ///@{
  /**
   * Toggle stopping front propagation as soon as the two fronts collide at every point (rather than
   * continuing to fill the whole image), passed through to
   * itk::CollidingFrontsImageFilter::SetStopOnTargets. Default: off.
   */
  vtkGetMacro(StopOnTargets,int);
  vtkSetMacro(StopOnTargets,int);
  vtkBooleanMacro(StopOnTargets,int);
  ///@}

  ///@{
  /**
   * Set/get the point ids (into the input image's points) of the seeds from which the first front is
   * propagated.
   */
  vtkSetObjectMacro(Seeds1,vtkIdList);
  vtkGetObjectMacro(Seeds1,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the point ids (into the input image's points) of the seeds from which the second front is
   * propagated.
   */
  vtkSetObjectMacro(Seeds2,vtkIdList);
  vtkGetObjectMacro(Seeds2,vtkIdList);
  ///@}

protected:

  vtkvmtkCollidingFrontsImageFilter();
  ~vtkvmtkCollidingFrontsImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkCollidingFrontsImageFilter(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.

  vtkIdList* Seeds1;
  vtkIdList* Seeds2;

  int ApplyConnectivity;
  double NegativeEpsilon;
  int StopOnTargets;
};

#endif
