/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkNormalizeImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

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

// .NAME vtkvmtkNormalizeImageFilter - Wrapper class around itk::NormalizeImageFilter
// .SECTION Description
// vtkvmtkNormalizeImageFilter


#ifndef __vtkvmtkNormalizeImageFilter_h
#define __vtkvmtkNormalizeImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkFloatArray;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkNormalizeImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkNormalizeImageFilter *New();
  vtkTypeMacro(vtkvmtkNormalizeImageFilter, vtkSimpleImageToImageFilter);

protected:
  vtkvmtkNormalizeImageFilter();
  ~vtkvmtkNormalizeImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkNormalizeImageFilter(const vtkvmtkNormalizeImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkNormalizeImageFilter&);  // Not implemented.
};

#endif

