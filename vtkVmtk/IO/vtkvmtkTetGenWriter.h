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
 * @class   vtkvmtkTetGenWriter
 * @brief   Writes TetGen .msh files.
 * @ingroup IO
 *
 * vtkvmtkTetGenWriter writes TetGen .msh files. Many thanks to M. Xenos, Y. Alemu and D. Bluestein, BioFluids Laboratory, Stony Brook University, Stony Brook, NY, for the inputs on the file format.
 *
 *
 */

#ifndef __vtkvmtkTetGenWriter_h
#define __vtkvmtkTetGenWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkTetGenWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkTetGenWriter *New();
  vtkTypeMacro(vtkvmtkTetGenWriter,vtkUnstructuredGridWriter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the name of the cell data array intended to hold boundary marker values to be written out
   * alongside node/element data (mirroring vtkvmtkTetGenReader::BoundaryDataArrayName). Not currently
   * used by WriteData: the .node/.ele files this writer produces have no attribute/boundary-marker
   * columns.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);
  ///@}

protected:
  vtkvmtkTetGenWriter();
  ~vtkvmtkTetGenWriter();

  void ConvertFaceToLeftHanded(vtkUnstructuredGrid* input, vtkIdType tetraCellId, vtkIdType& id0, vtkIdType& id1, vtkIdType& id2);

  void WriteData() override;

  char* BoundaryDataArrayName;

private:
  vtkvmtkTetGenWriter(const vtkvmtkTetGenWriter&);  // Not implemented.
  void operator=(const vtkvmtkTetGenWriter&);  // Not implemented.
};

#endif
