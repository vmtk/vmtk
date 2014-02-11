/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkNormalizeImageFilter.cxx,v $
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

#include "vtkvmtkNormalizeImageFilter.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkNormalizeImageFilter.h"


vtkStandardNewMacro(vtkvmtkNormalizeImageFilter);

vtkvmtkNormalizeImageFilter::vtkvmtkNormalizeImageFilter()
{
}

vtkvmtkNormalizeImageFilter::~vtkvmtkNormalizeImageFilter()
{
}

void vtkvmtkNormalizeImageFilter::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  typedef float PixelType;
  const int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeFilterType;

  NormalizeFilterType::Pointer imageFilter = NormalizeFilterType::New();
  imageFilter->SetInput(inImage);
  imageFilter->Update();

  ImageType::Pointer outputImage = imageFilter->GetOutput();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(outputImage,output);
}

