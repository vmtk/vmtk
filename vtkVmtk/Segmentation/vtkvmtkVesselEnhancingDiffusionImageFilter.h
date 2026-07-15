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
 * @class   vtkvmtkVesselEnhancingDiffusionImageFilter
 * @brief   Wraps itk::VesselEnhancingDiffusionImageFilter.
 * @ingroup Segmentation
 *
 * A simpler, scalar (non-tensor) alternative to vtkvmtkVesselEnhancingDiffusion3DImageFilter:
 * iteratively enhances tubular structures by combining, at each of NumberOfIterations outer
 * iterations, a multiscale Hessian-based vesselness response (evaluated at NumberOfSigmaSteps
 * scales between SigmaMin and SigmaMax, shaped by Alpha/Beta/Gamma/C) with
 * NumberOfDiffusionSubIterations steps of conductance-weighted diffusion (TimeStep, Epsilon,
 * WStrength, Sensitivity). This is the simpler "ved" vessel enhancement method offered by the
 * vmtkimagevesselenhancement pype script, as opposed to the full tensor-based "vedm" method.
 *
 * @sa vtkvmtkVesselEnhancingDiffusion3DImageFilter, vtkvmtkVesselnessMeasureImageFilter
 */

#ifndef __vtkvmtkVesselEnhancingDiffusionImageFilter_h
#define __vtkvmtkVesselEnhancingDiffusionImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselEnhancingDiffusionImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkVesselEnhancingDiffusionImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselEnhancingDiffusionImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/Get the smallest Gaussian scale (sigma) at which the Hessian-based vesselness driving the
   * diffusion is evaluated, roughly corresponding to the radius of the smallest vessels to enhance.
   * Default: 1.0.
   */
  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);
  ///@}

  ///@{
  /**
   * Set/Get the largest Gaussian scale (sigma) at which the Hessian-based vesselness is evaluated,
   * roughly corresponding to the radius of the largest vessels to enhance. Default: 1.0.
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
   * Set/Get the vesselness sensitivity to the ratio between the two smallest Hessian eigenvalues
   * (discriminates plate-like from line-like structures). Default: 1.0.
   */
  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);
  ///@}

  ///@{
  /**
   * Set/Get the vesselness sensitivity to the ratio between the largest and the geometric mean of
   * the two smallest Hessian eigenvalues (discriminates blob-like from line-like structures).
   * Default: 1.0.
   */
  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);
  ///@}

  ///@{
  /**
   * Set/Get the vesselness sensitivity to the overall Hessian eigenvalue magnitude (discriminates
   * structure from background noise). Default: 1.0.
   */
  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);
  ///@}

  ///@{
  /**
   * Set/Get the diffusion tensor construction parameter controlling how strongly the vesselness
   * response suppresses diffusion across the vessel wall. Default: 1.0.
   */
  vtkGetMacro(C,double);
  vtkSetMacro(C,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of outer iterations, each recomputing the multiscale vesselness response and
   * then applying NumberOfDiffusionSubIterations diffusion steps. Default: 1.
   */
  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/Get the number of diffusion sub-steps performed per outer iteration, each of size TimeStep.
   * Default: 1.
   */
  vtkGetMacro(NumberOfDiffusionSubIterations,int);
  vtkSetMacro(NumberOfDiffusionSubIterations,int);
  ///@}

  ///@{
  /**
   * Set/Get the time step size of each diffusion sub-iteration. Default: 0.01.
   */
  vtkGetMacro(TimeStep,double);
  vtkSetMacro(TimeStep,double);
  ///@}

  ///@{
  /**
   * Set/Get a small numerical regularization constant used when evaluating the diffusion
   * conductance to avoid division by zero. Default: 1E-6.
   */
  vtkGetMacro(Epsilon,double);
  vtkSetMacro(Epsilon,double);
  ///@}

  ///@{
  /**
   * Set/Get the strength of the diffusion conductance weighting. Default: 1.0.
   */
  vtkGetMacro(WStrength,double);
  vtkSetMacro(WStrength,double);
  ///@}

  ///@{
  /**
   * Set/Get the sensitivity of the diffusion conductance to the local vesselness response. Default:
   * 1.0.
   */
  vtkGetMacro(Sensitivity,double);
  vtkSetMacro(Sensitivity,double);
  ///@}

protected:

  vtkvmtkVesselEnhancingDiffusionImageFilter();
  ~vtkvmtkVesselEnhancingDiffusionImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkVesselEnhancingDiffusionImageFilter(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int NumberOfIterations;
  int NumberOfDiffusionSubIterations;
  double TimeStep;
  double Epsilon;
  double WStrength;
  double Sensitivity;
  double Alpha;
  double Beta;
  double Gamma;
  double C;
};

#endif

