/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkObjectnessMeasureImageFilter.txx,v $
Language:  C++
Date:      $Date: 2005/07/05 10:37:46 $
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

#ifndef _itkObjectnessMeasureImageFilter_txx
#define _itkObjectnessMeasureImageFilter_txx
#include "itkObjectnessMeasureImageFilter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
ObjectnessMeasureImageFilter<TInputImage,TOutputImage>
::ObjectnessMeasureImageFilter()
{
  m_Sigma = 1.0;
  m_NormalizeAcrossScale = false;
  
  m_AlphaArray.Fill(0.0);
  m_EnhancementFlagArray.Fill(HessianToObjectnessMeasureFilterType::Ignore);
  m_EigenValueSignConstraintArray.Fill(HessianToObjectnessMeasureFilterType::NoConstraint);
}


template< typename TInputImage, typename TOutputImage >
void
ObjectnessMeasureImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
//  this->AllocateOutputs();
  
  typename HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  hessianFilter->SetInput(this->GetInput());
  hessianFilter->SetSigma(m_Sigma);
  hessianFilter->SetNormalizeAcrossScale(m_NormalizeAcrossScale);
  hessianFilter->Update();
  
  typename HessianToObjectnessMeasureFilterType::Pointer objectnessFilter = HessianToObjectnessMeasureFilterType::New();
  objectnessFilter->SetInput(hessianFilter->GetOutput());
  objectnessFilter->SetAlphaArray(m_AlphaArray);
  objectnessFilter->SetEnhancementFlagArray(m_EnhancementFlagArray);
  objectnessFilter->SetEigenValueSignConstraintArray(m_EigenValueSignConstraintArray);
  objectnessFilter->Update();

  this->GraftOutput(objectnessFilter->GetOutput());
}

template <typename TInputImage, typename TOutputImage>
void
ObjectnessMeasureImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif
