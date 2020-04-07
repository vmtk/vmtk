/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkUpwindGradientMagnitudeImageFilter.txx,v $
Language:  C++
Date:      $Date: 2005/10/06 11:03:26 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
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

=========================================================================*/
#ifndef _itkUpwindGradientMagnitudeImageFilter_txx
#define _itkUpwindGradientMagnitudeImageFilter_txx
#include "itkUpwindGradientMagnitudeImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkForwardDifferenceOperator.h"
#include "itkBackwardDifferenceOperator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
void
UpwindGradientMagnitudeImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "UseImageSpacing = " << m_UseImageSpacing << std::endl;
}

template <typename TInputImage, typename TOutputImage>
void 
UpwindGradientMagnitudeImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() //throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  InputImagePointer  inputPtr = 
    const_cast< InputImageType * >( this->GetInput());
  OutputImagePointer outputPtr = this->GetOutput();
  
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // Build an operator so that we can determine the kernel size
  ForwardDifferenceOperator<RealType, ImageDimension> oper;
  oper.SetDirection(0);
  oper.CreateDirectional();
  unsigned long radius = oper.GetRadius()[0];
  
  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( radius );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    std::ostringstream msg;
    msg << static_cast<const char *>(this->GetNameOfClass())
        << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}


template< typename TInputImage, typename TOutputImage >
void
UpwindGradientMagnitudeImageFilter< TInputImage, TOutputImage >
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       ThreadIdType threadId)
{
  unsigned int i;
  ZeroFluxNeumannBoundaryCondition<TInputImage> nbc;

  ConstNeighborhoodIterator<TInputImage> nit;
  ConstNeighborhoodIterator<TInputImage> bit;
  ImageRegionIterator<TOutputImage> it;

 
  NeighborhoodInnerProduct<TInputImage, RealType> SIP;

  // Allocate output
  typename OutputImageType::Pointer       output = this->GetOutput();
  typename  InputImageType::ConstPointer  input  = this->GetInput();

  // Set up operators
  BackwardDifferenceOperator<RealType, ImageDimension> fdop[ImageDimension];
  ForwardDifferenceOperator<RealType, ImageDimension> bdop[ImageDimension];

  for (i = 0; i< ImageDimension; i++)
    {
    bdop[i].SetDirection(0);
    bdop[i].CreateDirectional();
 
    fdop[i].SetDirection(0);
    fdop[i].CreateDirectional();
   
    if (m_UseImageSpacing == true)
      {
      if ( this->GetInput()->GetSpacing()[i] == 0.0 )
        {
        itkExceptionMacro(<< "Image spacing cannot be zero.");
        }
      else
        {
        bdop[i].ScaleCoefficients( 1.0 / this->GetInput()->GetSpacing()[i] );
        fdop[i].ScaleCoefficients( 1.0 / this->GetInput()->GetSpacing()[i] );
        }
      }
    }
  
  // Calculate iterator radius
  Size<ImageDimension> radius;
  for (i = 0; i < ImageDimension; ++i)
    {
    radius[i]  = fdop[0].GetRadius()[0];
    radius[i]  = bdop[0].GetRadius()[0];
    }
  
  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::
    FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage> bC;
  faceList = bC(input, outputRegionForThread, radius);
  
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::
    FaceListType::iterator fit;
  fit = faceList.begin();
  
  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
  
  // Process non-boundary face
  nit = ConstNeighborhoodIterator<TInputImage>(radius, input, *fit);
  
  std::slice bd_x_slice[ImageDimension];
  std::slice fd_x_slice[ImageDimension];
  const unsigned long center = nit.Size() / 2;
  for (i = 0; i < ImageDimension; ++i)
    {
    bd_x_slice[i] = std::slice( center - nit.GetStride(i) * radius[i],
                             bdop[i].GetSize()[0], nit.GetStride(i));
    fd_x_slice[i] = std::slice( center - nit.GetStride(i) * radius[i],
                             fdop[i].GetSize()[0], nit.GetStride(i));
    }
  
  double weights[2];
  weights[0] = (1.0 + fabs(m_UpwindFactor)) / 2.0;
  weights[1] = 1.0 - weights[0];
  
  // Process each of the boundary faces.  These are N-d regions which border
  // the edge of the buffer.
  for (fit=faceList.begin(); fit != faceList.end(); ++fit)
    { 
    bit = ConstNeighborhoodIterator<InputImageType>(radius,
                                                    input, *fit);
    it = ImageRegionIterator<OutputImageType>(output, *fit);
    bit.OverrideBoundaryCondition(&nbc);
    bit.GoToBegin();

    while ( ! bit.IsAtEnd() )
      {
      RealType a = NumericTraits<RealType>::Zero;
      for (i = 0; i < ImageDimension; ++i)
        {
        const RealType bdg = SIP(bd_x_slice[i], bit, bdop[i]);
        const RealType fdg = SIP(fd_x_slice[i], bit, fdop[i]);
        RealType g = NumericTraits<RealType>::Zero;
        if (m_UpwindFactor > 0.0)
          {
          g = -fdg > bdg ? weights[0] * fdg + weights[1] * bdg : weights[1] * fdg + weights[0] * bdg;
          }
        else
          {
          g = -fdg < bdg ? weights[0] * fdg + weights[1] * bdg : weights[1] * fdg + weights[0] * bdg;
          }
        if (-fdg > 0.0 || bdg > 0.0)
          {
          a += g * g;
          }
        }
      it.Value() = static_cast<OutputPixelType>(::sqrt(a));
      ++bit;
      ++it;
      progress.CompletedPixel();
      }
    }
}

} // end namespace itk

#endif
