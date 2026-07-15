/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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

/**
 * @class   vtkvmtkITKFilterUtilities
 * @brief   Provides static helper methods for converting between vtkImageData and ITK images.
 * @ingroup Segmentation
 *
 * vtkvmtkITKFilterUtilities is a collection of static template methods used throughout vtkVmtk's
 * Segmentation module to bridge VTK and ITK image pipelines without copying the underlying pixel
 * buffer: VTKToITKImage and VTKToITKVectorImage wrap the memory owned by a vtkImageData into an
 * itk::Image (scalar or vector-valued) by importing the raw pointer, while ITKToVTKImage copies an
 * ITK image's buffer into a (compatible, pre-typed) vtkImageData. ConnectProgress/ProgressCallback
 * forward ITK progress events to the vtkAlgorithm driving the ITK pipeline, so that vtkvmtk*ImageFilter
 * wrapper classes (e.g. vtkvmtkSigmoidImageFilter, vtkvmtkVesselnessMeasureImageFilter) can report
 * progress through the usual VTK mechanism. Instances of this class are never created; all members
 * are static and it exists purely as a namespace-like utility used internally by the ITK filter
 * wrapper classes in this module.
 */

#ifndef __vtkvmtkITKFilterUtilities_h
#define __vtkvmtkITKFilterUtilities_h

#include "vtkvmtkITKFilterUtilities.h"
#include "vtkvmtkWin32Header.h"

#include "vtkImageData.h"
#include "itkImage.h"
#include "itkCommand.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkITKFilterUtilities
{
public:

  /**
   * Wrap a scalar vtkImageData's pixel buffer into an ITK image of type TImage, without copying
   * pixel data (the ITK image imports the VTK scalar pointer directly, so input must outlive
   * output and must not be modified/reallocated while output is in use). Dimensions, spacing,
   * origin, and (buffered/largest possible) region are copied from input's extent.
   */
  template<typename TImage>
  static void
  VTKToITKImage(vtkImageData* input, typename TImage::Pointer output) {

    typedef TImage ImageType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename ImageType::PixelType PixelType;

    int dims[3];
    input->GetDimensions(dims);
    double spacing[3];
    input->GetSpacing(spacing);
    double origin[3];
    input->GetOrigin(origin);
    int extent[6];
    input->GetExtent(extent);

    output->GetPixelContainer()->SetImportPointer(static_cast<PixelType*>(input->GetScalarPointer()),dims[0]*dims[1]*dims[2],false);
    typename ImageType::RegionType region;
    typename ImageType::IndexType index;
    typename ImageType::SizeType size;
    //index[0] = index[1] = index[2] = 0;
    //size[0] = dims[0];
    //size[1] = dims[1];
    //size[2] = dims[2];
    index[0] = extent[0];
    index[1] = extent[2];
    index[2] = extent[4];
    size[0] = extent[1] - extent[0] + 1;
    size[1] = extent[3] - extent[2] + 1;
    size[2] = extent[5] - extent[4] + 1;
    region.SetIndex(index);
    region.SetSize(size);
    output->SetLargestPossibleRegion(region);
    output->SetBufferedRegion(region);
    output->SetSpacing(spacing);
    output->SetOrigin(origin);
  }

  /**
   * Wrap a multi-component vtkImageData's pixel buffer into an ITK vector image of type TImage,
   * without copying pixel data (same aliasing caveats as VTKToITKImage). The number of VTK scalar
   * components becomes the ITK image's VectorLength.
   */
  template<typename TImage>
    static void
    VTKToITKVectorImage(vtkImageData* input, typename TImage::Pointer output) {

    typedef TImage ImageType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::InternalPixelType InternalPixelType;

    int dims[3];
    input->GetDimensions(dims);
    double spacing[3];
    input->GetSpacing(spacing);
    int components = input->GetNumberOfScalarComponents();
    double origin[3];
    input->GetOrigin(origin);
    int extent[6];
    input->GetExtent(extent);

    output->GetPixelContainer()->SetImportPointer(static_cast<InternalPixelType*>(input->GetScalarPointer()),dims[0]*dims[1]*dims[2]*components,false);
    typename ImageType::RegionType region;
    typename ImageType::IndexType index;
    typename ImageType::SizeType size;
    //index[0] = index[1] = index[2] = 0;
    //size[0] = dims[0];
    //size[1] = dims[1];
    //size[2] = dims[2];
    index[0] = extent[0];
    index[1] = extent[2];
    index[2] = extent[4];
    size[0] = extent[1] - extent[0] + 1;
    size[1] = extent[3] - extent[2] + 1;
    size[2] = extent[5] - extent[4] + 1;
    region.SetIndex(index);
    region.SetSize(size);
    output->SetLargestPossibleRegion(region);
    output->SetBufferedRegion(region);
    output->SetSpacing(spacing);
    output->SetOrigin(origin);
    output->SetVectorLength(components);
  }

  /**
   * Copy the buffer of an ITK image of type TImage into output, a vtkImageData that must already
   * have its scalar type set by the caller (this method reads output's current scalar type and
   * allocates accordingly; it does not infer it from TImage::PixelType). Origin, spacing, and
   * extent are set from input's region; the number of scalar components is set from
   * TImage::PixelType's number of components.
   */
  template<typename TImage>
  static void
  ITKToVTKImage(typename TImage::Pointer input, vtkImageData* output) {

    typedef TImage ImageType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::SpacingType SpacingType;
    typedef typename ImageType::RegionType RegionType;
    typedef typename ImageType::IndexType IndexType;
    typedef typename ImageType::SizeType SizeType;

    PointType origin = input->GetOrigin();
    SpacingType spacing = input->GetSpacing();

    double outputOrigin[3];
    double outputSpacing[3];
    
    outputOrigin[0] = origin[0];
    outputOrigin[1] = origin[1];
    outputOrigin[2] = origin[2];

    outputSpacing[0] = spacing[0];
    outputSpacing[1] = spacing[1];
    outputSpacing[2] = spacing[2];

    output->SetOrigin(outputOrigin);
    output->SetSpacing(outputSpacing);

    RegionType region = input->GetBufferedRegion();
    IndexType index = region.GetIndex();
    SizeType size = region.GetSize();

    //int dimensions[3];
    //dimensions[0] = size[0];
    //dimensions[1] = size[1];
    //dimensions[2] = size[2];
    int extent[6];
    extent[0] = index[0];
    extent[1] = index[0] + size[0] - 1;
    extent[2] = index[1];
    extent[3] = index[1] + size[1] - 1;
    extent[4] = index[2];
    extent[5] = index[2] + size[2] - 1;

    int components = input->GetNumberOfComponentsPerPixel();
    int dataType = output->GetScalarType(); // WARNING: we delegate setting type to caller

    //output->SetDimensions(dimensions);
    output->SetExtent(extent);
    output->AllocateScalars(dataType,components);

    memcpy(static_cast<PixelType*>(output->GetScalarPointer()),input->GetBufferPointer(),input->GetBufferedRegion().GetNumberOfPixels()*sizeof(PixelType));
  }

  /**
   * ITK progress event callback (itk::CStyleCommand signature) that forwards the progress of the
   * ITK process object o to the vtkAlgorithm passed as client data via UpdateProgress. Not meant to
   * be called directly; installed on an ITK filter by ConnectProgress.
   */
  static void
  ProgressCallback(itk::Object *o, const itk::EventObject &, void *data)
  {
    ((vtkAlgorithm*)data)->UpdateProgress(dynamic_cast<const itk::ProcessObject*>(o)->GetProgress());
  }

  /**
   * Register a progress observer on the ITK object obj so that its ProgressEvent notifications are
   * forwarded to alg's UpdateProgress, letting an ITK filter wrapped inside a vtkAlgorithm (e.g. a
   * SimpleExecute-based vtkvmtk*ImageFilter) report progress through the normal VTK mechanism.
   */
  static void
  ConnectProgress(itk::Object* obj, vtkAlgorithm* alg)
  {
    itk::CStyleCommand::Pointer progressCommand = itk::CStyleCommand::New();
    progressCommand->SetCallback(vtkvmtkITKFilterUtilities::ProgressCallback);
    progressCommand->SetClientData(alg);
    obj->AddObserver(itk::ProgressEvent(),progressCommand);
  }

protected:
  vtkvmtkITKFilterUtilities() {};
  ~vtkvmtkITKFilterUtilities() {};

private:
  vtkvmtkITKFilterUtilities(const vtkvmtkITKFilterUtilities&);  // Not implemented.
  void operator=(const vtkvmtkITKFilterUtilities&);  // Not implemented.
};

