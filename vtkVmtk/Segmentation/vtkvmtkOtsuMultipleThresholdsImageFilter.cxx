/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkOtsuMultipleThresholdsImageFilter.cxx,v $
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

#include "vtkvmtkOtsuMultipleThresholdsImageFilter.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkOtsuMultipleThresholdsImageFilter.h"


vtkStandardNewMacro(vtkvmtkOtsuMultipleThresholdsImageFilter);

vtkvmtkOtsuMultipleThresholdsImageFilter::vtkvmtkOtsuMultipleThresholdsImageFilter()
{
  this->NumberOfHistogramBins = 128;
  this->NumberOfThresholds = 1;
  this->LabelOffset = 0;
  this->Thresholds = NULL;
}

vtkvmtkOtsuMultipleThresholdsImageFilter::~vtkvmtkOtsuMultipleThresholdsImageFilter()
{
  if (this->Thresholds)
    {
    this->Thresholds->Delete();
    this->Thresholds = NULL;
    }
}

int vtkvmtkOtsuMultipleThresholdsImageFilter::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_SHORT, -1);

  return 1;
}

void vtkvmtkOtsuMultipleThresholdsImageFilter::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  typedef float PixelType;
  typedef unsigned short OutputPixelType;
  const int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::Image<OutputPixelType, Dimension> OutputImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::OtsuMultipleThresholdsImageFilter<ImageType, OutputImageType> OtsuFilterType;
  typedef OtsuFilterType::ThresholdVectorType ThresholdVectorType;

  OtsuFilterType::Pointer imageFilter = OtsuFilterType::New();
  imageFilter->SetInput(inImage);
  imageFilter->SetNumberOfHistogramBins(this->NumberOfHistogramBins);
  imageFilter->SetNumberOfThresholds(this->NumberOfThresholds);
  imageFilter->SetLabelOffset(this->LabelOffset);
  imageFilter->Update();

  const ThresholdVectorType& thresholds = imageFilter->GetThresholds();

  if (this->Thresholds)
    {
    this->Thresholds->Delete();
    this->Thresholds = NULL;
    }

  this->Thresholds = vtkFloatArray::New();
  this->Thresholds->SetNumberOfTuples(thresholds.size());

  for (unsigned long j=0; j<thresholds.size(); j++)
    {
    this->Thresholds->SetValue(j,thresholds[j]);
    }

  OutputImageType::Pointer outputImage = imageFilter->GetOutput();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<OutputImageType>(outputImage,output);
}

