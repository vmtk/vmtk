/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga 

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
 * @class   vtkvmtkDirichletBoundaryConditions
 * @brief   Set up Dirichlet (or first-type) boundary conditions that a linear system needs to take along the boundary of the domain.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkDirichletBoundaryConditions imposes a Dirichlet (essential) boundary condition on
 * LinearSystem's sparse matrix A and right-hand side vector B: for every node listed in
 * BoundaryNodes, the corresponding row of A is zeroed out except for a unit diagonal entry, and B
 * is set to the prescribed BoundaryValues entry, so that the solution x is forced to take that
 * value at the node. To preserve symmetry of the reduced system, the boundary node's column is also
 * eliminated from every other row, moving its known contribution to the right-hand side. This is
 * the standard way vmtk's finite-element solvers (e.g. distance-to-boundary / Laplacian-type
 * problems) impose fixed values on a subset of nodes, typically the mesh boundary.
 *
 * @sa
 * vtkvmtkBoundaryConditions, vtkvmtkLinearSystem, vtkvmtkEllipticProblem
 */

#ifndef __vtkvmtkDirichletBoundaryConditions_h
#define __vtkvmtkDirichletBoundaryConditions_h

#include "vtkObject.h"
#include "vtkvmtkBoundaryConditions.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDirichletBoundaryConditions : public vtkvmtkBoundaryConditions
{
public:
  static vtkvmtkDirichletBoundaryConditions* New();
  vtkTypeMacro(vtkvmtkDirichletBoundaryConditions,vtkvmtkBoundaryConditions);

  void Apply() override;

protected:
  vtkvmtkDirichletBoundaryConditions() {};
  ~vtkvmtkDirichletBoundaryConditions() {};

private:
  vtkvmtkDirichletBoundaryConditions(const vtkvmtkDirichletBoundaryConditions&);  // Not implemented.
  void operator=(const vtkvmtkDirichletBoundaryConditions&);  // Not implemented.
};

#endif

