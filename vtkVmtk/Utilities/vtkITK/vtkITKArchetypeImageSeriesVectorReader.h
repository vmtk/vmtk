/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/vtkITK/vtkITKArchetypeImageSeriesVectorReader.h $
  Date:      $Date: 2006-12-21 13:21:52 +0100 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1900 $

==========================================================================*/

#ifndef __vtkITKArchetypeImageSeriesVectorReader_h
#define __vtkITKArchetypeImageSeriesVectorReader_h

#include "vtkITKArchetypeImageSeriesReader.h"

class VTK_ITK_EXPORT vtkITKArchetypeImageSeriesVectorReader : public vtkITKArchetypeImageSeriesReader
{
 public:
  static vtkITKArchetypeImageSeriesVectorReader *New();
  vtkTypeRevisionMacro(vtkITKArchetypeImageSeriesVectorReader,vtkITKArchetypeImageSeriesReader);
  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkITKArchetypeImageSeriesVectorReader();
  ~vtkITKArchetypeImageSeriesVectorReader();

  void ExecuteData(vtkDataObject *data);

  // private:
};

#endif
