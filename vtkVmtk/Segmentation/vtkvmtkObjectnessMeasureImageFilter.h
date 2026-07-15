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
 * @class   vtkvmtkObjectnessMeasureImageFilter
 * @brief   Wraps itk::HessianToObjectnessMeasureImageFilter / itk::MultiScaleHessianBasedMeasureImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkObjectnessMeasureImageFilter computes a Frangi-style multiscale, Hessian-eigenvalue-based
 * "objectness" measure that responds to blob-, tube-, or plate-like structures depending on
 * ObjectDimension (0 = blob, 1 = line/vessel, 2 = plate), evaluated at multiple Gaussian scales
 * between SigmaMin and SigmaMax and combined by taking, at each voxel, the response of the scale
 * that maximizes it. It is the more general filter behind the vmtkimageobjectenhancement pype
 * script (vtkvmtkVesselnessMeasureImageFilter specializes the same underlying machinery to
 * ObjectDimension = 1 for vessel enhancement). Like the other ITK wrappers in this module, it is a
 * thin vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to a float itk::Image,
 * configures and runs itk::MultiScaleHessianBasedMeasureImageFilter with an
 * itk::HessianToObjectnessMeasureImageFilter as its per-scale measure, and converts the objectness
 * output (and, if requested, the per-voxel scale at which the maximum response was found, via
 * ScalesOutput) back to vtkImageData.
 *
 * @sa vtkvmtkVesselnessMeasureImageFilter, vtkvmtkSatoVesselnessMeasureImageFilter
 */

#ifndef __vtkvmtkObjectnessMeasureImageFilter_h
#define __vtkvmtkObjectnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkObjectnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkObjectnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkObjectnessMeasureImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the smallest Gaussian scale (standard deviation, in physical units) at which the
   * Hessian-based objectness measure is evaluated. Default: 1.0.
   */
  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);
  ///@}

  ///@{
  /**
   * Set/get the largest Gaussian scale (standard deviation, in physical units) at which the
   * Hessian-based objectness measure is evaluated. Default: 1.0.
   */
  vtkGetMacro(SigmaMax,double);
  vtkSetMacro(SigmaMax,double);
  ///@}

  ///@{
  /**
   * Set/get the number of discrete scales sampled between SigmaMin and SigmaMax (inclusive).
   * Default: 1.
   */
  vtkGetMacro(NumberOfSigmaSteps,int);
  vtkSetMacro(NumberOfSigmaSteps,int);
  ///@}

  ///@{
  /**
   * Set/get how intermediate scales between SigmaMin and SigmaMax are distributed: EQUISPACED for
   * linear spacing, LOGARITHMIC for logarithmic spacing. Use SetSigmaStepMethodToEquispaced /
   * SetSigmaStepMethodToLogarithmic for convenience. Default: EQUISPACED.
   */
  vtkGetMacro(SigmaStepMethod,int);
  vtkSetMacro(SigmaStepMethod,int);
  ///@}

  /**
   * Convenience method equivalent to SetSigmaStepMethod(EQUISPACED).
   */
  void SetSigmaStepMethodToEquispaced()
  {
    this->SetSigmaStepMethod(EQUISPACED);
  }

  /**
   * Convenience method equivalent to SetSigmaStepMethod(LOGARITHMIC).
   */
  void SetSigmaStepMethodToLogarithmic()
  {
    this->SetSigmaStepMethod(LOGARITHMIC);
  }

  enum
  {
    EQUISPACED,
    LOGARITHMIC
  };

  ///@{
  /**
   * Toggle scaling of the objectness measure output by the maximum Frobenius norm of the Hessian
   * found over the multiscale analysis, which normalizes the response range. Default: off.
   */
  vtkGetMacro(UseScaledObjectness,int);
  vtkSetMacro(UseScaledObjectness,int);
  ///@}

  ///@{
  /**
   * Set/get the Frangi objectness weight controlling sensitivity to the ratio between the smallest
   * and the geometric mean/other eigenvalues (deviation from a blob), i.e. the alpha parameter of
   * the underlying Hessian-based measure. Default: 1.0.
   */
  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);
  ///@}

  ///@{
  /**
   * Set/get the Frangi objectness weight controlling sensitivity to deviation from a plate-like
   * structure (the beta parameter of the underlying Hessian-based measure). Default: 1.0.
   */
  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);
  ///@}

  ///@{
  /**
   * Set/get the Frangi objectness weight controlling sensitivity to the overall Hessian magnitude
   * (used to suppress the response in low-contrast/background regions). Default: 1.0.
   */
  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);
  ///@}

  ///@{
  /**
   * Set/get the dimensionality of the structure to enhance: 0 for blob-like objects, 1 for
   * line/tube-like (vessel) objects, 2 for plate-like objects. Default: 1.
   */
  vtkGetMacro(ObjectDimension,int);
  vtkSetMacro(ObjectDimension,int);
  ///@}

  /**
   * Get the image, with the same extent as the output, holding the Gaussian scale at which the
   * maximum objectness response was found at each voxel. Valid only after Update() has been called.
   */
  vtkGetObjectMacro(ScalesOutput,vtkImageData);

protected:
  vtkvmtkObjectnessMeasureImageFilter();
  ~vtkvmtkObjectnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkObjectnessMeasureImageFilter(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int UseScaledObjectness;
  double Alpha;
  double Beta;
  double Gamma;
  int ObjectDimension;
  vtkImageData* ScalesOutput;
};

#endif

