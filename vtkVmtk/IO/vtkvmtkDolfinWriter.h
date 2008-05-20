/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkDolfinWriter.h,v $
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

// .NAME vtkvmtkDolfinWriter - 
// .SECTION Description
// vtkvmtkDolfinWriter writes libmesh Dolfin files

// .SECTION See Also

#ifndef __vtkvmtkDolfinWriter_h
#define __vtkvmtkDolfinWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkDolfinWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkDolfinWriter *New();
  vtkTypeRevisionMacro(vtkvmtkDolfinWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

protected:
  vtkvmtkDolfinWriter();
  ~vtkvmtkDolfinWriter();

  void WriteData();

  static void GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity);
  static void GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder);

  char* BoundaryDataArrayName;

private:
  vtkvmtkDolfinWriter(const vtkvmtkDolfinWriter&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter&);  // Not implemented.
};

#endif
