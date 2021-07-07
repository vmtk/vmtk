/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCollidingFrontsImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.4 $

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

// .NAME vtkvmtkCollidingFrontsImageFilter - Wrapper class around itk::CollidingFrontsImageFilter
// .SECTION Description
// vtkvmtkCollidingFrontsImageFilter


#ifndef __vtkvmtkCollidingFrontsImageFilter_h
#define __vtkvmtkCollidingFrontsImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkCollidingFrontsImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkCollidingFrontsImageFilter *New();
  vtkTypeMacro(vtkvmtkCollidingFrontsImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(ApplyConnectivity,int);
  vtkSetMacro(ApplyConnectivity,int);
  vtkBooleanMacro(ApplyConnectivity,int);

  vtkGetMacro(NegativeEpsilon,double);
  vtkSetMacro(NegativeEpsilon,double);

  vtkGetMacro(StopOnTargets,int);
  vtkSetMacro(StopOnTargets,int);
  vtkBooleanMacro(StopOnTargets,int);

  vtkSetObjectMacro(Seeds1,vtkIdList);
  vtkGetObjectMacro(Seeds1,vtkIdList);

  vtkSetObjectMacro(Seeds2,vtkIdList);
  vtkGetObjectMacro(Seeds2,vtkIdList);

protected:

  vtkvmtkCollidingFrontsImageFilter();
  ~vtkvmtkCollidingFrontsImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkCollidingFrontsImageFilter(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkCollidingFrontsImageFilter&);  // Not implemented.

  vtkIdList* Seeds1;
  vtkIdList* Seeds2;

  int ApplyConnectivity;
  double NegativeEpsilon;
  int StopOnTargets;
};

#endif
