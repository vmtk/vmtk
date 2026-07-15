/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK

==========================================================================*/

#ifndef __vtkvmtkITKImageWriter_h
#define __vtkvmtkITKImageWriter_h

#include "vtkImageAlgorithm.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

#include "vtkvmtkITK.h"
#include "itkImageIOBase.h"

class vtkStringArray;

/**
 * @class   vtkvmtkITKImageWriter
 * @brief   Write vtkImageData to a file using an ITK image IO.
 * @ingroup Utilities
 *
 * vtkvmtkITKImageWriter converts its input vtkImageData to an ITK image
 * (choosing the ITK pixel type from the VTK scalar type) and writes it
 * with itk::ImageFileWriter, so it supports any file format for which an
 * ITK ImageIO is available (e.g. NRRD, NIfTI, MetaImage, DICOM). The
 * orientation of the written image can be set explicitly via
 * SetRasToIJKMatrix() and, for diffusion data, SetMeasurementFrameMatrix().
 */
class VTK_VMTK_ITK_EXPORT vtkvmtkITKImageWriter : public vtkImageAlgorithm
{
public:
  static vtkvmtkITKImageWriter *New();
  vtkTypeMacro(vtkvmtkITKImageWriter,vtkImageAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///
  /// Specify file name for the image file. You should specify either
  /// a FileName or a FilePrefix. Use FilePrefix if the data is stored
  /// in multiple files.
  void SetFileName(const char *);

  char *GetFileName() {
    return FileName;
  }

  ///
  /// use compression if possible
  vtkGetMacro (UseCompression, int);
  vtkSetMacro (UseCompression, int);
  vtkBooleanMacro(UseCompression, int);

  ///
  /// Set/Get the ImageIO class name.
  vtkGetStringMacro (ImageIOClassName);
  vtkSetStringMacro (ImageIOClassName);

  ///
  /// The main interface which triggers the writer to start.
  void Write();

  /// Set orienation matrix
  void SetRasToIJKMatrix( vtkMatrix4x4* mat) {
    RasToIJKMatrix = mat;
  }

  /// Set orienation matrix
  void SetMeasurementFrameMatrix( vtkMatrix4x4* mat) {
    MeasurementFrameMatrix = mat;
  }

protected:
  vtkvmtkITKImageWriter();
  ~vtkvmtkITKImageWriter();

  char *FileName;
  vtkMatrix4x4* RasToIJKMatrix;
  vtkMatrix4x4* MeasurementFrameMatrix;
  int UseCompression;
  char* ImageIOClassName;

private:
  vtkvmtkITKImageWriter(const vtkvmtkITKImageWriter&);  /// Not implemented.
  void operator=(const vtkvmtkITKImageWriter&);  /// Not implemented.
};

//vtkStandardNewMacro(vtkvmtkITKImageWriter)

#endif
