/*=========================================================================
                                                                                                                                    
Program:   VMTK
Module:    $RCSfile: vtkvmtkXdaWriter.h,v $
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
// .NAME vtkvmtkXdaWriter - writes libmesh Xda files
// .SECTION Description
// vtkvmtkXdaWriter writes libmesh Xda files
//
// .SECTION See Also

#ifndef __vtkvmtkXdaWriter_h
#define __vtkvmtkXdaWriter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_IO_EXPORT vtkvmtkXdaWriter : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkXdaWriter *New();
  vtkTypeMacro(vtkvmtkXdaWriter,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);

protected:
  vtkvmtkXdaWriter();
  ~vtkvmtkXdaWriter();

  void WriteData() override;

  static void GetLibmeshConnectivity(int cellType, vtkIdList* libmeshConnectivity);
  static void GetLibmeshFaceOrder(int cellType, vtkIdList* libmeshFaceOrder);

  char* BoundaryDataArrayName;

private:
  vtkvmtkXdaWriter(const vtkvmtkXdaWriter&);  // Not implemented.
  void operator=(const vtkvmtkXdaWriter&);  // Not implemented.
};

#endif
