/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkFWHMFeatureImageFilter.txx,v $
Language:  C++
Date:      $Date: 2005/03/04 11:14:37 $
Version:   $Revision: 1.2 $

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

#ifndef _itkFWHMFeatureImageFilter_txx
#define _itkFWHMFeatureImageFilter_txx
#include "itkFWHMFeatureImageFilter.h"

#include "itkGrayscaleDilateImageFilter.h"
#include "itkSigmoidImageFilter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
FWHMFeatureImageFilter<TInputImage,TOutputImage>
::FWHMFeatureImageFilter()
  : m_UseImageSpacing( true )
{
  m_Radius.Fill(1);
  m_BackgroundValue = NumericTraits<InputPixelType>::Zero;
}


template< typename TInputImage, typename TOutputImage >
void
FWHMFeatureImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
  this->AllocateOutputs();

  StructuringElementRadiusType radius = m_Radius;
  
  typedef itk::GrayscaleDilateImageFilter<InputImageType,InputImageType,StructuringElementType> GrayscaleDilateFilterType;

  typename InputImageType::ConstPointer inputImage = this->GetInput();

  StructuringElementType dilateStructuringElement;
  dilateStructuringElement.SetRadius(radius);
  dilateStructuringElement.CreateStructuringElement();

  typename GrayscaleDilateFilterType::Pointer grayscaleDilateFilter = GrayscaleDilateFilterType::New();
  grayscaleDilateFilter->SetInput(inputImage);
  grayscaleDilateFilter->SetKernel(dilateStructuringElement);
  grayscaleDilateFilter->Update();

  typename OutputImageType::Pointer outputImage = this->GetOutput();

  // build output image as sigmoid-filtered difference between pixel and half-maximum
  // using Sigmoid functor 
  itk::Functor::Sigmoid<InputPixelType,OutputPixelType> sigmoid;
  sigmoid.SetAlpha(1.0/6.0);
  sigmoid.SetBeta(0.0);
  sigmoid.SetOutputMinimum(-1.0);
  sigmoid.SetOutputMaximum(1.0);

  ImageRegionConstIterator<InputImageType> inputIt(inputImage, inputImage->GetBufferedRegion());
  ImageRegionConstIterator<InputImageType> dilateIt(grayscaleDilateFilter->GetOutput(), grayscaleDilateFilter->GetOutput()->GetBufferedRegion());
  ImageRegionIterator<OutputImageType> outputIt(outputImage, outputImage->GetBufferedRegion());

  inputIt.GoToBegin();
  dilateIt.GoToBegin();
  outputIt.GoToBegin();

  while( !inputIt.IsAtEnd() || !dilateIt.IsAtEnd() || !outputIt.IsAtEnd())
    {
    InputPixelType inputPixel = inputIt.Get();
    InputPixelType inputPixelToBackgroundValue = inputPixel - m_BackgroundValue;
    InputPixelType dilatePixel = dilateIt.Get();
    InputPixelType halfMaximumToBackgroundValue = (dilatePixel - m_BackgroundValue) / 2.0;
    
    InputPixelType featureValue = sigmoid(inputPixelToBackgroundValue-halfMaximumToBackgroundValue);
 
    outputIt.Set(featureValue);
    ++inputIt;
    ++dilateIt;
    ++outputIt;
    }
}

template <typename TInputImage, typename TOutputImage>
void
FWHMFeatureImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "UseImageSpacing = " << m_UseImageSpacing << std::endl;
  os << indent << "Radius = " << m_Radius << std::endl;
  os << indent << "BackgroundValue = " << m_BackgroundValue << std::endl;
}

} // end namespace itk

#endif
