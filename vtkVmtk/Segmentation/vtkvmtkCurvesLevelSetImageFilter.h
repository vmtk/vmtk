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
 * @class   vtkvmtkCurvesLevelSetImageFilter
 * @brief   Wraps itk::CurvesLevelSetImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkCurvesLevelSetImageFilter wraps itk::CurvesLevelSetImageFilter, a level set segmentation
 * filter tailored to thin, curve-like (tubular) structures such as vessels. Given an initial level
 * set image (the filter's input, whose IsoSurfaceValue isocontour defines the starting front) and
 * either a FeatureImage -- from which speed and advection images are generated internally when
 * AutoGenerateSpeedAdvection is on -- or an explicit SpeedImage, it evolves the level set for up to
 * NumberOfIterations iterations, or until the RMS change of the level set values drops below
 * MaximumRMSError, combining propagation, curvature, and advection force terms weighted by
 * PropagationScaling, CurvatureScaling, and AdvectionScaling respectively. This is one of the
 * segmentation methods (the "curves" LevelSetsType) offered by the vmtklevelsetsegmentation pype
 * script.
 *
 * @sa vtkvmtkGeodesicActiveContourLevelSetImageFilter, vtkvmtkGeodesicActiveContourLevelSet2DImageFilter
 */

#ifndef __vtkvmtkCurvesLevelSetImageFilter_h
#define __vtkvmtkCurvesLevelSetImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCurvesLevelSetImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkCurvesLevelSetImageFilter *New();
  vtkTypeMacro(vtkvmtkCurvesLevelSetImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the isovalue, in the input level set image, that defines the initial front to evolve.
   * Default: 0.0.
   */
  vtkGetMacro(IsoSurfaceValue,double);
  vtkSetMacro(IsoSurfaceValue,double);
  ///@}

  ///@{
  /**
   * Set/get the maximum number of level set evolution iterations; evolution may stop earlier if
   * MaximumRMSError is reached. Default: 100.
   */
  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the weight of the propagation (speed-image-driven expansion/contraction) force term.
   * Default: 0.0.
   */
  vtkGetMacro(PropagationScaling,double);
  vtkSetMacro(PropagationScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the weight of the curvature-based regularization force term; larger values produce a
   * smoother evolving front. Default: 0.0.
   */
  vtkGetMacro(CurvatureScaling,double);
  vtkSetMacro(CurvatureScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the weight of the advection force term that pulls the front toward features (e.g. edges)
   * of the speed/feature image. Default: 0.0.
   */
  vtkGetMacro(AdvectionScaling,double);
  vtkSetMacro(AdvectionScaling,double);
  ///@}

  ///@{
  /**
   * Set/get the convergence threshold on the RMS change of the level set values between iterations;
   * evolution stops early once the RMS change falls below this value. Default: 1E-4.
   */
  vtkGetMacro(MaximumRMSError,double);
  vtkSetMacro(MaximumRMSError,double);
  ///@}

  ///@{
  /**
   * Toggle use of negative features, flipping the sign convention of the feature/speed image.
   * Default: off.
   */
  vtkGetMacro(UseNegativeFeatures,int);
  vtkSetMacro(UseNegativeFeatures,int);
  vtkBooleanMacro(UseNegativeFeatures,int);
  ///@}

  ///@{
  /**
   * Toggle use of the image spacing when computing derivatives and forces during evolution. Default:
   * on.
   */
  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);
  ///@}

  ///@{
  /**
   * Toggle automatic generation of the speed and advection images from FeatureImage. When on
   * (default), FeatureImage must be set and SpeedImage is ignored; when off, an explicit SpeedImage
   * must be supplied instead.
   */
  vtkGetMacro(AutoGenerateSpeedAdvection,int);
  vtkSetMacro(AutoGenerateSpeedAdvection,int);
  vtkBooleanMacro(AutoGenerateSpeedAdvection,int);
  ///@}

  ///@{
  /**
   * Toggle sub-voxel interpolation of the zero level set surface location when computing the output.
   * Default: on.
   */
  vtkGetMacro(InterpolateSurfaceLocation,int);
  vtkSetMacro(InterpolateSurfaceLocation,int);
  vtkBooleanMacro(InterpolateSurfaceLocation,int);
  ///@}

  ///@{
  /**
   * Set/get the Gaussian sigma used when computing derivatives of FeatureImage for the internally
   * generated speed and advection images (only relevant when AutoGenerateSpeedAdvection is on).
   * Default: 0.0 (no smoothing).
   */
  vtkGetMacro(DerivativeSigma,double);
  vtkSetMacro(DerivativeSigma,double);
  ///@}

  ///@{
  /**
   * Set/get the feature image (e.g. a gradient-magnitude-derived image) from which speed and
   * advection images are generated internally when AutoGenerateSpeedAdvection is on.
   */
  vtkGetObjectMacro(FeatureImage,vtkImageData);
  vtkSetObjectMacro(FeatureImage,vtkImageData);
  ///@}

  ///@{
  /**
   * Set/get an explicit speed image controlling front propagation rate, used instead of an
   * internally generated one when AutoGenerateSpeedAdvection is off.
   */
  vtkGetObjectMacro(SpeedImage,vtkImageData);
  vtkSetObjectMacro(SpeedImage,vtkImageData);
  ///@}

  /**
   * Get the RMS change of the level set values computed over the last iteration performed. Valid
   * only after Update() has been called.
   */
  vtkGetMacro(RMSChange,double);

  /**
   * Get the actual number of iterations performed, which may be less than NumberOfIterations if the
   * MaximumRMSError convergence criterion was reached first. Valid only after Update() has been
   * called.
   */
  vtkGetMacro(ElapsedIterations,int);

protected:

  vtkvmtkCurvesLevelSetImageFilter();
  ~vtkvmtkCurvesLevelSetImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkCurvesLevelSetImageFilter(const vtkvmtkCurvesLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCurvesLevelSetImageFilter&);  //
                                                                          // Not implemented

  double IsoSurfaceValue;
  int NumberOfIterations;
  double PropagationScaling;
  double CurvatureScaling;
  double AdvectionScaling;
  double MaximumRMSError;
  int UseNegativeFeatures;
  int UseImageSpacing;
  int AutoGenerateSpeedAdvection;
  int InterpolateSurfaceLocation;
  double DerivativeSigma;
  double RMSChange;
  int ElapsedIterations;

  vtkImageData* FeatureImage;
  vtkImageData* SpeedImage;
};

#endif




