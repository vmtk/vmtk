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
 * @class   vtkvmtkVesselEnhancingDiffusion3DImageFilter
 * @brief   Wraps itk::VesselEnhancingDiffusion3DImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkVesselEnhancingDiffusion3DImageFilter iteratively enhances tubular (vessel) structures by
 * solving the vessel-enhancing anisotropic diffusion equation of Manniesing et al. ("Vessel
 * Enhancing Diffusion: A Scale Space Representation of Vessel Structures", Medical Image Analysis,
 * 2006): at each iteration it recomputes the Hessian-based vesselness response at multiple scales
 * (see SigmaMin/SigmaMax/NumberOfSigmaSteps), builds a diffusion tensor from the locally dominant
 * vessel orientation, and diffuses the image with that tensor, smoothing across the vessel while
 * preserving/enhancing it along its axis. It is one of the vessel enhancement methods selectable
 * (as "vedm") by the vmtkimagevesselenhancement pype script, alongside the simpler, non-tensor-based
 * vtkvmtkVesselEnhancingDiffusionImageFilter (the "ved" method). Unlike the other filters in this
 * module, it is not a vtkSimpleImageToImageFilter but a vtkvmtkITKImageToImageFilterFF, which keeps
 * the underlying itk::VesselEnhancingDiffusion3DImageFilter alive across executions and exposes its
 * parameters through DelegateITKInputMacro-based setters; SigmaMin/SigmaMax/NumberOfSigmaSteps are
 * cached locally and converted to the underlying filter's Scales vector by ComputeSigmaValue when
 * Update() is called.
 *
 * @sa vtkvmtkVesselEnhancingDiffusionImageFilter, vtkvmtkVesselnessMeasureImageFilter
 */

