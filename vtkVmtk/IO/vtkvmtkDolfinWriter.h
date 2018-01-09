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
// vtkvmtkDolfinWriter writes Dolfin files - www.fenics.org

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
  vtkTypeMacro(vtkvmtkDolfinWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

  vtkSetMacro(BoundaryDataIdOffset,int);
  vtkGetMacro(BoundaryDataIdOffset,int);

  vtkSetMacro(StoreCellMarkers,int);
  vtkGetMacro(StoreCellMarkers,int);

protected:
  vtkvmtkDolfinWriter();
  ~vtkvmtkDolfinWriter();

  void WriteData() VTK_OVERRIDE;

  char* BoundaryDataArrayName;
  int BoundaryDataIdOffset;
  int StoreCellMarkers;

private:
  vtkvmtkDolfinWriter(const vtkvmtkDolfinWriter&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter&);  // Not implemented.
};

#endif
