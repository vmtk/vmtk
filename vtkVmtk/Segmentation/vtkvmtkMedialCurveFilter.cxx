/*=========================================================================

  Copyright : (C)opyright (Pompeu Fabra University) 2007++
              See COPYRIGHT statement in top level directory.
  Authors   : Ignacio Larrabide && Xavier Mellado
  Modified  : $Author: $
  Purpose   : Application for computing of medial curve inside a surface
              from the distance map representation of a surface.
  Date      : $Date: $
  Version   : $Revision: $

=========================================================================*/

//ITK
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkAverageOutwardFluxImageFilter.h"
#include "itkMedialCurveImageFilter.h"

#include "vtkvmtkMedialCurveFilter.h"
#include "vtkObjectFactory.h"
#include "vtkType.h"

#include "vtkImageData.h"
#include "vtkvmtkITKFilterUtilities.h"

template< class TImage >
void vtkvmtkMedialCurveFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
	typename TImage::Pointer inImage;
	using ImageType = TImage;
	using PixelType = float;
	using ImageType = itk::Image< PixelType, 3 >;

	vtkvmtkITKFilterUtilities::VTKToITKImage< ImageType >(input, inImage);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.	Compute the associated average outward flux
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// To have a good quality gradient of the distance map, perform a light smooth over it. Define  
	// convolution kernels in each direction and use them recursively. 
	// typedef itk::RecursiveGaussianImageFilter< ImageType, ImageType > RecursiveGaussianFilterType;
	using RecursiveGaussianFilterType = itk::RecursiveGaussianImageFilter< ImageType, ImageType >;
	RecursiveGaussianFilterType::Pointer gaussianFilterX = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterY = RecursiveGaussianFilterType::New();
	RecursiveGaussianFilterType::Pointer gaussianFilterZ = RecursiveGaussianFilterType::New();

	gaussianFilterX->SetDirection( 0 );
	gaussianFilterY->SetDirection( 1 );
	gaussianFilterZ->SetDirection( 2 );

	gaussianFilterX->SetOrder( RecursiveGaussianFilterType::ZeroOrder );
	gaussianFilterY->SetOrder( RecursiveGaussianFilterType::ZeroOrder );
	gaussianFilterZ->SetOrder( RecursiveGaussianFilterType::ZeroOrder );

	gaussianFilterX->SetNormalizeAcrossScale( false );
	gaussianFilterY->SetNormalizeAcrossScale( false );
	gaussianFilterZ->SetNormalizeAcrossScale( false );

	gaussianFilterX->SetInput( inImage );
	gaussianFilterY->SetInput( gaussianFilterX->GetOutput() );
	gaussianFilterZ->SetInput( gaussianFilterY->GetOutput() );

	gaussianFilterX->SetSigma( this->Sigma );
	gaussianFilterY->SetSigma( this->Sigma );
	gaussianFilterZ->SetSigma( this->Sigma );

	using GradientFilterType = itk::GradientImageFilter< ImageType, PixelType, PixelType >;
	GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput( gaussianFilterZ->GetOutput() );
	gradientFilter->SetInput( gaussianFilterY->GetOutput() );

	// Compute the average outward flux.
	using AOFFilterType = itk::AverageOutwardFluxImageFilter< ImageType, PixelType, GradientFilterType::OutputImageType::PixelType >;
	AOFFilterType::Pointer aofFilter = AOFFilterType::New();
	aofFilter->SetInput( inImage );
	aofFilter->SetGradientImage( gradientFilter->GetOutput() );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Compute the skeleton
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	using MedialCurveFilter = itk::MedialCurveImageFilter< ImageType >;
	MedialCurveFilter::Pointer medialFilter = MedialCurveFilter::New();
	medialFilter->SetInput( inImage );
	medialFilter->SetAverageOutwardFluxImage( aofFilter->GetOutput() );
	medialFilter->SetThreshold( this->Threshold );
	medialFilter->Update();

	vtkvmtkITKFilterUtilities::ITKToVTKImage< TImage >(medialFilter->GetOutput(), output);
}