/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSigmoidImageFilter.h,v $
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
// .NAME vtkvmtkSigmoidImageFilter - Wrapper class around itk::SigmoidImageFilter
// .SECTION Description
// vtkvmtkSigmoidImageFilter


#ifndef __vtkvmtkSigmoidImageFilter_h
#define __vtkvmtkSigmoidImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSigmoidImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkSigmoidImageFilter *New();
  vtkTypeMacro(vtkvmtkSigmoidImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);

  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);

  vtkGetMacro(OutputMinimum,double);
  vtkSetMacro(OutputMinimum,double);

  vtkGetMacro(OutputMaximum,double);
  vtkSetMacro(OutputMaximum,double);

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




