/*=========================================================================

  File      : $RCSfile: itkMedialCurveImageFilter.cpp,v $
  Copyright : (C)opyright (University of Zaragoza) 2005++
              See COPYRIGHT statement in top level directory.
  Authors   : Monica Hernandez && Xavier Mellado
  Modified  : $Author: ilarrabide $
  Purpose   : Computation of the Medial curve of a tube like object.
  Date      : $Date: 2007/05/18 15:38:40 $
  Version   : $Revision: 1.3 $
  Changes   : $Locker:  $
              $Log: itkMedialCurveImageFilter.cpp,v $
              Revision 1.3  2007/05/18 15:38:40  ilarrabide
              Trivial error in itkMedialCurveImageFilter corrected.

              Revision 1.2  2007/02/19 10:01:15  xmellado
              Release v1.0


=========================================================================*/

namespace itk
{
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
itk::MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::MedialCurveImageFilter()
//--------------------------------------------------
{
}

//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::~MedialCurveImageFilter()
//--------------------------------------------------
{
}

//An object pixel belongs to the boundary if its 27* neighborhood contains at least one foreground pixel.
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
bool MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsBoundary(OutputIndexType p )
  //--------------------------------------------------
{
	try
	{
		bool IsBoundary = false;
		typename OutputNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		OutputNeighborhoodIteratorType nit( radius, this->skeleton, this->skeleton->GetRequestedRegion() );

		OutputBCType cbc;
		nit.OverrideBoundaryCondition(&cbc);

		nit.SetLocation(p);

		if ( nit.GetCenterPixel() == 1 )
		{
			for( int i = 0; i < nit.Size() && !IsBoundary; i++ )
			{
				if ( nit.GetPixel( i ) == 0 )
				{
					IsBoundary = true;
				}
			}
		}

		return IsBoundary;
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::IsBoundary() - Unexpected error!" << std::endl );
	}
}

//An object pixel is simple for the object's topology if its deletion from the object does not change the
//topology in the pixel's neighborhood. The algorithm proceeds finding the first 26 connected point to
//the pixel in the object. A flood fill algorithm goes through the adjacent object pixels different
//from p using the 26 connectivity. Finally, if the number of flooded points equals to the number of
//object pixels-1 the local topology does not change and, therefore, the pixel is simple for the object.
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
bool MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsIntSimple( OutputIndexType p )
//--------------------------------------------------
{
	try
	{
		// Compute new index of the auxiliar queued image
		OutputIndexType start;

		for ( int i = 0; i < TOutputImage::ImageDimension; i++ )
		{
			start[i] = p[i]-1;
		}
		this->region.SetIndex( start );
		this->auxQueued->SetRegions( region );

		typename OutputNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		OutputNeighborhoodIteratorType nit( radius, this->skeleton, this->skeleton->GetRequestedRegion());
		radius.Fill(0);
		OutputNeighborhoodIteratorType qnit( radius, this->auxQueued, this->auxQueued->GetRequestedRegion());

		OutputBCType cbc;
		nit.OverrideBoundaryCondition(&cbc);

		nit.SetLocation(p);

		int in = 0;
		for ( int i = 0; i < nit.Size(); i++ )
		{
			if ( nit.GetPixel( i ) == 1 )
				in++;
		}

		if( in == 1 || in == pow((float)3,(int)TInputImage::ImageDimension) )
		{ // p is isolated or an interior point and therefore, not simple
			return false;
		}
		else
		{
			//Flood-fill

			typename TOutputImage::PixelType priority=0;
			HeapType heap;
			Pixel node;

			OutputIteratorType aux( this->auxQueued , this->auxQueued->GetRequestedRegion() );

			for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
					aux.Set( 0 );

			qnit.SetLocation( nit.GetIndex() );
			qnit.SetCenterPixel( 1 );

			OutputIndexType q;

			for( int i = 0; i < nit.Size(); i++ )
			{
				if ( nit.GetPixel( i ) == 1 )
				{
					//object pixel
					qnit.SetLocation( nit.GetIndex( i ) );
					if( qnit.GetCenterPixel() == 0 )

					{
						// not queued
						node.SetIndex( nit.GetIndex( i ) );
						node.SetValue( priority++ );
						heap.push( node );

						qnit.SetCenterPixel( 1 );
						i = nit.Size();
					}
				}
			}

			//Just inserted the first found object pixel in N26*

			while( !heap.empty() )
			{
				node = heap.top();
				heap.pop();

				nit.SetLocation( node.GetIndex() );

				for( int i = 0; i < nit.Size(); i++ )
				{
					if( nit.GetPixel( i ) == 1 )
					{
						//object pixel
						q = nit.GetIndex( i );
						qnit.SetLocation( q );
						bool cond = true;
						for (unsigned int d = 0; d < TInputImage::ImageDimension && cond; d++) 
						{
							cond = cond && (abs( p[d] - q[d] ) <= 1);
						}
						if ( ( cond ) && qnit.GetCenterPixel() == 0 )
						{
							// 26* neighborhood - not queued
							node.SetIndex( q );
							node.SetValue( priority++ );
							heap.push( node );
							qnit.SetCenterPixel( 1 );
						}
					}
				}
			}

			if ( priority == in-1 )
				return true;
			else
				return false;
		}
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::IsIntSimple() - Unexpected error!" << std::endl );
	}
}

//An object pixel is simple for the background's topology if its deletion from the object does not
//change the background's topology in the pixel's neighborhood. The algorithm proceeds finding
//the first pixel in the background. A flood fill algorithm goes through the adjacent background pixels
//different from p using the 6 connectivity in the 18 neighborhood. Finally, if the number of flooded
//points equals to the number of 6 connected background pixels-1 the local topology of the background
//does not change and, therefore, the pixel is simple for the background.
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType> 
bool MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsExtSimple( OutputIndexType p )
//--------------------------------------------------
{
	try
	{
		// Compute new index of the auxiliar queued image
		OutputIndexType start;

		for ( int i = 0; i < TOutputImage::ImageDimension; i++ )
		{
			start[i] = p[i]-1;
		}
		this->region.SetIndex( start );

		this->auxQueued->SetRegions( region );


		typename OutputNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		OutputNeighborhoodIteratorType nit(radius, this->skeleton, this->skeleton->GetRequestedRegion());
		radius.Fill(0);
		OutputNeighborhoodIteratorType qnit(radius, this->auxQueued, this->auxQueued->GetRequestedRegion());

		OutputBCType cbc;
		nit.OverrideBoundaryCondition(&cbc);

		nit.SetLocation(p);

		int out = 0;

		OutputIndexType q,r;
		OutputPixelType priority=0;
		HeapType heap;

		Pixel node;

		OutputIteratorType aux( this->auxQueued , this->auxQueued->GetRequestedRegion() );

		for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
				aux.Set( 0 );

		out = 0;
		node.SetIndex( p );
		node.SetValue( out++ );
		heap.push( node );

		qnit.SetLocation( p );
		qnit.SetCenterPixel( 1 );

		while ( !heap.empty() )
		{
			node = heap.top();
			heap.pop();
			r = node.GetIndex();
			nit.SetLocation( r );

			for( int i = 0; i < nit.Size(); i++ )
			{
				if ( nit.GetPixel( i ) == 0 )
				{
					q = nit.GetIndex( i );
					qnit.SetLocation( q );
					bool cond1 = true;
					bool cond2 = false;
					for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++) 
					{
						cond1 = cond1 && (abs( p[d] - q[d] ) <= 1);
					}
					for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++) 
					{
						cond2 = cond2 || (p[d] - q[d] == 0);
					}

					if ( ( cond1 ) && ( cond2 ) )
					{
						bool cond3 = false;
						for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++) 
						{
							cond3 = cond3 || ( r[d] - q[d] == 0 && r[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
						}
						//18 neighbor of p
						if ( cond3 )
						{
							//6 connected to r
							if ( qnit.GetCenterPixel() == 0 )
							{
								node.SetIndex( q );
								node.SetValue( out++ );
								heap.push( node );
								qnit.SetCenterPixel( 1 );
							}
						}
					}
				}
			}
		}

		if ( out == 0 || out == 18 )
			return false;
		else
		{

			//Flood-fill

			nit.SetLocation(p);

			for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
					aux.Set( 0 );

			qnit.SetLocation( p );
			qnit.SetCenterPixel( 1 );

			for ( int i = 0; i < nit.Size(); i++ )
			{
				if ( nit.GetPixel( i ) == 0 )
				{
					//background pixel
					q = nit.GetIndex( i );
					qnit.SetLocation( q );
					bool cond1 = true;
					bool cond2 = false;
					for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++) 
					{
						cond1 = cond1 && (abs( p[d] - q[d] ) <= 1);
					}
					for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++) 
					{
						cond2 = cond2 || (p[d] - q[d] == 0);
					}

					if ( ( cond1 ) && ( cond2 ) )
					{
						bool cond3 = false;
						for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++) 
						{
							cond3 = cond3 || ( p[d] - q[d] == 0 && p[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
						}
						//18 neighbor of p
						if ( cond3 )
						{
							//6 connected to p
							if ( qnit.GetCenterPixel() == 0 )
							{
								// not queued
								node.SetIndex( q );
								node.SetValue( priority++ );
								heap.push( node );

								qnit.SetCenterPixel( 1 );
								i = nit.Size();
							}
						}
					}
				}
			}

			//Just inserted the first found background pixel in N18
	    
			while( !heap.empty() )
			{
				node = heap.top();
				heap.pop();
				r = node.GetIndex();
				nit.SetLocation( r );

				for( int i = 0; i < nit.Size(); i++ )
				{
					if ( nit.GetPixel( i ) == 0 )
					{
						q = nit.GetIndex( i );
						qnit.SetLocation( q );
						bool cond1 = true;
						bool cond2 = false;
						for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++) 
						{
							cond1 = cond1 && (abs( p[d] - q[d] ) <= 1);
						}
						for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++) 
						{
							cond2 = cond2 || (p[d] - q[d] == 0);
						}

						if ( ( cond1 ) && ( cond2 ) )
						{
						bool cond3 = false;
						for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++) 
						{
							cond3 = cond3 || ( r[d] - q[d] == 0 && r[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
						}
						//18 neighbor of r
						if ( cond3 )
							{
								//6 connected
								if ( qnit.GetCenterPixel() == 0 )
								{
									node.SetIndex( q );
									node.SetValue( priority++ );
									heap.push( node );
									qnit.SetCenterPixel( 1 );
								}
							}
						}
					}
				}
			}

