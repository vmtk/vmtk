/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkVesselnessMeasureImageFilter.txx,v $
Language:  C++
Date:      $Date: 2005/06/20 13:36:54 $
Version:   $Revision: 1.1 $

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

#ifndef _itkVesselnessMeasureImageFilter_txx
#define _itkVesselnessMeasureImageFilter_txx
#include "itkVesselnessMeasureImageFilter.h"

#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
VesselnessMeasureImageFilter<TInputImage,TOutputImage>
::VesselnessMeasureImageFilter()
{
  m_Sigma = 1.0;
  m_NormalizeAcrossScale = false;
  m_Alpha1 = 0.5;
  m_Alpha2 = 2.0;
}


template< typename TInputImage, typename TOutputImage >
void
VesselnessMeasureImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
//  this->AllocateOutputs();
        
  typedef itk::HessianRecursiveGaussianImageFilter<InputImageType> HessianFilterType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<OutputPixelType> VesselnessMeasureFilterType;
  
  typename HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  hessianFilter->SetInput(this->GetInput());
  hessianFilter->SetSigma(m_Sigma);
  hessianFilter->SetNormalizeAcrossScale(m_NormalizeAcrossScale);
  hessianFilter->Update();
  
  typename VesselnessMeasureFilterType::Pointer vesselnessFilter = VesselnessMeasureFilterType::New();
  vesselnessFilter->SetInput(hessianFilter->GetOutput());
  vesselnessFilter->SetAlpha1(m_Alpha1);
  vesselnessFilter->SetAlpha2(m_Alpha2);
  vesselnessFilter->Update();
  
  this->GraftOutput(vesselnessFilter->GetOutput());
}

template <typename TInputImage, typename TOutputImage>
void
VesselnessMeasureImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end namespace itk

#endif
