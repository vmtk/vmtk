/*=========================================================================

Program:   VMTK 
Module:    $RCSfile: vtkvmtkBoundedReciprocalImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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

// .NAME vtkvmtkBoundedReciprocalImageFilter - Wrapper class around itk::BoundedReciprocalImageFilter
// .SECTION Description
// vtkvmtkBoundedReciprocalImageFilter


#ifndef __vtkvmtkBoundedReciprocalImageFilter_h
#define __vtkvmtkBoundedReciprocalImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkBoundedReciprocalImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkBoundedReciprocalImageFilter *New();
  vtkTypeMacro(vtkvmtkBoundedReciprocalImageFilter, vtkSimpleImageToImageFilter);

protected:

  vtkvmtkBoundedReciprocalImageFilter() {};
  ~vtkvmtkBoundedReciprocalImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

  //BTX
  //typedef itk::BoundedReciprocalImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  //vtkvmtkBoundedReciprocalImageFilter() : Superclass ( ImageFilterType::New() ){};
  //~vtkvmtkBoundedReciprocalImageFilter() {};
  //ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX

private:
  vtkvmtkBoundedReciprocalImageFilter(const vtkvmtkBoundedReciprocalImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkBoundedReciprocalImageFilter&);  // Not implemented.

};

#endif




