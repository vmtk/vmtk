/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsVesselnessMeasureImageFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.1 $

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

#include "vtkvmtkVesselnessMeasureImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"

vtkStandardNewMacro(vtkvmtkVesselnessMeasureImageFilter);

vtkvmtkVesselnessMeasureImageFilter::vtkvmtkVesselnessMeasureImageFilter()
{
  this->SigmaMin = 1.0;
  this->SigmaMax = 1.0;
  this->NumberOfSigmaSteps = 1;
  this->SigmaStepMethod = EQUISPACED;
  this->UseScaledVesselness = 0;
  this->Alpha = 1.0;
  this->Beta = 1.0;
  this->Gamma = 1.0;
  this->ScalesOutput = NULL;
  this->BrightObject = true;
}

vtkvmtkVesselnessMeasureImageFilter::~vtkvmtkVesselnessMeasureImageFilter()
{
  if (this->ScalesOutput)
    {
      this->ScalesOutput->Delete();
      this->ScalesOutput = NULL;
    }
}

void vtkvmtkVesselnessMeasureImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::SymmetricSecondRankTensor<float,3> HessianPixelType;
  typedef itk::Image<HessianPixelType,3> HessianImageType;
  typedef itk::HessianToObjectnessMeasureImageFilter<HessianImageType,ImageType> VesselnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<ImageType,HessianImageType,ImageType> MultiScaleFilterType;
  typedef MultiScaleFilterType::ScalesImageType ScalesImageType;

  VesselnessFilterType::Pointer vesselnessFilter = VesselnessFilterType::New();
  vesselnessFilter->SetScaleObjectnessMeasure(this->UseScaledVesselness);
  vesselnessFilter->SetBrightObject(this->BrightObject);
  vesselnessFilter->SetObjectDimension(1);
  vesselnessFilter->SetAlpha(this->Alpha);
  vesselnessFilter->SetBeta(this->Beta);
  vesselnessFilter->SetGamma(this->Gamma);

  MultiScaleFilterType::Pointer multiScaleFilter = MultiScaleFilterType::New();
  multiScaleFilter->SetInput(inImage);
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
  multiScaleFilter->GenerateScalesOutputOn();
  multiScaleFilter->SetHessianToMeasureFilter(vesselnessFilter);
  multiScaleFilter->Update();

  if (this->ScalesOutput)
    {
      this->ScalesOutput->Delete();
      this->ScalesOutput = NULL;
    }

  this->ScalesOutput = vtkImageData::New();

  ScalesImageType::Pointer scalesImage = const_cast<ScalesImageType*>(multiScaleFilter->GetScalesOutput());

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ScalesImageType>(scalesImage,this->ScalesOutput);

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(multiScaleFilter->GetOutput(),output);
}

