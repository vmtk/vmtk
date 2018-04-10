/*=========================================================================

  Module    : EvoLib
  File      : $RCSfile: itkAverageOutwardFluxImageFilter.h,v $
  Copyright : (C)opyright (University of Zaragoza) 2005++
              See COPYRIGHT statement in top level directory.
  Authors   : Monica Hernandez && Xavier Mellado
  Modified  : $Author: ilarrabide $
  Purpose   : itkAverageOutwardFluxImageFilter definition.
  Date      : $Date: 2007/06/01 14:52:56 $
  Version   : $Revision: 1.3 $
  Changes   : $Locker:  $
              $Log: itkAverageOutwardFluxImageFilter.h,v $
              Revision 1.3  2007/06/01 14:52:56  ilarrabide
              Medial curve computation.

              Revision 1.2  2007/02/19 10:01:15  xmellado
              Release v1.0


=========================================================================*/

#ifndef _itkAverageOutwardFluxImageFilter_h
#define _itkAverageOutwardFluxImageFilter_h

#include "time.h"

#include "itkImage.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"

#include "itkImageToImageFilter.h"
#include "vector"

using namespace std;

namespace itk
{

/// \brief This class computes the average outward flux associated to an object implicitly 
/// represented as a distance transform. This class implements the centerline extraction 
///	algorithm described in : "S. Bouix, K. Siddiqi, and A. Tannenbaum. Flux driven automatic centerline extraction. Technical Report
///	SOCS-04.2, School of Ccomputer Science, McGill University, 2004."

/// TODO:
/// 1. manual instantation
template< class TInputImage,
	  class TOutputPixelType = float,
	  class TInputVectorPixelType = ::itk::CovariantVector<TOutputPixelType,TInputImage::ImageDimension> > 
class ITK_EXPORT AverageOutwardFluxImageFilter:
	public ImageToImageFilter<TInputImage, ::itk::Image<TOutputPixelType,TInputImage::ImageDimension> >
{
	public:

		//-----------------------------------------------------
		// Typedefs
		//-----------------------------------------------------

		typedef typename itk::Image< TInputVectorPixelType, TInputImage::ImageDimension > TInputVectorImage;
		typedef typename itk::Image< TOutputPixelType, TInputImage::ImageDimension > TOutputImage;

		/** Standard class typedefs. */
		typedef AverageOutwardFluxImageFilter    Self;
		typedef ImageToImageFilter<TInputImage, TOutputImage> Superclass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		/** Method for creation through the object factory */
		itkNewMacro(Self);

		/** Run-time type information (and related methods). */
		itkTypeMacro( AverageOutwardFluxImageFilter, ImageToImageFilter );

		typedef typename TInputImage::ConstPointer InputConstPointerType;
		typedef typename TInputVectorImage::ConstPointer InputVectorConstPointerType;
		typedef typename TOutputImage::Pointer OutputPointerType;

		typedef itk::ImageRegionConstIterator< TInputImage > InputConstIteratorType;
		typedef itk::ConstNeighborhoodIterator< TInputVectorImage > InputVectorConstNeighborhoodIteratorType;
		typedef itk::ImageRegionIterator< TOutputImage > OutputIteratorType;

// #ifdef ITK_USE_CONCEPT_CHECKING
// 		/** Begin concept checking */
// 		itkConceptMacro(SameDimensionCheck,
// 			(Concept::SameDimension<InputImageDimension, OutputImageDimension>));
// 		itkConceptMacro(InputVectorIsReallyAVectorCheck,
// 			(Concept::HasValueType<TInputVectorPixelType>));
// 		itkConceptMacro(InputVectorIsFloatingPointCheck,
// 			(Concept::IsFloatingPoint<TInputVectorPixelType::ValueType>));
// 		//itkConceptMacro(InputVectorIsSameDimensionInputImageCheck,
// 		//	(Concept::SameDimension<TInputVectorPixelType::SizeType,InputImageDimension>));
// 		itkConceptMacro(OutputIsFloatingPointCheck,
// 			(Concept::IsFloatingPoint<TOutputPixelType>));
// 		/** End concept checking */
// #endif

		//-----------------------------------------------------
		// Methods
		//-----------------------------------------------------

		virtual void SetGradientImage(InputVectorConstPointerType gradient)
		{
			this->ProcessObject::SetNthInput( 1, const_cast< TInputVectorImage * >(gradient.GetPointer()) );
		}

		virtual InputVectorConstPointerType GetGradientImage()
		{
			return ( static_cast< TInputVectorImage *>(this->ProcessObject::GetInput(1)) );
		}
		
		/// \brief Compute the gradient.
		void GenerateData();

		void PrintSelf(std::ostream& os, Indent indent) const;

	protected:
		
		/// \brief Default Constructor
		AverageOutwardFluxImageFilter();

		/// \brief Destructor
		virtual ~AverageOutwardFluxImageFilter();


		//-----------------------------------------------------
		// Variables
		//-----------------------------------------------------

		InputConstPointerType distance;
		InputVectorConstPointerType gradient;
		OutputPointerType aof;

	private:

		AverageOutwardFluxImageFilter( const AverageOutwardFluxImageFilter& );  //purposely not implemented
		void operator = ( const AverageOutwardFluxImageFilter& );  //purposely not implemented
};

#include "itkAverageOutwardFluxImageFilter.txx"

} // end namespace itk
#endif // _itkAverageOutwardFluxImageFilter_h
