/*=========================================================================
                                                                                                                                    
Program:   VMTK
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
/**
 * @class   vtkvmtkTetGenReader
 * @brief   Reads unstructured grid data from Tetgen node/elem format.
 * @ingroup IO
 *
 * vtkvmtkTetGenReader reads unstructured grid data from Tetgen node/elem format
 * Thanks to Sebastian Ordas for getting the class going.
 */

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

  ///@{
  /**
   * Set/get the name used for the point data and cell data arrays that store TetGen boundary marker
   * values. Point boundary markers are read verbatim from the .node file (when present); the
   * per-tetrahedron cell marker is the maximum node boundary marker found among a tetrahedron's
   * points (excluding the first). If TetGen's .node file has no boundary marker column, no arrays are
   * added.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);
  ///@}

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
