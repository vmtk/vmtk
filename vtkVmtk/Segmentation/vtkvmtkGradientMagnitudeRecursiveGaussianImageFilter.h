/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter.h,v $
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

// .NAME vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter - Wrapper class around itk::GradientMagnitudeRecursiveGaussianImageFilter
// .SECTION Description
// vtkvmtkGradientMagnitudeImageFilter


#ifndef __vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter_h
#define __vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter, vtkITKImageToImageFilterFF);

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
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter(const vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkGradientMagnitudeRecursiveGaussianImageFilter);

#endif




