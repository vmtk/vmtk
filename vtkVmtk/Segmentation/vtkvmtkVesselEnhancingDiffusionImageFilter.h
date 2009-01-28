/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselEnhancingDiffusionImageFilter.h,v $
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

// .NAME vtkvmtkVesselEnhancingDiffusionImageFilter - Wrapper class around itk::VesselEnhancingDiffusionImageFilter
// .SECTION Description
// vtkvmtkVesselEnhancingDiffusionImageFilter


#ifndef __vtkvmtkVesselEnhancingDiffusionImageFilter_h
#define __vtkvmtkVesselEnhancingDiffusionImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkAnisotropicDiffusionVesselEnhancementImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianSmoothed3DToVesselnessMeasureImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselEnhancingDiffusionImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkVesselEnhancingDiffusionImageFilter *New();
  vtkTypeRevisionMacro(vtkvmtkVesselEnhancingDiffusionImageFilter, vtkITKImageToImageFilterFF);

  void SetSigmaMin(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->SetSigmaMin,value);
  }

  double GetSigmaMin()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetSigmaMin);
  }

  void SetSigmaMax(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->SetSigmaMax,value);
  }

  double GetSigmaMax()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetSigmaMax);
  }

  void SetNumberOfSigmaSteps(int value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->SetNumberOfSigmaSteps,value);
  }

  int GetNumberOfSigmaSteps()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetNumberOfSigmaSteps);
  }

  void SetSigmaStepMethodToEquispaced()
  {
    this->GetImageFilterPointer()->GetMultiScaleVesselnessFilter()->SetSigmaStepMethodToEquispaced();
    this->Modified();
  }
  
  void SetSigmaStepMethodToLogarithmic()
  {
    this->GetImageFilterPointer()->GetMultiScaleVesselnessFilter()->SetSigmaStepMethodToLogarithmic();
    this->Modified();
  }

  void SetTimeStep(double value)
  {
    DelegateITKInputMacro(SetTimeStep,value);
  }

  double GetTimeStep()
  {
    DelegateITKOutputMacro(GetTimeStep);
  }

  void SetEpsilon(double value)
  {
    DelegateITKInputMacro(SetEpsilon,value);
  }

  double GetEpsilon()
  {
    DelegateITKOutputMacro(GetEpsilon);
  }

  void SetWStrength(double value)
  {
    DelegateITKInputMacro(SetWStrength,value);
  }

  double GetWStrength()
  {
    DelegateITKOutputMacro(GetWStrength);
  }

  void SetSensitivity(double value)
  {
    DelegateITKInputMacro(SetSensitivity,value);
  }

  double GetSensitivity()
  {
    DelegateITKOutputMacro(GetSensitivity);
  }

  void SetNumberOfIterations(int value)
  {
    DelegateITKInputMacro(SetNumberOfIterations,value);
  }

  int GetNumberOfIterations()
  {
    DelegateITKOutputMacro(GetNumberOfIterations);
  }

  void SetAlpha(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->SetAlpha,value);
  }

  double GetAlpha()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->GetAlpha);
  }

  void SetBeta(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->SetBeta,value);
  }

  double GetBeta()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->GetBeta);
  }

  void SetGamma(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->SetGamma,value);
  }

  double GetGamma()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->GetGamma);
  }

  void SetC(double value)
  {
    DelegateITKInputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->SetC,value);
  }

  double GetC()
  {
    DelegateITKOutputMacro(GetMultiScaleVesselnessFilter()->GetHessianToMeasureFilter()->GetC);
  }

  void SetNumberOfDiffusionSubIterations(int value)
  {
    DelegateITKInputMacro(SetNumberOfDiffusionSubIterations,value);
  }

  int GetNumberOfDiffusionSubIterations()
  {
    DelegateITKOutputMacro(GetNumberOfDiffusionSubIterations);
  }

protected:
  //BTX
  typedef itk::AnisotropicDiffusionVesselEnhancementImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;

  vtkvmtkVesselEnhancingDiffusionImageFilter() : Superclass(ImageFilterType::New())
  {
    ImageFilterType* imageFilter = this->GetImageFilterPointer();
    imageFilter->GetMultiScaleVesselnessFilter()->SetSigmaStepMethodToEquispaced();
  }

  ~vtkvmtkVesselEnhancingDiffusionImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

private:
  vtkvmtkVesselEnhancingDiffusionImageFilter(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.
};

#endif




