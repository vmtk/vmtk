/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsRecursiveGaussian2DImageFilter.cxx,v $
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

#include "vtkvmtkRecursiveGaussian2DImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkRecursiveGaussianImageFilter.h"

vtkStandardNewMacro(vtkvmtkRecursiveGaussian2DImageFilter);

vtkvmtkRecursiveGaussian2DImageFilter::vtkvmtkRecursiveGaussian2DImageFilter()
{
  this->Sigma = 1.0;
  this->NormalizeAcrossScale = 0;
}

void vtkvmtkRecursiveGaussian2DImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef float PixelType;
  const int Dimension = 2;
  typedef itk::Image<PixelType, Dimension> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::RecursiveGaussianImageFilter<ImageType,ImageType> GaussianFilterType;

  GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
  gaussianFilter->SetInput(inImage);
  gaussianFilter->SetSigma(this->Sigma);
  gaussianFilter->SetNormalizeAcrossScale(this->NormalizeAcrossScale);
  gaussianFilter->Update();

  ImageType::Pointer outputImage = gaussianFilter->GetOutput();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(outputImage,output);
}

