/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkFWHMFeatureImageFilter.h,v $
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

// .NAME vtkvmtkFWHMFeatureImageFilter - Wrapper class around itk::FWHMFeatureImageFilter
// .SECTION Description
// vtkvmtkFWHMFeatureImageFilter


#ifndef __vtkvmtkFWHMFeatureImageFilter_h
#define __vtkvmtkFWHMFeatureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFWHMFeatureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkFWHMFeatureImageFilter *New();
  vtkTypeMacro(vtkvmtkFWHMFeatureImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(UseImageSpacing,int);
  vtkSetMacro(UseImageSpacing,int);
  vtkBooleanMacro(UseImageSpacing,int);

  vtkGetVectorMacro(Radius,int,3);
  vtkSetVectorMacro(Radius,int,3);

  vtkGetMacro(BackgroundValue,double);
  vtkSetMacro(BackgroundValue,double);

protected:
  vtkvmtkFWHMFeatureImageFilter();
  ~vtkvmtkFWHMFeatureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkFWHMFeatureImageFilter(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFWHMFeatureImageFilter&);  // Not implemented.

  double BackgroundValue;
  int UseImageSpacing;
  int* Radius;
};

#endif
