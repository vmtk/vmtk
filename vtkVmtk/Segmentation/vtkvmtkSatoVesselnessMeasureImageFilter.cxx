/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSatoVesselnessMeasureImageFilter.cxx,v $
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

#include "vtkvmtkSatoVesselnessMeasureImageFilter.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"


vtkStandardNewMacro(vtkvmtkSatoVesselnessMeasureImageFilter);

vtkvmtkSatoVesselnessMeasureImageFilter::vtkvmtkSatoVesselnessMeasureImageFilter()
{
  this->SigmaMin = 1.0;
  this->SigmaMax = 2.0;
  this->NumberOfSigmaSteps = 2;
  this->SetSigmaStepMethodToEquispaced();
  this->Alpha1 = 0.5;
  this->Alpha2 = 2.0;
}

vtkvmtkSatoVesselnessMeasureImageFilter::~vtkvmtkSatoVesselnessMeasureImageFilter()
{
}

void vtkvmtkSatoVesselnessMeasureImageFilter::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::SymmetricSecondRankTensor<double,3> HessianPixelType;
  typedef itk::Image<HessianPixelType,3> HessianImageType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<float> VesselnessFilterType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<ImageType,HessianImageType,ImageType> ImageFilterType;

  VesselnessFilterType::Pointer vesselnessFilter = VesselnessFilterType::New();
  vesselnessFilter->SetAlpha1(this->Alpha1);
  vesselnessFilter->SetAlpha2(this->Alpha2);

  ImageFilterType::Pointer imageFilter = ImageFilterType::New();
  imageFilter->SetSigmaMinimum(this->SigmaMin);
  imageFilter->SetSigmaMaximum(this->SigmaMax);
  imageFilter->SetNumberOfSigmaSteps(this->NumberOfSigmaSteps);
  if (this->SigmaStepMethod == EQUISPACED)
    {
    imageFilter->SetSigmaStepMethodToEquispaced();
    }
  else if (this->SigmaStepMethod == LOGARITHMIC)
    {
    imageFilter->SetSigmaStepMethodToLogarithmic();
    }
  imageFilter->GenerateScalesOutputOn();
  imageFilter->SetHessianToMeasureFilter(vesselnessFilter.GetPointer());

  imageFilter->SetInput(inImage);
  imageFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(imageFilter->GetOutput(),output);
}

