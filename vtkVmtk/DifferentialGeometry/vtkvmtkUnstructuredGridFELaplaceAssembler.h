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
 * @class   vtkvmtkUnstructuredGridFELaplaceAssembler
 * @brief   Construct a laplacian based finite element calculation on a mesh.
 * @ingroup DifferentialGeometry
 *
 * Assembles, over a vtkUnstructuredGrid, the finite-element stiffness matrix for the Laplace
 * operator (used e.g. by vtkvmtkUnstructuredGridHarmonicMappingFilter to solve for a harmonic
 * function with given boundary values).
 *
 * @sa vtkvmtkFEAssembler, vtkvmtkUnstructuredGridHarmonicMappingFilter
 */

#ifndef __vtkvmtkUnstructuredGridFELaplaceAssembler_h
#define __vtkvmtkUnstructuredGridFELaplaceAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFELaplaceAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFELaplaceAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFELaplaceAssembler,vtkvmtkFEAssembler);

  /**
   * Assemble the Laplace stiffness matrix.
   */
  virtual void Build() override;

protected:
  vtkvmtkUnstructuredGridFELaplaceAssembler();
  ~vtkvmtkUnstructuredGridFELaplaceAssembler();

private:
  vtkvmtkUnstructuredGridFELaplaceAssembler(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFELaplaceAssembler&);  // Not implemented.
};

#endif