			if( priority == out-1 )
				return true;
			else
				return false;
		}
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::IsExtSimple() - Unexpected error!" << std::endl );
	}
}

//p is a medial axis end point if in 26* there is only one foreground points
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
bool MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsEnd( OutputIndexType p )
//--------------------------------------------------
{
	try
	{
		typename OutputNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		OutputNeighborhoodIteratorType nit(radius, this->skeleton, this->skeleton->GetRequestedRegion());

		OutputBCType cbc;
		nit.OverrideBoundaryCondition(&cbc);

		nit.SetLocation(p);

		int n=0;

		for( int i = 0; i < nit.Size(); i++ )
		{
			if ( nit.GetIndex() != nit.GetIndex( i ) && nit.GetPixel( i ) == 1 ) //Belonging to the object - 26* connected
				n++;
		}

		if ( n < 2 )
			return true;
		else
			return false;
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::IsEnd() - Unexpected error!" << std::endl );
	}
}

//Computation o the binary image representing the object from its implicit representation.
//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
void MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::DistanceToObject()
//--------------------------------------------------
{
	try
	{
		InputConstIteratorType dit(this->distance, this->distance->GetRequestedRegion());
		OutputIteratorType skit(this->skeleton, this->skeleton->GetRequestedRegion());

		for ( dit.GoToBegin(), skit.GoToBegin(); !dit.IsAtEnd(), !skit.IsAtEnd(); ++dit, ++skit )
		{
			if ( dit.Get() <= 0.0 )
				skit.Set( 1 );
			else
				skit.Set( 0 );
		}
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::DistanceToObject() - Unexpected error!" << std::endl );
	}
}

