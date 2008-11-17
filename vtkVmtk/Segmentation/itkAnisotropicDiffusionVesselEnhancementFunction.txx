/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnisotropicDiffusionVesselEnhancementFunction.txx,v $
Language:  C++
Date:      $Date: 2007/06/20 16:03:23 $
Version:   $Revision: 1.14 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnisotropicDiffusionVesselEnhancementFunction_txx
#define __itkAnisotropicDiffusionVesselEnhancementFunction_txx

#include "itkAnisotropicDiffusionVesselEnhancementFunction.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace itk {

template< class TImageType >
AnisotropicDiffusionVesselEnhancementFunction< TImageType>
::AnisotropicDiffusionVesselEnhancementFunction()
{
  RadiusType r;

  for( unsigned int i=0 ; i < ImageDimension ; i++ )
    {
    r[i] = 1;
    } 

  this->SetRadius(r);
  
  // Dummy neighborhood.
  NeighborhoodType it;
  it.SetRadius( r );
  
  // Find the center index of the neighborhood.
  m_Center =  it.Size() / 2;

  // Get the stride length for each axis.
  for(unsigned int i = 0; i < ImageDimension; i++)
    {  m_xStride[i] = it.GetStride(i); }
}

template< class TImageType >
typename AnisotropicDiffusionVesselEnhancementFunction< TImageType >
::TimeStepType
AnisotropicDiffusionVesselEnhancementFunction<TImageType>
::ComputeGlobalTimeStep(void *GlobalData) const
{
  /* returns the time step supplied by the user. We don't need
     to use the global data supplied since we are returning a fixed value
  */

  return this->GetTimeStep();
}
 
template< class TImageType >
typename AnisotropicDiffusionVesselEnhancementFunction< TImageType >::PixelType
AnisotropicDiffusionVesselEnhancementFunction< TImageType >
::ComputeUpdate(const NeighborhoodType &it, void *globalData,
                const FloatOffsetType& offset)
{
  double value = 0.0;
  
  return (PixelType) (value);   
}

template< class TImageType >
typename AnisotropicDiffusionVesselEnhancementFunction< TImageType >::PixelType
AnisotropicDiffusionVesselEnhancementFunction< TImageType >
::ComputeUpdate(const NeighborhoodType &it, 
                const DiffusionTensorNeighborhoodType &gt,
                void *globalData,
                const FloatOffsetType& offset)
{
  unsigned int i, j;  
//  const ScalarValueType ZERO = NumericTraits<ScalarValueType>::Zero;
  const ScalarValueType center_value  = it.GetCenterPixel();

  // Global data structure
  GlobalDataStruct *gd = (GlobalDataStruct *)globalData;


  // m_dx -> Intensity first derivative 
  // m_dxy -> Intensity second derivative
  // m_DT_dxy -> Diffusion tensor first derivative

  // Compute the first and 2nd derivative 
  gd->m_GradMagSqr = 1.0e-6;
  for( i = 0; i < ImageDimension; i++)
    {
    const unsigned int positionA = 
      static_cast<unsigned int>( m_Center + m_xStride[i]);
    const unsigned int positionB = 
      static_cast<unsigned int>( m_Center - m_xStride[i]);

    gd->m_dx[i] = 0.5 * (it.GetPixel( positionA ) - 
                     it.GetPixel( positionB )    );

    gd->m_dxy[i][i] = it.GetPixel( positionA )
      + it.GetPixel( positionB ) - 2.0 * center_value;
    
    for( j = i+1; j < ImageDimension; j++ )
      {
      const unsigned int positionAa = static_cast<unsigned int>( 
        m_Center - m_xStride[i] - m_xStride[j] );
      const unsigned int positionBa = static_cast<unsigned int>( 
        m_Center - m_xStride[i] + m_xStride[j] );
      const unsigned int positionCa = static_cast<unsigned int>( 
        m_Center + m_xStride[i] - m_xStride[j] );
      const unsigned int positionDa = static_cast<unsigned int>( 
        m_Center + m_xStride[i] + m_xStride[j] );

      gd->m_dxy[i][j] = gd->m_dxy[j][i] = 0.25 *( it.GetPixel( positionAa )
                                          - it.GetPixel( positionBa )
                                          - it.GetPixel( positionCa )
                                          + it.GetPixel( positionDa )
        );
      }
    }

  // Compute the diffusion tensor matrix first derivatives 
  TensorPixelType center_Tensor_value  = gt.GetCenterPixel();

  for( i = 0; i < ImageDimension; i++)
    {
    const unsigned int positionA = 
      static_cast<unsigned int>( m_Center + m_xStride[i]);
    const unsigned int positionB = 
      static_cast<unsigned int>( m_Center - m_xStride[i]);
    
    TensorPixelType positionA_Tensor_value = gt.GetPixel( positionA );
    TensorPixelType positionB_Tensor_value = gt.GetPixel( positionB );

    for( j = 0; j < ImageDimension; j++)
      { 
      gd->m_DT_dxy[i][j] = 0.5 *  ( positionA_Tensor_value(i,j) - 
                                positionB_Tensor_value(i,j) ); 
      }
    }

  ScalarValueType   pdWrtDiffusion1;

  pdWrtDiffusion1 = gd->m_DT_dxy[0][0] * gd->m_dx[0]  
                    + gd->m_DT_dxy[0][1] * gd->m_dx[1] 
                    + gd->m_DT_dxy[0][2] * gd->m_dx[2];

  ScalarValueType   pdWrtDiffusion2;

  pdWrtDiffusion2 = gd->m_DT_dxy[1][0] * gd->m_dx[0]  
                    + gd->m_DT_dxy[1][1] * gd->m_dx[1] 
                    + gd->m_DT_dxy[1][2] * gd->m_dx[2];

  ScalarValueType  pdWrtDiffusion3;

  pdWrtDiffusion3 = gd->m_DT_dxy[2][0] * gd->m_dx[0]  
                    + gd->m_DT_dxy[2][1] * gd->m_dx[1] 
                    + gd->m_DT_dxy[2][2] * gd->m_dx[2];

  ScalarValueType   pdWrtImageIntensity1;

  pdWrtImageIntensity1 = center_Tensor_value(0,0) *  gd->m_dxy[0][0]  + 
                    center_Tensor_value(0,1) *  gd->m_dxy[0][1] +
                    center_Tensor_value(0,2) *  gd->m_dxy[0][2];
  
  ScalarValueType   pdWrtImageIntensity2;

  pdWrtImageIntensity2 = center_Tensor_value(1,0) *  gd->m_dxy[1][0]  + 
                    center_Tensor_value(1,1) *  gd->m_dxy[1][1] +
                    center_Tensor_value(1,2) *  gd->m_dxy[1][2];
 
  ScalarValueType   pdWrtImageIntensity3;

  pdWrtImageIntensity3 = center_Tensor_value(2,0) *  gd->m_dxy[2][0]  + 
                    center_Tensor_value(2,1) *  gd->m_dxy[2][1] +
                    center_Tensor_value(2,2) *  gd->m_dxy[2][2];
 
  ScalarValueType   total;

  total = pdWrtDiffusion1 + pdWrtDiffusion2 + pdWrtDiffusion3 +
         pdWrtImageIntensity1 + pdWrtImageIntensity2 + pdWrtImageIntensity3;

  return ( PixelType ) ( total );
} 

template <class TImageType>
void
AnisotropicDiffusionVesselEnhancementFunction<TImageType>::
PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace itk

#endif
