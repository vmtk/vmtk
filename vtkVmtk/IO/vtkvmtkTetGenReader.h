/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    vtkvmtkTetGenReader.h
Language:  C++
Date:      Sat Feb 19 15:15:16 CET 2011
Version:   Revision: 1.0
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
// .NAME vtkvmtkTetGenReader - reads unstructured grid data from Tetgen node/elem format
// .SECTION Description
// vtkvmtkTetGenReader reads unstructured grid data from Tetgen node/elem format
// Thanks to Sebastian Ordas for getting the class going.

#ifndef __vtkvmtkTetGenReader_h
#define __vtkvmtkTetGenReader_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridReader.h"

#include <vector>

// VTK_FILEPATH hint was introduced in VTK_VERSION_CHECK(9,1,0)
// (https://github.com/Kitware/VTK/commit/c30ddf9a6caedd65ae316080b0efd1833983844e)
#ifndef VTK_FILEPATH
#define VTK_FILEPATH
#endif

class VTK_VMTK_IO_EXPORT vtkvmtkTetGenReader : public vtkUnstructuredGridReader
{
public:
  vtkTypeMacro(vtkvmtkTetGenReader,vtkUnstructuredGridReader);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkTetGenReader *New();

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

  int ReadMeshSimple(VTK_FILEPATH const std::string& fname, vtkDataObject* output) override;

protected:
  vtkvmtkTetGenReader();
  ~vtkvmtkTetGenReader();

//BTX
  void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
//ETX

  char* BoundaryDataArrayName;

private:
  vtkvmtkTetGenReader(const vtkvmtkTetGenReader&);  // Not implemented.
  void operator=(const vtkvmtkTetGenReader&);  // Not implemented.
};

#endif
