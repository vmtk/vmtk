/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCollidingFrontsImageFilter.cxx,v $
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

#include "vtkvmtkCollidingFrontsImageFilter.h"
#include "vtkvmtkITKFilterUtilities.h"
#include "vtkObjectFactory.h"

#include "itkCollidingFrontsImageFilter.h"

// TODO Remove
#include "vtkPointData.h"
#include "vtkDataArray.h"

vtkStandardNewMacro(vtkvmtkCollidingFrontsImageFilter);

vtkvmtkCollidingFrontsImageFilter::vtkvmtkCollidingFrontsImageFilter()
{
  this->Seeds1 = NULL;
  this->Seeds2 = NULL;

  this->ApplyConnectivity = 0;
  this->NegativeEpsilon = -1E-6;
  this->StopOnTargets = 0;
}

vtkvmtkCollidingFrontsImageFilter::~vtkvmtkCollidingFrontsImageFilter()
{
  if (this->Seeds1)
    {
      this->Seeds1->Delete();
      this->Seeds1 = NULL;
    }

  if (this->Seeds2)
    {
      this->Seeds2->Delete();
      this->Seeds2 = NULL;
    }
}

void vtkvmtkCollidingFrontsImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  std::cout<<input->GetScalarType()<<std::endl;
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::CollidingFrontsImageFilter<ImageType, ImageType> CollidingFrontsFilterType;
  CollidingFrontsFilterType::Pointer collidingFrontsFilter = CollidingFrontsFilterType::New();
  collidingFrontsFilter->SetInput(inImage);

  CollidingFrontsFilterType::NodeContainerPointer seeds1 = CollidingFrontsFilterType::NodeContainer::New();
  int i;
  for (i=0; i<this->Seeds1->GetNumberOfIds(); i++)
    {
    // TODO: here we get the point. We should get the cell center instead.
    ImageType::PointType seedPoint(input->GetPoint(this->Seeds1->GetId(i)));
    CollidingFrontsFilterType::NodeType::IndexType seedIndex;
    inImage->TransformPhysicalPointToIndex(seedPoint,seedIndex);
    CollidingFrontsFilterType::PixelType seedValue = itk::NumericTraits<CollidingFrontsFilterType::PixelType>::Zero;
    CollidingFrontsFilterType::NodeType seed;
    seed.SetValue(seedValue);
    seed.SetIndex(seedIndex);
    seeds1->InsertElement(i,seed);
    }
  collidingFrontsFilter->SetSeedPoints1(seeds1);

  CollidingFrontsFilterType::NodeContainerPointer seeds2 = CollidingFrontsFilterType::NodeContainer::New();
  for (i=0; i<this->Seeds2->GetNumberOfIds(); i++)
    {
    // TODO: here we get the point. We should get the cell center instead.
    ImageType::PointType seedPoint(input->GetPoint(this->Seeds2->GetId(i)));
    CollidingFrontsFilterType::NodeType::IndexType seedIndex;
    inImage->TransformPhysicalPointToIndex(seedPoint,seedIndex);
    CollidingFrontsFilterType::PixelType seedValue = itk::NumericTraits<CollidingFrontsFilterType::PixelType>::Zero;
    CollidingFrontsFilterType::NodeType seed;
    seed.SetValue(seedValue);
    seed.SetIndex(seedIndex);
    seeds2->InsertElement(i,seed);
    }
  collidingFrontsFilter->SetSeedPoints2(seeds2);

  collidingFrontsFilter->SetApplyConnectivity(this->ApplyConnectivity);
  collidingFrontsFilter->SetNegativeEpsilon(this->NegativeEpsilon);
  collidingFrontsFilter->SetStopOnTargets(this->StopOnTargets);
  collidingFrontsFilter->Update();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(collidingFrontsFilter->GetOutput(),output);
}

