/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkDanielssonDistanceMapImageFilter.h,v $
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

// .NAME vtkvmtkDanielssonDistanceMapImageFilter - Wrapper class around itk::DanielssonDistanceMapImageFilter
// .SECTION Description
// vtkvmtkDanielssonDistanceMapImageFilter


#ifndef __vtkvmtkDanielssonDistanceMapImageFilter_h
#define __vtkvmtkDanielssonDistanceMapImageFilter_h


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkDanielssonDistanceMapImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkDanielssonDistanceMapImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkDanielssonDistanceMapImageFilter, vtkvmtkITKImageToImageFilterFF);

  void SetSquaredDistance ( int value )
  {
    DelegateITKInputMacro ( SetSquaredDistance, (bool) value );
  }

  void SquaredDistanceOn()
  {
    this->SetSquaredDistance (true);
  }
  void SquaredDistanceOff()
  {
    this->SetSquaredDistance (false);
  }
  int GetSquaredDistance()
  { DelegateITKOutputMacro ( GetSquaredDistance ); }


  void SetInputIsBinary ( int value )
  {
    DelegateITKInputMacro ( SetInputIsBinary, (bool) value );
  }
  void InputIsBinaryOn()
  {
    this->SetInputIsBinary (true);
  }
  void InputIsBinaryOff()
  {
    this->SetInputIsBinary (false);
  }
  int GetInputIsBinary()
  { DelegateITKOutputMacro ( GetInputIsBinary ); };

protected:
  //BTX
  typedef itk::DanielssonDistanceMapImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkvmtkDanielssonDistanceMapImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkDanielssonDistanceMapImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkvmtkDanielssonDistanceMapImageFilter(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.
};

#endif




