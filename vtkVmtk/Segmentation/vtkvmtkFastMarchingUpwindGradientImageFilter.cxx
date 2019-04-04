/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkFastMarchingUpwindGradientImageFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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

#include "vtkvmtkFastMarchingUpwindGradientImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include <itkFastMarchingUpwindGradientImageFilter.h>

vtkStandardNewMacro(vtkvmtkFastMarchingUpwindGradientImageFilter);

vtkvmtkFastMarchingUpwindGradientImageFilter::vtkvmtkFastMarchingUpwindGradientImageFilter()
{
  this->GenerateGradientImage = 0;
  this->TargetReachedMode = ONE_TARGET;
  this->TargetValue = 0.0;
  this->TargetOffset = 0.0;
  this->Seeds = NULL;
  this->Targets = NULL;
}

vtkvmtkFastMarchingUpwindGradientImageFilter::~vtkvmtkFastMarchingUpwindGradientImageFilter()
{
  if (this->Seeds)
    {
      this->Seeds->Delete();
      this->Seeds = NULL;
    }
  if (this->Targets)
    {
      this->Targets->Delete();
      this->Targets = NULL;
    }
}

void vtkvmtkFastMarchingUpwindGradientImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::FastMarchingUpwindGradientImageFilter<ImageType,ImageType> FastMarchingFilterType;

  FastMarchingFilterType::Pointer fastMarchingFilter = FastMarchingFilterType::New();
  fastMarchingFilter->SetInput(inImage);
  fastMarchingFilter->SetGenerateGradientImage(this->GenerateGradientImage);
  fastMarchingFilter->SetTargetOffset(this->TargetOffset);
  if (this->TargetReachedMode == ONE_TARGET)
    {
    fastMarchingFilter->SetTargetReachedMode(FastMarchingFilterType::OneTarget);
    }
  else
    {
    fastMarchingFilter->SetTargetReachedMode(FastMarchingFilterType::AllTargets);
    }

  FastMarchingFilterType::NodeContainerPointer seeds = FastMarchingFilterType::NodeContainer::New();
  int i;
  for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
    {
    // TODO: here we get the point. We should get the cell center instead.
    ImageType::PointType seedPoint(input->GetPoint(this->Seeds->GetId(i)));
    FastMarchingFilterType::NodeType::IndexType seedIndex;
    inImage->TransformPhysicalPointToIndex(seedPoint,seedIndex);
    FastMarchingFilterType::PixelType seedValue = itk::NumericTraits<FastMarchingFilterType::PixelType>::Zero;
    FastMarchingFilterType::NodeType seed;
    seed.SetValue(seedValue);
    seed.SetIndex(seedIndex);
    seeds->InsertElement(i,seed);
    }
  fastMarchingFilter->SetTrialPoints(seeds);

  FastMarchingFilterType::NodeContainerPointer targets = FastMarchingFilterType::NodeContainer::New();
  for (i=0; i<this->Targets->GetNumberOfIds(); i++)
    {
    // TODO: here we get the point. We should get the cell center instead.
    ImageType::PointType seedPoint(input->GetPoint(this->Targets->GetId(i)));
    FastMarchingFilterType::NodeType::IndexType seedIndex;
    inImage->TransformPhysicalPointToIndex(seedPoint,seedIndex);
    FastMarchingFilterType::PixelType seedValue = itk::NumericTraits<FastMarchingFilterType::PixelType>::Zero;
    FastMarchingFilterType::NodeType seed;
    seed.SetValue(seedValue);
    seed.SetIndex(seedIndex);
    targets->InsertElement(i,seed);
    }
  fastMarchingFilter->SetTargetPoints(targets);
  fastMarchingFilter->Update();

  this->TargetValue = fastMarchingFilter->GetTargetValue();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(fastMarchingFilter->GetOutput(),output);
}
