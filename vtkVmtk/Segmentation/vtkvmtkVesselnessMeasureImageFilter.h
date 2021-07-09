/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkVesselnessMeasureImageFilter - Wrapper class around itk::VesselnessMeasureImageFilter
// .SECTION Description
// vtkvmtkVesselnessMeasureImageFilter


#ifndef __vtkvmtkVesselnessMeasureImageFilter_h
#define __vtkvmtkVesselnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkVesselnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselnessMeasureImageFilter, vtkSimpleImageToImageFilter);

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

  vtkGetMacro(UseScaledVesselness,int);
  vtkSetMacro(UseScaledVesselness,int);

  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);

  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);

  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);

  vtkGetObjectMacro(ScalesOutput,vtkImageData);

protected:
  vtkvmtkVesselnessMeasureImageFilter();
  ~vtkvmtkVesselnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkVesselnessMeasureImageFilter(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int UseScaledVesselness;
  double Alpha;
  double Beta;
  double Gamma;
  vtkImageData* ScalesOutput;
};

#endif

