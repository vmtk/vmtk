/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselEnhancingDiffusion3DImageFilter.h,v $
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

// .NAME vtkvmtkVesselEnhancingDiffusion3DImageFilter - Wrapper class around itk::VesselEnhancingDiffusion3DImageFilter
// .SECTION Description
// vtkvmtkVesselEnhancingDiffusion3DImageFilter


#ifndef __vtkvmtkVesselEnhancingDiffusion3DImageFilter_h
#define __vtkvmtkVesselEnhancingDiffusion3DImageFilter_h


#include "vtkvmtkITKImageToImageFilterFF.h"
#include "itkVesselEnhancingDiffusion3DImageFilter.h"
#include "vtkvmtkWin32Header.h"
#include "vtkVersion.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselEnhancingDiffusion3DImageFilter : public vtkvmtkITKImageToImageFilterFF
{
 public:
  static vtkvmtkVesselEnhancingDiffusion3DImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselEnhancingDiffusion3DImageFilter, vtkvmtkITKImageToImageFilterFF);

  vtkSetMacro(SigmaMin,double);
  vtkGetMacro(SigmaMin,double);

  vtkSetMacro(SigmaMax,double);
  vtkGetMacro(SigmaMax,double);

  vtkSetMacro(NumberOfSigmaSteps,int);
  vtkGetMacro(NumberOfSigmaSteps,int);

  void SetSigmaStepMethodToEquispaced()
  {
    this->SigmaStepMethod = EQUISPACED_STEPS;
  }
  
  void SetSigmaStepMethodToLogarithmic()
  {
    this->SigmaStepMethod = LOGARITHMIC_STEPS;
  }

  void SetTimeStep(double value)
  {
    DelegateITKInputMacro(SetTimeStep,value);
  }

  void SetEpsilon(double value)
  {
    DelegateITKInputMacro(SetEpsilon,value);
  }

  void SetOmega(double value)
  {
    DelegateITKInputMacro(SetOmega,value);
  }

  void SetSensitivity(double value)
  {
    DelegateITKInputMacro(SetSensitivity,value);
  }

  void SetNumberOfIterations(int value)
  {
    DelegateITKInputMacro(SetIterations,value);
  }

  void SetAlpha(double value)
  {
    DelegateITKInputMacro(SetAlpha,value);
  }

  void SetBeta(double value)
  {
    DelegateITKInputMacro(SetBeta,value);
  }

  void SetGamma(double value)
  {
    DelegateITKInputMacro(SetGamma,value);
  }

  void SetRecalculateVesselness(int value)
  {
    DelegateITKInputMacro(SetRecalculateVesselness,value);
  }

  double ComputeSigmaValue(int scaleLevel)
  {
    double sigmaValue;

    if (this->NumberOfSigmaSteps < 2)
    {
      return this->SigmaMin;
    }

    switch (this->SigmaStepMethod)
      {
      case EQUISPACED_STEPS:
        {
        double stepSize = ( SigmaMax - SigmaMin ) / (NumberOfSigmaSteps-1);
        if (stepSize < 1e-10)
          {
          stepSize = 1e-10;
          }
        sigmaValue = SigmaMin + stepSize * scaleLevel;
        break;
        }
      case LOGARITHMIC_STEPS:
        {
        double stepSize = ( vcl_log(SigmaMax) - vcl_log(SigmaMin) ) / (NumberOfSigmaSteps-1);
        if (stepSize < 1e-10)
          {
          stepSize = 1e-10;
          }
        sigmaValue = vcl_exp( vcl_log (SigmaMin) + stepSize * scaleLevel);
        break;
        }
      default:
        vtkErrorMacro("Error: undefined sigma step method.");
        sigmaValue = 0.0;
        break;
      }

    return sigmaValue;
  }

  void Update()
  {
    std::vector<float> scales;
    for (int i=0; i<this->NumberOfSigmaSteps; i++)
      {
      scales.push_back(this->ComputeSigmaValue(i));
      }
    this->GetImageFilterPointer()->SetScales(scales);
  }

//BTX
  enum
  {
    EQUISPACED_STEPS,
    LOGARITHMIC_STEPS
  };
//ETX

protected:
  //BTX
  typedef itk::VesselEnhancingDiffusion3DImageFilter<Superclass::InputImageType::PixelType> ImageFilterType;

  vtkvmtkVesselEnhancingDiffusion3DImageFilter() : Superclass(ImageFilterType::New()) 
  {
    this->SigmaMin = 0.0;
    this->SigmaMax = 0.0;
    this->NumberOfSigmaSteps = 0;
    this->SigmaStepMethod = EQUISPACED_STEPS;

    this->GetImageFilterPointer()->SetDefaultPars();
  }

  ~vtkvmtkVesselEnhancingDiffusion3DImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()); }
  //ETX

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;

private:
  vtkvmtkVesselEnhancingDiffusion3DImageFilter(const vtkvmtkVesselEnhancingDiffusion3DImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselEnhancingDiffusion3DImageFilter&);  // Not implemented.
};

#endif




