/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLevelSetSigmoidFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkLevelSetSigmoidFilter - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkLevelSetSigmoidFilter_h
#define __vtkvmtkLevelSetSigmoidFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLevelSetSigmoidFilter : public vtkSimpleImageToImageFilter
{
  public: 
  vtkTypeMacro(vtkvmtkLevelSetSigmoidFilter,vtkSimpleImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkLevelSetSigmoidFilter *New();

  vtkSetObjectMacro(LevelSetsImage,vtkImageData);
  vtkGetObjectMacro(LevelSetsImage,vtkImageData);

  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);

  vtkSetMacro(ScaleValue,double);
  vtkGetMacro(ScaleValue,double);

  vtkSetMacro(ComputeScaleValueFromInput,int);
  vtkGetMacro(ComputeScaleValueFromInput,int);
  vtkBooleanMacro(ComputeScaleValueFromInput,int);

  protected:
  vtkvmtkLevelSetSigmoidFilter();
  ~vtkvmtkLevelSetSigmoidFilter();  

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) VTK_OVERRIDE;

  vtkImageData *LevelSetsImage;

  double Sigma;
  double ScaleValue;
  int ComputeScaleValueFromInput;

  private:
  vtkvmtkLevelSetSigmoidFilter(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
  void operator=(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
};

#endif
