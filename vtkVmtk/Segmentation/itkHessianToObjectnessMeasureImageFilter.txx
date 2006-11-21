/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessianToObjectnessMeasureImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/05 10:37:46 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkHessianToObjectnessMeasureImageFilter_txx
#define _itkHessianToObjectnessMeasureImageFilter_txx

#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
HessianToObjectnessMeasureImageFilter< TInputImage, TOutputImage >
::HessianToObjectnessMeasureImageFilter()
{
  m_AlphaArray.Fill(0.0);
  m_EnhancementFlagArray.Fill(Ignore);
  m_EigenValueSignConstraintArray.Fill(NoConstraint);
  
  // Hessian( Image ) = Jacobian( Gradient ( Image ) )  is symmetric
  m_SymmetricEigenValueFilter = EigenAnalysisFilterType::New();
  m_SymmetricEigenValueFilter->SetDimension( ImageDimension );
  m_SymmetricEigenValueFilter->OrderEigenValuesBy( 
  EigenAnalysisFilterType::FunctorType::OrderByValue );
  
}

template <typename TInputImage, typename TOutputImage >
void 
HessianToObjectnessMeasureImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
  itkDebugMacro(<< "HessianToObjectnessMeasureImageFilter generating data ");

  m_SymmetricEigenValueFilter->SetInput( this->GetInput() );
  
  typename OutputImageType::Pointer output = this->GetOutput();

  typedef typename EigenAnalysisFilterType::OutputImageType
                                                    EigenValueImageType;

  m_SymmetricEigenValueFilter->Update();
  
  const typename EigenValueImageType::ConstPointer eigenImage = 
                    m_SymmetricEigenValueFilter->GetOutput();
  
  // walk the region of eigen values and get the objectness measure
  EigenValueArrayType eigenValues;
  ImageRegionConstIterator<EigenValueImageType> it;
  it = ImageRegionConstIterator<EigenValueImageType>(eigenImage, eigenImage->GetRequestedRegion());
  ImageRegionIterator<OutputImageType> oit;
  this->AllocateOutputs();
  oit = ImageRegionIterator<OutputImageType>(output,output->GetRequestedRegion());
  oit.GoToBegin();
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    // Get the eigen values
    eigenValues = it.Get();
  
    EigenValueArrayType sortedEigenValues = eigenValues;
    bool done = false;
    while (!done)
    {
      done = true;
      for (unsigned int i=0; i<InputPixelType::Dimension-1; i++)
      {
        if (vnl_math_abs(sortedEigenValues[i]) > vnl_math_abs(sortedEigenValues[i+1]))
        {
          EigenValueType temp = sortedEigenValues[i+1];
          sortedEigenValues[i+1] = sortedEigenValues[i];
          sortedEigenValues[i] = temp;
          done = false;
        }
      }
    }
    
    EigenValueRatioArrayType eigenValueRatioArray;
    eigenValueRatioArray.Fill(0.0);
    
    for (unsigned int i=0; i<InputPixelType::Dimension; i++)
    {
      if (i==0)
      {
        eigenValueRatioArray[i] = 0.0;
        for (unsigned int j=0; j<InputPixelType::Dimension; j++)
        {
          eigenValueRatioArray[i] += vnl_math_squared_magnitude(sortedEigenValues[j]);
        }
        eigenValueRatioArray[i] = vnl_math_sqr(eigenValueRatioArray[i]);
      }
      else
      {
        eigenValueRatioArray[i] = vnl_math_abs(sortedEigenValues[i]);
        double product = 1.0;
        for (unsigned int j=i+1; j<InputPixelType::Dimension; j++)
        {
          product *= vnl_math_abs(sortedEigenValues[j]);
        }
        eigenValueRatioArray[i] /= pow(product,1.0/(InputPixelType::Dimension-i));
      }
    }
    
    bool constraintsSatisfied= true;
    for (unsigned int i=0; i<InputPixelType::Dimension; i++)
    {
      if ((m_EigenValueSignConstraintArray[i] == Positive && sortedEigenValues[i] < 0.0) ||
          (m_EigenValueSignConstraintArray[i] == Negative && sortedEigenValues[i] > 0.0) )
      {
        constraintsSatisfied = false;
        break;
      }
    }

    if (!constraintsSatisfied)
    {
      oit.Set( NumericTraits< OutputPixelType >::Zero);
    }
    else
    {
      double objectnessMeasure = 1.0;
      bool anyEnhanced = false;
      for (unsigned int i=0; i<InputPixelType::Dimension; i++)
      {
        if (m_EnhancementFlagArray[i] == Ignore)
        {
          continue;
        }
        anyEnhanced = true;
        double factor = exp(- 0.5 * vnl_math_squared_magnitude(eigenValueRatioArray[i]) / vnl_math_squared_magnitude(m_AlphaArray[i]));
        
        if (m_EnhancementFlagArray[i] == Enhance)
        {
          objectnessMeasure *= 1.0 - factor;
        }
        else if (m_EnhancementFlagArray[i] == Suppress)
        {
          objectnessMeasure *= factor;
        }
      }

      if (!anyEnhanced)
      {
        objectnessMeasure = 0.0;
      }
            
// VESSELNESS
//      double objectnessMeasure = (1.0 - exp(-rA*rA/(2.0*m_Alpha1*m_Alpha1))) * exp(-rB*rB/(2.0*m_Alpha2*m_Alpha2)) * (1.0 - exp(-s*s/(2.0*m_Alpha3*m_Alpha3)));
// BLOBNESS
//      double objectnessMeasure = (exp(-rA*rA/(2.0*m_Alpha1*m_Alpha1))) * (exp(-rB*rB/(2.0*m_Alpha2*m_Alpha2))) * (1.0 - exp(-s*s/(2.0*m_Alpha3*m_Alpha3)));
      oit.Set( static_cast< OutputPixelType >(objectnessMeasure));
    }
    
    ++it;
    ++oit;
    }
    
}

template <typename TInputImage, typename TOutputImage >
void
HessianToObjectnessMeasureImageFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "AlphaArray: " << m_AlphaArray << std::endl;
  os << indent << "EnhancementFlags: " << m_EnhancementFlagArray << std::endl;
  os << indent << "EigenValueSignConstraints: " << m_EigenValueSignConstraintArray << std::endl;
}


} // end namespace itk
  
#endif
