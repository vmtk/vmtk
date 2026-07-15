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
 * @class   vtkvmtkLaplacianSegmentationLevelSetImageFilter
 * @brief   Wraps itk::LaplacianSegmentationLevelSetImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkLaplacianSegmentationLevelSetImageFilter evolves an initial level set (the input image,
 * interpreted as a signed distance-like function) driven by the second derivative (Laplacian) of a
 * feature image, so that the zero level set is attracted to zero crossings of the Laplacian, i.e.
 * to intensity inflection points/edges of FeatureImage. It is one of the segmentation methods
 * selectable (as "laplacian") by the vmtklevelsetsegmentation pype script, where FeatureImage is
 * set directly from the (float-cast) input image rather than from a gradient-derived feature image.
 * Like the other vtkvmtk level set wrappers in this module (vtkvmtkGeodesicActiveContourLevelSetImageFilter,
 * vtkvmtkCurvesLevelSetImageFilter, vtkvmtkThresholdSegmentationLevelSetImageFilter), it is a thin
 * vtkSimpleImageToImageFilter wrapper: SimpleExecute() converts the VTK input/FeatureImage/SpeedImage
 * to ITK images, configures and runs the underlying itk::LaplacianSegmentationLevelSetImageFilter,
 * and converts the result back to a vtkImageData output.
 *
 * @sa vtkvmtkGeodesicActiveContourLevelSetImageFilter, vtkvmtkCurvesLevelSetImageFilter,
 * vtkvmtkThresholdSegmentationLevelSetImageFilter
 */

#ifndef __vtkvmtkLaplacianSegmentationLevelSetImageFilter_h
#define __vtkvmtkLaplacianSegmentationLevelSetImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkLaplacianSegmentationLevelSetImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkLaplacianSegmentationLevelSetImageFilter *New();
  vtkTypeMacro(vtkvmtkLaplacianSegmentationLevelSetImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the intensity value of the input level set image that is taken to represent the
   * zero level set (i.e. the current surface) at the start of evolution. Default: 0.0.
   */
  vtkGetMacro(IsoSurfaceValue,double);
  vtkSetMacro(IsoSurfaceValue,double);
  ///@}

  ///@{
  /**
   * Set/get the maximum number of level set evolution iterations to run. Evolution stops earlier
   * if MaximumRMSError is reached. Default: 0 (no evolution).
   */
  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the scaling factor applied to the propagation (inflation/expansion) term of the level
   * set speed function. Larger magnitudes make the front advance faster along its normal. Default: 0.0.
   */
  vtkGetMacro(PropagationScaling,double);
  vtkSetMacro(PropagationScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the scaling factor applied to the curvature (smoothing) term of the level set speed
   * function. Larger values yield a smoother, more regularized evolving surface. Default: 0.0.
   */
  vtkGetMacro(CurvatureScaling,double);
  vtkSetMacro(CurvatureScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the scaling factor applied to the advection term of the level set speed function,
   * which attracts the front towards the Laplacian zero-crossings of FeatureImage. Default: 0.0.
   */
  vtkGetMacro(AdvectionScaling,double);
  vtkSetMacro(AdvectionScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the RMS change convergence threshold: evolution stops once the RMS change of the level
   * set function between iterations drops below this value. Default: 1E-6.
   */
  vtkGetMacro(MaximumRMSError,double);
  vtkSetMacro(MaximumRMSError,double);
  ///@}

  ///@{
  /**
   * Toggle reversal of the expansion direction of the propagation term (equivalent to negating the
   * sign convention of FeatureImage), used when the surface must contract rather than expand.
   * Default: off.
   */
  vtkGetMacro(UseNegativeFeatures,int);
  vtkSetMacro(UseNegativeFeatures,int);
  vtkBooleanMacro(UseNegativeFeatures,int);
  ///@}

  ///@{
  /**
   * Toggle automatic generation of the propagation/advection speed and advection field from
   * FeatureImage by the underlying ITK filter. When off, SpeedImage (and the advection field) must
   * be supplied explicitly. Default: on.
   */
  vtkGetMacro(AutoGenerateSpeedAdvection,int);
  vtkSetMacro(AutoGenerateSpeedAdvection,int);
  vtkBooleanMacro(AutoGenerateSpeedAdvection,int);
  ///@}

  ///@{
  /**
   * Toggle sub-voxel interpolation of the zero level set location when computing curvature and
   * output. Default: on.
   */
  vtkGetMacro(InterpolateSurfaceLocation,int);
  vtkSetMacro(InterpolateSurfaceLocation,int);
  vtkBooleanMacro(InterpolateSurfaceLocation,int);
  ///@}

  ///@{
  /**
   * Toggle whether the level set evolution accounts for the input image's voxel spacing (i.e.
   * operates in physical units) rather than treating voxels as unit spacing. Default: on.
   */
  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);
  ///@}

  ///@{
  /**
   * Set/get the feature image used to derive the Laplacian-based advection term (and, when
   * AutoGenerateSpeedAdvection is on, the propagation speed as well). Required for evolution.
   */
  vtkGetObjectMacro(FeatureImage,vtkImageData);
  vtkSetObjectMacro(FeatureImage,vtkImageData);
  ///@}

  ///@{
  /**
   * Set/get an explicit propagation speed image, used instead of one auto-generated from
   * FeatureImage when AutoGenerateSpeedAdvection is off.
   */
  vtkGetObjectMacro(SpeedImage,vtkImageData);
  vtkSetObjectMacro(SpeedImage,vtkImageData);
  ///@}

  /**
   * Get the RMS change of the level set function computed at the last iteration of the most
   * recent evolution. Valid only after Update() has been called.
   */
  vtkGetMacro(RMSChange,double);

  /**
   * Get the number of iterations actually performed during the most recent evolution (may be less
   * than NumberOfIterations if MaximumRMSError was reached first). Valid only after Update() has
   * been called.
   */
  vtkGetMacro(ElapsedIterations,int);

protected:
  vtkvmtkLaplacianSegmentationLevelSetImageFilter();
  ~vtkvmtkLaplacianSegmentationLevelSetImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkLaplacianSegmentationLevelSetImageFilter(const vtkvmtkLaplacianSegmentationLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkLaplacianSegmentationLevelSetImageFilter&); // Not implemented

  double IsoSurfaceValue;
  int NumberOfIterations;
  double PropagationScaling;
  double CurvatureScaling;
  double AdvectionScaling;
  double MaximumRMSError;
  int UseNegativeFeatures;
  int AutoGenerateSpeedAdvection;
  int InterpolateSurfaceLocation;
  int UseImageSpacing;
  double RMSChange;
  int ElapsedIterations;
  vtkImageData* FeatureImage;
  vtkImageData* SpeedImage;
};

#endif




