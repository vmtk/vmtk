/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnisotropicDiffusionVesselEnhancementImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2007/06/20 16:03:23 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnisotropicDiffusionVesselEnhancementImageFilter_txx_
#define __itkAnisotropicDiffusionVesselEnhancementImageFilter_txx_

#include "itkAnisotropicDiffusionVesselEnhancementImageFilter.h"
#include "itkAnisotropicDiffusionVesselEnhancementFunction.h"

#include <list>
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkNeighborhoodAlgorithm.h"

#include "itkImageFileWriter.h"
#include "itkVector.h"

namespace itk {

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage, class TVesselnessFilter>
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::AnisotropicDiffusionVesselEnhancementImageFilter()
{
  m_UpdateBuffer = UpdateBufferType::New(); 

  m_DiffusionTensorImage  = DiffusionTensorImageType::New();
 
  this->SetNumberOfIterations(1);

  m_TimeStep = 1e-2;

  //set the function
  typename AnisotropicDiffusionVesselEnhancementFunction<UpdateBufferType>::Pointer q
      = AnisotropicDiffusionVesselEnhancementFunction<UpdateBufferType>::New();
  this->SetDifferenceFunction(q);

  //instantiate the SymmetricEigenVectorAnalysis filter
  m_EigenVectorMatrixAnalysisFilter = EigenVectorMatrixAnalysisFilterType::New();
  m_EigenVectorMatrixAnalysisFilter->SetDimension( TensorPixelType::Dimension ); 
 
  //instantiate the vesselness filter

  typename VesselnessFilterType::Pointer vesselnessFilter = VesselnessFilterType::New();

  m_MultiScaleVesselnessFilter  = MultiScaleVesselnessFilterType::New();
  m_MultiScaleVesselnessFilter->GenerateHessianOutputOn();
  m_MultiScaleVesselnessFilter->SetHessianToMeasureFilter(vesselnessFilter);

  // Vesselness guided vesselness function algorithm parameter
  m_WStrength = 25.0;
  m_Sensitivity = 5.0;
  m_Epsilon = 1e-1;

  m_NumberOfDiffusionSubIterations = 1;
}

/** Prepare for the iteration process. */
template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::InitializeIteration()
{
  itkDebugMacro( << "InitializeIteration() called " );

  AnisotropicDiffusionVesselEnhancementFunction<UpdateBufferType> *f = 
     dynamic_cast<AnisotropicDiffusionVesselEnhancementFunction<UpdateBufferType> *>
     (this->GetDifferenceFunction().GetPointer());

  if (! f)
    {
    throw ExceptionObject(__FILE__, __LINE__, 
        "Anisotropic diffusion Vessel Enhancement function is not set.",
         ITK_LOCATION);
    }
   
  f->SetTimeStep(m_TimeStep);
   
  // Check the timestep for stability
  double minSpacing;
  if (this->GetUseImageSpacing())
    {
    minSpacing = this->GetInput()->GetSpacing()[0];
    for (unsigned int i = 1; i < ImageDimension; i++)
      {
      if (this->GetInput()->GetSpacing()[i] < minSpacing)
        {
        minSpacing = this->GetInput()->GetSpacing()[i];
        }
      }
    }
  else
    {
    minSpacing = 1.0;
    }

  double ratio = 
     minSpacing /std::pow(2.0, static_cast<double>(ImageDimension) + 1);

  if ( m_TimeStep > ratio ) 
    {
    itkWarningMacro(<< std::endl << "Anisotropic diffusion unstable time step:" 
                    << m_TimeStep << std::endl << "Minimum stable time step" 
                    << "for this image is " 
                    << ratio ); 
    }
   
  f->InitializeIteration();
   
  if (this->GetNumberOfIterations() != 0)
    {
    this->UpdateProgress(((float)(this->GetElapsedIterations()))
                          /((float)(this->GetNumberOfIterations())));
    }
  else
    {
    this->UpdateProgress(0);
    }

  if (m_NumberOfDiffusionSubIterations == 0)
    {
    m_NumberOfDiffusionSubIterations = 1;
    }

  if (this->GetElapsedIterations() % m_NumberOfDiffusionSubIterations == 0)
    {
    //Update the Diffusion tensor image
    this->UpdateDiffusionTensorImage();
    }
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::CopyInputToOutput()
{
  typename TInputImage::ConstPointer  input  = this->GetInput();
  typename TOutputImage::Pointer      output = this->GetOutput();

  if ( !input || !output )
    {
    itkExceptionMacro(<< "Either input and/or output is NULL.");
    }

  // Check if we are doing in-place filtering
  if ( this->GetInPlace() && (typeid(TInputImage) == typeid(TOutputImage)) )
    {
    typename TInputImage::Pointer tempPtr = 
      dynamic_cast<TInputImage *>( output.GetPointer() );
    if ( tempPtr && tempPtr->GetPixelContainer() == input->GetPixelContainer() )
      {
      // the input and output container are the same - no need to copy
      return;
      }
    }
  
  ImageRegionConstIterator<TInputImage>  in(input, output->GetRequestedRegion());
  ImageRegionIterator<TOutputImage> out(output, output->GetRequestedRegion());

  while( ! out.IsAtEnd() )
    {
    out.Value() =  static_cast<PixelType>(in.Get());  
    ++in;
    ++out;
    }
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::AllocateUpdateBuffer()
{
  /* The update buffer looks just like the output and holds the change in 
   the pixel  */
  
  typename TOutputImage::Pointer output = this->GetOutput();

  m_UpdateBuffer->SetSpacing(output->GetSpacing());
  m_UpdateBuffer->SetOrigin(output->GetOrigin());
  m_UpdateBuffer->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
  m_UpdateBuffer->SetRequestedRegion(output->GetRequestedRegion());
  m_UpdateBuffer->SetBufferedRegion(output->GetBufferedRegion());
  m_UpdateBuffer->Allocate();
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::AllocateDiffusionTensorImage()
{
  itkDebugMacro( << "AllocateDiffusionTensorImage() called" ); 

  /* The diffusionTensor image has the same size as the output and holds 
     the diffusion tensor matrix for each pixel */

  typename TOutputImage::Pointer output = this->GetOutput();

  m_DiffusionTensorImage->SetSpacing(output->GetSpacing());
  m_DiffusionTensorImage->SetOrigin(output->GetOrigin());
  m_DiffusionTensorImage->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
  m_DiffusionTensorImage->SetRequestedRegion(output->GetRequestedRegion());
  m_DiffusionTensorImage->SetBufferedRegion(output->GetBufferedRegion());
  m_DiffusionTensorImage->Allocate();
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::UpdateDiffusionTensorImage()
{
  itkDebugMacro( << "UpdateDiffusionTensorImage() called" ); 

  m_MultiScaleVesselnessFilter->SetInput( this->GetOutput() );
  m_MultiScaleVesselnessFilter->Modified();
  m_MultiScaleVesselnessFilter->Update();

  // Hessian matrix 
  typename HessianImageType::ConstPointer hessianOutputImage;
  hessianOutputImage = m_MultiScaleVesselnessFilter->GetHessianOutput();

  // Pass it to the eigenVector matrix analyzer
  m_EigenVectorMatrixAnalysisFilter->SetInput( hessianOutputImage ); 

  m_EigenVectorMatrixAnalysisFilter->Update();

  typename OutputMatrixImageType::Pointer eigenVectorMatrixOutputImage =
                              m_EigenVectorMatrixAnalysisFilter->GetOutput();

  typedef  itk::ImageRegionIterator< OutputMatrixImageType > 
                                    EigenVectorMatrixIteratorType;

  EigenVectorMatrixIteratorType ig(eigenVectorMatrixOutputImage, eigenVectorMatrixOutputImage->GetLargestPossibleRegion());

  ig.GoToBegin();

  // Vessleness response
  typename VesselnessImageType::Pointer vesselnessImage;
  vesselnessImage = m_MultiScaleVesselnessFilter->GetOutput();

  typedef itk::ImageRegionIterator<VesselnessImageType> VesselnessIteratorType;
  VesselnessIteratorType iv(vesselnessImage, vesselnessImage->GetLargestPossibleRegion());
  iv.GoToBegin();

  typename DiffusionTensorImageType::PixelType tensor;

  double Lambda1;
  double Lambda2;
  double Lambda3;

  double iS = 1.0 / m_Sensitivity; 

  MatrixType eigenValueMatrix;
  MatrixType hessianEigenVectorMatrix;
  MatrixType hessianEigenVectorMatrixTranspose;
  MatrixType productMatrix;

  typedef itk::ImageRegionIterator<DiffusionTensorImageType> DiffusionTensorIteratorType;

  DiffusionTensorIteratorType it(m_DiffusionTensorImage,m_DiffusionTensorImage->GetLargestPossibleRegion());

  it.GoToBegin();

  while( !it.IsAtEnd() )
    {
    // Generate matrix "Q" with the eigenvectors of the Hessian
    hessianEigenVectorMatrix = ig.Get();
    hessianEigenVectorMatrixTranspose = hessianEigenVectorMatrix.GetTranspose(); 

    // Generate the diagonal matrix with the eigen values
    eigenValueMatrix.SetIdentity();

    double vesselnessValue = static_cast<double> (iv.Get());
    
    Lambda1 = 1 + ( m_WStrength - 1 ) * std::pow ( vesselnessValue, iS ); 
    Lambda2 = Lambda3 = 1 + ( m_Epsilon - 1 ) * std::pow ( vesselnessValue, iS ); 
//    Lambda1 = m_Epsilon + (1 - m_Epsilon) * vesselnessValue; 
//    Lambda2 = Lambda3 = m_Epsilon; 

    eigenValueMatrix(0,0) = Lambda1;
    eigenValueMatrix(1,1) = Lambda2;
    eigenValueMatrix(2,2) = Lambda3;

    // Generate the tensor matrix
    productMatrix = hessianEigenVectorMatrix * eigenValueMatrix * hessianEigenVectorMatrixTranspose;

    //Copy the ITK::Matrix to the tensor...there should be a better way of doing this TODO
    tensor(0,0) = productMatrix(0,0);
    tensor(0,1) = productMatrix(0,1);
    tensor(0,2) = productMatrix(0,2);

    tensor(1,0) = productMatrix(1,0);
    tensor(1,1) = productMatrix(1,1);
    tensor(1,2) = productMatrix(1,2);

    tensor(2,0) = productMatrix(2,0);
    tensor(2,1) = productMatrix(2,1);
    tensor(2,2) = productMatrix(2,2);

    it.Set( tensor );

    ++it;
    ++ig;
    ++iv;
    }
}

template<class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::ApplyUpdate(const TimeStepType& dt)
{
  itkDebugMacro( << "ApplyUpdate Invoked with time step size: " << dt ); 
  // Set up for multithreaded processing.
  DenseFDThreadStruct str;
  str.Filter = this;
  str.TimeStep = dt;
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ApplyUpdateThreaderCallback,
                                            &str);
  // Multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();
}

template<class TInputImage, class TOutputImage, class TVesselnessFilter>
ITK_THREAD_RETURN_TYPE
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::ApplyUpdateThreaderCallback( void * arg )
{
  DenseFDThreadStruct * str;
  ThreadIdType threadId;
  int total, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (DenseFDThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  ThreadRegionType splitRegion;
  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  ThreadDiffusionImageRegionType    splitRegionDiffusionImage;
  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegionDiffusionImage);
  if (threadId < total)
    {
    str->Filter->ThreadedApplyUpdate(str->TimeStep, splitRegion, splitRegionDiffusionImage, threadId);
    }

#if ITK_VERSION_MAJOR >= 5
  return itk::ITK_THREAD_RETURN_DEFAULT_VALUE;
#else
  return ITK_THREAD_RETURN_VALUE;
#endif
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
typename
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>::TimeStepType
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::CalculateChange()
{
  itkDebugMacro( << "CalculateChange called" );

  int threadCount;
  TimeStepType dt;

  // Set up for multithreaded processing.
  DenseFDThreadStruct str;
  str.Filter = this;
  str.TimeStep = NumericTraits<TimeStepType>::Zero;  // Not used during the
  // calculate change step.
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->CalculateChangeThreaderCallback,
                                            &str);

  // Initialize the list of time step values that will be generated by the
  // various threads.  There is one distinct slot for each possible thread,
  // so this data structure is thread-safe.
  threadCount = this->GetMultiThreader()->GetNumberOfThreads();  
  str.TimeStepList.resize(threadCount);
  str.ValidTimeStepList.resize(threadCount);
  for (int i =0; i < threadCount; ++i)
    {      str.ValidTimeStepList[i] = false;    } 

  // Multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Resolve the single value time step to return
  dt = this->ResolveTimeStep(str.TimeStepList, str.ValidTimeStepList);
  //delete [] str.TimeStepList;
  //delete [] str.ValidTimeStepList;

  return  dt;
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
ITK_THREAD_RETURN_TYPE
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::CalculateChangeThreaderCallback( void * arg )
{
  DenseFDThreadStruct * str;
  ThreadIdType threadId;
  int total, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (DenseFDThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  ThreadRegionType splitRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  ThreadDiffusionImageRegionType splitDiffusionimageRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitDiffusionimageRegion);

  if (threadId < total)
    { 
    str->TimeStepList[threadId]
      = str->Filter->ThreadedCalculateChange(splitRegion, splitDiffusionimageRegion, threadId);
    str->ValidTimeStepList[threadId] = true;
    }

#if ITK_VERSION_MAJOR >= 5
  return itk::ITK_THREAD_RETURN_DEFAULT_VALUE;
#else
  return ITK_THREAD_RETURN_VALUE;
#endif
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::ThreadedApplyUpdate(TimeStepType dt, const ThreadRegionType &regionToProcess,
                      const ThreadDiffusionImageRegionType & diffusionRegionToProcess,
                      ThreadIdType threadId)
{
  ImageRegionIterator<UpdateBufferType> u(m_UpdateBuffer,    regionToProcess);
  ImageRegionIterator<OutputImageType>  o(this->GetOutput(), regionToProcess);

  u.GoToBegin();
  o.GoToBegin();

  while ( !u.IsAtEnd() )
    {

    o.Value() += static_cast<PixelType>(u.Value() * dt);  // no adaptor support here

    ++o;
    ++u;
    }
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
typename
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>::TimeStepType
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::ThreadedCalculateChange(const ThreadRegionType &regionToProcess, 
    const ThreadDiffusionImageRegionType & diffusionRegionToProcess, ThreadIdType threadId)
{
  typedef typename OutputImageType::RegionType      RegionType;
  typedef typename OutputImageType::SizeType        SizeType;
  typedef typename OutputImageType::SizeValueType   SizeValueType;
  typedef typename OutputImageType::IndexType       IndexType;
  typedef typename OutputImageType::IndexValueType  IndexValueType;

  typedef typename FiniteDifferenceFunctionType::NeighborhoodType
                                           NeighborhoodIteratorType;
  
  typedef ImageRegionIterator<UpdateBufferType> UpdateIteratorType;

  typename OutputImageType::Pointer output = this->GetOutput();
  TimeStepType timeStep;
  void *globalData;

  // Get the FiniteDifferenceFunction to use in calculations.
  const typename FiniteDifferenceFunctionType::Pointer df = 
     dynamic_cast<AnisotropicDiffusionVesselEnhancementFunction<UpdateBufferType> *>
     ( this->GetDifferenceFunction().GetPointer());

  const SizeType  radius = df->GetRadius();
  
  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<OutputImageType>
                                                        FaceCalculatorType;

  typedef typename FaceCalculatorType::FaceListType FaceListType;

  FaceCalculatorType faceCalculator;
    
  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt = faceList.begin();

   // Process the non-boundary region.
  NeighborhoodIteratorType nD(radius, output, *fIt);

  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<DiffusionTensorImageType>
    DiffusionTensorFaceCalculatorType;

  typedef typename DiffusionTensorFaceCalculatorType::FaceListType 
                                                DiffusionTensorFaceListType;

  DiffusionTensorFaceCalculatorType diffusionTensorFaceCalculator;
  
  DiffusionTensorFaceListType diffusionTensorFaceList = 
     diffusionTensorFaceCalculator(m_DiffusionTensorImage, diffusionRegionToProcess, radius);

  typename DiffusionTensorFaceListType::iterator dfIt = diffusionTensorFaceList.begin();
  
  DiffusionTensorNeighborhoodType  dTN(radius,m_DiffusionTensorImage, *dfIt); 

  // Ask the function object for a pointer to a data structure it
  // will use to manage any global values it needs.  We'll pass this
  // back to the function object at each calculation and then
  // again so that the function object can use it to determine a
  // time step for this iteration.
  globalData = df->GetGlobalDataPointer();

  UpdateIteratorType nU(m_UpdateBuffer,  *fIt);
  nD.GoToBegin();
  while( !nD.IsAtEnd() )
    {
    nU.Value() = df->ComputeUpdate(nD, dTN, globalData);
    ++nD;
    ++nU;
    }

  // Process each of the boundary faces.

  NeighborhoodIteratorType bD;
  
  DiffusionTensorNeighborhoodType bDD;

  UpdateIteratorType   bU;
  for (++fIt; fIt != faceList.end(); ++fIt)
    {
    bD = NeighborhoodIteratorType(radius, output, *fIt);
    bDD = DiffusionTensorNeighborhoodType(radius, m_DiffusionTensorImage, *dfIt);
    bU = UpdateIteratorType  (m_UpdateBuffer, *fIt);
     
    bD.GoToBegin();
    bU.GoToBegin();
    while ( !bD.IsAtEnd() )
      {
      bU.Value() = df->ComputeUpdate(bD,bDD,globalData);
      ++bD;
      ++bU;
      }
    ++dfIt;
    }

  // Ask the finite difference function to compute the time step for
  // this iteration.  We give it the global data pointer to use, then
  // ask it to free the global data memory.
  timeStep = df->ComputeGlobalTimeStep(globalData);
  df->ReleaseGlobalDataPointer(globalData);

  return timeStep;
}

template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::GenerateData()
{
  itkDebugMacro( << "GenerateData is called" );

  if (!this->GetIsInitialized())
    {

    // Allocate the output image
    this->AllocateOutputs();

    // Copy the input image to the output image.  Algorithms will operate
    // directly on the output image and the update buffer.
    this->CopyInputToOutput();

    // Allocate the internal update buffer.  
    this->AllocateUpdateBuffer();

    // Allocate buffer for the diffusion tensor image
    this->AllocateDiffusionTensorImage();

    this->SetStateToInitialized();

    this->SetElapsedIterations( 0 );
    }
    
  // Iterative algorithm
  TimeStepType dt;
  unsigned int iter = 0;

  while ( ! this->Halt() )
    {
    std::cout << "Iteration: " << iter << std::endl;
    this->InitializeIteration(); // An optional method for precalculating
                                 // global values, or otherwise setting up
                                 // for the next iteration
    dt = this->CalculateChange();

    this->ApplyUpdate(dt);

    ++iter;

    this->SetElapsedIterations( iter );

    // Invoke the iteration event.
    this->InvokeEvent( IterationEvent() );
    if( this->GetAbortGenerateData() )
      {
      this->InvokeEvent( IterationEvent() );
      this->ResetPipeline(); 
      throw ProcessAborted(__FILE__,__LINE__);
      }
    }
} 
 
template <class TInputImage, class TOutputImage, class TVesselnessFilter>
void
AnisotropicDiffusionVesselEnhancementImageFilter<TInputImage, TOutputImage, TVesselnessFilter>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

}// end namespace itk

#endif
