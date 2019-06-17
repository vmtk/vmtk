/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFastMarchingDirectionalFreezeImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/02/09 15:06:19 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _itkFastMarchingDirectionalFreezeImageFilter_txx
#define _itkFastMarchingDirectionalFreezeImageFilter_txx

#include "itkFastMarchingDirectionalFreezeImageFilter.h"
#include "itkGradientImageFilter.h"

namespace itk
{

/*
 *
 */
template <class TLevelSet, class TSpeedImage>
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::FastMarchingDirectionalFreezeImageFilter()
{
  m_SpeedGradientImage = SpeedGradientImageType::New();
}


/*
 *
 */
template <class TLevelSet, class TSpeedImage>
void
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Speed gradient image: " << m_SpeedGradientImage.GetPointer() << std::endl;
}

/*
 *
 */
template <class TLevelSet, class TSpeedImage>
void
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::AllocateSpeedGradientImage()
{
  SpeedImageConstPointer speedImage = this->GetInput();
  m_SpeedGradientImage->SetRequestedRegion(speedImage->GetRequestedRegion());
  m_SpeedGradientImage->SetBufferedRegion(speedImage->GetBufferedRegion());
  m_SpeedGradientImage->SetLargestPossibleRegion(speedImage->GetLargestPossibleRegion());
  m_SpeedGradientImage->Allocate();
}

/*
 *
 */
template <class TLevelSet, class TSpeedImage>
void
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::ComputeSpeedGradientImage()
{
  if (this->GetInput() == NULL)
    {
    ExceptionObject err(__FILE__, __LINE__);
    err.SetLocation( ITK_LOCATION );
    err.SetDescription("Input image must be set");
    throw err;
    }

  SpeedImageConstPointer speedImage = this->GetInput();
  
  typedef GradientImageFilter<SpeedImageType> DerivativeFilterType;

  typename DerivativeFilterType::Pointer derivative = DerivativeFilterType::New();
  derivative->SetInput(speedImage);
  derivative->SetUseImageSpacingOn();
  derivative->Update();

  typedef typename DerivativeFilterType::OutputImageType DerivativeOutputImageType;

  ImageRegionIterator<SpeedGradientImageType>
    dit(derivative->GetOutput(),speedImage->GetRequestedRegion());
  ImageRegionIterator<SpeedGradientImageType>
    sgit(m_SpeedGradientImage,speedImage->GetRequestedRegion());

  for(dit.GoToBegin(),sgit.GoToBegin(); !dit.IsAtEnd(); ++dit,++sgit)
    {
    //TODO: cast
    sgit.Set(dit.Get());
    }
}

/*
 *
 */
template <class TLevelSet, class TSpeedImage>
void
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::GenerateData()
{
  this->AllocateSpeedGradientImage();
  this->ComputeSpeedGradientImage();
  // run the GenerateData() method of the superclass
  try 
    {
    Superclass::GenerateData();
    }
  catch (ProcessAborted &exc)
    {
    // process was aborted, clean up the state of the filter
    // (most of the cleanup will have already been done by the
    // superclass)

    throw exc;
    }
}

template <class TLevelSet, class TSpeedImage>
void
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::UpdateNeighbors(
  const IndexType& index,
  const SpeedImageType * speedImage,
  LevelSetImageType * output )
{
  GradientPixelType gradientValue = this->ComputeGradientValue(index, output);
  SpeedGradientPixelType speedGradientValue = m_SpeedGradientImage->GetPixel(index);

  //TODO: set threshold to speedGradientNorm

  // normalizing is probably not an excellent idea, but it's the only way to threshold by angle
  gradientValue.Normalize();
  speedGradientValue.Normalize();
  //TODO: cast here
  if (gradientValue * speedGradientValue < -0.6)
    {
    return;
    }
  
  Superclass::UpdateNeighbors(index,speedImage,output);
}

/*
 *
 */
template <class TLevelSet, class TSpeedImage>
typename FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>::GradientPixelType
FastMarchingDirectionalFreezeImageFilter<TLevelSet,TSpeedImage>
::ComputeGradientValue( const IndexType& index,
  const LevelSetImageType * output)
{
  IndexType neighIndex = index;
  typedef typename TLevelSet::PixelType LevelSetPixelType;
  LevelSetPixelType centerPixel;
  LevelSetPixelType dx_forward;
  LevelSetPixelType dx_backward;
  GradientPixelType gradientPixel;

  const LevelSetIndexType & lastIndex = this->GetLastIndex();
  const LevelSetIndexType & startIndex = this->GetStartIndex();

  const LevelSetPixelType ZERO =
            NumericTraits< LevelSetPixelType >::Zero;

  OutputSpacingType spacing = this->GetOutput()->GetSpacing();

  unsigned int xStride[itkGetStaticConstMacro(SetDimension)];

  for ( unsigned int j = 0; j < SetDimension; j++ )
    {
    centerPixel = output->GetPixel(index);

    neighIndex = index;

    // Set stride of one in each direction
    xStride[j] = 1;

    // Compute one-sided finite differences with alive neighbors 
    // (the front can only come from there)
    dx_backward = 0.0;
    neighIndex[j] = index[j] - xStride[j];

    if(! (neighIndex[j] > lastIndex[j] ||
          neighIndex[j] < startIndex[j]) )
      {
      if ( this->GetLabelImage()->GetPixel( neighIndex ) == Superclass::AlivePoint )
        {
        dx_backward = centerPixel - output->GetPixel( neighIndex );
        }
      }

    dx_forward = 0.0;
    neighIndex[j] = index[j] + xStride[j];

    if(! (neighIndex[j] > lastIndex[j] ||
          neighIndex[j] < startIndex[j]) )
      {
      if ( this->GetLabelImage()->GetPixel( neighIndex ) == Superclass::AlivePoint )
        {
        dx_forward = output->GetPixel( neighIndex ) - centerPixel;
        }
      }

    // Compute upwind finite differences
    if (std::max(dx_backward,-dx_forward) < ZERO)
      {
      gradientPixel[j] = ZERO;
      }
    else if (dx_backward > -dx_forward)
      {
      gradientPixel[j] = dx_backward;
      }
    else
      {
      gradientPixel[j] = dx_forward;
      }

    gradientPixel[j] /= spacing[j];
    }

  return gradientPixel;
}

} // namespace itk


#endif
