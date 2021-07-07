/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkFastMarchingUpwindGradientImageFilter.h,v $
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

// .NAME vtkvmtkFastMarchingUpwindGradientImageFilter - Wrapper class around itk::FastMarchingUpwindGradientImageFilter
// .SECTION Description
// vtkvmtkFastMarchingUpwindGradientImageFilter


#ifndef __vtkvmtkFastMarchingUpwindGradientImageFilter_h
#define __vtkvmtkFastMarchingUpwindGradientImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkFastMarchingUpwindGradientImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkFastMarchingUpwindGradientImageFilter *New();
  vtkTypeMacro(vtkvmtkFastMarchingUpwindGradientImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(GenerateGradientImage,int);
  vtkSetMacro(GenerateGradientImage,int);
  vtkBooleanMacro(GenerateGradientImage,int);

  vtkGetMacro(TargetReachedMode,int);
  vtkSetMacro(TargetReachedMode,int);

  void SetTargetReachedModeToOneTarget()
  {
    this->SetTargetReachedMode(ONE_TARGET);
  }

  void SetTargetReachedModeToAllTargets()
  {
    this->SetTargetReachedMode(ALL_TARGETS);
  }

  void SetTargetReachedModeToNoTargets()
  {
    this->SetTargetReachedMode(NO_TARGETS);
  }

  enum
  {
    ONE_TARGET,
    ALL_TARGETS,
    NO_TARGETS
  };

  vtkGetMacro(TargetValue,double);

  vtkGetMacro(TargetOffset,double);
  vtkSetMacro(TargetOffset,double);

  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);

  vtkSetObjectMacro(Targets,vtkIdList);
  vtkGetObjectMacro(Targets,vtkIdList);

protected:
  vtkvmtkFastMarchingUpwindGradientImageFilter();
  ~vtkvmtkFastMarchingUpwindGradientImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkFastMarchingUpwindGradientImageFilter(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkFastMarchingUpwindGradientImageFilter&);  // Not implemented.

  int GenerateGradientImage;
  int TargetReachedMode;
  double TargetValue;
  double TargetOffset;

  vtkIdList* Seeds;
  vtkIdList* Targets;
};

#endif
