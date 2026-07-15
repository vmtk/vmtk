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
 * @class   vtkvmtkPolyDataNeighborhood
 * @brief   Build the one-ring neighborhood of a point on a vtkPolyData surface mesh.
 * @ingroup DifferentialGeometry
 *
 * Given a point id set on the underlying vtkvmtkNeighborhood (via SetDataSet/SetDataSetPointId)
 * and a vtkPolyData data set, Build() collects the ids of all points connected to that point by a
 * shared cell (its one-ring / umbrella neighborhood) and stores them for later access through the
 * inherited vtkvmtkNeighborhood point-id array. This is the neighborhood implementation used by
 * mesh-processing classes that operate on triangulated surfaces, and is instantiated internally
 * by classes such as vtkvmtkSparseMatrix when building neighborhoods from a vtkPolyData input.
 *
 * @sa vtkvmtkNeighborhood, vtkvmtkUnstructuredGridNeighborhood, vtkvmtkStencil
 */

#ifndef __vtkvmtkPolyDataNeighborhood_h
#define __vtkvmtkPolyDataNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkPolyDataNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataNeighborhood,vtkvmtkNeighborhood);

  /**
   * Return the neighborhood item type identifier (VTK_VMTK_POLYDATA_NEIGHBORHOOD), used by
   * vtkvmtkNeighborhoods/vtkvmtkItems factories to instantiate the correct neighborhood subclass.
   */
  virtual vtkIdType GetItemType() override {return VTK_VMTK_POLYDATA_NEIGHBORHOOD;};

  /**
   * Build the neighborhood.
   */
  virtual void Build() override;

protected:
  vtkvmtkPolyDataNeighborhood() {};
  ~vtkvmtkPolyDataNeighborhood() {};

private:
  vtkvmtkPolyDataNeighborhood(const vtkvmtkPolyDataNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataNeighborhood&);  // Not implemented.
};

#endif

