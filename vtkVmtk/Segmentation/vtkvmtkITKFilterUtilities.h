/*=========================================================================

Program:   VMTK
Module:    vtkvmtkITKFilterUtilities.h
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

// .NAME vtkvmtkITKFilterUtilities - Abstract class for wrapping ITK filters
// .SECTION Description
// vtkvmtkSimpleImageToImageITKFilter


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

  static void
  ProgressCallback(itk::Object *o, const itk::EventObject &, void *data)
  {
    ((vtkAlgorithm*)data)->UpdateProgress(dynamic_cast<const itk::ProcessObject*>(o)->GetProgress());
  }

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

