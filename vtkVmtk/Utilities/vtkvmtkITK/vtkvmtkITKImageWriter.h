/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

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

class VTK_VMTK_ITK_EXPORT vtkvmtkITKImageWriter : public vtkImageAlgorithm
{
public:
  static vtkvmtkITKImageWriter *New();
  vtkTypeMacro(vtkvmtkITKImageWriter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

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
