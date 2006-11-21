/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkGradientMagnitudeImageFilter.h,v $
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


#ifndef __vtkvmtkGradientMagnitudeImageFilter_h
#define __vtkvmtkGradientMagnitudeImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkGradientMagnitudeImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkGradientMagnitudeImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkGradientMagnitudeImageFilter, vtkITKImageToImageFilterFF);

protected:
  //BTX
  typedef itk::GradientMagnitudeImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkGradientMagnitudeImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkGradientMagnitudeImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkGradientMagnitudeImageFilter(const vtkvmtkGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkGradientMagnitudeImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkvmtkGradientMagnitudeImageFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkGradientMagnitudeImageFilter);

#endif