//--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
void MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::GenerateData()
//--------------------------------------------------
{
	try
	{
		this->distance = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );
		this->aof = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(1) );

		this->skeleton = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0) );

		// Create the region of the auxuliar queued image
		OutputSizeType size;
		OutputIndexType start;

		for ( int i = 0; i < TOutputImage::ImageDimension; i++ )
		{
			start[i]=0;
			size[i]=3;
		}
		this->region.SetIndex( start );
		this->region.SetSize( size );

		this->skeleton->SetSpacing( this->distance->GetSpacing() );
		this->skeleton->SetOrigin( this->distance->GetOrigin() );
		this->skeleton->SetRegions( this->distance->GetRequestedRegion() );
		this->skeleton->Allocate();

		this->queued = TOutputImage::New();
		this->queued->SetSpacing( this->distance->GetSpacing() );
		this->queued->SetOrigin( this->distance->GetOrigin() );
		this->queued->SetRegions( this->distance->GetRequestedRegion() );
		this->queued->Allocate();

		this->auxQueued = TOutputImage::New();
		this->auxQueued->SetSpacing( this->distance->GetSpacing() );
		this->auxQueued->SetOrigin( this->distance->GetOrigin() );
		this->auxQueued->SetRegions( region );
		this->auxQueued->Allocate();

		// Initialization of binary object
		this->DistanceToObject();

		// Topological prunning

		//Iterators
		InputConstIteratorType dit( this->distance, this->distance->GetRequestedRegion() );
		OutputIteratorType skit( this->skeleton, this->skeleton->GetRequestedRegion() );
		OutputIteratorType qit( this->queued, this->queued->GetRequestedRegion() );

		typename OutputNeighborhoodIteratorType::RadiusType radius;
		radius.Fill(1);
		InputConstNeighborhoodIteratorType dnit( radius, this->distance, this->distance->GetRequestedRegion() );
		radius.Fill(0);
		AOFConstNeighborhoodIteratorType aofnit( radius, this->aof, this->aof->GetRequestedRegion() );
		radius.Fill(1);
		OutputNeighborhoodIteratorType sknit( radius, this->skeleton, this->skeleton->GetRequestedRegion() );
		radius.Fill(1);
		OutputNeighborhoodIteratorType qnit( radius, this->queued, this->queued->GetRequestedRegion() );

		//First step...
		InputIndexType q;
		InputPixelType priority=0;
		HeapType heap;
		Pixel node;

		for ( skit.GoToBegin(), dit.GoToBegin(), qit.GoToBegin(); !skit.IsAtEnd() && !dit.IsAtEnd() && !qit.IsAtEnd(); ++skit, ++dit, ++qit )
		{
			q = skit.GetIndex();
			if ( this->IsBoundary( q ) )
			{
				if ( IsIntSimple( q ) && IsExtSimple( q ) )
				{
					//Simple pixel
					node.SetIndex( q );
					priority = dit.Get();
   					node.SetValue( -priority );
 					heap.push( node );
					qit.Set( 1 );
				}
				else qit.Set( 0 );
			}
			else qit.Set( 0 );
		}

		//Second step 

		InputIndexType r;

		while ( !heap.empty() )
		{

		node=heap.top();
			heap.pop();

			q = node.GetIndex();

			qnit.SetLocation( q );
			qnit.SetCenterPixel( 0 );

			if ( IsIntSimple( q ) && IsExtSimple( q ) )
			{
				aofnit.SetLocation(q);
				if ( ( aofnit.GetCenterPixel() < this->m_Threshold ) && ( IsEnd( q ) ) )
				{
					//std::cout<<q<<" Is medial "<<aofnit.GetCenterPixel()<<std::endl;
				}
				else
				{
					//std::cout<<q<<" Is deleted"<<std::endl;
					sknit.SetLocation( q );
					sknit.SetCenterPixel( 0 ); //Deletion from object
					dnit.SetLocation( q );

					// Este iterador de nh se sale fuera de la imagen, pero como tenemos la
					// boundary condition a cero, al pedir por un pixel de fuera nos
					// devuelve 0 y no haremos nada
					// En queued por lo tanto tampoco escribiremos fuera de rango
					for ( int i = 0; i < sknit.Size(); i++ )
					{
						if ( sknit.GetPixel( i ) == 1 )
						{
							//Object pixel
							r = sknit.GetIndex( i );
							if ( qnit.GetPixel( i ) == 0 )
							{
								//Not queued pixel
								if ( IsIntSimple( r ) && IsExtSimple( r ) )
								{
									priority = dnit.GetPixel( i );
									node.SetIndex( r );
									node.SetValue( -priority );
									heap.push( node );
									qnit.SetPixel( i, 1 ); 
								}
							}
						}
					}
				}
			}
		}
	}
	catch ( itk::ExceptionObject &err )
	{
		throw err;
	}
	catch (...)
	{
		itkExceptionMacro( << "MedialCurveImageFilter::GenerateData() - Unexpected error!" << std::endl );
	}
}

/**
 *  Print Self
 */
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
void MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Medial Curve." << std::endl;
  os << indent << "Threshold         : " << m_Threshold << std::endl;
}
}//end namespace itk
