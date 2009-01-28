/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSatoVesselnessMeasureImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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


#include "vtkITKImageToImageFilterFF.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSatoVesselnessMeasureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkSatoVesselnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkSatoVesselnessMeasureImageFilter, vtkITKImageToImageFilterFF);

  void SetSigmaMin(double value)
  {
    DelegateITKInputMacro(SetSigmaMin,value);
  }

  double GetSigmaMin()
  {
    DelegateITKOutputMacro(GetSigmaMin);
  }

  void SetSigmaMax(double value)
  {
    DelegateITKInputMacro(SetSigmaMax,value);
  }

  double GetSigmaMax()
  {
    DelegateITKOutputMacro(GetSigmaMax);
  }

  void SetNumberOfSigmaSteps(int value)
  {
    DelegateITKInputMacro(SetNumberOfSigmaSteps,value);
  }

  int GetNumberOfSigmaSteps()
  {
    DelegateITKOutputMacro(GetNumberOfSigmaSteps);
  }

  void SetSigmaStepMethodToEquispaced()
  {
    this->GetImageFilterPointer()->SetSigmaStepMethodToEquispaced();
    this->Modified();
  }
  
  void SetSigmaStepMethodToLogarithmic()
  {
    this->GetImageFilterPointer()->SetSigmaStepMethodToLogarithmic();
    this->Modified();
  }

  void SetAlpha1(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetAlpha1,value);
  }

  double GetAlpha1()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetAlpha1);
  }

  void SetAlpha2(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetAlpha2,value);
  }

  double GetAlpha2()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetAlpha2);
  }

protected:
  //BTX
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<double> VesselnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<Superclass::InputImageType,VesselnessFilterType> ImageFilterType;

  vtkvmtkSatoVesselnessMeasureImageFilter() : Superclass(ImageFilterType::New())
  {
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->SetSigmaStepMethodToEquispaced();
  }

  ~vtkvmtkSatoVesselnessMeasureImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

private:
  vtkvmtkSatoVesselnessMeasureImageFilter(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
};

#endif




