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
 * @class   vtkvmtkBoundaryConditions
 * @brief   Serves as the base class that stores nodes, value, and the linear system composing a boundary condition.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkBoundaryConditions holds the three ingredients needed to impose a boundary condition on a
 * finite-element linear system: the vtkvmtkLinearSystem to modify, the list of node (row) ids the
 * condition applies to (BoundaryNodes), and the prescribed value at each of those nodes
 * (BoundaryValues). Apply() checks that all three are consistently set (in particular that
 * BoundaryNodes and BoundaryValues have the same length) but performs no actual modification of the
 * linear system; concrete subclasses (e.g. vtkvmtkDirichletBoundaryConditions) override Apply() to
 * carry out the specific boundary condition type.
 *
 * @sa
 * vtkvmtkDirichletBoundaryConditions, vtkvmtkLinearSystem, vtkvmtkEllipticProblem
 */

#ifndef __vtkvmtkBoundaryConditions_h
#define __vtkvmtkBoundaryConditions_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkBoundaryConditions : public vtkObject
{
public:

  vtkTypeMacro(vtkvmtkBoundaryConditions,vtkObject);

  ///@{
  /**
   * Set/get the linear system that the boundary condition is applied to when Apply() is called.
   */
  vtkSetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  vtkGetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  ///@}

  ///@{
  /**
   * Set/get the list of node (linear system row) ids that the boundary condition applies to, in the
   * same order as BoundaryValues.
   */
  vtkSetObjectMacro(BoundaryNodes,vtkIdList);
  vtkGetObjectMacro(BoundaryNodes,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the prescribed value at each node listed in BoundaryNodes, one value per node and in the
   * same order.
   */
  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);
  ///@}

  /**
   * Impose the boundary condition on LinearSystem. This base class implementation only validates
   * that LinearSystem, BoundaryNodes, and BoundaryValues are set and mutually consistent (same
   * number of nodes and values); subclasses override this method to actually modify the system
   * matrix and right-hand side.
   */
  virtual void Apply();

protected:
  vtkvmtkBoundaryConditions();
  ~vtkvmtkBoundaryConditions();

  vtkvmtkLinearSystem* LinearSystem;

  vtkIdList* BoundaryNodes;
  vtkDoubleArray* BoundaryValues;

private:
  vtkvmtkBoundaryConditions(const vtkvmtkBoundaryConditions&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryConditions&);  // Not implemented.
};

#endif

