/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkDanielssonDistanceMapImageFilter.cxx,v $
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

#include "vtkvmtkDanielssonDistanceMapImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkDanielssonDistanceMapImageFilter.h"

vtkStandardNewMacro(vtkvmtkDanielssonDistanceMapImageFilter);

vtkvmtkDanielssonDistanceMapImageFilter::vtkvmtkDanielssonDistanceMapImageFilter()
{
  this->SquaredDistance = 0;
  this->InputIsBinary = 0;
}

void vtkvmtkDanielssonDistanceMapImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::DanielssonDistanceMapImageFilter<ImageType, ImageType> DanielssonFilterType;

  DanielssonFilterType::Pointer danielssonFilter = DanielssonFilterType::New();
  danielssonFilter->SetInput(inImage);
  danielssonFilter->SetSquaredDistance(this->SquaredDistance);
  danielssonFilter->SetInputIsBinary(this->InputIsBinary);
  danielssonFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(danielssonFilter->GetOutput(),output);
}

