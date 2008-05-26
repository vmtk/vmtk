/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselnessMeasureImageFilter.h,v $
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

// .NAME vtkvmtkVesselnessMeasureImageFilter - Wrapper class around itk::VesselnessMeasureImageFilter
// .SECTION Description
// vtkvmtkVesselnessMeasureImageFilter


#ifndef __vtkvmtkVesselnessMeasureImageFilter_h
#define __vtkvmtkVesselnessMeasureImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselnessMeasureImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkVesselnessMeasureImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkVesselnessMeasureImageFilter, vtkITKImageToImageFilterFF);

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

  void SetAlpha(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetAlpha,value);
  }

  double GetAlpha()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetAlpha);
  }

  void SetBeta(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetBeta,value);
  }

  double GetBeta()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetBeta);
  }

  void SetGamma(double value)
  {
    DelegateITKInputMacro(GetHessianToMeasureFilter()->SetGamma,value);
  }

  double GetGamma()
  {
    DelegateITKOutputMacro(GetHessianToMeasureFilter()->GetGamma);
  }

protected:
  //BTX
  typedef itk::HessianToObjectnessMeasureImageFilter<double,3> ObjectnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<Superclass::InputImageType,ObjectnessFilterType> ImageFilterType;

  vtkvmtkVesselnessMeasureImageFilter() : Superclass(ImageFilterType::New())
  {
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->SetSigmaStepMethodToEquispaced();
    ObjectnessFilterType* objectnessFilter = imageFilter->GetHessianToMeasureFilter();
    objectnessFilter->SetScaleObjectnessMeasure(false);
    objectnessFilter->SetBrightObject(true);
    objectnessFilter->SetObjectDimension(1);
  }

  ~vtkvmtkVesselnessMeasureImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

private:
  vtkvmtkVesselnessMeasureImageFilter(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselnessMeasureImageFilter&);  // Not implemented.
};

#endif




