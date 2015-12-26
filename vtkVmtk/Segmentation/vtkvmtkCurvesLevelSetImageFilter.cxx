/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCurvesLevelSetImageFilter.cxx,v $
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

#include "vtkvmtkCurvesLevelSetImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkCurvesLevelSetImageFilter.h"

vtkStandardNewMacro(vtkvmtkCurvesLevelSetImageFilter);

vtkvmtkCurvesLevelSetImageFilter::vtkvmtkCurvesLevelSetImageFilter()
{
  this->IsoSurfaceValue = 0.0;
  this->NumberOfIterations = 100;
  this->PropagationScaling = 0.0;
  this->CurvatureScaling = 0.0;
  this->AdvectionScaling = 0.0;
  this->MaximumRMSError = 1E-4;
  this->UseNegativeFeatures = 0;
  this->UseImageSpacing = 1;
  this->AutoGenerateSpeedAdvection = 1;
  this->InterpolateSurfaceLocation = 1;
  this->DerivativeSigma = 0.0;
  this->RMSChange = 0.0;
  this->ElapsedIterations = 0;

  this->FeatureImage = NULL;
  this->SpeedImage = NULL;
}

vtkvmtkCurvesLevelSetImageFilter::~vtkvmtkCurvesLevelSetImageFilter()
{
  if (this->FeatureImage)
    {
      this->FeatureImage->Delete();
      this->FeatureImage = NULL;
    }

  if (this->SpeedImage)
    {
      this->SpeedImage->Delete();
      this->SpeedImage = NULL;
    }
}

void vtkvmtkCurvesLevelSetImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  ImageType::Pointer speedImage = ImageType::New();
  ImageType::Pointer featureImage = ImageType::New();

  typedef itk::CurvesLevelSetImageFilter<ImageType,ImageType> CurvesLevelSetFilterType;

  CurvesLevelSetFilterType::Pointer curvesLevelSetFilter = CurvesLevelSetFilterType::New();
  curvesLevelSetFilter->SetInput(inImage);
  curvesLevelSetFilter->SetIsoSurfaceValue(this->IsoSurfaceValue);
  curvesLevelSetFilter->SetNumberOfIterations(this->NumberOfIterations);
  curvesLevelSetFilter->SetPropagationScaling(this->PropagationScaling);
  curvesLevelSetFilter->SetCurvatureScaling(this->CurvatureScaling);
  curvesLevelSetFilter->SetAdvectionScaling(this->AdvectionScaling);
  curvesLevelSetFilter->SetMaximumRMSError(this->MaximumRMSError);
  curvesLevelSetFilter->SetUseNegativeFeatures(this->UseNegativeFeatures);
  curvesLevelSetFilter->SetUseImageSpacing(this->UseImageSpacing);
  curvesLevelSetFilter->SetAutoGenerateSpeedAdvection(this->AutoGenerateSpeedAdvection);
  curvesLevelSetFilter->SetInterpolateSurfaceLocation(this->InterpolateSurfaceLocation);
  curvesLevelSetFilter->SetDerivativeSigma(this->DerivativeSigma);
  if (this->SpeedImage)
  {
    vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(this->SpeedImage,speedImage);
    curvesLevelSetFilter->SetSpeedImage(speedImage);
  }
  if (this->FeatureImage)
  {
    vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(this->FeatureImage,featureImage);
    curvesLevelSetFilter->SetFeatureImage(featureImage);
  }
  vtkvmtkITKFilterUtilities::ConnectProgress(curvesLevelSetFilter,this);
  curvesLevelSetFilter->Update();

  this->RMSChange = curvesLevelSetFilter->GetRMSChange();
  this->ElapsedIterations = curvesLevelSetFilter->GetElapsedIterations();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(curvesLevelSetFilter->GetOutput(),output);
}

