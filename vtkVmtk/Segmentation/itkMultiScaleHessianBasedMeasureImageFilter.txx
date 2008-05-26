/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiScaleHessianBasedMeasureImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/06/20 16:03:23 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiScaleHessianBasedMeasureImageFilter_txx
#define __itkMultiScaleHessianBasedMeasureImageFilter_txx

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "vnl/vnl_math.h"

#define EPSILON  1e-03

namespace itk
{

/**
 * Constructor
 */
template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::MultiScaleHessianBasedMeasureImageFilter()
{
  m_SigmaMin = 0.2;
  m_SigmaMax = 2.0;

  m_NumberOfSigmaSteps = 10;
  m_SigmaStepMethod = Self::LogarithmicSigmaSteps;

  m_HessianFilter                = HessianFilterType::New();
  m_HessianToMeasureFilter       = HessianToMeasureFilterType::New();

  //Instantiate Update buffer
  m_UpdateBuffer                 = UpdateBufferType::New();

  typename TOutputImage::Pointer scalesImage = TOutputImage::New();
  this->ProcessObject::SetNumberOfRequiredOutputs(2);
  this->ProcessObject::SetNthOutput(1, scalesImage.GetPointer());
}

template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
void
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::AllocateUpdateBuffer()
{
  /* The update buffer looks just like the output and holds the best response
     in the  objectness measure */
  
  typename TOutputImage::Pointer output = this->GetOutput();

  m_UpdateBuffer->SetSpacing(output->GetSpacing());
  m_UpdateBuffer->SetOrigin(output->GetOrigin());
  m_UpdateBuffer->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
  m_UpdateBuffer->SetRequestedRegion(output->GetRequestedRegion());
  m_UpdateBuffer->SetBufferedRegion(output->GetBufferedRegion());
  m_UpdateBuffer->Allocate();

  // Update buffer is used for > comparisons so make it really really small, just to be sure. Thanks to Hauke Heibel. 
  m_UpdateBuffer->FillBuffer(itk::NumericTraits<typename UpdateBufferType::ValueType>::NonpositiveMin());  
}

template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
void
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::GenerateData()
{
  // Allocate the output
  this->GetOutput(0)->SetBufferedRegion( this->GetOutput(0)->GetRequestedRegion() );
  this->GetOutput(0)->Allocate();

  this->GetOutput(1)->SetBufferedRegion( this->GetOutput(1)->GetRequestedRegion() );
  this->GetOutput(1)->Allocate();

  // The values are all positive and it is never used for any comparisons. Thanks to Hauke Heibel. 
  this->GetOutput(1)->FillBuffer(0);

  // Allocate the buffer
  AllocateUpdateBuffer();
  
  typename InputImageType::ConstPointer input = this->GetInput();
 
  this->m_HessianFilter->SetInput(input);

  this->m_HessianFilter->SetNormalizeAcrossScale(true);
 
  double sigma = m_SigmaMin;

  int scaleLevel = 1;

  while (sigma <= m_SigmaMax)
    {
    std::cout << "Computing measure for scale with sigma= " 
              << sigma << std::endl;

    m_HessianFilter->SetSigma( sigma );

    m_HessianToMeasureFilter->SetInput ( m_HessianFilter->GetOutput() ); 

    m_HessianToMeasureFilter->Update();
 
    this->UpdateMaximumResponse(sigma);

    sigma  = this->ComputeSigmaValue( scaleLevel );

    scaleLevel++;
    } 

  //Write out the best response to the output image
  ImageRegionIterator<UpdateBufferType> it(m_UpdateBuffer,m_UpdateBuffer->GetLargestPossibleRegion());
  it.GoToBegin();

  ImageRegionIterator<TOutputImage> oit(this->GetOutput(0),this->GetOutput(0)->GetLargestPossibleRegion());
  oit.GoToBegin();

  while(!oit.IsAtEnd())
    {
    oit.Value() = static_cast< OutputPixelType >( it.Get() );
    ++oit;
    ++it;
    }
}

template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
void
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::UpdateMaximumResponse(double sigma)
{

  ImageRegionIterator<UpdateBufferType> oit(m_UpdateBuffer,m_UpdateBuffer->GetLargestPossibleRegion());
  ImageRegionIterator<TOutputImage> osit(this->GetOutput(1),this->GetOutput(1)->GetLargestPossibleRegion());

  oit.GoToBegin();
  osit.GoToBegin();

  typedef typename HessianToMeasureFilterType::OutputImageType HessianToMeasureOutputImageType;

  ImageRegionIterator<HessianToMeasureOutputImageType> it(m_HessianToMeasureFilter->GetOutput(),
    this->m_HessianToMeasureFilter->GetOutput()->GetLargestPossibleRegion());

  it.GoToBegin();

  while(!oit.IsAtEnd())
    {
    if( oit.Value() < it.Value() )
      {
      oit.Value() = it.Value();
      osit.Value() = sigma;
      }
    ++oit;
    ++osit;
    ++it;
    }
}

template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
double
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::ComputeSigmaValue(int scaleLevel)
{
  double sigmaValue;

  switch (m_SigmaStepMethod)
    {
    case Self::EquispacedSigmaSteps:
      {
      const double stepSize = std::max(1e-10, ( m_SigmaMax - m_SigmaMin ) / m_NumberOfSigmaSteps);
      sigmaValue = m_SigmaMin + stepSize * scaleLevel;
      break;
      }
    case Self::LogarithmicSigmaSteps:
      {
      const double stepSize = std::max(1e-10, ( vcl_log(m_SigmaMax) - vcl_log(m_SigmaMin) ) / m_NumberOfSigmaSteps);
      sigmaValue = vcl_exp( vcl_log (m_SigmaMin) + stepSize * scaleLevel);
      break;
      }
    default:
      throw ExceptionObject(__FILE__, __LINE__,"Invalid SigmaStepMethod.",ITK_LOCATION);
      break;
    }

  return sigmaValue;
}

template <typename TInputImage, typename THessianToMeasureFilter, typename TOutputImage >
void
MultiScaleHessianBasedMeasureImageFilter
<TInputImage,THessianToMeasureFilter,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "SigmaMin:  " << m_SigmaMin << std::endl;
  os << indent << "SigmaMax:  " << m_SigmaMax  << std::endl;
  os << indent << "NumberOfSigmaSteps:  " << m_NumberOfSigmaSteps  << std::endl;
  os << indent << "SigmaStepMethod:  " << m_SigmaStepMethod  << std::endl;
}


} // end namespace itk
  
#endif
