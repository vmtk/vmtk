/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkXdaReader.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.3 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
// .NAME vtkvmtkXdaReader - reads libmesh Xda files
// .SECTION Description
// vtkvmtkXdaReader reads libmesh Xda files
//
// .SECTION See Also

#ifndef __vtkvmtkXdaReader_h
#define __vtkvmtkXdaReader_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridReader.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkXdaReader : public vtkUnstructuredGridReader
{
public:
  static vtkvmtkXdaReader *New();
  vtkTypeMacro(vtkvmtkXdaReader,vtkUnstructuredGridReader);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

#if (VTK_MAJOR_VERSION >= 9 && VTK_MINOR_VERSION >= 1)
    int ReadMeshSimple(VTK_FILEPATH const std::string& fname, vtkDataObject* output) override;
#endif

protected:
  vtkvmtkXdaReader();
  ~vtkvmtkXdaReader();

#if (VTK_MAJOR_VERSION >= 9 && VTK_MINOR_VERSION >= 1)

#else
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
#endif

  static void GetLibmeshConnectivity(int cellType, vtkIdList* libmeshConnectivity);

  char* BoundaryDataArrayName;

private:
  vtkvmtkXdaReader(const vtkvmtkXdaReader&);  // Not implemented.
  void operator=(const vtkvmtkXdaReader&);  // Not implemented.
};

#endif
