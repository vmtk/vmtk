/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsObjectnessMeasureImageFilter.cxx,v $
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

#include "vtkvmtkObjectnessMeasureImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"

vtkStandardNewMacro(vtkvmtkObjectnessMeasureImageFilter);

vtkvmtkObjectnessMeasureImageFilter::vtkvmtkObjectnessMeasureImageFilter()
{
  this->SigmaMin = 1.0;
  this->SigmaMax = 1.0;
  this->NumberOfSigmaSteps = 1;
  this->SigmaStepMethod = EQUISPACED;
  this->UseScaledObjectness = 0;
  this->Alpha = 1.0;
  this->Beta = 1.0;
  this->Gamma = 1.0;
  this->ObjectDimension = 1;
  this->ScalesOutput = NULL;
}

vtkvmtkObjectnessMeasureImageFilter::~vtkvmtkObjectnessMeasureImageFilter()
{
  if (this->ScalesOutput)
    {
      this->ScalesOutput->Delete();
      this->ScalesOutput = NULL;
    }
}

void vtkvmtkObjectnessMeasureImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::SymmetricSecondRankTensor<float,3> HessianPixelType;
  typedef itk::Image<HessianPixelType,3> HessianImageType;
  typedef itk::HessianToObjectnessMeasureImageFilter<HessianImageType,ImageType> ObjectnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<ImageType,HessianImageType,ImageType> MultiScaleFilterType;
  typedef MultiScaleFilterType::ScalesImageType ScalesImageType;

  ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
  objectnessFilter->SetScaleObjectnessMeasure(this->UseScaledObjectness);
  objectnessFilter->SetBrightObject(true);
  objectnessFilter->SetObjectDimension(this->ObjectDimension);
  objectnessFilter->SetAlpha(this->Alpha);
  objectnessFilter->SetBeta(this->Beta);
  objectnessFilter->SetGamma(this->Gamma);

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
  multiScaleFilter->SetHessianToMeasureFilter(objectnessFilter);
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

