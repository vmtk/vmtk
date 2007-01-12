/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkObjectnessMeasure2DImageFilter.h,v $
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

// .NAME vtkvmtkObjectnessMeasure2DImageFilter - Wrapper class around itk::ObjectnessMeasureImageFilter
// .SECTION Description
// vtkvmtkObjectnessMeasure2DImageFilter


#ifndef __vtkvmtkObjectnessMeasure2DImageFilter_h
#define __vtkvmtkObjectnessMeasure2DImageFilter_h


#include "vtkITKImageToImageFilter2DFF.h"
#include "itkObjectnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkObjectnessMeasure2DImageFilter : public vtkITKImageToImageFilter2DFF
{
 public:
  static vtkvmtkObjectnessMeasure2DImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkObjectnessMeasure2DImageFilter, vtkITKImageToImageFilter2DFF);

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
  vtkvmtkObjectnessMeasure2DImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkvmtkObjectnessMeasure2DImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkvmtkObjectnessMeasure2DImageFilter(const vtkvmtkObjectnessMeasure2DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkObjectnessMeasure2DImageFilter&);  // Not implemented.
};

#endif




