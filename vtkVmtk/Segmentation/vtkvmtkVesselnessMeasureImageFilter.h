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
 * @class   vtkvmtkVesselnessMeasureImageFilter
 * @brief   Wraps itk::VesselnessMeasureImageFilter.
 * @ingroup Segmentation
 *
 * Computes the multiscale Hessian-based "vesselness" measure of Frangi et al. ("Multiscale Vessel
 * Enhancement Filtering", MICCAI 1998): at each of NumberOfSigmaSteps scales between SigmaMin and
 * SigmaMax, the Hessian of the Gaussian-smoothed image is analyzed via its eigenvalues, and a
 * vesselness response (shaped by the Alpha/Beta/Gamma sensitivity parameters) is computed that is
 * high on tubular structures and low on blob-like or planar structures; the maximum response across
 * scales is kept at each voxel. BrightObject selects whether bright-on-dark (default) or
 * dark-on-bright tubular structures are enhanced. This is the "vesselness"-based method used by the
 * vmtkimagevesselenhancement pype script.
 *
 * @sa vtkvmtkVesselEnhancingDiffusionImageFilter, vtkvmtkVesselEnhancingDiffusion3DImageFilter,
 *     vtkvmtkSatoVesselnessMeasureImageFilter, vtkvmtkObjectnessMeasureImageFilter
 */

#ifndef __vtkvmtkVesselnessMeasureImageFilter_h
#define __vtkvmtkVesselnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkVesselnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselnessMeasureImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/Get the smallest Gaussian scale (sigma) at which the Hessian-based vesselness is evaluated,
   * roughly corresponding to the radius of the smallest vessels to detect. Default: 1.0.
   */
  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);
  ///@}

  ///@{
  /**
   * Set/Get the largest Gaussian scale (sigma) at which the Hessian-based vesselness is evaluated,
   * roughly corresponding to the radius of the largest vessels to detect. Default: 1.0.
   */
  vtkGetMacro(SigmaMax,double);
  vtkSetMacro(SigmaMax,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of scales sampled between SigmaMin and SigmaMax. Default: 1.
   */
  vtkGetMacro(NumberOfSigmaSteps,int);
  vtkSetMacro(NumberOfSigmaSteps,int);
  ///@}

  ///@{
  /**
   * Set/Get how intermediate scales between SigmaMin and SigmaMax are spaced: EQUISPACED (default,
   * linear spacing) or LOGARITHMIC (logarithmic spacing, more scales concentrated near SigmaMin).
   * See also SetSigmaStepMethodToEquispaced / SetSigmaStepMethodToLogarithmic.
   */
  vtkGetMacro(SigmaStepMethod,int);
  vtkSetMacro(SigmaStepMethod,int);
  ///@}

  /**
   * Convenience method: set SigmaStepMethod to EQUISPACED (default).
   */
  void SetSigmaStepMethodToEquispaced()
  {
    this->SetSigmaStepMethod(EQUISPACED);
  }

  /**
   * Convenience method: set SigmaStepMethod to LOGARITHMIC.
   */
  void SetSigmaStepMethodToLogarithmic()
  {
    this->SetSigmaStepMethod(LOGARITHMIC);
  }

  /**
   * Values for SigmaStepMethod.
   */
  enum
  {
    EQUISPACED,
    LOGARITHMIC
  };

  ///@{
  /**
   * Toggle scaling the vesselness response at each scale by sigma^2 (Lindeberg's gamma-normalized
   * derivative scaling), which makes responses comparable across scales. Default: off.
   */
  vtkGetMacro(UseScaledVesselness,int);
  vtkSetMacro(UseScaledVesselness,int);
  ///@}

  ///@{
  /**
   * Set/Get the Frangi vesselness sensitivity to the ratio between the two smallest Hessian
   * eigenvalues (discriminates plate-like from line-like structures). Default: 1.0.
   */
  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);
  ///@}

  ///@{
  /**
   * Set/Get the Frangi vesselness sensitivity to the ratio between the largest and the geometric
   * mean of the two smallest Hessian eigenvalues (discriminates blob-like from line-like
   * structures). Default: 1.0.
   */
  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);
  ///@}

  ///@{
  /**
   * Set/Get the Frangi vesselness sensitivity to the overall Hessian eigenvalue magnitude
   * (discriminates structure from background noise). Default: 1.0.
   */
  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);
  ///@}

  /**
   * Get an image recording, at each voxel, the scale (sigma value) at which the maximum vesselness
   * response was found across all sampled scales. Valid only after Update() has been called.
   */
  vtkGetObjectMacro(ScalesOutput,vtkImageData);

  ///@{
  /**
   * Toggle whether the filter enhances bright tubular structures on a dark background (on, default)
   * or dark tubular structures on a bright background (off).
   */
  vtkSetMacro(BrightObject, bool);
  vtkGetMacro(BrightObject, bool);
  vtkBooleanMacro(BrightObject, bool);
  ///@}

protected:
  vtkvmtkVesselnessMeasureImageFilter();
  ~vtkvmtkVesselnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkVesselnessMeasureImageFilter(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int UseScaledVesselness;
  double Alpha;
  double Beta;
  double Gamma;
  bool BrightObject;
  vtkImageData* ScalesOutput;
};

#endif

