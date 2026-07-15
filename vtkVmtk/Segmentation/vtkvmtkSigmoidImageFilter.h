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
 * @class   vtkvmtkSigmoidImageFilter
 * @brief   Wraps itk::SigmoidImageFilter.
 * @ingroup Segmentation
 *
 * vtkvmtkSigmoidImageFilter remaps input image intensities through a sigmoid function into the
 * range [OutputMinimum, OutputMaximum], with Alpha controlling the width and Beta the intensity
 * location of the transition. It is used by vmtkimagefeatures / vmtklevelsetsegmentation (as the
 * "SigmoidRemapping" option) to remap a gradient-magnitude-derived feature image into [0,1] with
 * low values near strong edges and high values elsewhere, for use as a level set speed/feature
 * image (an alternative to vtkvmtkBoundedReciprocalImageFilter). Like the other ITK wrappers in
 * this module, it is a thin vtkSimpleImageToImageFilter: SimpleExecute() converts the VTK input to
 * a float itk::Image, runs itk::SigmoidImageFilter, and converts the result back to vtkImageData.
 *
 * @sa vtkvmtkBoundedReciprocalImageFilter
 */

#ifndef __vtkvmtkSigmoidImageFilter_h
#define __vtkvmtkSigmoidImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSigmoidImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkSigmoidImageFilter *New();
  vtkTypeMacro(vtkvmtkSigmoidImageFilter, vtkSimpleImageToImageFilter);

  ///@{
  /**
   * Set/get the width parameter of the sigmoid transition (the intensity range over which the
   * output ramps between OutputMinimum and OutputMaximum). A negative value inverts the sigmoid so
   * that output decreases with increasing input intensity. Default: 1.0.
   */
  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);
  ///@}

  ///@{
  /**
   * Set/get the input intensity value at which the sigmoid output is halfway between OutputMinimum
   * and OutputMaximum. Default: 1.0.
   */
  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);
  ///@}

  ///@{
  /**
   * Set/get the output intensity value approached as input intensity goes to (Alpha positive: minus
   * infinity; Alpha negative: plus infinity). Default: 0.0.
   */
  vtkGetMacro(OutputMinimum,double);
  vtkSetMacro(OutputMinimum,double);
  ///@}

  ///@{
  /**
   * Set/get the output intensity value approached as input intensity goes to (Alpha positive: plus
   * infinity; Alpha negative: minus infinity). Default: 1.0.
   */
  vtkGetMacro(OutputMaximum,double);
  vtkSetMacro(OutputMaximum,double);
  ///@}

protected:

  vtkvmtkSigmoidImageFilter();
  ~vtkvmtkSigmoidImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkSigmoidImageFilter(const vtkvmtkSigmoidImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSigmoidImageFilter&);  // Not implemented.

  double Alpha;
  double Beta;
  double OutputMinimum;
  double OutputMaximum;
};

#endif




