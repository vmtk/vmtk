/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkFDNEUTWriter.h,v $
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
// .NAME vtkvmtkFDNEUTWriter - writes FDNEUT Fidap files
// .SECTION Description
// vtkvmtkFDNEUTWriter writes FDNEUT Fidap files
//
// .SECTION See Also
// vtkvmtkFDNEUTWriter

#ifndef __vtkvmtkFDNEUTWriter_h
#define __vtkvmtkFDNEUTWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class VTK_VMTK_IO_EXPORT vtkvmtkFDNEUTWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkFDNEUTWriter *New();
  vtkTypeMacro(vtkvmtkFDNEUTWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //BTX
  enum
  {
    EDGE = 0,
    QUADRILATERAL,
    TRIANGLE,
    BRICK,
    WEDGE,
    TETRAHEDRON
  };
  //ETX

protected:
  vtkvmtkFDNEUTWriter();
  ~vtkvmtkFDNEUTWriter();

  void WriteData() override;

  static void ZeroToOneOffset(vtkIdType npts, vtkIdType* pts)
  { for (int i=0; i<npts; i++) ++pts[i]; }

private:
  vtkvmtkFDNEUTWriter(const vtkvmtkFDNEUTWriter&);  // Not implemented.
  void operator=(const vtkvmtkFDNEUTWriter&);  // Not implemented.
};

#endif
