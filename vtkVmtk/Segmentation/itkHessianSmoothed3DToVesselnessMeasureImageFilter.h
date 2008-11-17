/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessianSmoothed3DToVesselnessMeasureImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2007/06/12 22:59:15 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHessianSmoothed3DToVesselnessMeasureImageFilter_h
#define __itkHessianSmoothed3DToVesselnessMeasureImageFilter_h

#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"

namespace itk
{
/** \class HessianSmoothed3DToVesselnessMeasureImageFilter
 * \brief A filter to enhance 3D vascular structures 
 * 
 * The vesselness measure is based on the analysis of the the Hessian 
 * eigen system. The vesseleness function is a smoothed (continuous) 
 * version of the Frang's vesselness function. The filter takes an 
 * image of a Hessian pixels ( SymmetricSecondRankTensor pixels ) and
 * produces an enhanced image. The Hessian input image can be produced using 
 * itkHessianSmoothedRecursiveGaussianImageFilter. 
 *  
 *
 * \par References
 *  Manniesing, R, Viergever, MA, & Niessen, WJ (2006). Vessel Enhancing 
 *  Diffusion: A Scale Space Representation of Vessel Structures. Medical 
 *  Image Analysis, 10(6), 815-825. 
 * 
 * \sa MultiScaleHessianSmoothed3DToVesselnessMeasureImageFilter 
 * \sa Hessian3DToVesselnessMeasureImageFilter
 * \sa HessianSmoothedRecursiveGaussianImageFilter 
 * \sa SymmetricEigenAnalysisImageFilter
 * \sa SymmetricSecondRankTensor
 * 
 * \ingroup IntensityImageFilters TensorObjects
 *
 */
  
template < typename  TPixel >
class ITK_EXPORT HessianSmoothed3DToVesselnessMeasureImageFilter : public
ImageToImageFilter< Image< SymmetricSecondRankTensor< double, 3 >, 3 >, 
                                                  Image< TPixel, 3 > >
{
public:
  /** Standard class typedefs. */
  typedef HessianSmoothed3DToVesselnessMeasureImageFilter Self;

  typedef ImageToImageFilter< 
          Image< SymmetricSecondRankTensor< double, 3 >, 3 >, 
          Image< TPixel, 3 > >                 Superclass;

  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>             ConstPointer;
  
  typedef typename Superclass::InputImageType            InputImageType;
  typedef typename Superclass::OutputImageType           OutputImageType;
  typedef typename InputImageType::PixelType             InputPixelType;
  typedef TPixel                                         OutputPixelType;
  
  /** Image dimension = 3. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                    ::itk::GetImageDimension<InputImageType>::ImageDimension);

  itkStaticConstMacro(InputPixelDimension, unsigned int,
                    InputPixelType::Dimension);

  typedef  FixedArray< double, itkGetStaticConstMacro(InputPixelDimension) >
                                                          EigenValueArrayType;
  typedef  Image< EigenValueArrayType, itkGetStaticConstMacro(ImageDimension) >
                                                          EigenValueImageType;
  typedef   SymmetricEigenAnalysisImageFilter< 
            InputImageType, EigenValueImageType >     EigenAnalysisFilterType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Set/Get macros for Alpha */
  itkSetMacro(Alpha, double);
  itkGetMacro(Alpha, double);
  
  /** Set/Get macros for Beta */
  itkSetMacro(Beta, double);
  itkGetMacro(Beta, double);

  /** Set/Get macros for Gamma */
  itkSetMacro(Gamma, double);
  itkGetMacro(Gamma, double);

  /** Set/Get macros for C */
  itkSetMacro(C, double);
  itkGetMacro(C, double);
 
  /** Macro to scale the vesselness measure with the 
      largest eigenvalue or not */
  itkSetMacro( ScaleVesselnessMeasure, bool );
  itkGetMacro( ScaleVesselnessMeasure, bool );
  itkBooleanMacro(ScaleVesselnessMeasure);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(DoubleConvertibleToOutputCheck,
                  (Concept::Convertible<double, OutputPixelType>));
  /** End concept checking */
#endif

protected:
  HessianSmoothed3DToVesselnessMeasureImageFilter();
  ~HessianSmoothed3DToVesselnessMeasureImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

private:

  //purposely not implemented
  HessianSmoothed3DToVesselnessMeasureImageFilter(const Self&); 

  void operator=(const Self&); //purposely not implemented

  typename EigenAnalysisFilterType::Pointer     m_SymmetricEigenValueFilter;

  double                                        m_Alpha;
  double                                        m_Beta;
  double                                        m_Gamma;
 
  double                                        m_C;

  bool                                          m_ScaleVesselnessMeasure;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHessianSmoothed3DToVesselnessMeasureImageFilter.txx"
#endif
  
#endif
