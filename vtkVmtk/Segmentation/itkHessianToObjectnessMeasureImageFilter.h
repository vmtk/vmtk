/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessianToObjectnessMeasureImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/07/05 10:37:46 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHessianToObjectnessMeasureImageFilter_h
#define __itkHessianToObjectnessMeasureImageFilter_h

#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"

namespace itk
{
/** \class HessianToObjectnessMeasureImageFilter
 * \brief 
 * 
 * \sa HessianRecursiveGaussianImageFilter 
 * \sa SymmetricEigenAnalysisImageFilter
 * \sa SymmetricSecondRankTensor
 * 
 * \ingroup IntensityImageFilters TensorObjects
 *
 */
  
template <typename TInputImage = Image< SymmetricSecondRankTensor< double, 3>, 3>, 
          typename TOutputImage = Image< double, 3> >
class ITK_EXPORT HessianToObjectnessMeasureImageFilter : public
ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef HessianToObjectnessMeasureImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  
  typedef TInputImage            InputImageType;
  typedef TOutputImage           OutputImageType;
  typedef typename InputImageType::PixelType             InputPixelType;
  typedef typename OutputImageType::PixelType            OutputPixelType;
  
  /** Image dimension */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      ::itk::GetImageDimension<InputImageType>::ImageDimension);

  typedef   double EigenValueType;
  typedef   itk::FixedArray< EigenValueType, InputPixelType::Dimension > EigenValueArrayType;
  typedef itk::Image< EigenValueArrayType, InputImageType::ImageDimension > EigenValueImageType;
  typedef SymmetricEigenAnalysisImageFilter< InputImageType, EigenValueImageType > EigenAnalysisFilterType;

  typedef enum
  {
    Ignore,
    Enhance,
    Suppress
  }EnhancementFlagType;
 
  typedef enum
  {
    NoConstraint,
    Positive,
    Negative
  }EigenValueSignConstraintType;
 
  typedef   itk::FixedArray< EnhancementFlagType, InputPixelType::Dimension >
                                                          EnhancementFlagArrayType;
 
  typedef   itk::FixedArray< EigenValueSignConstraintType, InputPixelType::Dimension >
                                                          EigenValueSignConstraintArrayType;
 
  typedef   itk::FixedArray< double, InputPixelType::Dimension >
                                                          AlphaArrayType;

  typedef   itk::FixedArray< double, InputPixelType::Dimension >
                                                          EigenValueRatioArrayType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  void SetEnhancementFlagArray(const EnhancementFlagArrayType& enhancementFlagArray)
  {
    m_EnhancementFlagArray = enhancementFlagArray;
    this->Modified();
  }
 
  const EnhancementFlagArrayType& GetEnhancementFlagArray() const
  {
    return m_EnhancementFlagArray;
  }
 
  void SetEigenValueSignConstraintArray(const EigenValueSignConstraintArrayType& eigenValueSignConstraintArray)
  {
    m_EigenValueSignConstraintArray = eigenValueSignConstraintArray;
    this->Modified();
  }
 
  const EigenValueSignConstraintArrayType& GetEigenValueSignConstraintArray() const
  {
    return m_EigenValueSignConstraintArray;
  }

  void SetAlphaArray(const AlphaArrayType& alphaArray)
  {
    m_AlphaArray = alphaArray;
    this->Modified();
  }
 
  const AlphaArrayType& GetAlphaArray() const
  {
    return m_AlphaArray;
  }

  void SetEnhancementFlag(unsigned int i, EnhancementFlagType enhancementFlag)
  {
    m_EnhancementFlagArray[i] = enhancementFlag;
    this->Modified();
  }

  void SetEnhancementFlagToEnhance(unsigned int i)
  {
    m_EnhancementFlagArray[i] = Enhance;
    this->Modified();
  }
  
  void SetEnhancementFlagToSuppress(unsigned int i)
  {
    m_EnhancementFlagArray[i] = Suppress;
    this->Modified();
  }
  
  void SetEnhancementFlagToIgnore(unsigned int i)
  {
    m_EnhancementFlagArray[i] = Ignore;
    this->Modified();
  }

  EnhancementFlagType GetEnhancementFlag(unsigned int i)
  {
    return m_EnhancementFlagArray[i];
  }
 
  void SetEigenValueSignConstraint(unsigned int i, EigenValueSignConstraintType eigenValueSignConstraint)
  {
    m_EigenValueSignConstraint[i] = eigenValueSignConstraint;
    this->Modified();
  }
  
  void SetEigenValueSignConstraintToNoConstraint(unsigned int i)
  {
    m_EigenValueSignConstraint[i] = NoConstraint;
    this->Modified();
  }
  
  void SetEigenValueSignConstraintToPositive(unsigned int i)
  {
    m_EigenValueSignConstraint[i] = Positive;
    this->Modified();
  }
  
  void SetEigenValueSignConstraintToNegative(unsigned int i)
  {
    m_EigenValueSignConstraint[i] = Negative;
    this->Modified();
  }

  EigenValueSignConstraintType GetEigenValueSignConstraint(unsigned int i)
  {
    return m_EigenValueSignConstraintArray[i];
  }

  void SetAlpha(unsigned int i, double alpha)
  {
    m_AlphaArray[i] = alpha;
    this->Modified();
  }

  double GetAlpha(unsigned int i)
  {
    return m_AlphaArray[i];
  }
 
protected:
  HessianToObjectnessMeasureImageFilter();
  ~HessianToObjectnessMeasureImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

private:
  HessianToObjectnessMeasureImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename EigenAnalysisFilterType::Pointer         m_SymmetricEigenValueFilter;

  EnhancementFlagArrayType m_EnhancementFlagArray;
  EigenValueSignConstraintArrayType m_EigenValueSignConstraintArray;
  AlphaArrayType m_AlphaArray;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHessianToObjectnessMeasureImageFilter.txx"
#endif
  
#endif
