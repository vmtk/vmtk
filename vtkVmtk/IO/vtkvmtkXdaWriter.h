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
 * @class   vtkvmtkXdaWriter
 * @brief   Writes libmesh Xda files.
 * @ingroup IO
 *
 * vtkvmtkXdaWriter writes libmesh Xda files
 *
 *
 */

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
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the name of the input cell data array holding integer boundary marker values for boundary
   * (triangle/quad) cells. If set, one boundary-condition line per boundary cell is written to the
   * output, giving the owning volume element, the libMesh-numbered local face id, and this marker
   * value. If left NULL (default), no boundary condition section is written.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(BoundaryDataArrayName);
  vtkGetStringMacro(BoundaryDataArrayName);
  ///@}

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
