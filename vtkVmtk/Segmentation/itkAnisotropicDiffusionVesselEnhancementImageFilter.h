/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnisotropicDiffusionVesselEnhancementImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2007/06/20 16:03:23 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnisotropicDiffusionVesselEnhancementImageFilter_h
#define __itkAnisotropicDiffusionVesselEnhancementImageFilter_h

#include "itkFiniteDifferenceImageFilter.h"
#include "itkHessianSmoothed3DToVesselnessMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkAnisotropicDiffusionVesselEnhancementFunction.h"
#include "itkMultiThreader.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenVectorAnalysisImageFilter.h"

namespace itk {
/** \class AnisotropicDiffusionVesselEnhancementFunction
 * \brief This class iteratively enhances vessels in an image by solving
 * non-linear diffusion equation developed by Manniesing et al.
 *
 * \par References
 *  Manniesing, R, Viergever, MA, & Niessen, WJ (2006). Vessel Enhancing 
 *  Diffusion: A Scale Space Representation of Vessel Structures. Medical 
 *  Image Analysis, 10(6), 815-825. 
 * 
 * \sa AnisotropicDiffusionVesselEnhancementImageFilter 
 * \ingroup FiniteDifferenceFunctions
 * \ingroup Functions
 */


template <class TInputImage, class TOutputImage, class TVesselnessFilter = itk::HessianSmoothed3DToVesselnessMeasureImageFilter<double> >
class ITK_EXPORT AnisotropicDiffusionVesselEnhancementImageFilter  
  : public FiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs */
  typedef AnisotropicDiffusionVesselEnhancementImageFilter Self;

  typedef FiniteDifferenceImageFilter<TInputImage, TOutputImage> 
                                                           Superclass;

  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;
 

  /** Method for creation through the object factory */
  itkNewMacro( Self );

  /** Run-time type information (and related methods) */
  itkTypeMacro(AnisotropicDiffusionVesselEnhancementImageFilter,
                                                ImageToImageFilter );
  
  /** Convenient typedefs */
  typedef typename Superclass::InputImageType  InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename Superclass::PixelType       PixelType;

  /** Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  typedef itk::Image< SymmetricSecondRankTensor< double, itkGetStaticConstMacro(ImageDimension) >, itkGetStaticConstMacro(ImageDimension) > DiffusionTensorImageType;

  typedef AnisotropicDiffusionVesselEnhancementFunction<InputImageType> FiniteDifferenceFunctionType;

  typedef TVesselnessFilter VesselnessFilterType;
  typedef typename VesselnessFilterType::InputImageType HessianImageType;
  typedef typename VesselnessFilterType::OutputImageType VesselnessImageType;
  typedef itk::MultiScaleHessianBasedMeasureImageFilter<InputImageType, HessianImageType, VesselnessImageType> MultiScaleVesselnessFilterType;

  typedef itk::Matrix<double, ImageDimension, ImageDimension> MatrixType;

  // Define image of matrix pixel type 
  typedef itk::Image< MatrixType, ImageDimension>  OutputMatrixImageType;

  // Define the symmetric tensor pixel type
  typedef itk::SymmetricSecondRankTensor< double, ImageDimension> 
                                                         TensorPixelType;
  typedef itk::Image< TensorPixelType, ImageDimension>  
                                                         TensorImageType;

   // Define the type for storing the eigen-value
  typedef itk::FixedArray< double, ImageDimension >      EigenValueArrayType;
  
  // Declare the types of the output images
  typedef itk::Image< EigenValueArrayType, ImageDimension >  
                                                  EigenAnalysisOutputImageType;
  
  // Declare the type for the filter
  typedef itk::SymmetricEigenVectorAnalysisImageFilter< 
                                    TensorImageType, 
                                    EigenAnalysisOutputImageType,
                                    OutputMatrixImageType 
                                    >  EigenVectorMatrixAnalysisFilterType;

  /** The value type of a time step.  Inherited from the superclass. */
  typedef typename Superclass::TimeStepType TimeStepType;

  /** The container type for the update buffer. */
  typedef OutputImageType UpdateBufferType;

  /** Define diffusion image nbd type */
  typedef typename FiniteDifferenceFunctionType::DiffusionTensorNeighborhoodType
                                               DiffusionTensorNeighborhoodType;

  /** Get the filter used to compute the Hessian based measure */
  MultiScaleVesselnessFilterType* GetMultiScaleVesselnessFilter()
  {
    return m_MultiScaleVesselnessFilter;
  }

  /** Set/Get Macro for VED parameters */
  itkSetMacro( TimeStep, double ); 
  itkSetMacro( Epsilon, double ); 
  itkSetMacro( WStrength, double ); 
  itkSetMacro( Sensitivity, double ); 

  itkGetMacro( TimeStep, double ); 
  itkGetMacro( Epsilon, double ); 
  itkGetMacro( WStrength, double ); 
  itkGetMacro( Sensitivity, double ); 

