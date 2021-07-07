/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLevelSetSigmoidFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkLevelSetSigmoidFilter - Apply a sigmoid function to every voxel of an image.
// .SECTION Description
// This is primarily used in feature image correction (see http://www.vmtk.org/tutorials/ImageFeatureCorrection.html for details). 


#ifndef __vtkvmtkLevelSetSigmoidFilter_h
#define __vtkvmtkLevelSetSigmoidFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLevelSetSigmoidFilter : public vtkSimpleImageToImageFilter
{
  public: 
  vtkTypeMacro(vtkvmtkLevelSetSigmoidFilter,vtkSimpleImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkLevelSetSigmoidFilter *New();

  vtkSetObjectMacro(LevelSetsImage,vtkImageData);
  vtkGetObjectMacro(LevelSetsImage,vtkImageData);

  // Get/Set  sigma value of the sigmoid function. Sigma determines the width of the sigmoid function. The value is given in pixel units.
  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);

  // Description:
  // Set/Get the Scalevalue of the sigmoind function. This determines the height of the sigmoid.
  vtkSetMacro(ScaleValue,double);
  vtkGetMacro(ScaleValue,double);

  // Description:
  // Set/Get The the ComputeScaleValueFromInput option. When enabled, vmtkimagefeaturecorrection computes the mean value from the featureimage and uses this value to set the height of the sigmoid function instead of the value given to the option scalevalue.
  vtkSetMacro(ComputeScaleValueFromInput,int);
  vtkGetMacro(ComputeScaleValueFromInput,int);
  vtkBooleanMacro(ComputeScaleValueFromInput,int);

  protected:
  vtkvmtkLevelSetSigmoidFilter();
  ~vtkvmtkLevelSetSigmoidFilter();  

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

  vtkImageData *LevelSetsImage;

  double Sigma;
  double ScaleValue;
  int ComputeScaleValueFromInput;

  private:
  vtkvmtkLevelSetSigmoidFilter(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
  void operator=(const vtkvmtkLevelSetSigmoidFilter&);  // Not implemented.
};

#endif
