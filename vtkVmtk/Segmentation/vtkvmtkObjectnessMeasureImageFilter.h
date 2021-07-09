/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkObjectnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkObjectnessMeasureImageFilter - Wrapper class around itk::ObjectnessMeasureImageFilter
// .SECTION Description
// vtkvmtkObjectnessMeasureImageFilter


#ifndef __vtkvmtkObjectnessMeasureImageFilter_h
#define __vtkvmtkObjectnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkObjectnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkObjectnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkObjectnessMeasureImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);

  vtkGetMacro(SigmaMax,double);
  vtkSetMacro(SigmaMax,double);

  vtkGetMacro(NumberOfSigmaSteps,int);
  vtkSetMacro(NumberOfSigmaSteps,int);

  vtkGetMacro(SigmaStepMethod,int);
  vtkSetMacro(SigmaStepMethod,int);

  void SetSigmaStepMethodToEquispaced()
  {
    this->SetSigmaStepMethod(EQUISPACED);
  }

  void SetSigmaStepMethodToLogarithmic()
  {
    this->SetSigmaStepMethod(LOGARITHMIC);
  }

  enum
  {
    EQUISPACED,
    LOGARITHMIC
  };

  vtkGetMacro(UseScaledObjectness,int);
  vtkSetMacro(UseScaledObjectness,int);

  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);

  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);

  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);

  vtkGetMacro(ObjectDimension,int);
  vtkSetMacro(ObjectDimension,int);

  vtkGetObjectMacro(ScalesOutput,vtkImageData);

protected:
  vtkvmtkObjectnessMeasureImageFilter();
  ~vtkvmtkObjectnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkObjectnessMeasureImageFilter(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkObjectnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int UseScaledObjectness;
  double Alpha;
  double Beta;
  double Gamma;
  int ObjectDimension;
  vtkImageData* ScalesOutput;
};

#endif

