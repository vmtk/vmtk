/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFluentWriter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:47 $
Version:   $Revision: 1.2 $
                                                                                                                                    
  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
                                                                                                                                    
=========================================================================*/
// .NAME vtkvmtkFluentWriter - writes Fluent .msh files.
// .SECTION Description
// vtkvmtkFluentWriter writes Fluent .msh files. Many thanks to M. Xenos, Y. Alemu and D. Bluestein, BioFluids Laboratory, Stony Brook University, Stony Brook, NY, for the inputs on the file format.
//
// .SECTION See Also

#ifndef __vtkvmtkFluentWriter_h
#define __vtkvmtkFluentWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkFluentWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkFluentWriter *New();
  vtkTypeMacro(vtkvmtkFluentWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

protected:
  vtkvmtkFluentWriter();
  ~vtkvmtkFluentWriter();

  void ConvertFaceToLeftHanded(vtkUnstructuredGrid* input, vtkIdType tetraCellId, vtkIdType& id0, vtkIdType& id1, vtkIdType& id2);

  void WriteData() override;

  char* BoundaryDataArrayName;

private:
  vtkvmtkFluentWriter(const vtkvmtkFluentWriter&);  // Not implemented.
  void operator=(const vtkvmtkFluentWriter&);  // Not implemented.
};

#endif
