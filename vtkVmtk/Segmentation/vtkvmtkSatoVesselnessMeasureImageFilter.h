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
 * @class   vtkvmtkSatoVesselnessMeasureImageFilter
 * @brief   Wraps itk::Hessian3DToVesselnessMeasureImageFilter / itk::MultiScaleHessianBasedMeasureImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkSatoVesselnessMeasureImageFilter computes a multiscale, Hessian-eigenvalue-based
 * vesselness measure using the line filter of Sato et al. ("Three-dimensional multi-scale line
 * filter for segmentation and visualization of curvilinear structures in medical images", 1998),
 * evaluated at multiple Gaussian scales between SigmaMin and SigmaMax and combined by taking, at
 * each voxel, the maximum response over scales. It is one of the vessel enhancement methods
 * selectable (as "sato") by the vmtkimagevesselenhancement pype script, alongside the Frangi-style
 * measure computed by vtkvmtkVesselnessMeasureImageFilter. Like the other ITK wrappers in this
 * module, it is a thin vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to a
 * float itk::Image, configures and runs itk::MultiScaleHessianBasedMeasureImageFilter with an
 * itk::Hessian3DToVesselnessMeasureImageFilter as its per-scale measure, and converts the
 * vesselness output back to vtkImageData.
 *
 * @sa vtkvmtkVesselnessMeasureImageFilter, vtkvmtkObjectnessMeasureImageFilter
 */

#ifndef __vtkvmtkSatoVesselnessMeasureImageFilter_h
#define __vtkvmtkSatoVesselnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSatoVesselnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkSatoVesselnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkSatoVesselnessMeasureImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the smallest Gaussian scale (standard deviation, in physical units) at which the
   * Hessian-based vesselness measure is evaluated. Default: 1.0.
   */
  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);
  ///@}

  ///@{
  /**
   * Set/get the largest Gaussian scale (standard deviation, in physical units) at which the
   * Hessian-based vesselness measure is evaluated. Default: 2.0.
   */
  vtkGetMacro(SigmaMax,double);
  vtkSetMacro(SigmaMax,double);
  ///@}

  ///@{
  /**
   * Set/get the number of discrete scales sampled between SigmaMin and SigmaMax (inclusive).
   * Default: 2.
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

  ///@{
  /**
   * Set/get the alpha1 parameter of Sato's line filter, which weights the response when the
   * smallest-eigenvalue ratio indicates a blob-like (rather than tube-like) structure, suppressing
   * blobs. Default: 0.5.
   */
  vtkGetMacro(Alpha1,double);
  vtkSetMacro(Alpha1,double);
  ///@}

  ///@{
  /**
   * Set/get the alpha2 parameter of Sato's line filter, which weights the response when the
   * eigenvalue ratio indicates a plate-like (rather than tube-like) structure, suppressing plates.
   * Default: 2.0.
   */
  vtkGetMacro(Alpha2,double);
  vtkSetMacro(Alpha2,double);
  ///@}
//BTX
  enum
  {
    EQUISPACED,
    LOGARITHMIC
  };
//ETX
protected:
  vtkvmtkSatoVesselnessMeasureImageFilter();
  ~vtkvmtkSatoVesselnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkSatoVesselnessMeasureImageFilter(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  double Alpha1;
  double Alpha2;
};

#endif




