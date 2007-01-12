/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkObjectnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkObjectnessMeasureImageFilter - Wrapper class around itk::ObjectnessMeasureImageFilter
// .SECTION Description
// vtkvmtkObjectnessMeasureImageFilter


#ifndef __vtkvmtkObjectnessMeasureImageFilter_h
#define __vtkvmtkObjectnessMeasureImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkObjectnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkObjectnessMeasureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkObjectnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkObjectnessMeasureImageFilter, vtkITKImageToImageFilterFF);

  void SetSigma ( float value )
  {
    DelegateITKInputMacro ( SetSigma, value );
  };

  void SetNormalizeAcrossScale ( int value )
  {
    DelegateITKInputMacro ( SetNormalizeAcrossScale, value );
  };

  void SetEnhancementFlagToEnhance(unsigned int value)
  {
    DelegateITKInputMacro ( SetEnhancementFlagToEnhance, value );
  }
  
  void SetEnhancementFlagToSuppress(unsigned int value)
  {
    DelegateITKInputMacro ( SetEnhancementFlagToSuppress, value );
  }
  
  void SetEnhancementFlagToIgnore(unsigned int value)
  {
    DelegateITKInputMacro ( SetEnhancementFlagToIgnore, value );
  }

  void SetEigenValueSignConstraintToNoConstraint(unsigned int value)
  {
    DelegateITKInputMacro ( SetEigenValueSignConstraintToNoConstraint, value );
  }
  
  void SetEigenValueSignConstraintToPositive(unsigned int value)
  {
    DelegateITKInputMacro ( SetEigenValueSignConstraintToPositive, value );
  }
  
  void SetEigenValueSignConstraintToNegative(unsigned int value)
  {
    DelegateITKInputMacro ( SetEigenValueSignConstraintToNegative, value );
  }

  void SetAlpha(unsigned int value1, double value2)
  {
    DelegateITKInputMacro2 ( SetAlpha, value1, value2 );
  }
  
protected:
  //BTX
  typedef itk::ObjectnessMeasureImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkvmtkObjectnessMeasureImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkObjectnessMeasureImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkObjectnessMeasureImageFilter(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
};

#endif




