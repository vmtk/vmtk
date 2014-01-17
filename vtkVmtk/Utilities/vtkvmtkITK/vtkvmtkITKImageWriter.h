/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKImageWriter.h $
  Date:      $Date: 2006-12-21 07:21:52 -0500 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

// .NAME vtkvmtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkvmtkITKImageToImageFilter provides a 

#ifndef __vtkvmtkITKImageWriter_h
#define __vtkvmtkITKImageWriter_h

#include "vtkProcessObject.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

#include "vtkvmtkITK.h"

class VTK_VMTK_ITK_EXPORT vtkvmtkITKImageWriter : public vtkProcessObject
{
public:

  static vtkvmtkITKImageWriter *New();
  vtkTypeMacro(vtkvmtkITKImageWriter,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Specify file name for the image file. You should specify either
  // a FileName or a FilePrefix. Use FilePrefix if the data is stored 
  // in multiple files.
  void SetFileName(const char *);

  char *GetFileName() {
    return FileName;
  }

  // Description:
  // use compression if possible
  vtkGetMacro (UseCompression, int);
  vtkSetMacro (UseCompression, int);

  // Description:
  // Set/Get the input object from the image pipeline.
  void SetInput(vtkImageData *input);

  vtkImageData *GetInput();

  // Description:
  // The main interface which triggers the writer to start.
  void Write();

  // Set orienation matrix
  void SetRasToIJKMatrix( vtkMatrix4x4* mat) {
    RasToIJKMatrix = mat;
  }

protected:
  vtkvmtkITKImageWriter();
  ~vtkvmtkITKImageWriter();

  char *FileName;
  vtkMatrix4x4* RasToIJKMatrix;
  int UseCompression;

private:
  vtkvmtkITKImageWriter(const vtkvmtkITKImageWriter&);  // Not implemented.
  void operator=(const vtkvmtkITKImageWriter&);  // Not implemented.
};

//vtkStandardNewMacro(vtkvmtkITKImageWriter)

#endif





