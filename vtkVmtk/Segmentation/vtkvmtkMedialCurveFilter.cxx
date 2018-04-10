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
#include "vtkvmtkITKFilterUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkType.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkAverageOutwardFluxImageFilter.h"
#include "itkMedialCurveImageFilter.h"

vtkStandardNewMacro(vtkvmtkMedialCurveFilter);

vtkvmtkMedialCurveFilter::vtkvmtkMedialCurveFilter()
{
	this->Sigma = 0.5;
	this->Threshold = 0.0;
}

template< class TImage >
void vtkvmtkMedialCurveFilter::FilterImage(vtkImageData* input,
                                          vtkImageData* output,
                                          typename TImage::Pointer image)
{
	using ImageType = TImage;
	vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,image);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1.	Compute the associated average outward flux
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// To have a good quality gradient of the distance map, perform a light smooth over it. Define  
	// convolution kernels in each direction and use them recursively. 
	typedef itk::RecursiveGaussianImageFilter< ImageType, ImageType > RecursiveGaussianFilterType;
	typename RecursiveGaussianFilterType::Pointer gaussianFilterX = RecursiveGaussianFilterType::New();
	typename RecursiveGaussianFilterType::Pointer gaussianFilterY = RecursiveGaussianFilterType::New();
	typename RecursiveGaussianFilterType::Pointer gaussianFilterZ = RecursiveGaussianFilterType::New();

	gaussianFilterX->SetDirection( 0 );
	gaussianFilterY->SetDirection( 1 );
	gaussianFilterZ->SetDirection( 2 );

	gaussianFilterX->SetOrder( RecursiveGaussianFilterType::ZeroOrder );
	gaussianFilterY->SetOrder( RecursiveGaussianFilterType::ZeroOrder );
	gaussianFilterZ->SetOrder( RecursiveGaussianFilterType::ZeroOrder );

	gaussianFilterX->SetNormalizeAcrossScale( false );
	gaussianFilterY->SetNormalizeAcrossScale( false );
	gaussianFilterZ->SetNormalizeAcrossScale( false );

	gaussianFilterX->SetInput( image );
	gaussianFilterY->SetInput( gaussianFilterX->GetOutput() );
	gaussianFilterZ->SetInput( gaussianFilterY->GetOutput() );

	gaussianFilterX->SetSigma( this->Sigma );
	gaussianFilterY->SetSigma( this->Sigma );
	gaussianFilterZ->SetSigma( this->Sigma );

	// Compute the gradient.
	typedef itk::GradientImageFilter< ImageType, PixelType, PixelType > GradientFilterType;
	typename GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
	gradientFilter->SetInput( gaussianFilterZ->GetOutput() );
	gradientFilter->SetInput( gaussianFilterY->GetOutput() );

	// Compute the average outward flux.
	typedef itk::AverageOutwardFluxImageFilter< ImageType, PixelType, GradientFilterType::OutputImageType::PixelType > AOFFilterType;
	typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
	aofFilter->SetInput( image );
	aofFilter->SetGradientImage( gradientFilter->GetOutput() );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Compute the skeleton
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef itk::MedialCurveImageFilter< ImageType > MedialCurveFilter;
	typename MedialCurveFilter::Pointer medialFilter = MedialCurveFilter::New();
	medialFilter->SetInput( image );
	medialFilter->SetAverageOutwardFluxImage( aofFilter->GetOutput() );
	medialFilter->SetThreshold( this->Threshold );

	medialFilter->Update();
	vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(medialFilter->GetOutput(),output);

	return EXIT_SUCCESS;
}


template< int VDimension >
int vtkvmtkMedialCurveFilter::FilterScalarImage(vtkImageData* input,
												vtkImageData* output,
												int componentType )
{
  switch( componentType )
  {
    default:
    case VTK_UNSIGNED_CHAR:
    {
      using PixelType = unsigned char;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_CHAR:
    {
      using PixelType = char;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_UNSIGNED_SHORT:
    {
      using PixelType = unsigned short;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_SHORT:
    {
      using PixelType = short;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_UNSIGNED_INT:
    {
      using PixelType = unsigned int;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }

      break;
    }

    case VTK_INT:
    {
      using PixelType = int;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_UNSIGNED_LONG:
    {
      using PixelType = unsigned long;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }

      break;
    }

    case VTK_LONG:
    {
      using PixelType = long;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_FLOAT:
    {
      using PixelType = float;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case VTK_DOUBLE:
    {
      using PixelType = double;
      using ImageType = itk::Image< PixelType, VDimension >;
      typename ImageType::Pointer image = ImageType::New();
      if( FilterImage< ImageType >( input, output, image ) == EXIT_FAILURE )
      {
        return EXIT_FAILURE;
      }
      break;
    }
  }
  return EXIT_SUCCESS;
}



void vtkvmtkMedialCurveFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  int imageDimension = input->GetDataDimension();
  int componentType = input->GetScalarType();
  
  if( imageDimension == 2 )
  {
    FilterScalarImage< 2 >( input, output, componentType );
  }
  else if( imageDimension == 3 )
  {
    FilterScalarImage< 3 >(input, output, componentType );
  }

}