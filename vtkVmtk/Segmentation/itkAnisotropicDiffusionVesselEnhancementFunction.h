/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnisotropicDiffusionVesselEnhancementFunction.h,v $
  Language:  C++
  Date:      $Date: 2007/06/12 20:59:44 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnisotropicDiffusionVesselEnhancementFunction_h
#define __itkAnisotropicDiffusionVesselEnhancementFunction_h

#include "itkFiniteDifferenceFunction.h"
#include "vnl/vnl_matrix_fixed.h"
#include "itkSymmetricSecondRankTensor.h"

namespace itk {

/** \class AnisotropicDiffusionVesselEnhancementFunction
 * \brief This class is a function object that is used
 * to create a solver filter for vessel enhancment diffuion equation 
 * developed by Manniesing et al.
 *
 * \par References
 *  Manniesing, R, Viergever, MA, & Niessen, WJ (2006). Vessel Enhancing 
 *  Diffusion: A Scale Space Representation of Vessel Structures. Medical 
 *  Image Analysis, 10(6), 815-825. 
 * 
 * \sa MultiScaleHessianSmoothed3DToVesselnessMeasureImageFilter 
 * \sa AnisotropicDiffusionVesselEnhancementImageFilter 
 * \ingroup FiniteDifferenceFunctions
 * \ingroup Functions
 */
template <class TImageType>
class ITK_EXPORT AnisotropicDiffusionVesselEnhancementFunction
  : public FiniteDifferenceFunction<TImageType>
{
public:
  /** Standard class typedefs. */
  typedef AnisotropicDiffusionVesselEnhancementFunction Self;
  typedef FiniteDifferenceFunction<TImageType>          Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( AnisotropicDiffusionVesselEnhancementFunction, 
                                           FiniteDifferenceFunction );

  /** Extract some parameters from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** Convenient typedefs. */
  typedef double                                       TimeStepType;
  typedef typename Superclass::ImageType               ImageType;
  typedef typename Superclass::PixelType               PixelType;
  typedef double                                       ScalarValueType;
  typedef typename Superclass::RadiusType              RadiusType;
  typedef typename Superclass::NeighborhoodType        NeighborhoodType;
  typedef typename Superclass::FloatOffsetType         FloatOffsetType;


  typedef itk::Image< SymmetricSecondRankTensor<double,itkGetStaticConstMacro(ImageDimension)>,
                                itkGetStaticConstMacro(ImageDimension)> DiffusionTensorImageType;


  /** The default boundary condition for finite difference
   * functions that is used unless overridden in the Evaluate() method. */
  typedef ZeroFluxNeumannBoundaryCondition<DiffusionTensorImageType>
    DefaultBoundaryConditionType;


  /** Define diffusion image nbd type */
  typedef ConstNeighborhoodIterator<DiffusionTensorImageType, 
                                    DefaultBoundaryConditionType> 
                                           DiffusionTensorNeighborhoodType;

  /** Tensor pixel type */
  typedef itk::SymmetricSecondRankTensor< double >  TensorPixelType; 

  /** A global data type for this class of equations.  Used to store
   * values that are needed in calculating the time step and other intermediate
   * products such as derivatives that may be used by virtual functions called
   * from ComputeUpdate.  Caching these values here allows the ComputeUpdate
   * function to be const and thread safe.*/
  struct GlobalDataStruct
    {
    /** Hessian matrix */
    vnl_matrix_fixed<ScalarValueType,
                     itkGetStaticConstMacro(ImageDimension),
                     itkGetStaticConstMacro(ImageDimension)> m_dxy;

    /** diffusion tensor first derivative matrix */
    vnl_matrix_fixed<ScalarValueType,
                     itkGetStaticConstMacro(ImageDimension),
                     itkGetStaticConstMacro(ImageDimension)> m_DT_dxy;
    
    /** Array of first derivatives*/
    ScalarValueType m_dx[itkGetStaticConstMacro(ImageDimension)];
    
    ScalarValueType m_GradMagSqr;
    };

  /** Compute the equation value. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                void *globalData,
                                const FloatOffsetType& = FloatOffsetType(0.0)) ITK_OVERRIDE;
  
  /** Compute the equation value. */
  virtual PixelType ComputeUpdate(
                     const NeighborhoodType &neighborhood,
                     const DiffusionTensorNeighborhoodType &neighborhoodTensor,
                     void *globalData,
                     const FloatOffsetType& = FloatOffsetType(0.0));

  /** Computes the time step for an update given a global data structure. */
  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const ITK_OVERRIDE;

  /** Returns a pointer to a global data structure that is passed to this
   * object from the solver at each calculation.*/
  virtual void *GetGlobalDataPointer() const ITK_OVERRIDE
    {
    GlobalDataStruct *ans = new GlobalDataStruct();
    return ans; 
    }

  virtual void ReleaseGlobalDataPointer(void *GlobalData) const ITK_OVERRIDE
    { delete (GlobalDataStruct *) GlobalData; }

  /** Set/Get the time step. For this class of anisotropic diffusion filters,
      the time-step is supplied by the user and remains fixed for all
      updates. */
  void SetTimeStep(const TimeStepType &t)
    { 
    m_TimeStep = t; 
    }

  const TimeStepType &GetTimeStep() const
    { 
    return m_TimeStep;
    }

protected:
  AnisotropicDiffusionVesselEnhancementFunction();

  virtual ~AnisotropicDiffusionVesselEnhancementFunction() {}
  void PrintSelf(std::ostream &s, Indent indent) const ITK_OVERRIDE;
  
  /** Slices for the ND neighborhood. */
  std::slice x_slice[itkGetStaticConstMacro(ImageDimension)];

  /** The offset of the center pixel in the neighborhood. */
  ::size_t m_Center;

  /** Stride length along the y-dimension. */
  ::size_t m_xStride[itkGetStaticConstMacro(ImageDimension)];

private:
  //purposely not implemented
  AnisotropicDiffusionVesselEnhancementFunction(const Self&); 
  void operator=(const Self&);   //purposely not implemented

  TimeStepType    m_TimeStep;
};

} // namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkAnisotropicDiffusionVesselEnhancementFunction+-.h"
#endif

#ifndef ITK_MANUAL_INSTANTIATION
# include "itkAnisotropicDiffusionVesselEnhancementFunction.txx"
#endif

#endif
