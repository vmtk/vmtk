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
 * @class   vtkvmtkUnstructuredGridNeighborhood
 * @brief   Apply a neighborhood to the vertices on of a unstructured grid data set.
 * @ingroup DifferentialGeometry
 *
 * Concrete vtkvmtkNeighborhood implementation for a vtkUnstructuredGrid: Build() collects, for the
 * point set via SetDataSetPointId, the ids of every other point sharing a cell with it (its
 * one-ring neighbors in the volumetric mesh).
 *
 * @sa vtkvmtkNeighborhood, vtkvmtkNeighborhoods
 */

#ifndef __vtkvmtkUnstructuredGridNeighborhood_h
#define __vtkvmtkUnstructuredGridNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkUnstructuredGridNeighborhood *New();
  vtkTypeMacro(vtkvmtkUnstructuredGridNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_UNSTRUCTUREDGRID_NEIGHBORHOOD;};

  /**
   * Build the neighborhood.
   */
  virtual void Build() override;

protected:
  vtkvmtkUnstructuredGridNeighborhood() {};
  ~vtkvmtkUnstructuredGridNeighborhood() {};

private:
  vtkvmtkUnstructuredGridNeighborhood(const vtkvmtkUnstructuredGridNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridNeighborhood&);  // Not implemented.
};

#endif

