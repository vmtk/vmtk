/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkObjectnessMeasureImageFilter.h,v $
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

#ifndef __itkObjectnessMeasureImageFilter_h
#define __itkObjectnessMeasureImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkHessianToObjectnessMeasureImageFilter.h"

namespace itk
{
/** \class ObjectnessMeasureImageFilter
 * \brief Wraps Hessian and vesselness computations together.
 *
 */
template <typename TInputImage, typename TOutputImage>
class ObjectnessMeasureImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef ObjectnessMeasureImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(ObjectnessMeasureImageFilter, ImageToImageFilter);
  
  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TOutputImage::PixelType OutputPixelType;
  typedef typename TInputImage::PixelType InputPixelType;
  typedef typename NumericTraits<InputPixelType>::RealType RealType;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  
  /** Image typedef support */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef typename OutputImageType::Pointer OutputImagePointer;
  
  /** Superclass typedefs. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  /** Hessian sigma. */
  itkSetMacro(Sigma, RealType);
  itkGetMacro(Sigma, RealType);
  
  /** Define which normalization factor will be used for the Gaussian */
  itkSetMacro(NormalizeAcrossScale, bool);
  itkGetMacro(NormalizeAcrossScale, bool);

  typedef HessianRecursiveGaussianImageFilter<InputImageType> HessianFilterType;
  typedef HessianToObjectnessMeasureImageFilter<typename HessianFilterType::OutputImageType,OutputImageType> HessianToObjectnessMeasureFilterType;

  typedef typename HessianToObjectnessMeasureFilterType::EnhancementFlagType EnhancementFlagType;
  typedef typename HessianToObjectnessMeasureFilterType::EigenValueSignConstraintType EigenValueSignConstraintType;
  
  typedef typename HessianToObjectnessMeasureFilterType::EnhancementFlagArrayType EnhancementFlagArrayType;
  typedef typename HessianToObjectnessMeasureFilterType::EigenValueSignConstraintArrayType EigenValueSignConstraintArrayType;
  typedef typename HessianToObjectnessMeasureFilterType::AlphaArrayType AlphaArrayType;
  typedef typename HessianToObjectnessMeasureFilterType::EigenValueRatioArrayType EigenValueRatioArrayType;

  void SetEnhancementFlag(unsigned int i, EnhancementFlagType enhancementFlag)
  {
    m_EnhancementFlagArray[i] = enhancementFlag;
    this->Modified();
  }

  void SetEnhancementFlagToEnhance(unsigned int i)
  {
    m_EnhancementFlagArray[i] = HessianToObjectnessMeasureFilterType::Enhance;
    this->Modified();
  }
  
  void SetEnhancementFlagToSuppress(unsigned int i)
  {
    m_EnhancementFlagArray[i] = HessianToObjectnessMeasureFilterType::Suppress;
    this->Modified();
  }
  
  void SetEnhancementFlagToIgnore(unsigned int i)
  {
    m_EnhancementFlagArray[i] = HessianToObjectnessMeasureFilterType::Ignore;
    this->Modified();
  }

  EnhancementFlagType GetEnhancementFlag(unsigned int i)
  {
    return m_EnhancementFlagArray[i];
  }
 
  void SetEigenValueSignConstraint(unsigned int i, EigenValueSignConstraintType eigenValueSignConstraint)
  {
    m_EigenValueSignConstraintArray[i] = eigenValueSignConstraint;
    this->Modified();
  }

  void SetEigenValueSignConstraintToNoConstraint(unsigned int i)
  {
    m_EigenValueSignConstraintArray[i] = HessianToObjectnessMeasureFilterType::NoConstraint;
    this->Modified();
  }
  
  void SetEigenValueSignConstraintToPositive(unsigned int i)
  {
    m_EigenValueSignConstraintArray[i] = HessianToObjectnessMeasureFilterType::Positive;
    this->Modified();
  }
  
  void SetEigenValueSignConstraintToNegative(unsigned int i)
  {
    m_EigenValueSignConstraintArray[i] = HessianToObjectnessMeasureFilterType::Negative;
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
  ObjectnessMeasureImageFilter();
  virtual ~ObjectnessMeasureImageFilter() {}

  void GenerateData();

  void PrintSelf(std::ostream&, Indent) const;
  
private:
  ObjectnessMeasureImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RealType m_Sigma;
  bool m_NormalizeAcrossScale;

  EnhancementFlagArrayType m_EnhancementFlagArray;
  EigenValueSignConstraintArrayType m_EigenValueSignConstraintArray;
  AlphaArrayType m_AlphaArray;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkObjectnessMeasureImageFilter.txx"
#endif

#endif