#if 0
template< class TInputPixel, class TOutputPixel>
void
vtkvmtkSimpleImageToImageITKFilter<TInputPixel,TOutputPixel>::
SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  int inputDims[3];
  input->GetDimensions(inputDims);
  double inputSpacing[3];
  input->GetSpacing(inputSpacing);

  InputImagePointer inImage = InputImageType::New();
  inImage->GetPixelContainer()->SetImportPointer(static_cast<InputPixelType*>(input->GetScalarPointer()),inputDims[0]*inputDims[1]*inputDims[2],false);
  typename InputImageType::RegionType inputRegion;
  typename InputImageType::IndexType inputIndex;
  typename InputImageType::SizeType inputSize;
  inputIndex[0] = inputIndex[1] = inputIndex[2] = 0;
  inputSize[0] = inputDims[0];
  inputSize[1] = inputDims[1];
  inputSize[2] = inputDims[2];
  inputRegion.SetIndex(inputIndex);
  inputRegion.SetSize(inputSize);
  inImage->SetLargestPossibleRegion(inputRegion);
  inImage->SetBufferedRegion(inputRegion);
  inImage->SetSpacing(inputSpacing);

  int outputDims[3];
  output->GetDimensions(outputDims);
  double outputSpacing[3];
  output->GetSpacing(outputSpacing);

  OutputImagePointer outImage = OutputImageType::New();
  outImage->GetPixelContainer()->SetImportPointer(static_cast<OutputPixelType*>(output->GetScalarPointer()),outputDims[0]*outputDims[1]*outputDims[2],false);
  typename OutputImageType::RegionType outputRegion;
  typename OutputImageType::IndexType outputIndex;
  typename OutputImageType::SizeType outputSize;
  outputIndex[0] = outputIndex[1] = outputIndex[2] = 0;
  outputSize[0] = outputDims[0];
  outputSize[1] = outputDims[1];
  outputSize[2] = outputDims[2];
  outputRegion.SetIndex(outputIndex);
  outputRegion.SetSize(outputSize);
  outImage->SetLargestPossibleRegion(outputRegion);
  outImage->SetBufferedRegion(outputRegion);
  outImage->SetSpacing(outputSpacing);

  this->SimpleExecuteITK(inImage,outImage);

  memcpy(static_cast<OutputPixelType*>(output->GetScalarPointer()),outImage->GetBufferPointer(),outImage->GetBufferedRegion().GetNumberOfPixels()*sizeof(OutputPixelType));
}

typedef vtkvmtkSimpleImageToImageITKFilter<float,float> vtkvmtkSimpleImageToImageITKFilterFF;
#endif

#endif

