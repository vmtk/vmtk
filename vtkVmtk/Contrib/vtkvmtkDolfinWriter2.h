/*=========================================================================
                                                                                                                                    
  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDolfinWriter2.h,v $
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
  
  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
// .NAME vtkvmtkDolfinWriter2 - lightly modified version of vtkvmtkDolfinWriter Modifications include bug fixes, and a new region indicators array.
// .SECTION Description
// Slightly modified version of vtkvmtkDolfinWriter Modifications include bug fixes, and a new region indicators array which corresponds to the different cell entity Ids of the tetrahedra.
// .SECTION See Also

#ifndef __vtkvmtkDolfinWriter2_h
#define __vtkvmtkDolfinWriter2_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridWriter.h"

class vtkCell;
class vtkIdList;

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkDolfinWriter2 : public vtkUnstructuredGridWriter
{
public:
  static vtkvmtkDolfinWriter2 *New();
  vtkTypeMacro(vtkvmtkDolfinWriter2,vtkUnstructuredGridWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(CellEntityIdsOffset,int);
  vtkGetMacro(CellEntityIdsOffset,int);

protected:
  vtkvmtkDolfinWriter2();
  ~vtkvmtkDolfinWriter2();

  void WriteData() override;

  static void GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity);
  static void GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder);
  
  //Description:
  //Get the connectivity for a specific cell. The point ids are sorted in increasing order
  static void GetDolfinCellConnectivity(vtkCell *cell, vtkIdList* dolfinConnectivity);
  //Description:
  //Get the face order for a specific cell
  static void GetDolfinCellFaceOrder(vtkCell *cell, vtkIdList* dolfinFaceOrder);

  char* CellEntityIdsArrayName;
  int CellEntityIdsOffset;

private:
  vtkvmtkDolfinWriter2(const vtkvmtkDolfinWriter2&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter2&);  // Not implemented.
};

#endif
