/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsSigmoidImageFilter.cxx,v $
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

#include "vtkvmtkSigmoidImageFilter.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkSigmoidImageFilter.h"

vtkStandardNewMacro(vtkvmtkSigmoidImageFilter);

vtkvmtkSigmoidImageFilter::vtkvmtkSigmoidImageFilter()
{
  this->Alpha = 1.0;
  this->Beta = 1.0;
  this->OutputMinimum = 0.0;
  this->OutputMaximum = 1.0;
}

void vtkvmtkSigmoidImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;

  SigmoidFilterType::Pointer sigmoidFilter = SigmoidFilterType::New();
  sigmoidFilter->SetInput(inImage);
  sigmoidFilter->SetAlpha(this->Alpha);
  sigmoidFilter->SetBeta(this->Beta);
  sigmoidFilter->SetOutputMinimum(this->OutputMinimum);
  sigmoidFilter->SetOutputMaximum(this->OutputMaximum);
  sigmoidFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(sigmoidFilter->GetOutput(),output);
}

