/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkRecursiveGaussianImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

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

// .NAME vtkvmtkRecursiveGaussianImageFilter - Wrapper class around itk::RecursiveGaussianImageFilter
// .SECTION Description
// vtkvmtkImageFilter


#ifndef __vtkvmtkRecursiveGaussianImageFilter_h
#define __vtkvmtkRecursiveGaussianImageFilter_h


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkRecursiveGaussianImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkRecursiveGaussianImageFilter *New();
  vtkTypeMacro(vtkvmtkRecursiveGaussianImageFilter, vtkvmtkITKImageToImageFilterFF);

  void SetSigma ( float value )
  {
    DelegateITKInputMacro ( SetSigma, value );
  };

  void SetNormalizeAcrossScale ( int value )
  {
    DelegateITKInputMacro ( SetNormalizeAcrossScale, value );
  };

  int GetNormalizeAcrossScale()
  {
    DelegateITKOutputMacro( GetNormalizeAcrossScale );
  };

protected:
  //BTX
  typedef itk::RecursiveGaussianImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkRecursiveGaussianImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkRecursiveGaussianImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkRecursiveGaussianImageFilter(const vtkvmtkRecursiveGaussianImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkRecursiveGaussianImageFilter&);  // Not implemented.
};

#endif




