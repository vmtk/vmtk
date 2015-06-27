/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsUpwindGradientMagnitudeImageFilter.cxx,v $
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

#include "vtkvmtkUpwindGradientMagnitudeImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkUpwindGradientMagnitudeImageFilter.h"

vtkStandardNewMacro(vtkvmtkUpwindGradientMagnitudeImageFilter);

vtkvmtkUpwindGradientMagnitudeImageFilter::vtkvmtkUpwindGradientMagnitudeImageFilter()
{
  this->UpwindFactor = 1.0;
}

void vtkvmtkUpwindGradientMagnitudeImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef float PixelType;
  const int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::UpwindGradientMagnitudeImageFilter<ImageType,ImageType> UpwindGradientFilterType;

  UpwindGradientFilterType::Pointer upwindGradientFilter = UpwindGradientFilterType::New();
  upwindGradientFilter->SetInput(inImage);
  upwindGradientFilter->SetUpwindFactor(this->UpwindFactor);
  upwindGradientFilter->Update();

  ImageType::Pointer outputImage = upwindGradientFilter->GetOutput();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(outputImage,output);

}

