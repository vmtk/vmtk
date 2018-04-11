/*=========================================================================

  Module    : EvoLib
  File      : $RCSfile: itkAverageOutwardFluxImageFilter.cpp,v $
  Copyright : (C)opyright (University of Zaragoza) 2005++
              See COPYRIGHT statement in top level directory.
  Authors   : Monica Hernandez && Xavier Mellado
  Modified  : $Author: ilarrabide $
  Purpose   : AverageOutwardFluxImageFilter implementation.
  Date      : $Date: 2007/06/01 14:52:56 $
  Version   : $Revision: 1.3 $
  Changes   : $Locker:  $
              $Log: AverageOutwardFluxImageFilter.cpp,v $
              Revision 1.3  2007/06/01 14:52:56  ilarrabide
              Medial curve computation.

              Revision 1.2  2007/02/19 10:01:15  xmellado
              Release v1.0


=========================================================================*/

#ifndef _itkAverageOutwardFluxImageFilter_h
#define _itkAverageOutwardFluxImageFilter_h

#include "itkAverageOutwardFluxImageFilter.h"
#include "time.h"

#include "itkImage.h"
#include "itkNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"

#include "itkImageToImageFilter.h"
#include <vector>

namespace itk 
{
//--------------------------------------------------
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::AverageOutwardFluxImageFilter()
//--------------------------------------------------
{
}

//--------------------------------------------------
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::~AverageOutwardFluxImageFilter()
//--------------------------------------------------
{
}

//--------------------------------------------------
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::GenerateData()
//--------------------------------------------------
{
	try
	{
		this->distance = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );
		this->gradient = dynamic_cast<const TInputVectorImage  *>( ProcessObject::GetInput(1) );

		this->aof = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0) );

		// Initialize output image
		this->aof->SetSpacing( this->gradient->GetSpacing() );
		this->aof->SetOrigin( this->gradient->GetOrigin() );
		this->aof->SetRegions( this->gradient->GetRequestedRegion() );
		this->aof->Allocate();

		// aof and neighborhood iterators
		InputConstIteratorType dit = InputConstIteratorType( this->distance, this->distance->GetRequestedRegion() );
		OutputIteratorType aofit = OutputIteratorType( this->aof, this->aof->GetRequestedRegion() );

		typename InputVectorConstNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		InputVectorConstNeighborhoodIteratorType nit( radius, this->gradient, this->gradient->GetRequestedRegion() );

		// Computation of the average outward flux inside the object
		double f = 0, norm = 0;
		vector<double> n(TInputImage::ImageDimension);
		
		for ( nit.GoToBegin(), dit.GoToBegin(), aofit.GoToBegin(); !nit.IsAtEnd(); ++nit, ++dit, ++aofit )
		{
			f = 0.0;
			for ( int i = 0; i < pow((float)3.0,(int)n.size()); i++ )
			{
				if ( nit.GetIndex() != nit.GetIndex(i) )
				{
					norm = 0.0;
					for ( unsigned char d = 0; d < n.size(); d++ ) {
						// Compute the normal of the sphere centered in p in the i-th 27 neighborhood
						n[d] = nit.GetIndex(i)[d] - nit.GetIndex()[d];
					}
					for ( unsigned char d = 0; d < n.size(); d++ ) {
						norm += n[d] * n[d];
					}
					norm = sqrt( norm );

					for ( unsigned char d = 0; d < n.size(); d++ ) {
						n[d] = n[d] / norm;
					}

					// Average formula
					for ( unsigned char d = 0; d < n.size(); d++ ) {
						f -= nit.GetPixel(i)[d]*n[d];
 					}

				}
			}

			if ( dit.Get() > 0.0 )
				aofit.Set(0.0);
			else
				aofit.Set(f);
		}
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "AverageOutwardFluxImageFilter::GenerateData() - Unexpected error!" << std::endl );
	}
}

/**
*  Print Self
*/
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::PrintSelf(std::ostream& os, Indent indent) const
{
	Superclass::PrintSelf(os,indent);
	os << indent << "Average Outward Flux." << std::endl;
}
}//end namespace itk
#endif