/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSigmoidImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

// .NAME vtkvmtkGradientMagnitudeImageFilter - Wrapper class around itk::GradientMagnitudeImageFilterImageFilter
// .SECTION Description
// vtkvmtkGradientMagnitudeImageFilter


#ifndef __vtkvmtkSigmoidImageFilter_h
#define __vtkvmtkSigmoidImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkSigmoidImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSigmoidImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkSigmoidImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkSigmoidImageFilter, vtkITKImageToImageFilterFF);

  void SetAlpha ( float value )
  {
    DelegateITKInputMacro ( SetAlpha, value );
  };

  void SetBeta ( float value )
  {
    DelegateITKInputMacro ( SetBeta, value );
  };

  void SetOutputMinimum ( float value )
  {
    DelegateITKInputMacro ( SetOutputMinimum, value );
  };

  void SetOutputMaximum ( float value )
  {
    DelegateITKInputMacro ( SetOutputMaximum, value );
  };

protected:

  //BTX
  typedef itk::SigmoidImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkSigmoidImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkSigmoidImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkvmtkSigmoidImageFilter(const vtkvmtkSigmoidImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSigmoidImageFilter&);  // Not implemented.
};

#endif




