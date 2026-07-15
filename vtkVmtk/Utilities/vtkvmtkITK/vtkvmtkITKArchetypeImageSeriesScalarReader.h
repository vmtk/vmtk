/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK

==========================================================================*/

#ifndef __vtkvmtkITKArchetypeImageSeriesScalarReader_h
#define __vtkvmtkITKArchetypeImageSeriesScalarReader_h

#include "vtkvmtkITKArchetypeImageSeriesReader.h"

#include "itkImageFileReader.h"

/**
 * @class   vtkvmtkITKArchetypeImageSeriesScalarReader
 * @brief   Read a series of files into a single-component (scalar) volume.
 * @ingroup Utilities
 *
 * Specializes vtkvmtkITKArchetypeImageSeriesReader for single-component
 * pixel types, reading the series through ITK's itk::ImageSeriesReader /
 * itk::ImageFileReader and exporting the result as a scalar vtkImageData.
 *
 * @sa vtkvmtkITKArchetypeImageSeriesReader
 */
class VTK_VMTK_ITK_EXPORT vtkvmtkITKArchetypeImageSeriesScalarReader : public vtkvmtkITKArchetypeImageSeriesReader
{
 public:
  static vtkvmtkITKArchetypeImageSeriesScalarReader *New();
  vtkTypeMacro(vtkvmtkITKArchetypeImageSeriesScalarReader,vtkvmtkITKArchetypeImageSeriesReader);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

 protected:
  vtkvmtkITKArchetypeImageSeriesScalarReader();
  ~vtkvmtkITKArchetypeImageSeriesScalarReader();

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override;
  static void ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&, void* data);
};

#endif

