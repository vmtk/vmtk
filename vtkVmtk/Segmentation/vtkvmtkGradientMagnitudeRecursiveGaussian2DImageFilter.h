/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter.h,v $
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

// .NAME vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter - Wrapper class around itk::GradientMagnitudeRecursiveGaussian2DImageFilter
// .SECTION Description
// vtkvmtkGradientMagnitudeImageFilter


#ifndef __vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter_h
#define __vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter_h


#include "vtkITKImageToImageFilter2DFF.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter : public vtkITKImageToImageFilter2DFF
{
 public:
  static vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter, vtkITKImageToImageFilter2DFF);

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
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter(const vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkvmtkGradientMagnitudeRecursiveGaussian2DImageFilter);

#endif




