/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkDanielssonDistanceMapImageFilter.h,v $
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

// .NAME vtkvmtkDanielssonDistanceMapImageFilter - Wrapper class around itk::DanielssonDistanceMapImageFilter
// .SECTION Description
// vtkvmtkDanielssonDistanceMapImageFilter


#ifndef __vtkvmtkDanielssonDistanceMapImageFilter_h
#define __vtkvmtkDanielssonDistanceMapImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkDanielssonDistanceMapImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkDanielssonDistanceMapImageFilter *New();
  vtkTypeMacro(vtkvmtkDanielssonDistanceMapImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(SquaredDistance,int);
  vtkSetMacro(SquaredDistance,int);
  vtkBooleanMacro(SquaredDistance,int);

  vtkGetMacro(InputIsBinary,int);
  vtkSetMacro(InputIsBinary,int);
  vtkBooleanMacro(InputIsBinary,int);

protected:

  vtkvmtkDanielssonDistanceMapImageFilter();
  ~vtkvmtkDanielssonDistanceMapImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkDanielssonDistanceMapImageFilter(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkDanielssonDistanceMapImageFilter&);  // Not implemented.

  int SquaredDistance;
  int InputIsBinary;
};

#endif




