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
  
  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkDolfinWriter2
 * @brief   Provides a lightly modified version of vtkvmtkDolfinWriter, with bug fixes and a new region indicators array.
 * @ingroup Contrib
 *
 * Slightly modified version of vtkvmtkDolfinWriter Modifications include bug fixes, and a new region indicators array which corresponds to the different cell entity Ids of the tetrahedra.
 *
 */

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
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the name of the input cell data array holding per-tetrahedron
   * region/entity ids. When set, its values (offset by CellEntityIdsOffset)
   * are written out as a Dolfin XML "mesh_function" region-indicator array,
   * and are also used to tag the boundary faces inherited from the
   * boundary triangle with the matching id.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/get the offset added to the values read from CellEntityIdsArrayName
   * before they are written to the Dolfin region-indicator array. Default: 0.
   */
  vtkSetMacro(CellEntityIdsOffset,int);
  vtkGetMacro(CellEntityIdsOffset,int);
  ///@}

protected:
  vtkvmtkDolfinWriter2();
  ~vtkvmtkDolfinWriter2();

  void WriteData() override;

  static void GetDolfinConnectivity(int cellType, vtkIdList* dolfinConnectivity);
  static void GetDolfinFaceOrder(int cellType, vtkIdList* dolfinFaceOrder);
  
  ///@{
  /**
   * Get the connectivity for a specific cell. The point ids are sorted in increasing order.
   */
  static void GetDolfinCellConnectivity(vtkCell *cell, vtkIdList* dolfinConnectivity);
  ///@}

  /**
   * Get the face order for a specific cell.
   */
  static void GetDolfinCellFaceOrder(vtkCell *cell, vtkIdList* dolfinFaceOrder);

  char* CellEntityIdsArrayName;
  int CellEntityIdsOffset;

private:
  vtkvmtkDolfinWriter2(const vtkvmtkDolfinWriter2&);  // Not implemented.
  void operator=(const vtkvmtkDolfinWriter2&);  // Not implemented.
};

#endif
