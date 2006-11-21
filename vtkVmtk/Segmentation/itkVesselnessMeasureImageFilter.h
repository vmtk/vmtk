/*=========================================================================

Program:   VMTK
Module:    $RCSfile: itkVesselnessMeasureImageFilter.h,v $
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

#ifndef __itkVesselnessMeasureImageFilter_h
#define __itkVesselnessMeasureImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/** \class VesselnessMeasureImageFilter
 * \brief Wraps Hessian and vesselness computations together.
 *
 */
template <typename TInputImage, typename TOutputImage>
class VesselnessMeasureImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef VesselnessMeasureImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(VesselnessMeasureImageFilter, ImageToImageFilter);
  
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

  itkSetMacro(Alpha1, double);
  itkGetMacro(Alpha1, double);

  itkSetMacro(Alpha2, double);
  itkGetMacro(Alpha2, double);

protected:
  VesselnessMeasureImageFilter();
  virtual ~VesselnessMeasureImageFilter() {}

  void GenerateData();

  void PrintSelf(std::ostream&, Indent) const;
  
private:
  VesselnessMeasureImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RealType m_Sigma;
  bool m_NormalizeAcrossScale;
  double m_Alpha1;
  double m_Alpha2;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVesselnessMeasureImageFilter.txx"
#endif

#endif
