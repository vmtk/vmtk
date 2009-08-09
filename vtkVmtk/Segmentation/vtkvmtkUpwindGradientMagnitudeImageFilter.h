/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUpwindGradientMagnitudeImageFilter.h,v $
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

// .NAME vtkvmtkUpwindGradientMagnitudeImageFilter - Wrapper class around itk::UpwindGradientMagnitudeImageFilter
// .SECTION Description
// vtkvmtkUpwindGradientMagnitudeImageFilter


#ifndef __vtkvmtkUpwindGradientMagnitudeImageFilter_h
#define __vtkvmtkUpwindGradientMagnitudeImageFilter_h


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkUpwindGradientMagnitudeImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkUpwindGradientMagnitudeImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkUpwindGradientMagnitudeImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkUpwindGradientMagnitudeImageFilter, vtkvmtkITKImageToImageFilterFF);

  void SetUpwindFactor ( float value )
  {
    DelegateITKInputMacro ( SetUpwindFactor, value );
  };

protected:
  //BTX
  typedef itk::UpwindGradientMagnitudeImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkUpwindGradientMagnitudeImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkUpwindGradientMagnitudeImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkUpwindGradientMagnitudeImageFilter(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkUpwindGradientMagnitudeImageFilter&);  // Not implemented.
};

#endif




