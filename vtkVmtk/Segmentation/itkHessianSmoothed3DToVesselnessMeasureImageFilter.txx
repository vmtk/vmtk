/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessianSmoothed3DToVesselnessMeasureImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/06/12 20:59:44 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHessianSmoothed3DToVesselnessMeasureImageFilter_txx
#define __itkHessianSmoothed3DToVesselnessMeasureImageFilter_txx

#include "itkHessianSmoothed3DToVesselnessMeasureImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "vnl/vnl_math.h"

#define EPSILON  1e-03

namespace itk
{

/**
 * Constructor
 */
template < typename TPixel >
HessianSmoothed3DToVesselnessMeasureImageFilter< TPixel >
::HessianSmoothed3DToVesselnessMeasureImageFilter()
{
  m_Alpha = 0.5;
  m_Beta  = 0.5;
  m_Gamma = 5.0;

  m_C = 1e-5;

  m_SymmetricEigenValueFilter = EigenAnalysisFilterType::New();
  m_SymmetricEigenValueFilter->SetDimension( ImageDimension );
  m_SymmetricEigenValueFilter->OrderEigenValuesBy( 
      EigenAnalysisFilterType::FunctorType::OrderByValue );

  // By default, *do not* scale the vesselness measure by the largest
  // eigen value
  m_ScaleVesselnessMeasure  = false;
}


template < typename TPixel >
void 
HessianSmoothed3DToVesselnessMeasureImageFilter< TPixel >
::GenerateData()
{
  itkDebugMacro(
      << "HessianSmoothed3DToVesselnessMeasureImageFilter generating data ");

  m_SymmetricEigenValueFilter->SetInput( this->GetInput() );
  
  typename OutputImageType::Pointer output = this->GetOutput();

  typedef typename EigenAnalysisFilterType::OutputImageType
                                            EigenValueImageType;

  m_SymmetricEigenValueFilter->Update();
  
  const typename EigenValueImageType::ConstPointer eigenImage = 
                    m_SymmetricEigenValueFilter->GetOutput();
  
  // walk the region of eigen values and get the vesselness measure
  EigenValueArrayType eigenValue;
  ImageRegionConstIterator<EigenValueImageType> it;
  it = ImageRegionConstIterator<EigenValueImageType>(
      eigenImage, eigenImage->GetRequestedRegion());
  ImageRegionIterator<OutputImageType> oit;
  this->AllocateOutputs();
  oit = ImageRegionIterator<OutputImageType>(output,
                                             output->GetRequestedRegion());
  oit.GoToBegin();
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    // Get the eigen value
    eigenValue = it.Get();


    // Find the smallest eigenvalue
    double smallest = itk::Math::abs( eigenValue[0] );
    double Lambda1 = eigenValue[0];
 
    for ( unsigned int i=1; i <=2; i++ )
      {
      if ( itk::Math::abs( eigenValue[i] ) < smallest )
        {
        Lambda1 = eigenValue[i];
        smallest = itk::Math::abs( eigenValue[i] );
        }
      }

    // Find the largest eigenvalue
    double largest = itk::Math::abs( eigenValue[0] );
    double Lambda3 = eigenValue[0];
 
    for ( unsigned int i=1; i <=2; i++ )
      {
      if (  itk::Math::abs( eigenValue[i] > largest ) )
        {
        Lambda3 = eigenValue[i];
        largest = itk::Math::abs( eigenValue[i] );
        }
      }


    //  find Lambda2 so that |Lambda1| < |Lambda2| < |Lambda3|
    double Lambda2 = eigenValue[0];

    for ( unsigned int i=0; i <=2; i++ )
      {
      if ( eigenValue[i] != Lambda1 && eigenValue[i] != Lambda3 )
        {
        Lambda2 = eigenValue[i];
        break;
        }
      }

    if ( Lambda2 >= 0.0 ||  Lambda3 >= 0.0 || 
         itk::Math::abs( Lambda2) < EPSILON  || 
         itk::Math::abs( Lambda3 ) < EPSILON )
      {
      oit.Set( NumericTraits< OutputPixelType >::Zero );
      } 
    else
      {
   
      double Lambda1Abs = itk::Math::abs( Lambda1 );
      double Lambda2Abs = itk::Math::abs( Lambda2 );
      double Lambda3Abs = itk::Math::abs( Lambda3 );

      double Lambda1Sqr = itk::Math::sqr( Lambda1 );
      double Lambda2Sqr = itk::Math::sqr( Lambda2 );
      double Lambda3Sqr = itk::Math::sqr( Lambda3 );

      double AlphaSqr = itk::Math::sqr( m_Alpha );
      double BetaSqr = itk::Math::sqr( m_Beta );
      double GammaSqr = itk::Math::sqr( m_Gamma );

      double A  = Lambda2Abs / Lambda3Abs; 
      double B  = Lambda1Abs / std::sqrt ( itk::Math::abs( Lambda2 * Lambda3 )); 
      double S  = std::sqrt( Lambda1Sqr + Lambda2Sqr + Lambda3Sqr );

      double vesMeasure_1  = 
         ( 1 - std::exp(-1.0*(( itk::Math::sqr(A) ) / ( 2.0 * ( AlphaSqr)))));

      double vesMeasure_2  = 
         std::exp ( -1.0 * ((itk::Math::sqr( B )) /  ( 2.0 * (BetaSqr))));

      double vesMeasure_3  = 
         ( 1 - std::exp( -1.0 * (( itk::Math::sqr( S )) / ( 2.0 * ( GammaSqr)))));

      double vesMeasure_4  = 
         std::exp ( -1.0 * ( 2.0 * itk::Math::sqr( m_C )) / 
                                   ( Lambda2Abs * (Lambda3Sqr))); 

      double vesselnessMeasure = 
         vesMeasure_1 * vesMeasure_2 * vesMeasure_3 * vesMeasure_4; 

      if(  m_ScaleVesselnessMeasure ) 
        {
        oit.Set( static_cast< OutputPixelType >(
                                     Lambda3Abs*vesselnessMeasure ) );
        }
      else
        {
        oit.Set( static_cast< OutputPixelType >( vesselnessMeasure ) );
        }
      }
    ++it;
    ++oit;
    }
    
}

template < typename TPixel >
void
HessianSmoothed3DToVesselnessMeasureImageFilter< TPixel >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "Alpha: " << m_Alpha << std::endl;
  os << indent << "Beta:  " << m_Beta  << std::endl;
  os << indent << "Gamma: " << m_Gamma << std::endl;

  os << indent << "C: " << m_C << std::endl;
}


} // end namespace itk
  
#endif
