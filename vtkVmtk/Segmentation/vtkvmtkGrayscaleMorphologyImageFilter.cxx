/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkGrayscaleMorphologyImageFilter.cxx,v $
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

#include "vtkvmtkGrayscaleMorphologyImageFilter.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"


vtkStandardNewMacro(vtkvmtkGrayscaleMorphologyImageFilter);

vtkvmtkGrayscaleMorphologyImageFilter::vtkvmtkGrayscaleMorphologyImageFilter()
{
  this->BallRadius[0] = this->BallRadius[1] = this->BallRadius[2] = 1;
  this->Operation = ERODE;
}

vtkvmtkGrayscaleMorphologyImageFilter::~vtkvmtkGrayscaleMorphologyImageFilter()
{
}

int vtkvmtkGrayscaleMorphologyImageFilter::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, -1);

  return 1;
}

void vtkvmtkGrayscaleMorphologyImageFilter::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  typedef float PixelType;
  const int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::BinaryBallStructuringElement<PixelType, Dimension> KernelType;
  typedef itk::GrayscaleMorphologicalClosingImageFilter<ImageType, ImageType, KernelType> ClosingFilterType;
  typedef itk::GrayscaleMorphologicalOpeningImageFilter<ImageType, ImageType, KernelType> OpeningFilterType;
  typedef itk::GrayscaleErodeImageFilter<ImageType, ImageType, KernelType> ErodeFilterType;
  typedef itk::GrayscaleDilateImageFilter<ImageType, ImageType, KernelType> DilateFilterType;

  KernelType ball;
  KernelType::SizeType ballRadius;
  ballRadius[0] = this->BallRadius[0];
  ballRadius[1] = this->BallRadius[1];
  ballRadius[2] = this->BallRadius[2];
  ball.SetRadius(ballRadius);
  ball.CreateStructuringElement();

  ImageType::Pointer outputImage;

  if (this->Operation == CLOSE)
    {
    ClosingFilterType::Pointer imageFilter = ClosingFilterType::New();
    imageFilter->SetKernel(ball);
    imageFilter->SetInput(inImage);
    imageFilter->SafeBorderOff();
    try {
      imageFilter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception catched !" << std::endl;
      std::cerr << excep << std::endl;
    }
    outputImage = imageFilter->GetOutput();
    }
  if (this->Operation == OPEN)
    {
    OpeningFilterType::Pointer imageFilter = OpeningFilterType::New();
    imageFilter->SetKernel(ball);
    imageFilter->SetInput(inImage);
    imageFilter->SafeBorderOff();
    try {
      imageFilter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception catched !" << std::endl;
      std::cerr << excep << std::endl;
    }
    outputImage = imageFilter->GetOutput();
    }
  if (this->Operation == DILATE)
    {
    DilateFilterType::Pointer imageFilter = DilateFilterType::New();
    imageFilter->SetKernel(ball);
    imageFilter->SetInput(inImage);
    try {
      imageFilter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception catched !" << std::endl;
      std::cerr << excep << std::endl;
    }
    outputImage = imageFilter->GetOutput();
    }
  if (this->Operation == ERODE)
    {
    ErodeFilterType::Pointer imageFilter = ErodeFilterType::New();
    imageFilter->SetKernel(ball);
    imageFilter->SetInput(inImage);
    try {
      imageFilter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception catched !" << std::endl;
      std::cerr << excep << std::endl;
    }
    outputImage = imageFilter->GetOutput();
    }

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(outputImage,output);
}

