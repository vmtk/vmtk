/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsFWHMFeatureImageFilter.cxx,v $
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

#include "vtkvmtkFWHMFeatureImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkFWHMFeatureImageFilter.h"

vtkStandardNewMacro(vtkvmtkFWHMFeatureImageFilter);

vtkvmtkFWHMFeatureImageFilter::vtkvmtkFWHMFeatureImageFilter()
{
  int radius[3];
  radius[0] = radius[1] = radius[2] = 1;
  this->SetRadius(radius);
  this->BackgroundValue = 0.0;
  this->UseImageSpacing = 1;
}

vtkvmtkFWHMFeatureImageFilter::~vtkvmtkFWHMFeatureImageFilter()
{
  if (this->Radius)
    {
      delete[] this->Radius;
      this->Radius = NULL;
    }
}

void vtkvmtkFWHMFeatureImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::FWHMFeatureImageFilter<ImageType, ImageType> FWHMFeatureFilterType;
  typedef FWHMFeatureFilterType::StructuringElementRadiusType RadiusType;

  RadiusType radius;
  radius[0] = this->Radius[0];
  radius[1] = this->Radius[1];
  radius[2] = this->Radius[2];

  FWHMFeatureFilterType::Pointer fwhmFeatureFilter = FWHMFeatureFilterType::New();
  fwhmFeatureFilter->SetInput(inImage);
  fwhmFeatureFilter->SetRadius(radius);
  fwhmFeatureFilter->SetUseImageSpacing(this->UseImageSpacing);
  fwhmFeatureFilter->SetBackgroundValue(this->BackgroundValue);
  fwhmFeatureFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(fwhmFeatureFilter->GetOutput(),output);
}
