/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkVesselnessMeasureImageFilter - Wrapper class around itk::VesselnessMeasureImageFilter
// .SECTION Description
// vtkvmtkVesselnessMeasureImageFilter


#ifndef __vtkvmtkVesselnessMeasureImageFilter_h
#define __vtkvmtkVesselnessMeasureImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkVesselnessMeasureImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselnessMeasureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkVesselnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkVesselnessMeasureImageFilter, vtkITKImageToImageFilterFF);

  void SetSigma ( float value )
  {
    DelegateITKInputMacro ( SetSigma, value );
  };

  void SetNormalizeAcrossScale ( int value )
  {
    DelegateITKInputMacro ( SetNormalizeAcrossScale, value );
  };

  void SetAlpha1 ( float value )
  {
    DelegateITKInputMacro ( SetAlpha1, value );
  };
 
  void SetAlpha2 ( float value )
  {
    DelegateITKInputMacro ( SetAlpha2, value );
  };

protected:
  //BTX
  typedef itk::VesselnessMeasureImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkVesselnessMeasureImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkVesselnessMeasureImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkVesselnessMeasureImageFilter(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkvmtkVesselnessMeasureImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkvmtkVesselnessMeasureImageFilter);

#endif




