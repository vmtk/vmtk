/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkFWHMFeatureImageFilter.h,v $
Language:  C++
Date:      $Date: 2005/03/04 11:14:37 $
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

#ifndef __itkFWHMFeatureImageFilter_h
#define __itkFWHMFeatureImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkBinaryBallStructuringElement.h"

namespace itk
{
/** \class FWHMFeatureImageFilter
 * \brief Builds a feature image which allows to perform FWHM level sets segmentation.
 *
 */
template <typename TInputImage, typename TOutputImage>
class FWHMFeatureImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef FWHMFeatureImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(FWHMFeatureImageFilter, ImageToImageFilter);
  
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

  /** StructuringElement typedefs. */
  typedef itk::BinaryBallStructuringElement<InputPixelType,3> StructuringElementType;

  /** StructuringElementRadius typedefs. */
  typedef typename StructuringElementType::RadiusType StructuringElementRadiusType;

  /** Use the image spacing information in calculations. 
      Default is UseImageSpacingOn. */
  void SetUseImageSpacingOn()
  { this->SetUseImageSpacing(true); }
  
  /** Ignore the image spacing. Use this option if you want calculations to be 
      performed in an isotropic pixel space.  Default is UseImageSpacingOn. */
  void SetUseImageSpacingOff()
  { this->SetUseImageSpacing(false); }
  
  /** Set/Get whether or not the filter will use the spacing of the input
      image in its calculations */
  itkSetMacro(UseImageSpacing, bool);
  itkGetMacro(UseImageSpacing, bool);

  /** Set/Get the structuring element radius */
  itkSetMacro(Radius, StructuringElementRadiusType);
  itkGetMacro(Radius, StructuringElementRadiusType);

  /** Set/Get the background value used to determine the half maximum */
  itkSetMacro(BackgroundValue, InputPixelType);
  itkGetMacro(BackgroundValue, InputPixelType);

protected:
  FWHMFeatureImageFilter();
  virtual ~FWHMFeatureImageFilter() {}

  void GenerateData() ITK_OVERRIDE;

  void PrintSelf(std::ostream&, Indent) const ITK_OVERRIDE;
  
private:
  FWHMFeatureImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_UseImageSpacing;
  StructuringElementRadiusType m_Radius;
  InputPixelType m_BackgroundValue;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFWHMFeatureImageFilter.txx"
#endif

#endif