#ifndef __vtkvmtkVesselEnhancingDiffusion3DImageFilter_h
#define __vtkvmtkVesselEnhancingDiffusion3DImageFilter_h


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkVesselEnhancingDiffusion3DImageFilter.h"
#include "vtkvmtkWin32Header.h"
#include "vtkVersion.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselEnhancingDiffusion3DImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkVesselEnhancingDiffusion3DImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselEnhancingDiffusion3DImageFilter, vtkvmtkITKImageToImageFilterFF);

  ///@{
  /**
   * Set/Get the smallest Gaussian scale (sigma) at which the vesselness-derived diffusion tensor is
   * evaluated, roughly corresponding to the radius of the smallest vessels to enhance. Cached
   * locally and converted to the underlying ITK filter's Scales vector on Update(). Default: 0.0.
   */
  vtkSetMacro(SigmaMin,double);
  vtkGetMacro(SigmaMin,double);
  ///@}

  ///@{
  /**
   * Set/Get the largest Gaussian scale (sigma) at which the vesselness-derived diffusion tensor is
   * evaluated, roughly corresponding to the radius of the largest vessels to enhance. Cached locally
   * and converted to the underlying ITK filter's Scales vector on Update(). Default: 0.0.
   */
  vtkSetMacro(SigmaMax,double);
  vtkGetMacro(SigmaMax,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of scales sampled between SigmaMin and SigmaMax. Cached locally and
   * converted to the underlying ITK filter's Scales vector on Update(). Default: 0.
   */
  vtkSetMacro(NumberOfSigmaSteps,int);
  vtkGetMacro(NumberOfSigmaSteps,int);
  ///@}

  /**
   * Convenience method: set the (protected, no public getter) SigmaStepMethod to EQUISPACED_STEPS
   * (linear spacing between SigmaMin and SigmaMax; default).
   */
  void SetSigmaStepMethodToEquispaced()
  {
    this->SigmaStepMethod = EQUISPACED_STEPS;
  }

  /**
   * Convenience method: set the (protected, no public getter) SigmaStepMethod to
   * LOGARITHMIC_STEPS (logarithmic spacing, more scales concentrated near SigmaMin).
   */
  void SetSigmaStepMethodToLogarithmic()
  {
    this->SigmaStepMethod = LOGARITHMIC_STEPS;
  }

  /**
   * Set the diffusion time step size, delegated directly to the underlying
   * itk::VesselEnhancingDiffusion3DImageFilter (no local caching or corresponding getter).
   */
  void SetTimeStep(double value)
  {
    DelegateITKInputMacro(SetTimeStep,value);
  }

  /**
   * Set a small numerical regularization constant used by the underlying ITK filter to avoid
   * division by zero, delegated directly (no local caching or corresponding getter).
   */
  void SetEpsilon(double value)
  {
    DelegateITKInputMacro(SetEpsilon,value);
  }

  /**
   * Set the diffusion tensor construction parameter controlling background (non-vessel) smoothing
   * strength, delegated directly to the underlying ITK filter (no local caching or corresponding
   * getter).
   */
  void SetOmega(double value)
  {
    DelegateITKInputMacro(SetOmega,value);
  }

  /**
   * Set the sensitivity of the diffusion tensor construction to the local vesselness response,
   * delegated directly to the underlying ITK filter (no local caching or corresponding getter).
   */
  void SetSensitivity(double value)
  {
    DelegateITKInputMacro(SetSensitivity,value);
  }

  /**
   * Set the number of diffusion iterations, delegated directly to the underlying ITK filter's
   * SetIterations (no local caching or corresponding getter).
   */
  void SetNumberOfIterations(int value)
  {
    DelegateITKInputMacro(SetIterations,value);
  }

  /**
   * Set the vesselness sensitivity to the ratio between the two smallest Hessian eigenvalues,
   * delegated directly to the underlying ITK filter (no local caching or corresponding getter).
   */
  void SetAlpha(double value)
  {
    DelegateITKInputMacro(SetAlpha,value);
  }

  /**
   * Set the vesselness sensitivity to the ratio between the largest and the geometric mean of the
   * two smallest Hessian eigenvalues, delegated directly to the underlying ITK filter (no local
   * caching or corresponding getter).
   */
  void SetBeta(double value)
  {
    DelegateITKInputMacro(SetBeta,value);
  }

  /**
   * Set the vesselness sensitivity to the overall Hessian eigenvalue magnitude, delegated directly
   * to the underlying ITK filter (no local caching or corresponding getter).
   */
  void SetGamma(double value)
  {
    DelegateITKInputMacro(SetGamma,value);
  }

  /**
   * Set whether the vesselness response (and diffusion tensor) is recomputed at every iteration
   * (rather than only once), delegated directly to the underlying ITK filter (no local caching or
   * corresponding getter).
   */
  void SetRecalculateVesselness(int value)
  {
    DelegateITKInputMacro(SetRecalculateVesselness,value);
  }

  /**
   * Compute the sigma value for the given 0-based scale level, according to SigmaStepMethod, between
   * SigmaMin and SigmaMax over NumberOfSigmaSteps steps. Used internally by Update() to build the
   * underlying ITK filter's Scales vector.
   */
  double ComputeSigmaValue(int scaleLevel)
  {
    double sigmaValue;

    if (this->NumberOfSigmaSteps < 2)
    {
      return this->SigmaMin;
    }

    switch (this->SigmaStepMethod)
      {
      case EQUISPACED_STEPS:
        {
        double stepSize = ( SigmaMax - SigmaMin ) / (NumberOfSigmaSteps-1);
        if (stepSize < 1e-10)
          {
          stepSize = 1e-10;
          }
        sigmaValue = SigmaMin + stepSize * scaleLevel;
        break;
        }
      case LOGARITHMIC_STEPS:
        {
        double stepSize = ( vcl_log(SigmaMax) - vcl_log(SigmaMin) ) / (NumberOfSigmaSteps-1);
        if (stepSize < 1e-10)
          {
          stepSize = 1e-10;
          }
        sigmaValue = vcl_exp( vcl_log (SigmaMin) + stepSize * scaleLevel);
        break;
        }
      default:
        vtkErrorMacro("Error: undefined sigma step method.");
        sigmaValue = 0.0;
        break;
      }

    return sigmaValue;
  }

  /**
   * Build the underlying ITK filter's Scales vector from SigmaMin/SigmaMax/NumberOfSigmaSteps/
   * SigmaStepMethod via ComputeSigmaValue, then execute the filter.
   */
  void Update()
  {
    std::vector<float> scales;
    for (int i=0; i<this->NumberOfSigmaSteps; i++)
      {
      scales.push_back(this->ComputeSigmaValue(i));
      }
    this->GetImageFilterPointer()->SetScales(scales);
  }

//BTX
  /**
   * Values for SigmaStepMethod (set via SetSigmaStepMethodToEquispaced /
   * SetSigmaStepMethodToLogarithmic).
   */
  enum
  {
    EQUISPACED_STEPS,
    LOGARITHMIC_STEPS
  };
//ETX

protected:
  //BTX
  typedef itk::VesselEnhancingDiffusion3DImageFilter<Superclass::InputImageType::PixelType> ImageFilterType;

  vtkvmtkVesselEnhancingDiffusion3DImageFilter() : Superclass(ImageFilterType::New()) 
  {
    this->SigmaMin = 0.0;
    this->SigmaMax = 0.0;
    this->NumberOfSigmaSteps = 0;
    this->SigmaStepMethod = EQUISPACED_STEPS;

    this->GetImageFilterPointer()->SetDefaultPars();
  }

  ~vtkvmtkVesselEnhancingDiffusion3DImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;

private:
  vtkvmtkVesselEnhancingDiffusion3DImageFilter(const vtkvmtkVesselEnhancingDiffusion3DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselEnhancingDiffusion3DImageFilter&);  // Not implemented.
};

#endif




