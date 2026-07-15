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
 * @class   vtkvmtkFDNEUTWriter
 * @brief   Writes FDNEUT Fidap files.
 * @ingroup IO
 *
 * vtkvmtkFDNEUTWriter writes an unstructured grid to the Fidap FDNEUT neutral file format. Cells are
 * grouped by VTK cell type, each group being written as one FDNEUT "ELEMENT GROUP" (in decreasing
 * cell-type order); node ids are 1-based in the output, per the FDNEUT convention. This is the
 * writer behind the "fdneut" format option of the vmtkmeshwriter pype script.
 *
 * @sa
 * vtkvmtkFDNEUTReader
 */

#ifndef __vtkvmtkFDNEUTWriter_h
#define __vtkvmtkFDNEUTWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class VTK_VMTK_IO_EXPORT vtkvmtkFDNEUTWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkFDNEUTWriter *New();
  vtkTypeMacro(vtkvmtkFDNEUTWriter,vtkUnstructuredGridWriter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

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
