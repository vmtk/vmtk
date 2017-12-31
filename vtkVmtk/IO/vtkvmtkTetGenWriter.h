/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkTetGenWriter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.2 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/

// .NAME vtkvmtkTetGenWriter - 
// .SECTION Description
// vtkvmtkTetGenWriter writes TetGen .msh files. Many thanks to M. Xenos, Y. Alemu and D. Bluestein, BioFluids Laboratory, Stony Brook University, Stony Brook, NY, for the inputs on the file format.

// .SECTION See Also

#ifndef __vtkvmtkTetGenWriter_h
#define __vtkvmtkTetGenWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkTetGenWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkTetGenWriter *New();
  vtkTypeMacro(vtkvmtkTetGenWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

protected:
  vtkvmtkTetGenWriter();
  ~vtkvmtkTetGenWriter();

  void ConvertFaceToLeftHanded(vtkUnstructuredGrid* input, vtkIdType tetraCellId, vtkIdType& id0, vtkIdType& id1, vtkIdType& id2);

  void WriteData() VTK_OVERRIDE;

  char* BoundaryDataArrayName;

private:
  vtkvmtkTetGenWriter(const vtkvmtkTetGenWriter&);  // Not implemented.
  void operator=(const vtkvmtkTetGenWriter&);  // Not implemented.
};

#endif
