/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAnisotropicDiffusionImageFilter.cxx,v $
Language:  C++
Date:      $Date: 2018/03/05 16:48:25 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga. All rights reserved.
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
  Note: this class was contributed by 
      Kurt Sansom
      https://github.com/kayarre
      

=========================================================================*/

#include "vtkvmtkAnisotropicDiffusionImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkType.h"

#include "vtkvmtkITKFilterUtilities.h"

#include "itkGradientAnisotropicDiffusionImageFilter.h"

vtkStandardNewMacro(vtkvmtkAnisotropicDiffusionImageFilter);

vtkvmtkAnisotropicDiffusionImageFilter::vtkvmtkAnisotropicDiffusionImageFilter()
{
  this->Conductance = 1.0;
  this->TimeStep = 0.0625;
  this->NumberOfIterations = 5;
}


template< class TImage >
int vtkvmtkAnisotropicDiffusionImageFilter::FilterImage( vtkImageData* input,
                                                         vtkImageData* output,
                                                         typename TImage::Pointer image )
{
  using ImageType = TImage;
  vtkvmtkITKFilterUtilities::VTKToITKImage<ImageType>(input,image);

  typedef itk::GradientAnisotropicDiffusionImageFilter< ImageType, ImageType > AnisotropicDiffusionFilterType;
  typename AnisotropicDiffusionFilterType::Pointer anisotropicDiffusionFilter = AnisotropicDiffusionFilterType::New();
  
  anisotropicDiffusionFilter->SetInput(image);
  anisotropicDiffusionFilter->SetTimeStep(this->TimeStep);
  anisotropicDiffusionFilter->SetNumberOfIterations(this->NumberOfIterations);
  anisotropicDiffusionFilter->SetConductanceParameter(this->Conductance);

  anisotropicDiffusionFilter->Update();
  vtkvmtkITKFilterUtilities::ITKToVTKImage<ImageType>(anisotropicDiffusionFilter->GetOutput(),output);

  return EXIT_SUCCESS;
}


template< int VDimension >
int vtkvmtkAnisotropicDiffusionImageFilter::FilterScalarImage(vtkImageData* input,
                                                              vtkImageData* output,
                                                              int componentType )
{
  switch( componentType )
  {
    default:
#if ITK_VERSION_MAJOR >= 5
      // itkGradientAnisotropicDiffusionImageFilter only supports floating-point types
      return EXIT_FAILURE;
#else
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
#endif
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



void vtkvmtkAnisotropicDiffusionImageFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
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

