/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundedReciprocalImageFilter.cxx,v $
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

#include "vtkvmtkBoundedReciprocalImageFilter.h"
#include "vtkvmtkITKFilterUtilities.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "itkBoundedReciprocalImageFilter.h"

vtkStandardNewMacro(vtkvmtkBoundedReciprocalImageFilter);

void vtkvmtkBoundedReciprocalImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::BoundedReciprocalImageFilter<ImageType, ImageType> BoundedReciprocalFilterType;
  BoundedReciprocalFilterType::Pointer boundedReciprocalFilter = BoundedReciprocalFilterType::New();
  boundedReciprocalFilter->SetInput(inImage);
  boundedReciprocalFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(boundedReciprocalFilter->GetOutput(),output);
}
