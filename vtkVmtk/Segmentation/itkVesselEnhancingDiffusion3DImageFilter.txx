/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVesselEnhancingDiffusion3DImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009/01/09 11:15:27 $
  Version:   $Revision: 1.2 $

=========================================================================*/
#ifndef __itkVesselEnhancingDiffusion3DImageFilter_txx
#define __itkVesselEnhancingDiffusion3DImageFilter_txx
#include "itkVesselEnhancingDiffusion3DImageFilter.h"

#include "itkCastImageFilter.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkNumericTraits.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include<iostream>

namespace itk
{

// constructor
template <class PixelType, unsigned int Dimension>
VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::VesselEnhancingDiffusion3DImageFilter():
    m_TimeStep(NumericTraits<Precision>::Zero),
    m_Iterations(0),
    m_RecalculateVesselness(0),
    m_Epsilon(0.0),
    m_Omega(0.0),
    m_Sensitivity(0.0),
    m_DarkObjectLightBackground(false)
{
	this->SetNumberOfRequiredInputs(1);
}

// printself for debugging
template <class PixelType, unsigned int Dimension>
void VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::PrintSelf(std::ostream &os, Indent indent) const
{
	Superclass::PrintSelf(os,indent);
	os << indent << "TimeStep 		            : " << m_TimeStep  << std::endl;
	os << indent << "Iterations 		        : " << m_Iterations << std::endl;
	os << indent << "RecalculateVesselness      : " << m_RecalculateVesselness << std::endl;
	os << indent << "Scales 		: ";
	for (unsigned int i=0; i<m_Scales.size(); ++i)
		os << m_Scales[i] << " ";
	os << std::endl;
	os << indent << "Epsilon 		            : " << m_Epsilon << std::endl;
	os << indent << "Omega 			            : " << m_Omega << std::endl;
	os << indent << "Sensitivity 		        : " << m_Sensitivity << std::endl;
  	os << indent << "DarkObjectLightBackground  : " << m_DarkObjectLightBackground << std::endl;
}
// singleiter
template <class PixelType, unsigned int Dimension>
void VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::VED3DSingleIteration(typename ImageType::Pointer ci)
{
    bool rec(false);
    if ( 
            (m_CurrentIteration == 1) || 
            (m_RecalculateVesselness == 0) ||
            (m_CurrentIteration % m_RecalculateVesselness == 0)
       )
    {

        rec = true;
        if (m_Verbose)
        {
            std::cout << "v ";
            std::cout.flush();
            
        }
        MaxVesselResponse (ci);
        DiffusionTensor (); 
    }
    if (m_Verbose)
    {
        if (!rec)
        {
            std::cout << ". ";
            std::cout.flush();
        }
    }


    // calculate d = nonlineardiffusion(ci)
    // using 3x3x3 stencil, afterwards copy
    // result from d back to ci
    typename PrecisionImageType::Pointer d = PrecisionImageType::New();
    d->SetOrigin(ci->GetOrigin());
    d->SetSpacing(ci->GetSpacing());
    d->SetDirection(ci->GetDirection());
    d->SetRegions(ci->GetLargestPossibleRegion());
    d->Allocate();
    d->FillBuffer(NumericTraits<Precision>::Zero);

    // shapedneighborhood iter, zeroflux boundary condition
    // division into faces and inner region
    typedef ZeroFluxNeumannBoundaryCondition<PrecisionImageType>    BT;
    typedef ConstShapedNeighborhoodIterator<PrecisionImageType,BT>  NT;
    typedef typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<PrecisionImageType> 
                                                                    FT; 
    BT                      b;
    typename NT::RadiusType r;
    r.Fill(1);


    // offsets
    const typename NT::OffsetType oxp = {{1,0,0}};
    const typename NT::OffsetType oxm = {{-1,0,0}};
    const typename NT::OffsetType oyp = {{0,1,0}};
    const typename NT::OffsetType oym = {{0,-1,0}};
    const typename NT::OffsetType ozp = {{0,0,1}};
    const typename NT::OffsetType ozm = {{0,0,-1}};

    const typename NT::OffsetType oxpyp = {{1,1,0}};
    const typename NT::OffsetType oxmym = {{-1,-1,0}};
    const typename NT::OffsetType oxpym = {{1,-1,0}};
    const typename NT::OffsetType oxmyp = {{-1,1,0}};

    const typename NT::OffsetType oxpzp = {{1,0,1}};
    const typename NT::OffsetType oxmzm = {{-1,0,-1}};
    const typename NT::OffsetType oxpzm = {{1,0,-1}};
    const typename NT::OffsetType oxmzp = {{-1,0,1}};

    const typename NT::OffsetType oypzp = {{0,1,1}};
    const typename NT::OffsetType oymzm = {{0,-1,-1}};
    const typename NT::OffsetType oypzm = {{0,1,-1}};
    const typename NT::OffsetType oymzp = {{0,-1,1}};

    // fixed weights (timers)
	const typename PrecisionImageType::SpacingType ispacing = ci->GetSpacing();
    const Precision rxx = m_TimeStep / (2.0 * ispacing[0] * ispacing[0]);
    const Precision ryy = m_TimeStep / (2.0 * ispacing[1] * ispacing[1]);
    const Precision rzz = m_TimeStep / (2.0 * ispacing[2] * ispacing[2]);
    const Precision rxy = m_TimeStep / (4.0 * ispacing[0] * ispacing[1]);
    const Precision rxz = m_TimeStep / (4.0 * ispacing[0] * ispacing[2]);
    const Precision ryz = m_TimeStep / (4.0 * ispacing[1] * ispacing[2]);

    // faces
    FT                            fc;
    typename FT::FaceListType     fci = fc(ci,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fxx = fc(m_Dxx,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fxy = fc(m_Dxy,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fxz = fc(m_Dxz,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fyy = fc(m_Dyy,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fyz = fc(m_Dyz,d->GetLargestPossibleRegion(),r);
    typename FT::FaceListType     fzz = fc(m_Dzz,d->GetLargestPossibleRegion(),r);

    typename FT::FaceListType::iterator fitci,fitxx,fitxy,fitxz,fityy,fityz,fitzz;

    for ( fitci = fci.begin(),
            fitxx = fxx.begin(), fitxy = fxy.begin(), fitxz = fxz.begin(),
            fityy = fyy.begin(), fityz = fyz.begin(), fitzz = fzz.begin();
            fitci != fci.end();
            ++fitci, ++fitxx, ++fitxy, ++fitxz, ++fityy, ++fityz, ++fitzz)
    {
        // output iter
        ImageRegionIterator<PrecisionImageType> dit(d,*fitci);

        // input iters
        NT itci (r,ci,*fitci);
        NT itxx (r,m_Dxx,*fitxx);
        NT itxy (r,m_Dxy,*fitxy);
        NT itxz (r,m_Dxz,*fitxz);
        NT ityy (r,m_Dyy,*fityy);
        NT ityz (r,m_Dyz,*fityz);
        NT itzz (r,m_Dzz,*fitzz);

        itci.OverrideBoundaryCondition(&b);
        itxx.OverrideBoundaryCondition(&b);
        itxy.OverrideBoundaryCondition(&b);
        itxz.OverrideBoundaryCondition(&b);
        ityy.OverrideBoundaryCondition(&b);
        ityz.OverrideBoundaryCondition(&b);
        itzz.OverrideBoundaryCondition(&b);

        // setting active offsets (yeah there must
        // be some smarter way of doing this)
        itci.ClearActiveList();
        itxx.ClearActiveList();
        itxy.ClearActiveList();
        itxz.ClearActiveList();
        ityy.ClearActiveList();
        ityz.ClearActiveList();
        itzz.ClearActiveList();

        itci.ActivateOffset(oxp);
        itxx.ActivateOffset(oxp);
        itxy.ActivateOffset(oxp);
        itxz.ActivateOffset(oxp);
        ityy.ActivateOffset(oxp);
        ityz.ActivateOffset(oxp);
        itzz.ActivateOffset(oxp);

        itci.ActivateOffset(oxm);
        itxx.ActivateOffset(oxm);
        itxy.ActivateOffset(oxm);
        itxz.ActivateOffset(oxm);
        ityy.ActivateOffset(oxm);
        ityz.ActivateOffset(oxm);
        itzz.ActivateOffset(oxm);

        itci.ActivateOffset(oyp);
        itxx.ActivateOffset(oyp);
        itxy.ActivateOffset(oyp);
        itxz.ActivateOffset(oyp);
        ityy.ActivateOffset(oyp);
        ityz.ActivateOffset(oyp);
        itzz.ActivateOffset(oyp);

        itci.ActivateOffset(oym);
        itxx.ActivateOffset(oym);
        itxy.ActivateOffset(oym);
        itxz.ActivateOffset(oym);
        ityy.ActivateOffset(oym);
        ityz.ActivateOffset(oym);
        itzz.ActivateOffset(oym);

        itci.ActivateOffset(ozp);
        itxx.ActivateOffset(ozp);
        itxy.ActivateOffset(ozp);
        itxz.ActivateOffset(ozp);
        ityy.ActivateOffset(ozp);
        ityz.ActivateOffset(ozp);
        itzz.ActivateOffset(ozp);

        itci.ActivateOffset(ozm);
        itxx.ActivateOffset(ozm);
        itxy.ActivateOffset(ozm);
        itxz.ActivateOffset(ozm);
        ityy.ActivateOffset(ozm);
        ityz.ActivateOffset(ozm);
        itzz.ActivateOffset(ozm);

        itci.ActivateOffset(oxpyp);
        itxx.ActivateOffset(oxpyp);
        itxy.ActivateOffset(oxpyp);
        itxz.ActivateOffset(oxpyp);
        ityy.ActivateOffset(oxpyp);
        ityz.ActivateOffset(oxpyp);
        itzz.ActivateOffset(oxpyp);

        itci.ActivateOffset(oxmym);
        itxx.ActivateOffset(oxmym);
        itxy.ActivateOffset(oxmym);
        itxz.ActivateOffset(oxmym);
        ityy.ActivateOffset(oxmym);
        ityz.ActivateOffset(oxmym);
        itzz.ActivateOffset(oxmym);

        itci.ActivateOffset(oxpym);
        itxx.ActivateOffset(oxpym);
        itxy.ActivateOffset(oxpym);
        itxz.ActivateOffset(oxpym);
        ityy.ActivateOffset(oxpym);
        ityz.ActivateOffset(oxpym);
        itzz.ActivateOffset(oxpym);

        itci.ActivateOffset(oxmyp);
        itxx.ActivateOffset(oxmyp);
        itxy.ActivateOffset(oxmyp);
        itxz.ActivateOffset(oxmyp);
        ityy.ActivateOffset(oxmyp);
        ityz.ActivateOffset(oxmyp);
        itzz.ActivateOffset(oxmyp);

        itci.ActivateOffset(oxpzp);
        itxx.ActivateOffset(oxpzp);
        itxy.ActivateOffset(oxpzp);
        itxz.ActivateOffset(oxpzp);
        ityy.ActivateOffset(oxpzp);
        ityz.ActivateOffset(oxpzp);
        itzz.ActivateOffset(oxpzp);

        itci.ActivateOffset(oxmzm);
        itxx.ActivateOffset(oxmzm);
        itxy.ActivateOffset(oxmzm);
        itxz.ActivateOffset(oxmzm);
        ityy.ActivateOffset(oxmzm);
        ityz.ActivateOffset(oxmzm);
        itzz.ActivateOffset(oxmzm);

        itci.ActivateOffset(oxpzm);
        itxx.ActivateOffset(oxpzm);
        itxy.ActivateOffset(oxpzm);
        itxz.ActivateOffset(oxpzm);
        ityy.ActivateOffset(oxpzm);
        ityz.ActivateOffset(oxpzm);
        itzz.ActivateOffset(oxpzm);

        itci.ActivateOffset(oxmzp);
        itxx.ActivateOffset(oxmzp);
        itxy.ActivateOffset(oxmzp);
        itxz.ActivateOffset(oxmzp);
        ityy.ActivateOffset(oxmzp);
        ityz.ActivateOffset(oxmzp);
        itzz.ActivateOffset(oxmzp);

        itci.ActivateOffset(oypzp);
        itxx.ActivateOffset(oypzp);
        itxy.ActivateOffset(oypzp);
        itxz.ActivateOffset(oypzp);
        ityy.ActivateOffset(oypzp);
        ityz.ActivateOffset(oypzp);
        itzz.ActivateOffset(oypzp);

        itci.ActivateOffset(oymzm);
        itxx.ActivateOffset(oymzm);
        itxy.ActivateOffset(oymzm);
        itxz.ActivateOffset(oymzm);
        ityy.ActivateOffset(oymzm);
        ityz.ActivateOffset(oymzm);
        itzz.ActivateOffset(oymzm);

        itci.ActivateOffset(oypzm);
        itxx.ActivateOffset(oypzm);
        itxy.ActivateOffset(oypzm);
        itxz.ActivateOffset(oypzm);
        ityy.ActivateOffset(oypzm);
        ityz.ActivateOffset(oypzm);
        itzz.ActivateOffset(oypzm);

        itci.ActivateOffset(oymzp);
        itxx.ActivateOffset(oymzp);
        itxy.ActivateOffset(oymzp);
        itxz.ActivateOffset(oymzp);
        ityy.ActivateOffset(oymzp);
        ityz.ActivateOffset(oymzp);
        itzz.ActivateOffset(oymzp);

        // run for each face diffusion
        for (itci.GoToBegin(), dit.GoToBegin(),
                itxx.GoToBegin(),itxy.GoToBegin(), itxz.GoToBegin(),
                ityy.GoToBegin(),ityz.GoToBegin(), itzz.GoToBegin();
                !itci.IsAtEnd();
                ++itci, ++dit, ++itxx, ++itxy, ++itxz, ++ityy, ++ityz, ++itzz)
        {
            // weights
            const Precision xp = itxx.GetPixel(oxp) + itxx.GetCenterPixel();
            const Precision xm = itxx.GetPixel(oxm) + itxx.GetCenterPixel();
            const Precision yp = ityy.GetPixel(oyp) + ityy.GetCenterPixel();
            const Precision ym = ityy.GetPixel(oym) + ityy.GetCenterPixel();
            const Precision zp = itzz.GetPixel(ozp) + itzz.GetCenterPixel();
            const Precision zm = itzz.GetPixel(ozm) + itzz.GetCenterPixel();

            const Precision xpyp = itxy.GetPixel(oxpyp) + itxy.GetCenterPixel();
            const Precision xmym = itxy.GetPixel(oxmym) + itxy.GetCenterPixel();
            const Precision xpym = - itxy.GetPixel(oxpym) - itxy.GetCenterPixel();
            const Precision xmyp = - itxy.GetPixel(oxmyp) - itxy.GetCenterPixel();

            const Precision xpzp =   itxz.GetPixel(oxpzp) + itxz.GetCenterPixel();
            const Precision xmzm =   itxz.GetPixel(oxmzm) + itxz.GetCenterPixel();
            const Precision xpzm = - itxz.GetPixel(oxpzm) - itxz.GetCenterPixel();
            const Precision xmzp = - itxz.GetPixel(oxmzp) - itxz.GetCenterPixel();

            const Precision ypzp =   ityz.GetPixel(oypzp) + ityz.GetCenterPixel();
            const Precision ymzm =   ityz.GetPixel(oymzm) + ityz.GetCenterPixel();
            const Precision ypzm = - ityz.GetPixel(oypzm) - ityz.GetCenterPixel();
            const Precision ymzp = - ityz.GetPixel(oymzp) - ityz.GetCenterPixel();

            // evolution
            const Precision cv = itci.GetCenterPixel();
            dit.Value() = cv + 
                + rxx * ( xp * (itci.GetPixel(oxp) - cv)
                        + xm * (itci.GetPixel(oxm) - cv) )
                + ryy * ( yp * (itci.GetPixel(oyp) - cv)
                        + ym * (itci.GetPixel(oym) - cv) )
                + rzz * ( zp * (itci.GetPixel(ozp) - cv)
                        + zm * (itci.GetPixel(ozm) - cv) )
                + rxy * ( xpyp * (itci.GetPixel(oxpyp) - cv)
                        + xmym * (itci.GetPixel(oxmym) - cv)
                        + xpym * (itci.GetPixel(oxpym) - cv)
                        + xmyp * (itci.GetPixel(oxmyp) - cv) )
                + rxz * ( xpzp * (itci.GetPixel(oxpzp) - cv)
                        + xmzm * (itci.GetPixel(oxmzm) - cv)
                        + xpzm * (itci.GetPixel(oxpzm) - cv)
                        + xmzp * (itci.GetPixel(oxmzp) - cv) )
                + ryz * ( ypzp * (itci.GetPixel(oypzp) - cv)
                        + ymzm * (itci.GetPixel(oymzm) - cv)
                        + ypzm * (itci.GetPixel(oypzm) - cv)
                        + ymzp * (itci.GetPixel(oymzp) - cv) );

        }
    }

    // copying
    ImageRegionConstIterator<PrecisionImageType> iti (d,d->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType>      ito (ci,ci->GetLargestPossibleRegion());
    for (iti.GoToBegin(), ito.GoToBegin(); !iti.IsAtEnd(); ++iti,++ito)
    {
        ito.Value() = iti.Value();
    }

    return;
} 
// maxvesselresponse
template <class PixelType, unsigned int Dimension>
void VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::MaxVesselResponse(const typename ImageType::Pointer im)	
{

    // alloc memory for hessian/tensor
    m_Dxx = PrecisionImageType::New();
    m_Dxx->SetOrigin(im->GetOrigin());
    m_Dxx->SetSpacing(im->GetSpacing());
    m_Dxx->SetDirection(im->GetDirection());
    m_Dxx->SetRegions(im->GetLargestPossibleRegion());
    m_Dxx->Allocate();
    m_Dxx->FillBuffer(NumericTraits<Precision>::One);

    m_Dxy = PrecisionImageType::New();
    m_Dxy->SetOrigin(im->GetOrigin());
    m_Dxy->SetSpacing(im->GetSpacing());
    m_Dxy->SetDirection(im->GetDirection());
    m_Dxy->SetRegions(im->GetLargestPossibleRegion());
    m_Dxy->Allocate();
    m_Dxy->FillBuffer(NumericTraits<Precision>::Zero);

    m_Dxz = PrecisionImageType::New();
    m_Dxz->SetOrigin(im->GetOrigin());
    m_Dxz->SetSpacing(im->GetSpacing());
    m_Dxz->SetDirection(im->GetDirection());
    m_Dxz->SetRegions(im->GetLargestPossibleRegion());
    m_Dxz->Allocate();
    m_Dxz->FillBuffer(NumericTraits<Precision>::Zero);

    m_Dyy = PrecisionImageType::New();
    m_Dyy->SetOrigin(im->GetOrigin());
    m_Dyy->SetSpacing(im->GetSpacing());
    m_Dyy->SetDirection(im->GetDirection());
    m_Dyy->SetRegions(im->GetLargestPossibleRegion());
    m_Dyy->Allocate();
    m_Dyy->FillBuffer(NumericTraits<Precision>::One);

    m_Dyz = PrecisionImageType::New();
    m_Dyz->SetOrigin(im->GetOrigin());
    m_Dyz->SetSpacing(im->GetSpacing());
    m_Dyz->SetDirection(im->GetDirection());
    m_Dyz->SetRegions(im->GetLargestPossibleRegion());
    m_Dyz->Allocate();
    m_Dyz->FillBuffer(NumericTraits<Precision>::Zero);

    m_Dzz = PrecisionImageType::New();
    m_Dzz->SetOrigin(im->GetOrigin());
    m_Dzz->SetSpacing(im->GetSpacing());
    m_Dzz->SetDirection(im->GetDirection());
    m_Dzz->SetRegions(im->GetLargestPossibleRegion());
    m_Dzz->Allocate();
    m_Dzz->FillBuffer(NumericTraits<Precision>::One);


	// create temp vesselness image to store maxvessel
	typename PrecisionImageType::Pointer vi = PrecisionImageType::New();

	vi->SetOrigin(im->GetOrigin());
	vi->SetSpacing(im->GetSpacing());
	vi->SetDirection(im->GetDirection());
	vi->SetRegions(im->GetLargestPossibleRegion());
	vi->Allocate();
    vi->FillBuffer(NumericTraits<Precision>::Zero);
    

	for (unsigned int i=0; i< m_Scales.size(); ++i)
	{
        typedef HessianRecursiveGaussianImageFilter<PrecisionImageType> HessianType;
        typename HessianType::Pointer hessian = HessianType::New();
        hessian->SetInput(im);
        hessian->SetNormalizeAcrossScale(true);
        hessian->SetSigma(m_Scales[i]);
        hessian->Update();

        ImageRegionIterator<PrecisionImageType> itxx (m_Dxx, m_Dxx->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> itxy (m_Dxy, m_Dxy->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> itxz (m_Dxz, m_Dxz->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> ityy (m_Dyy, m_Dyy->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> ityz (m_Dyz, m_Dyz->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> itzz (m_Dzz, m_Dzz->GetLargestPossibleRegion());
        ImageRegionIterator<PrecisionImageType> vit(vi, vi->GetLargestPossibleRegion());

        ImageRegionConstIterator<typename HessianType::OutputImageType> hit 
            (hessian->GetOutput(), hessian->GetOutput()->GetLargestPossibleRegion());

        for (itxx.GoToBegin(), itxy.GoToBegin(), itxz.GoToBegin(), 
                ityy.GoToBegin(), ityz.GoToBegin(), itzz.GoToBegin(),
                vit.GoToBegin(), hit.GoToBegin(); !vit.IsAtEnd(); 
                ++itxx, ++itxy, ++itxz, ++ityy, ++ityz, ++itzz, ++hit, ++vit)
        {
            vnl_matrix<Precision> H(3,3);

            H(0,0) = hit.Value()(0,0);
            H(0,1) = H(1,0) = hit.Value()(0,1);
            H(0,2) = H(2,0) = hit.Value()(0,2);
            H(1,1) = hit.Value()(1,1);
            H(1,2) = H(2,1) = hit.Value()(1,2);
            H(2,2) = hit.Value()(2,2);

            vnl_symmetric_eigensystem<Precision> ES(H);
            vnl_vector<Precision> ev(3);

            ev[0] = ES.get_eigenvalue(0); 
            ev[1] = ES.get_eigenvalue(1); 
            ev[2] = ES.get_eigenvalue(2);

            if ( vcl_abs(ev[0]) > vcl_abs(ev[1])  ) std::swap(ev[0], ev[1]);
            if ( vcl_abs(ev[1]) > vcl_abs(ev[2])  ) std::swap(ev[1], ev[2]);
            if ( vcl_abs(ev[0]) > vcl_abs(ev[1])  ) std::swap(ev[0], ev[1]);

			const Precision vesselness = VesselnessFunction3D(ev[0],ev[1],ev[2]);

			if ( vesselness > 0 && vesselness > vit.Value() )
			{
				vit.Value() = vesselness;

                itxx.Value() = hit.Value()(0,0);
                itxy.Value() = hit.Value()(0,1);
                itxz.Value() = hit.Value()(0,2);
                ityy.Value() = hit.Value()(1,1);
                ityz.Value() = hit.Value()(1,2);
                itzz.Value() = hit.Value()(2,2);
			}
		} 
	} 
 
    return;

}
// vesselnessfunction
template <class PixelType, unsigned int Dimension>
typename VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>::Precision
VesselEnhancingDiffusion3DImageFilter<PixelType,Dimension>::VesselnessFunction3D
(
	const Precision l1,
	const Precision l2,
	const Precision l3
)
			
{
	Precision vesselness;

    if (  
            (m_DarkObjectLightBackground && ((l2<=0) || (l3<=0))) 
            ||
            (!m_DarkObjectLightBackground && ( (l2>=0) || (l3 >=0)))
       )
    {
            vesselness = NumericTraits<Precision>::Zero;
    }
    else
    {

	    const Precision smoothC=1E-5;

	    const Precision va2= 2.0*m_Alpha*m_Alpha;
	    const Precision vb2= 2.0*m_Beta*m_Beta;
	    const Precision vc2= 2.0*m_Gamma*m_Gamma;

	    const Precision   Ra2 = (l2 * l2) / (l3 * l3);
	    const Precision   Rb2 = (l1 * l1) / vcl_abs(l2 * l3);
	    const Precision   S2 =  (l1 * l1) + (l2 *l2) + (l3 * l3);
	    const Precision   T = vcl_exp(-(2*smoothC*smoothC)/(vcl_abs(l2)*l3*l3));

	    vesselness = T * (1.0 - vcl_exp( - Ra2/va2)) *
		    vcl_exp(-Rb2/vb2) *
		    (1.0 - vcl_exp(-S2/vc2));

    }

return vesselness;
}
// diffusiontensor
template <class PixelType, unsigned int Dimension>
void VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::DiffusionTensor() 
{
    ImageRegionIterator<PrecisionImageType> itxx (m_Dxx, m_Dxx->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType> itxy (m_Dxy, m_Dxy->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType> itxz (m_Dxz, m_Dxz->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType> ityy (m_Dyy, m_Dyy->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType> ityz (m_Dyz, m_Dyz->GetLargestPossibleRegion());
    ImageRegionIterator<PrecisionImageType> itzz (m_Dzz, m_Dzz->GetLargestPossibleRegion());

    for  ( itxx.GoToBegin(), itxy.GoToBegin(), itxz.GoToBegin(),
            ityy.GoToBegin(), ityz.GoToBegin(), itzz.GoToBegin();
            !itxx.IsAtEnd();
            ++itxx, ++itxy, ++itxz, ++ityy, ++ityz, ++itzz)
    {
        vnl_matrix<Precision> H(3,3);
        H(0,0) = itxx.Value();
        H(0,1) = H(1,0) = itxy.Value();
        H(0,2) = H(2,0) = itxz.Value();
        H(1,1) = ityy.Value();
        H(1,2) = H(2,1) = ityz.Value();
        H(2,2) = itzz.Value();

        vnl_symmetric_eigensystem<Precision> ES(H);

        vnl_matrix<Precision> EV(3,3);
        EV.set_column(0,ES.get_eigenvector(0));
        EV.set_column(1,ES.get_eigenvector(1));
        EV.set_column(2,ES.get_eigenvector(2));

        vnl_vector<Precision> ev(3);
        ev[0] = ES.get_eigenvalue(0);
        ev[1] = ES.get_eigenvalue(1);
        ev[2] = ES.get_eigenvalue(2);

        if ( vcl_abs(ev[0]) > vcl_abs(ev[1])  ) std::swap(ev[0], ev[1]);
        if ( vcl_abs(ev[1]) > vcl_abs(ev[2])  ) std::swap(ev[1], ev[2]);
        if ( vcl_abs(ev[0]) > vcl_abs(ev[1])  ) std::swap(ev[0], ev[1]);

        const Precision V=VesselnessFunction3D(ev[0],ev[1],ev[2]);
        vnl_vector<Precision> evn(3);

        // adjusting eigenvalues
        // static_cast required to prevent error with gcc 4.1.2
        evn[0]   = 1.0 + (m_Epsilon - 1.0) * vcl_pow(V,static_cast<Precision>(1.0/m_Sensitivity));
        evn[1]   = 1.0 + (m_Epsilon - 1.0) * vcl_pow(V,static_cast<Precision>(1.0/m_Sensitivity)); 
        evn[2]   = 1.0 + (m_Omega - 1.0 ) * vcl_pow(V,static_cast<Precision>(1.0/m_Sensitivity)); 

        vnl_matrix<Precision> LAM(3,3);
        LAM.fill(0);
        LAM(0,0) = evn[0];
        LAM(1,1) = evn[1];
        LAM(2,2) = evn[2];

        const vnl_matrix<Precision> HN = EV * LAM * EV.transpose();

        itxx.Value() = HN(0,0);
        itxy.Value() = HN(0,1);
        itxz.Value() = HN(0,2);
        ityy.Value() = HN(1,1);
        ityz.Value() = HN(1,2);
        itzz.Value() = HN(2,2);
    }
    return;

} 
// generatedata
template <class PixelType, unsigned int Dimension>
void VesselEnhancingDiffusion3DImageFilter<PixelType, Dimension>
::GenerateData()
{
    if (m_Verbose)
    {
        std::cout << std::endl << "begin vesselenhancingdiffusion3Dimagefilter ... " << std::endl;
    }

    typedef MinimumMaximumImageFilter<ImageType> MinMaxType;
    typename MinMaxType::Pointer minmax = MinMaxType::New();
	
    minmax->SetInput(this->GetInput());
    minmax->Update();

	const typename ImageType::SpacingType ispacing = this->GetInput()->GetSpacing();
  	const Precision htmax = 0.5 / 
					(  1.0 / (ispacing[0] * ispacing[0]) 
					 + 1.0 / (ispacing[1] * ispacing[1]) 
					 + 1.0 / (ispacing[2] * ispacing[2])
					 );

    if (m_TimeStep == NumericTraits<Precision>::Zero)
    {
        m_TimeStep = htmax;
    }

  	if (m_TimeStep> htmax) 
	{
	  std::cerr << "the time step size is too large!" << std::endl;
      this->AllocateOutputs();
      return;
  	}

    if (m_Verbose)
    {
        std::cout << "min/max             \t" << minmax->GetMinimum() << " " << minmax->GetMaximum() << std::endl;
        std::cout << "iterations/timestep \t" << m_Iterations << " " << m_TimeStep << std::endl; 
        std::cout << "recalc v            \t" << m_RecalculateVesselness << std::endl;
        std::cout << "scales              \t";
        for (unsigned int i=0; i<m_Scales.size(); ++i)
        {
            std::cout << m_Scales[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "alpha/beta/gamma    \t" << m_Alpha << " " << m_Beta << " " << m_Gamma << std::endl;
        std::cout << "eps/omega/sens      \t" << m_Epsilon <<  " " << m_Omega << " " << m_Sensitivity << std::endl;
    }

    // cast to precision
    typedef CastImageFilter<ImageType,PrecisionImageType> CT;
    typename CT::Pointer cast = CT::New();
    cast->SetInput(this->GetInput());
    cast->Update();

    typename PrecisionImageType::Pointer ci = cast->GetOutput();


    if (m_Verbose)
    {
  	    std::cout << "start algorithm ... " << std::endl;
    }

	for (m_CurrentIteration=1; m_CurrentIteration<=m_Iterations; m_CurrentIteration++)
    {
        VED3DSingleIteration (ci);
    } 

    typedef MinimumMaximumImageFilter<PrecisionImageType> MMT;
    typename MMT::Pointer mm = MMT::New();
    mm->SetInput(ci);
    mm->Update();

    if (m_Verbose)
    {
        std::cout << std::endl;
        std::cout << "min/max             \t" << mm->GetMinimum() << " " << mm->GetMaximum() << std::endl;
        std::cout << "end vesselenhancingdiffusion3Dimagefilter" << std::endl;
    }

    // cast back to pixeltype
    this->AllocateOutputs();
    typedef CastImageFilter<PrecisionImageType,ImageType> CTI;
    typename CTI::Pointer casti = CTI::New();
    casti->SetInput(ci);
    casti->GraftOutput(this->GetOutput());
    casti->Update();
    this->GraftOutput(casti->GetOutput());
}


} // end namespace itk

#endif
