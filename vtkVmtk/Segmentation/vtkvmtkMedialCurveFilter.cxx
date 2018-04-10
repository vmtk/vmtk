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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkImageData.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkAverageOutwardFluxImageFilter.h"
#include "itkMedialCurveImageFilter.h"

vtkStandardNewMacro(vtkvmtkMedialCurveFilter);

vtkvmtkMedialCurveFilter::vtkvmtkMedialCurveFilter()
{
	this->DistanceImage = NULL;
	this->Sigma = 0.5;
	this->Threshold = 0.0;
}

vtkvmtkMedialCurveFilter::~vtkvmtkMedialCurveFilter()
{
	if (this->DistanceImage)
	{
		this->DistanceImage->Delete();
		this->DistanceImage = NULL;
	}
}

vtkCxxSetObjectMacro(vtkvmtkMedialCurveFilter,DistanceImage,vtkImageData)

int main( int argc, char **argv )
{
	ImageType::Pointer distance = reader->GetOutput();
	double sigma = 0.5;
	double threshold = 0.0


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.	Compute the associated average outward flux
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// To have a good quality gradient of the distance map, perform a light smooth over it. Define  
	// convolution kernels in each direction and use them recursively. 
	typedef itk::RecursiveGaussianImageFilter< ImageType, ImageType > RecursiveGaussianFilterType;
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

	gaussianFilterX->SetInput( distance );
	gaussianFilterY->SetInput( gaussianFilterX->GetOutput() );
	gaussianFilterZ->SetInput( gaussianFilterY->GetOutput() );

	gaussianFilterX->SetSigma( sigma );
	gaussianFilterY->SetSigma( sigma );
	gaussianFilterZ->SetSigma( sigma );

	typedef itk::GradientImageFilter< ImageType, PixelType, PixelType > GradientFilterType;

	// Compute the gradient.
	GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput( gaussianFilterZ->GetOutput() );
	gradientFilter->SetInput( gaussianFilterY->GetOutput() );

	// Compute the average outward flux.
	typedef itk::AverageOutwardFluxImageFilter< ImageType, PixelType, GradientFilterType::OutputImageType::PixelType > AOFFilterType;
	AOFFilterType::Pointer aofFilter = AOFFilterType::New();
	aofFilter->SetInput( distance );
	aofFilter->SetGradientImage( gradientFilter->GetOutput() );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Compute the skeleton
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::MedialCurveImageFilter< ImageType > MedialCurveFilter;
	MedialCurveFilter::Pointer medialFilter = MedialCurveFilter::New();
	medialFilter->SetInput( distance );
	medialFilter->SetAverageOutwardFluxImage( aofFilter->GetOutput() );
	medialFilter->SetThreshold( threshold );

	// Write the output image containing the skeleton.
	typedef itk::ImageFileWriter< MedialCurveFilter::TOutputImage > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( medialFilter->GetOutput() );
	writer->SetFileName( output_name );
	try
	{
		writer->Update();
	}
	catch ( itk::ExceptionObject &err )
	{
		std::cout << "ExceptionObject caught !" << std::endl; 
		std::cout << err << std::endl; 
		return -1;
	}

	return 0;	
}
