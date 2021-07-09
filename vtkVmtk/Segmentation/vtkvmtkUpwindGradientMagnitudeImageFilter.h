/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUpwindGradientMagnitudeImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

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

// .NAME vtkvmtkUpwindGradientMagnitudeImageFilter - Wrapper class around itk::UpwindGradientMagnitudeImageFilter
// .SECTION Description
// vtkvmtkUpwindGradientMagnitudeImageFilter


#ifndef __vtkvmtkUpwindGradientMagnitudeImageFilter_h
#define __vtkvmtkUpwindGradientMagnitudeImageFilter_h


#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkUpwindGradientMagnitudeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkUpwindGradientMagnitudeImageFilter *New();
  vtkTypeMacro(vtkvmtkUpwindGradientMagnitudeImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(UpwindFactor,double);
  vtkSetMacro(UpwindFactor,double);

protected:
  vtkvmtkUpwindGradientMagnitudeImageFilter();
  ~vtkvmtkUpwindGradientMagnitudeImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkUpwindGradientMagnitudeImageFilter(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.

  double UpwindFactor;
};

#endif




