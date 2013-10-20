/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKArchetypeImageSeriesScalarReader.h $
  Date:      $Date: 2010-01-22 20:45:00 +0100 (Fri, 22 Jan 2010) $
  Version:   $Revision: 11747 $

==========================================================================*/

#ifndef __vtkvmtkITKArchetypeImageSeriesScalarReader_h
#define __vtkvmtkITKArchetypeImageSeriesScalarReader_h

#include "vtkvmtkITKArchetypeImageSeriesReader.h"

#include "itkImageFileReader.h"

class VTK_VMTK_ITK_EXPORT vtkvmtkITKArchetypeImageSeriesScalarReader : public vtkvmtkITKArchetypeImageSeriesReader
{
 public:
  static vtkvmtkITKArchetypeImageSeriesScalarReader *New();
  vtkTypeRevisionMacro(vtkvmtkITKArchetypeImageSeriesScalarReader,vtkvmtkITKArchetypeImageSeriesReader);
  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkvmtkITKArchetypeImageSeriesScalarReader();
  ~vtkvmtkITKArchetypeImageSeriesScalarReader();

  void ExecuteData(vtkDataObject *data);
//BTX
  static void ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&, void* data);
//ETX
  /// private:
};

#endif
