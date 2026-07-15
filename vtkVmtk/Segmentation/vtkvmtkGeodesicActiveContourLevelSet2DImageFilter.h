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
 * @class   vtkvmtkGeodesicActiveContourLevelSet2DImageFilter
 * @brief   Wraps itk::GeodesicActiveContourLevelSetImageFilter.
 * @ingroup Segmentation
 *
 * The 2D counterpart of vtkvmtkGeodesicActiveContourLevelSetImageFilter: wraps ITK's geodesic
 * active contour level set filter operating on a single 2D (internally float-cast) image slice,
 * with the same set of parameters and evolution behavior. See
 * vtkvmtkGeodesicActiveContourLevelSetImageFilter for the full description.
 *
 * @sa vtkvmtkGeodesicActiveContourLevelSetImageFilter
 */

#ifndef __vtkvmtkGeodesicActiveContourLevelSet2DImageFilter_h
#define __vtkvmtkGeodesicActiveContourLevelSet2DImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGeodesicActiveContourLevelSet2DImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkGeodesicActiveContourLevelSet2DImageFilter *New();
  vtkTypeMacro(vtkvmtkGeodesicActiveContourLevelSet2DImageFilter, vtkSimpleImageToImageFilter);

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
   * MaximumRMSError is reached. Default: 0.
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
   * evolution stops early once the RMS change falls below this value. Default: 1E-6.
   */
  vtkGetMacro(MaximumRMSError,double);
  vtkSetMacro(MaximumRMSError,double);
  ///@}

  ///@{
  /**
   * Toggle reversal of the front's expansion direction (passed through to the underlying ITK
   * filter's SetReverseExpansionDirection). Default: off.
   */
  vtkGetMacro(UseNegativeFeatures,int);
  vtkSetMacro(UseNegativeFeatures,int);
  vtkBooleanMacro(UseNegativeFeatures,int);
  ///@}

  ///@{
  /**
   * Toggle automatic generation of the speed and advection images from FeatureImage. When on
   * (default), FeatureImage must be set; when off, explicit SpeedImage (and advection) images must be
   * supplied instead.
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
   * Toggle use of the image spacing when computing derivatives and forces during evolution. Default:
   * on.
   */
  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);
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
  vtkvmtkGeodesicActiveContourLevelSet2DImageFilter();
  ~vtkvmtkGeodesicActiveContourLevelSet2DImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkGeodesicActiveContourLevelSet2DImageFilter(const vtkvmtkGeodesicActiveContourLevelSet2DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGeodesicActiveContourLevelSet2DImageFilter&);  // Not implemented

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
  double DerivativeSigma;
  double RMSChange;
  int ElapsedIterations;
  vtkImageData* FeatureImage;
  vtkImageData* SpeedImage;
};

#endif




