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
 * @class   vtkvmtkEllipticProblem
 * @brief   Serves as a high-level container holding references to a vtkvmtkLinearSystem and its associated vtkvmtkDirichletBoundaryConditions.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkEllipticProblem groups together the pieces that make up a discretized elliptic PDE
 * (e.g. a Laplace/Poisson-type problem): a vtkvmtkLinearSystem holding the assembled system matrix
 * and vectors, and a vtkvmtkDirichletBoundaryConditions object holding the essential boundary
 * conditions to be applied to it. This base class currently only declares the two member
 * references; assembling the system and driving the solve is left to subclasses / calling code.
 *
 * @sa
 * vtkvmtkLinearSystem, vtkvmtkDirichletBoundaryConditions, vtkvmtkFEAssembler
 */

#ifndef __vtkvmtkEllipticProblem_h
#define __vtkvmtkEllipticProblem_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkEllipticProblem : public vtkObject
{
public:
  static vtkvmtkEllipticProblem* New();
  vtkTypeMacro(vtkvmtkEllipticProblem,vtkObject);

protected:
  vtkvmtkEllipticProblem();
  ~vtkvmtkEllipticProblem();

  vtkvmtkLinearSystem* LinearSystem;
  vtkvmtkDirichletBoundaryConditions* DirichletBoundaryConditions;

private:
  vtkvmtkEllipticProblem(const vtkvmtkEllipticProblem&);  // Not implemented.
  void operator=(const vtkvmtkEllipticProblem&);  // Not implemented.
};

#endif

