/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkRecursiveGaussian2DImageFilter.h,v $
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

// .NAME vtkvmtkRecursiveGaussian2DImageFilter - Wrapper class around itk::RecursiveGaussian2DImageFilter
// .SECTION Description
// vtkvmtkImageFilter


#ifndef __vtkvmtkRecursiveGaussian2DImageFilter_h
#define __vtkvmtkRecursiveGaussian2DImageFilter_h


#include "vtkvmtkITKImageToImageFilter2DFF.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkRecursiveGaussian2DImageFilter : public vtkvmtkITKImageToImageFilter2DFF
{
 public:
  static vtkvmtkRecursiveGaussian2DImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkRecursiveGaussian2DImageFilter, vtkvmtkITKImageToImageFilter2DFF);

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
  vtkvmtkRecursiveGaussian2DImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkRecursiveGaussian2DImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkRecursiveGaussian2DImageFilter(const vtkvmtkRecursiveGaussian2DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkRecursiveGaussian2DImageFilter&);  // Not implemented.
};

#endif




