/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsVesselEnhancingDiffusionImageFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.1 $

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

#include "vtkvmtkVesselEnhancingDiffusionImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkAnisotropicDiffusionVesselEnhancementImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianSmoothed3DToVesselnessMeasureImageFilter.h"

vtkStandardNewMacro(vtkvmtkVesselEnhancingDiffusionImageFilter);

vtkvmtkVesselEnhancingDiffusionImageFilter::vtkvmtkVesselEnhancingDiffusionImageFilter()
{
  this->SigmaMin = 1.0;
  this->SigmaMax = 1.0;
  this->NumberOfSigmaSteps = 1;
  this->SigmaStepMethod = EQUISPACED;
  this->NumberOfIterations = 1;
  this->NumberOfDiffusionSubIterations = 1;
  this->TimeStep = 0.01;
  this->Epsilon = 1E-6;
  this->WStrength = 1.0;
  this->Sensitivity = 1.0;
  this->Alpha = 1.0;
  this->Beta = 1.0;
  this->Gamma = 1.0;
  this->C = 1.0;
}

void vtkvmtkVesselEnhancingDiffusionImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::AnisotropicDiffusionVesselEnhancementImageFilter<ImageType, ImageType> AnisotropicDiffusionFilterType;
  typedef AnisotropicDiffusionFilterType::MultiScaleVesselnessFilterType MultiScaleFilterType;
  typedef AnisotropicDiffusionFilterType::VesselnessFilterType VesselnessFilterType;

  AnisotropicDiffusionFilterType::Pointer anisotropicDiffusionFilter = AnisotropicDiffusionFilterType::New();

  MultiScaleFilterType::Pointer multiScaleFilter = anisotropicDiffusionFilter->GetMultiScaleVesselnessFilter();
  multiScaleFilter->SetSigmaMinimum(this->SigmaMin);
  multiScaleFilter->SetSigmaMaximum(this->SigmaMax);
  multiScaleFilter->SetNumberOfSigmaSteps(this->NumberOfSigmaSteps);
  if (this->SigmaStepMethod == EQUISPACED)
    {
      multiScaleFilter->SetSigmaStepMethodToEquispaced();
    }
  else if (this->SigmaStepMethod == LOGARITHMIC)
    {
      multiScaleFilter->SetSigmaStepMethodToLogarithmic();
    }

  VesselnessFilterType* vesselnessFilter = dynamic_cast<VesselnessFilterType*>(multiScaleFilter->GetHessianToMeasureFilter());
  vesselnessFilter->SetAlpha(this->Alpha);
  vesselnessFilter->SetBeta(this->Beta);
  vesselnessFilter->SetGamma(this->Gamma);
  vesselnessFilter->SetC(this->C);

  anisotropicDiffusionFilter->SetInput(inImage);
  anisotropicDiffusionFilter->SetTimeStep(this->TimeStep);
  anisotropicDiffusionFilter->SetEpsilon(this->Epsilon);
  anisotropicDiffusionFilter->SetWStrength(this->WStrength);
  anisotropicDiffusionFilter->SetSensitivity(this->Sensitivity);
  anisotropicDiffusionFilter->SetNumberOfIterations(this->NumberOfIterations);
  anisotropicDiffusionFilter->SetNumberOfDiffusionSubIterations(this->NumberOfDiffusionSubIterations);
  anisotropicDiffusionFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(anisotropicDiffusionFilter->GetOutput(),output);
}

