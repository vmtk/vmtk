/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkDolfinWriter.h,v $
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
// .NAME vtkvmtkDolfinWriter - write mesh data in Dolfin file formats.
// .SECTION Description
// vtkvmtkDolfinWriter writes Dolfin files - www.fenics.org.
// Compatible file formats include:
//     * Binary (.bin)
//     * RAW    (.raw)
//     * SVG    (.svg)
//     * XD3    (.xd3)
//     * XML    (.xml)
//     * XYZ    (.xyz)
//     * VTK    (.pvd)
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
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

  vtkSetMacro(BoundaryDataIdOffset,int);
  vtkGetMacro(BoundaryDataIdOffset,int);

  vtkSetMacro(StoreCellMarkers,int);
  vtkGetMacro(StoreCellMarkers,int);

protected:
  vtkvmtkDolfinWriter();
  ~vtkvmtkDolfinWriter();

  void WriteData() override;

  char* BoundaryDataArrayName;
  int BoundaryDataIdOffset;
  int StoreCellMarkers;

private:
  vtkvmtkDolfinWriter(const vtkvmtkDolfinWriter&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter&);  // Not implemented.
};

#endif
