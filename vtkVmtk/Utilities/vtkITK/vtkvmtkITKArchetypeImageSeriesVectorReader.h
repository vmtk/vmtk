/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkvmtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkvmtkITK/vtkvmtkITKArchetypeImageSeriesVectorReader.h $
  Date:      $Date: 2007-01-19 13:21:56 -0500 (Fri, 19 Jan 2007) $
  Version:   $Revision: 2267 $

==========================================================================*/

#ifndef __vtkvmtkITKArchetypeImageSeriesVectorReader_h
#define __vtkvmtkITKArchetypeImageSeriesVectorReader_h

#include "vtkvmtkITKArchetypeImageSeriesReader.h"

#include "itkImageFileReader.h"

class VTK_VMTK_ITK_EXPORT vtkvmtkITKArchetypeImageSeriesVectorReader : public vtkvmtkITKArchetypeImageSeriesReader
{
 public:
  static vtkvmtkITKArchetypeImageSeriesVectorReader *New();
  vtkTypeRevisionMacro(vtkvmtkITKArchetypeImageSeriesVectorReader,vtkvmtkITKArchetypeImageSeriesReader);
  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkvmtkITKArchetypeImageSeriesVectorReader();
  ~vtkvmtkITKArchetypeImageSeriesVectorReader();

  void ExecuteData(vtkDataObject *data);
//BTX
  static void ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&, void* data);
//ETX
  // private:
};

#endif
