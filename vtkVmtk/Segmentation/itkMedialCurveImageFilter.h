/*=========================================================================

  Module    : EvoLib
  File      : $RCSfile: itkMedialCurveImageFilter.h,v $
  Copyright : (C)opyright (University of Zaragoza) 2005++
              See COPYRIGHT statement in top level directory.
  Authors   : Monica Hernandez && Xavier Mellado
  Modified  : $Author: xmellado $
  Purpose   : Computation of the Medial curve of a tube like object.
  Date      : $Date: 2007/02/19 10:01:15 $
  Version   : $Revision: 1.2 $
  Changes   : $Locker:  $
              $Log: itkMedialCurveImageFilter.h,v $
              Revision 1.2  2007/02/19 10:01:15  xmellado
              Release v1.0


=========================================================================*/

#ifndef _itkMedialCurveImageFilter_h
#define _itkMedialCurveImageFilter_h

#include "time.h"

//STL
#include <functional>
#include <queue>

#include "itkImage.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstantBoundaryCondition.h"

#include "itkImageToImageFilter.h"

namespace itk
{

/// \brief Computation of the Medial curve of a tube like object.
///
/// The object is represented as a binary image. A topological prunning is performed guided
/// by the distance transform to the object. The prunning stops when all voxels that are end points
/// are not simple. The average outward flux imposes an extra criterion for deletion. This class implements the centerline extraction 
///	algorithm described in : "S. Bouix, K. Siddiqi, and A. Tannenbaum. Flux driven automatic centerline extraction. Technical Report
///	SOCS-04.2, School of Ccomputer Science, McGill University, 2004."

/// TODO:
/// 1. manual instantation
template< class TInputImage,
	  class TAverageOutwardFluxPixelType = float,
	  class TOutputPixelType = unsigned char>
class ITK_EXPORT MedialCurveImageFilter:
	public ImageToImageFilter<TInputImage, ::itk::Image<TOutputPixelType,TInputImage::ImageDimension> >
{
	public:

		//-----------------------------------------------------
		// Typedefs
		//-----------------------------------------------------

		typedef typename itk::Image< TAverageOutwardFluxPixelType, TInputImage::ImageDimension > TAverageOutwardFluxFImage;
		typedef typename itk::Image< TOutputPixelType, TInputImage::ImageDimension > TOutputImage;

		/** Standard class typedefs. */
		typedef MedialCurveImageFilter Self;
		typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		/** Method for creation through the object factory */
		itkNewMacro(Self);

		/** Run-time type information (and related methods). */
		itkTypeMacro( MedialCurveImageFilter, ImageToImageFilter );

		typedef itk::ConstantBoundaryCondition< TOutputImage > OutputBCType;

		typedef typename TInputImage::ConstPointer InputConstPointerType;
		typedef typename TAverageOutwardFluxFImage::ConstPointer AOFConstPointerType;
		typedef typename TOutputImage::Pointer OutputPointerType;

		typedef typename TOutputImage::IndexType OutputIndexType;
		typedef typename TOutputImage::SizeType OutputSizeType;
		typedef typename TOutputImage::RegionType OutputRegionType;
		typedef typename TOutputImage::PixelType OutputPixelType;

		typedef typename TInputImage::IndexType InputIndexType;
		typedef typename TInputImage::PixelType InputPixelType;

		typedef itk::ImageRegionConstIterator< TInputImage > InputConstIteratorType;
		typedef itk::ImageRegionIterator< TOutputImage > OutputIteratorType;
		typedef itk::ConstNeighborhoodIterator< TInputImage > InputConstNeighborhoodIteratorType;
		typedef itk::ConstNeighborhoodIterator< TAverageOutwardFluxFImage > AOFConstNeighborhoodIteratorType;
		typedef itk::NeighborhoodIterator< TOutputImage > OutputNeighborhoodIteratorType;

		struct Pixel 
		{
			private:
				InputIndexType pixelIndex;
				InputPixelType pixelValue;

			public:
				InputPixelType GetPriority() const {return pixelValue;};
				void SetIndex(InputIndexType p) { pixelIndex=p;};
				void SetValue(InputPixelType v) {pixelValue=v;};
				InputIndexType GetIndex() {return pixelIndex;};
				InputPixelType GetValue() {return pixelValue;};
		};

		struct Greater:public std::binary_function<Pixel, Pixel, bool>
		{
			public:
				bool operator()(const Pixel &p1, const Pixel &p2)  const
				{
					return p1.GetPriority() > p2.GetPriority();
				}
		};

		typedef std::vector<Pixel> HeapContainer;
		typedef std::priority_queue<Pixel, HeapContainer, Greater > HeapType;

		/** The dimension of the input and output images. */
		itkStaticConstMacro(InputImageDimension, unsigned int,
			TInputImage::ImageDimension);
		itkStaticConstMacro(OutputImageDimension, unsigned int,
			TOutputImage::ImageDimension);


		//-----------------------------------------------------
		// Methods
		//-----------------------------------------------------

		virtual void SetAverageOutwardFluxImage(AOFConstPointerType aofImage)
		{
			this->ProcessObject::SetNthInput( 1, const_cast< TAverageOutwardFluxFImage * >(aofImage.GetPointer()) );
		}

		virtual AOFConstPointerType GetAverageOutwardFluxImage()
		{
			return ( static_cast< TAverageOutwardFluxFImage *>(this->ProcessObject::GetInput(1)) );
		}

		/** Set the AOF threshold. */
		itkSetMacro( Threshold, double );

		/** Get the AOF threshold . */
		itkGetConstReferenceMacro( Threshold, double );

#ifdef ITK_USE_CONCEPT_CHECKING
		/** Begin concept checking */
		itkConceptMacro(SameDimensionCheck,
			(Concept::SameDimension<InputImageDimension, OutputImageDimension>));
		itkConceptMacro(AOFIsFloatingPointCheck,
			(Concept::IsFloatingPoint<TAverageOutwardFluxPixelType>));
		/** End concept checking */
#endif

	protected:
		
		/// \brief Default Constructor
		MedialCurveImageFilter();

		/// \brief Destructor
		virtual ~MedialCurveImageFilter();

		///\brief Returns true if p belongs to the object and at least one of its 27 neighbors
		/// belong to the background.
		bool IsBoundary( OutputIndexType p );
    
		///\brief Returns true if its deletion from the object changes the local object topology
		/// in the 27 neighborhood.
		bool IsIntSimple( OutputIndexType p );

		///\brief Returns true if its deletion from the object changes the local background topology
		/// in the 18 neighborhood. 
		bool IsExtSimple( OutputIndexType p );

		///\brief Returns true if the point has less than two object neighbors.
		bool IsEnd( OutputIndexType p );

		///\brief Computes the binary object from its implicit representation.
		void DistanceToObject();

		/** Compute the medial curve. */
		void GenerateData() ITK_OVERRIDE;  

		void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE;


		//-----------------------------------------------------
		// Variables
		//-----------------------------------------------------
		
		InputConstPointerType distance; // Implicit representation of the object. Is at input 0.
		AOFConstPointerType aof;        // Average outward flux. Is at input 1.
		double m_Threshold;             // Threshold for the average outward flux.
		OutputPointerType queued;       // Image that stores queued labels in Update().
		OutputPointerType auxQueued;    // Image that stores queued labels in IsIntSimple() and IsExtSimple().
		OutputPointerType skeleton;     // Skeleton.
		OutputRegionType region;


	private:

		MedialCurveImageFilter( const MedialCurveImageFilter& );  //purposely not implemented
		void operator = ( const MedialCurveImageFilter& );  //purposely not implemented
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMedialCurveImageFilter.txx"
#endif

#endif