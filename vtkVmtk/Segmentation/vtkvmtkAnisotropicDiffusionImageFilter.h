/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga. All rights reserved.
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
  Note: this class was contributed by 
      Kurt Sansom
      https://github.com/kayarre

=========================================================================*/
/**
 * @class   vtkvmtkAnisotropicDiffusionImageFilter
 * @brief   Wraps itk::GradientAnisotropicDiffusionImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkAnisotropicDiffusionImageFilter runs ITK's gradient anisotropic diffusion on a scalar
 * vtkImageData (2D or 3D, any of the usual integer/floating-point scalar types when ITK < 5; only
 * float/double when ITK >= 5, since the ITK filter requires a floating-point pixel type). Diffusion
 * smooths within homogeneous regions while preserving edges, controlled by the number of iterations,
 * the time step of each iteration, and the conductance parameter of the underlying diffusion
 * equation. This is one of the vessel-enhancement methods offered by the vmtkimagesmoothing pype
 * script (its "anisotropic" Method).
 *
 * @sa vtkvmtkVesselEnhancingDiffusionImageFilter, vtkvmtkVesselEnhancingDiffusion3DImageFilter
 */

#ifndef __vtkvmtkAnisotropicDiffusionImageFilter_h
#define __vtkvmtkAnisotropicDiffusionImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkAnisotropicDiffusionImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkAnisotropicDiffusionImageFilter *New();
  vtkTypeMacro(vtkvmtkAnisotropicDiffusionImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the number of anisotropic diffusion iterations applied to the image. Default: 5.
   */
  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the time step of each diffusion iteration. Must be small enough relative to the image
   * spacing and dimensionality to remain numerically stable (vmtkimagesmoothing computes a stable
   * value automatically as pixel spacing / 2^(dimensionality+1) when its AutoCalculateTimeStep option
   * is on). Default: 0.0625.
   */
  vtkGetMacro(TimeStep,double);
  vtkSetMacro(TimeStep,double);
  ///@}

  ///@{
  /**
   * Set/get the conductance parameter of the underlying gradient anisotropic diffusion equation,
   * controlling the sensitivity of the diffusion to image gradients (lower values preserve sharper
   * edges). Default: 1.0.
   */
  vtkGetMacro(Conductance,double);
  vtkSetMacro(Conductance,double);
  ///@}


protected:

  vtkvmtkAnisotropicDiffusionImageFilter();
  ~vtkvmtkAnisotropicDiffusionImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  template< class TImage >
  int FilterImage( vtkImageData*, vtkImageData*, typename TImage::Pointer );
  
  template< int VDimension >
  int FilterScalarImage( vtkImageData* , vtkImageData* , int );
  
  vtkvmtkAnisotropicDiffusionImageFilter(const vtkvmtkAnisotropicDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkAnisotropicDiffusionImageFilter&);  // Not implemented.

  int NumberOfIterations;
  double TimeStep;
  double Conductance;
  
};

#endif