  itkSetMacro( NumberOfDiffusionSubIterations, unsigned int ); 
  itkGetMacro( NumberOfDiffusionSubIterations, unsigned int ); 

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputTimesDoubleCheck,
    (Concept::MultiplyOperator<PixelType, double>));
  itkConceptMacro(OutputAdditiveOperatorsCheck,
    (Concept::AdditiveOperators<PixelType>));
  itkConceptMacro(InputConvertibleToOutputCheck,
    (Concept::Convertible<typename TInputImage::PixelType, PixelType>));
  /** End concept checking */
#endif

protected:
  AnisotropicDiffusionVesselEnhancementImageFilter();
 ~AnisotropicDiffusionVesselEnhancementImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE;

  /* overloaded GenerateData method */
  virtual void GenerateData() ITK_OVERRIDE; 

  /** A simple method to copy the data from the input to the output. ( Supports
   * "read-only" image adaptors in the case where the input image type converts
   * to a different output image type. )  */
  virtual void CopyInputToOutput() ITK_OVERRIDE;

  /** This method applies changes from the m_UpdateBuffer to the output using
   * the ThreadedApplyUpdate() method and a multithreading mechanism.  "dt" is
   * the time step to use for the update of each pixel. */
  virtual void ApplyUpdate(const TimeStepType& dt) ITK_OVERRIDE;

  /** Method to allow subclasses to get direct access to the update
   * buffer */
  virtual UpdateBufferType* GetUpdateBuffer()
    { return m_UpdateBuffer; }

  /** This method populates an update buffer with changes for each pixel in the
   * output using the ThreadedCalculateChange() method and a multithreading
   * mechanism. Returns value is a time step to be used for the update. */
  virtual TimeStepType CalculateChange() ITK_OVERRIDE;

  /** This method allocates storage in m_UpdateBuffer.  It is called from
   * Superclass::GenerateData(). */
  virtual void AllocateUpdateBuffer() ITK_OVERRIDE;

  /** This method allocates storage for the diffusion tensor image */
  void AllocateDiffusionTensorImage();
 
  /** Update diffusion tensor image */
  void UpdateDiffusionTensorImage();
 
  /** The type of region used for multithreading */
  typedef typename UpdateBufferType::RegionType ThreadRegionType;

  /** The type of region used for multithreading */
  typedef typename DiffusionTensorImageType::RegionType 
                                        ThreadDiffusionImageRegionType;

  /**  Does the actual work of updating the output from the UpdateContainer 
   *   over an output region supplied by the multithreading mechanism.
   *  \sa ApplyUpdate
   *  \sa ApplyUpdateThreaderCallback */ 
  virtual
  void ThreadedApplyUpdate(
                TimeStepType dt,
                const ThreadRegionType &regionToProcess,
                const ThreadDiffusionImageRegionType &diffusionRegionToProcess,
                ThreadIdType threadId);

  /** Does the actual work of calculating change over a region supplied by
   * the multithreading mechanism.
   * \sa CalculateChange
   * \sa CalculateChangeThreaderCallback */
  virtual
  TimeStepType ThreadedCalculateChange(
               const ThreadRegionType &regionToProcess,
               const ThreadDiffusionImageRegionType &diffusionRegionToProcess,
               ThreadIdType threadId);

  /** Prepare for the iteration process. */
  virtual void InitializeIteration() ITK_OVERRIDE;

private:
  //purposely not implemented
  AnisotropicDiffusionVesselEnhancementImageFilter(const Self&); 
  void operator=(const Self&); //purposely not implemented

  /** Structure for passing information into static callback methods.  Used in
   * the subclasses' threading mechanisms. */
  struct DenseFDThreadStruct {
    AnisotropicDiffusionVesselEnhancementImageFilter *Filter;
    TimeStepType TimeStep;
    std::vector< TimeStepType > TimeStepList;
    std::vector< bool > ValidTimeStepList;
  };

  //struct DenseFDThreadStruct
  //  {
  //  AnisotropicDiffusionVesselEnhancementImageFilter *Filter;
  //  TimeStepType TimeStep;
  //  TimeStepType *TimeStepList;
  //  bool *ValidTimeStepList;
  //  };
    
  /** This callback method uses ImageSource::SplitRequestedRegion to acquire an
   * output region that it passes to ThreadedApplyUpdate for processing. */
  static ITK_THREAD_RETURN_TYPE ApplyUpdateThreaderCallback( void *arg );
  
  /** This callback method uses SplitUpdateContainer to acquire a region
   * which it then passes to ThreadedCalculateChange for processing. */
  static ITK_THREAD_RETURN_TYPE CalculateChangeThreaderCallback( void *arg );
 
  /** The buffer that holds the updates for an iteration of the algorithm. */
  typename UpdateBufferType::Pointer m_UpdateBuffer;

  TimeStepType                                          m_TimeStep;
  typename DiffusionTensorImageType::Pointer            m_DiffusionTensorImage;
  typename MultiScaleVesselnessFilterType::Pointer                m_MultiScaleVesselnessFilter;  

  typename EigenVectorMatrixAnalysisFilterType::Pointer 
                                      m_EigenVectorMatrixAnalysisFilter; 

  // Vesselness guided diffusion parameters
  double m_Epsilon;
  double m_WStrength;
  double m_Sensitivity;

  unsigned int m_NumberOfDiffusionSubIterations;
};
  

}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
# include "itkAnisotropicDiffusionVesselEnhancementImageFilter.txx"
#endif

#endif
