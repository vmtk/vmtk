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


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkSatoVesselnessMeasureImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkSatoVesselnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkSatoVesselnessMeasureImageFilter, vtkvmtkITKImageToImageFilterFF);

  void SetSigmaMin(double value)
  {
    DelegateITKInputMacro(SetSigmaMinimum,value);
  }

  double GetSigmaMin()
  {
    DelegateITKOutputMacro(GetSigmaMinimum);
  }

  void SetSigmaMax(double value)
  {
    DelegateITKInputMacro(SetSigmaMaximum,value);
  }

  double GetSigmaMax()
  {
    DelegateITKOutputMacro(GetSigmaMaximum);
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
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<VesselnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetAlpha1(value); 
      this->Modified(); 
      }
  }

  double GetAlpha1()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<VesselnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetAlpha1(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0.0;
      }
  }

  void SetAlpha2(double value)
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      dynamic_cast<VesselnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->SetAlpha2(value); 
      this->Modified(); 
      }
  }

  double GetAlpha2()
  {
    ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*>(this->m_Filter.GetPointer()); 
    if (tempFilter) 
      { 
      return dynamic_cast<VesselnessFilterType*>(tempFilter->GetHessianToMeasureFilter())->GetAlpha2(); 
      }
    else
      {
      vtkErrorMacro ( << this->GetClassName() << " Error getting method. Dynamic cast returned 0" );
      return 0.0;
      }
  }

protected:
  //BTX
  typedef itk::SymmetricSecondRankTensor<double,3> HessianPixelType;
  typedef itk::Image<HessianPixelType,3> HessianImageType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<float> VesselnessFilterType;
  //typedef itk::MultiScaleHessianBasedMeasureImageFilter<Superclass::InputImageType,VesselnessFilterType> ImageFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<InputImageType,HessianImageType,InputImageType> ImageFilterType;

  vtkvmtkSatoVesselnessMeasureImageFilter() : Superclass(ImageFilterType::New())
  {
    VesselnessFilterType::Pointer vesselnessFilter = VesselnessFilterType::New();
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->SetSigmaStepMethodToEquispaced();
    imageFilter->GenerateScalesOutputOn();
    imageFilter->SetHessianToMeasureFilter(vesselnessFilter.GetPointer());
  }

  ~vtkvmtkSatoVesselnessMeasureImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

private:
  vtkvmtkSatoVesselnessMeasureImageFilter(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkSatoVesselnessMeasureImageFilter&);  // Not implemented.
};

#endif




