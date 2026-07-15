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
 * @class   vtkvmtkXdaReader
 * @brief   Reads libmesh Xda files.
 * @ingroup IO
 *
 * vtkvmtkXdaReader is intended to read unstructured grid data from the libmesh Xda ASCII mesh format
 * (the counterpart of vtkvmtkXdaWriter). Currently ReadMeshSimple() is a stub that does not parse the
 * file; the "xda" format option of the vmtkmeshreader pype script explicitly reports it as not yet
 * implemented and refuses to use this class.
 *
 * @sa
 * vtkvmtkXdaWriter
 */

#ifndef __vtkvmtkXdaReader_h
#define __vtkvmtkXdaReader_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridReader.h"

class vtkCell;
class vtkIdList;

// VTK_FILEPATH hint was introduced in VTK_VERSION_CHECK(9,1,0)
// (https://github.com/Kitware/VTK/commit/c30ddf9a6caedd65ae316080b0efd1833983844e)
#ifndef VTK_FILEPATH
#define VTK_FILEPATH
#endif

class VTK_VMTK_IO_EXPORT vtkvmtkXdaReader : public vtkUnstructuredGridReader
{
public:
  static vtkvmtkXdaReader *New();
  vtkTypeMacro(vtkvmtkXdaReader,vtkUnstructuredGridReader);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the name of the cell data array intended to hold boundary marker values (mirroring
   * vtkvmtkXdaWriter::BoundaryDataArrayName). Unused while ReadMeshSimple() remains a stub.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);
  ///@}

  int ReadMeshSimple(VTK_FILEPATH const std::string& fname, vtkDataObject* output) override;

protected:
  vtkvmtkXdaReader();
  ~vtkvmtkXdaReader();

  static void GetLibmeshConnectivity(int cellType, vtkIdList* libmeshConnectivity);

  char* BoundaryDataArrayName;

private:
  vtkvmtkXdaReader(const vtkvmtkXdaReader&);  // Not implemented.
  void operator=(const vtkvmtkXdaReader&);  // Not implemented.
};

#endif
