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
 * @class   vtkvmtkUnstructuredGridFEVorticityAssembler
 * @brief   Construct a vorticity based finite element calculation on a mesh.
 * @ingroup DifferentialGeometry
 *
 * Assembles, over a vtkUnstructuredGrid, the finite-element right-hand side for a single component
 * (Direction) of the vorticity field (curl) of the 3-component point data velocity array named
 * VelocityArrayName. Used internally by vtkvmtkUnstructuredGridVorticityFilter.
 *
 * @sa vtkvmtkFEAssembler, vtkvmtkUnstructuredGridVorticityFilter
 */

#ifndef __vtkvmtkUnstructuredGridFEVorticityAssembler_h
#define __vtkvmtkUnstructuredGridFEVorticityAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFEVorticityAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFEVorticityAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFEVorticityAssembler,vtkvmtkFEAssembler);

  /**
   * Assemble the vorticity right-hand side for component Direction.
   */
  virtual void Build() override;

  ///@{
  /**
   * Set/Get the name of the 3-component point data array holding the velocity field.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the coordinate direction (0=x, 1=y, 2=z) of the vorticity component being assembled for.
   */
  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);
  ///@}

protected:
  vtkvmtkUnstructuredGridFEVorticityAssembler();
  ~vtkvmtkUnstructuredGridFEVorticityAssembler();

  char* VelocityArrayName;
  int Direction;

private:
  vtkvmtkUnstructuredGridFEVorticityAssembler(const vtkvmtkUnstructuredGridFEVorticityAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFEVorticityAssembler&);  // Not implemented.
};

#endif
