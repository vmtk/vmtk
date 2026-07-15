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
 * @class   vtkvmtkRecursiveGaussianImageFilter
 * @brief   Wraps itk::RecursiveGaussianImageFilter, applied to a 3D image.
 * @ingroup Segmentation
 *
 * vtkvmtkRecursiveGaussianImageFilter smooths a 3D input image with a Gaussian kernel of standard
 * deviation Sigma, using ITK's IIR (recursive) approximation, which is efficient for large Sigma
 * values compared to a direct convolution. Note that itk::RecursiveGaussianImageFilter smooths
 * along a single direction (the default, direction 0 / X); this wrapper does not expose the
 * direction, so it only blurs along the first image axis. Like the other ITK wrappers in this
 * module, it is a thin vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to a
 * 3D float itk::Image, runs itk::RecursiveGaussianImageFilter, and converts the result back to
 * vtkImageData.
 *
 * @sa vtkvmtkRecursiveGaussian2DImageFilter
 */

#ifndef __vtkvmtkRecursiveGaussianImageFilter_h
#define __vtkvmtkRecursiveGaussianImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkRecursiveGaussianImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkRecursiveGaussianImageFilter *New();
  vtkTypeMacro(vtkvmtkRecursiveGaussianImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the standard deviation, in physical units, of the Gaussian smoothing kernel applied
   * along the first image axis. Default: 1.0.
   */
  vtkGetMacro(Sigma,double);
  vtkSetMacro(Sigma,double);
  ///@}

  ///@{
  /**
   * Toggle normalization of the Gaussian kernel across scale (ITK's NormalizeAcrossScale option),
   * which keeps the response comparable across different Sigma values, at the cost of no longer
   * exactly preserving the input's DC (mean) value. Default: off.
   */
  vtkGetMacro(NormalizeAcrossScale,int);
  vtkSetMacro(NormalizeAcrossScale,int);
  vtkBooleanMacro(NormalizeAcrossScale,int);
  ///@}

 protected:
  vtkvmtkRecursiveGaussianImageFilter();
  ~vtkvmtkRecursiveGaussianImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

 private:
  vtkvmtkRecursiveGaussianImageFilter(const vtkvmtkRecursiveGaussianImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkRecursiveGaussianImageFilter&);  // Not implemented.

  double Sigma;
  int NormalizeAcrossScale;
};

#endif

