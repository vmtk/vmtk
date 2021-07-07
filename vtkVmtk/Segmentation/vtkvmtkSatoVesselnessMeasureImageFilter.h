/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSatoVesselnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkSatoVesselnessMeasureImageFilter - Wrapper class around itk::SatoVesselnessMeasureImageFilter
// .SECTION Description
// vtkvmtkSatoVesselnessMeasureImageFilter


#ifndef __vtkvmtkSatoVesselnessMeasureImageFilter_h
#define __vtkvmtkSatoVesselnessMeasureImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSatoVesselnessMeasureImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkSatoVesselnessMeasureImageFilter *New();
  vtkTypeMacro(vtkvmtkSatoVesselnessMeasureImageFilter, vtkSimpleImageToImageFilter);

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

  vtkGetMacro(Alpha1,double);
  vtkSetMacro(Alpha1,double);

  vtkGetMacro(Alpha2,double);
  vtkSetMacro(Alpha2,double);
//BTX
  enum 
  {
    EQUISPACED,
    LOGARITHMIC
  };
//ETX
protected:
  vtkvmtkSatoVesselnessMeasureImageFilter();
  ~vtkvmtkSatoVesselnessMeasureImageFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkSatoVesselnessMeasureImageFilter(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  double Alpha1;
  double Alpha2;
};

#endif




