/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#ifndef __vtkvmtkITKArchetypeImageSeriesScalarReader_h
#define __vtkvmtkITKArchetypeImageSeriesScalarReader_h

#include "vtkvmtkITKArchetypeImageSeriesReader.h"

#include "itkImageFileReader.h"

class VTK_VMTK_ITK_EXPORT vtkvmtkITKArchetypeImageSeriesScalarReader : public vtkvmtkITKArchetypeImageSeriesReader
{
 public:
  static vtkvmtkITKArchetypeImageSeriesScalarReader *New();
  vtkTypeMacro(vtkvmtkITKArchetypeImageSeriesScalarReader,vtkvmtkITKArchetypeImageSeriesReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

 protected:
  vtkvmtkITKArchetypeImageSeriesScalarReader();
  ~vtkvmtkITKArchetypeImageSeriesScalarReader();

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override;
  static void ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&, void* data);
};

#endif

