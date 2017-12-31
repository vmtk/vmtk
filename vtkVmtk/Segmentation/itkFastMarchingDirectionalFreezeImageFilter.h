/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFastMarchingDirectionalFreezeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/09/01 21:33:26 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _itkFastMarchingDirectionalFreezeImageFilter_h
#define _itkFastMarchingDirectionalFreezeImageFilter_h

#include "itkFastMarchingUpwindGradientImageFilter.h"
#include "itkImage.h"

namespace itk
{
/** \class FastMarchingDirectionalFreezeImageFilter
 *
 * \brief Propagates a front freezing points based on the relative direction of 
 * the front and the gradient vector of the speed image.
 * 
 * 
 */
template <
  class TLevelSet, 
  class TSpeedImage = Image<float,TLevelSet::ImageDimension> >
class ITK_EXPORT FastMarchingDirectionalFreezeImageFilter :
    public FastMarchingUpwindGradientImageFilter<TLevelSet,TSpeedImage>
{

public:
  /** Standard class typdedefs. */
  typedef FastMarchingDirectionalFreezeImageFilter Self;
  typedef FastMarchingUpwindGradientImageFilter<TLevelSet,TSpeedImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FastMarchingDirectionalFreezeImageFilter, FastMarchingUpwindGradientImageFilter);

  /** Inherited typedefs. */
  typedef typename Superclass::LevelSetType  LevelSetType;
  typedef typename Superclass::SpeedImageType SpeedImageType;
  typedef typename Superclass::LevelSetImageType  LevelSetImageType;
  typedef typename Superclass::LevelSetPointer  LevelSetPointer;
  typedef typename Superclass::SpeedImageConstPointer  SpeedImageConstPointer;
  typedef typename Superclass::LabelImageType  LabelImageType;
  typedef typename Superclass::PixelType  PixelType;
  typedef typename Superclass::AxisNodeType  AxisNodeType;
  typedef typename Superclass::NodeType  NodeType;
  typedef typename Superclass::NodeContainer  NodeContainer;
  typedef typename Superclass::NodeContainerPointer  NodeContainerPointer;
  typedef typename Superclass::IndexType  IndexType;
  typedef typename Superclass::OutputSpacingType  OutputSpacingType;
  typedef typename Superclass::LevelSetIndexType  LevelSetIndexType;
  typedef typename Superclass::GradientImageType  GradientImageType;
  typedef typename Superclass::GradientImagePointer  GradientImagePointer;
  typedef typename Superclass::GradientPixelType  GradientPixelType;

  typedef typename SpeedImageType::PixelType SpeedPixelType;

  /** The dimension of the level set. */
  itkStaticConstMacro(SetDimension, unsigned int,Superclass::SetDimension); 

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  /** End concept checking */
#endif

  /** SpeedGradientPixel typedef support. */
  typedef CovariantVector<SpeedPixelType,
          itkGetStaticConstMacro(SetDimension)> SpeedGradientPixelType;

  /** SpeedGradientImage typedef support. */
  typedef Image<SpeedGradientPixelType,
          itkGetStaticConstMacro(SetDimension)> SpeedGradientImageType;

  /** SpeedGradientImagePointer typedef support. */
  typedef typename SpeedGradientImageType::Pointer SpeedGradientImagePointer;

  /** Get the speed gradient image. */
  SpeedGradientImagePointer GetSpeedGradientImage() const
  { return m_SpeedGradientImage; };

  //TODO: Set the speed gradient image from outside 

protected:
  FastMarchingDirectionalFreezeImageFilter();
  ~FastMarchingDirectionalFreezeImageFilter(){};
  void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;

  void GenerateData() ITK_OVERRIDE;

  void AllocateSpeedGradientImage();
  void ComputeSpeedGradientImage();

  GradientPixelType ComputeGradientValue( const IndexType& index ,
                                const LevelSetImageType * output);

  virtual void UpdateNeighbors( const IndexType& index,
                                const SpeedImageType *, LevelSetImageType * ) ITK_OVERRIDE;

private:
  FastMarchingDirectionalFreezeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
 
  SpeedGradientImagePointer m_SpeedGradientImage; 
};

} // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastMarchingDirectionalFreezeImageFilter.txx"
#endif

#endif
