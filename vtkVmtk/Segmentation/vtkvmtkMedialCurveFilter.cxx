/*=========================================================================

Original Method Developed By:
	Copyright : (C)opyright (Pompeu Fabra University) 2007++
				See COPYRIGHT statement in top level directory.
	Authors   : Ignacio Larrabide && Xavier Mellado
	Modified  : $Author: $
	Purpose   : Application for computing of medial curve inside a surface
				from the distance map representation of a surface.
	Date      : $Date: $
	Version   : $Revision: $

Modifications Made To Support Integration Into:
	The Vascular Modeling Toolkit (VMTK)
	www.vmtk.org

Modification Authors:
	Richard Izzo
		University at Buffalo
		Github: rlizzo
		Date: April 12, 2018
	Luca Antiga
		Orobix Srl.
		Github: lantiga
		Date: April 12, 2018

=========================================================================*/

#include "vtkvmtkMedialCurveFilter.h"
#include "vtkSimpleImageToImageFilter.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkAverageOutwardFluxImageFilter.h"
#include "itkMedialCurveImageFilter.h"
#include "itkApproximateSignedDistanceMapImageFilter.h"

vtkStandardNewMacro(vtkvmtkMedialCurveFilter);

vtkvmtkMedialCurveFilter::vtkvmtkMedialCurveFilter()
{
	this->Sigma = 0.5;
	this->Threshold = 0.0;
}

vtkvmtkMedialCurveFilter::~vtkvmtkMedialCurveFilter() {}


void vtkvmtkMedialCurveFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
	typedef itk::Image<unsigned char, 3> UnsignedCharImageType;
	typedef itk::Image<float, 3> FloatImageType;
	typedef float PixelType;
	typedef itk::Image<PixelType, 3> ImageType;

    UnsignedCharImageType::Pointer inImage = UnsignedCharImageType::New();

    vtkvmtkITKFilterUtilities::VTKToITKImage<UnsignedCharImageType>(input,inImage); 

	typedef itk::ApproximateSignedDistanceMapImageFilter<UnsignedCharImageType, FloatImageType> ApproximateSignedDistanceMapImageFilterType;
	ApproximateSignedDistanceMapImageFilterType::Pointer approximateSignedDistanceMapImageFilter = ApproximateSignedDistanceMapImageFilterType::New();
	approximateSignedDistanceMapImageFilter->SetInput(inImage);
	approximateSignedDistanceMapImageFilter->SetInsideValue(255);
	approximateSignedDistanceMapImageFilter->SetOutsideValue(0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.	Compute the associated average outward flux
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// To have a good quality gradient of the distance map, perform a light smooth over it. Define
	// convolution kernels in each direction and use them recursively.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> RecursiveGaussianFilterType;
	RecursiveGaussianFilterType::Pointer gaussianFilterX = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterY = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterZ = RecursiveGaussianFilterType::New();

	gaussianFilterX->SetDirection(0);
	gaussianFilterY->SetDirection(1);
	gaussianFilterZ->SetDirection(2);

	gaussianFilterX->SetOrder(RecursiveGaussianFilterType::ZeroOrder);
	gaussianFilterY->SetOrder(RecursiveGaussianFilterType::ZeroOrder);
	gaussianFilterZ->SetOrder(RecursiveGaussianFilterType::ZeroOrder);

	gaussianFilterX->SetNormalizeAcrossScale(false);
	gaussianFilterY->SetNormalizeAcrossScale(false);
	gaussianFilterZ->SetNormalizeAcrossScale(false);

	gaussianFilterX->SetInput(approximateSignedDistanceMapImageFilter->GetOutput());
	gaussianFilterY->SetInput(gaussianFilterX->GetOutput());
	gaussianFilterZ->SetInput(gaussianFilterY->GetOutput());

	gaussianFilterX->SetSigma(this->Sigma);
	gaussianFilterY->SetSigma(this->Sigma);
	gaussianFilterZ->SetSigma(this->Sigma);

	// Compute the gradient.
	typedef itk::GradientImageFilter<ImageType, PixelType, PixelType> GradientFilterType;
	GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput(gaussianFilterZ->GetOutput());
	gradientFilter->SetInput(gaussianFilterY->GetOutput());

	// Compute the average outward flux.
	typedef itk::AverageOutwardFluxImageFilter<ImageType, PixelType, GradientFilterType::OutputImageType::PixelType> AOFFilterType;
	AOFFilterType::Pointer aofFilter = AOFFilterType::New();
	aofFilter->SetInput(approximateSignedDistanceMapImageFilter->GetOutput());
	aofFilter->SetGradientImage(gradientFilter->GetOutput());

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Compute the skeleton
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::MedialCurveImageFilter<ImageType> MedialCurveFilter;
	MedialCurveFilter::Pointer medialFilter = MedialCurveFilter::New();
	medialFilter->SetInput(approximateSignedDistanceMapImageFilter->GetOutput());
	medialFilter->SetAverageOutwardFluxImage(aofFilter->GetOutput());
	medialFilter->SetThreshold(this->Threshold);
	medialFilter->Update();

	vtkvmtkITKFilterUtilities::ITKToVTKImage<UnsignedCharImageType>(medialFilter->GetOutput(),output);
}