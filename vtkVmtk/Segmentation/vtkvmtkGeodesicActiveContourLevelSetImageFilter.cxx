/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkbvsGeodesicActiveContourLevelSetImageFilter.cxx,v $
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

#include "vtkvmtkGeodesicActiveContourLevelSetImageFilter.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkGeodesicActiveContourLevelSetImageFilter.h"

vtkStandardNewMacro(vtkvmtkGeodesicActiveContourLevelSetImageFilter);

vtkvmtkGeodesicActiveContourLevelSetImageFilter::vtkvmtkGeodesicActiveContourLevelSetImageFilter()
{
  this->IsoSurfaceValue = 0.0;
  this->NumberOfIterations = 0;
  this->PropagationScaling = 0.0;
  this->CurvatureScaling = 0.0;
  this->AdvectionScaling = 0.0;
  this->MaximumRMSError = 1E-6;
  this->UseNegativeFeatures = 0;
  this->AutoGenerateSpeedAdvection = 1;
  this->InterpolateSurfaceLocation = 1;
  this->UseImageSpacing = 1;
  this->DerivativeSigma = 0.0;
  this->RMSChange = 0.0;
  this->ElapsedIterations = 0;
  this->FeatureImage = NULL;
  this->SpeedImage = NULL;
}

vtkvmtkGeodesicActiveContourLevelSetImageFilter::~vtkvmtkGeodesicActiveContourLevelSetImageFilter()
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

void vtkvmtkGeodesicActiveContourLevelSetImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  typedef itk::Image<float,3> ImageType;

  ImageType::Pointer inImage = ImageType::New();

  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,inImage);

  typedef itk::GeodesicActiveContourLevelSetImageFilter<ImageType,ImageType> LevelSetFilterType;

  ImageType::Pointer featureImage = ImageType::New();
  ImageType::Pointer speedImage = ImageType::New();

  LevelSetFilterType::Pointer levelSetFilter = LevelSetFilterType::New();
  levelSetFilter->SetInput(inImage);
  if (this->FeatureImage)
  {
    vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(this->FeatureImage,featureImage);
    levelSetFilter->SetFeatureImage(featureImage);
  }
  if (this->SpeedImage)
  {
    vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(this->SpeedImage,speedImage);
    levelSetFilter->SetSpeedImage(speedImage);
  }
  levelSetFilter->SetIsoSurfaceValue(this->IsoSurfaceValue);
  levelSetFilter->SetNumberOfIterations(this->NumberOfIterations);
  levelSetFilter->SetPropagationScaling(this->PropagationScaling);
  levelSetFilter->SetCurvatureScaling(this->CurvatureScaling);
  levelSetFilter->SetAdvectionScaling(this->AdvectionScaling);
  levelSetFilter->SetMaximumRMSError(this->MaximumRMSError);
  levelSetFilter->SetReverseExpansionDirection(this->UseNegativeFeatures);
  levelSetFilter->SetAutoGenerateSpeedAdvection(this->AutoGenerateSpeedAdvection);
  levelSetFilter->SetInterpolateSurfaceLocation(this->InterpolateSurfaceLocation);
  levelSetFilter->SetUseImageSpacing(this->UseImageSpacing);
  levelSetFilter->SetDerivativeSigma(this->DerivativeSigma);

  vtkvmtkITKFilterUtilities::ConnectProgress(levelSetFilter,this);
 
  levelSetFilter->Update();

  this->RMSChange = levelSetFilter->GetRMSChange();
  this->ElapsedIterations = levelSetFilter->GetElapsedIterations();

  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(levelSetFilter->GetOutput(),output);
}

