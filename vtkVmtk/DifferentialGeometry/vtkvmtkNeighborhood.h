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
 * @class   vtkvmtkNeighborhood
 * @brief   Query the neighborhood surrounding the set point id in an data set item.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkNeighborhood is the abstract base class for a data-set item representing the set of point
 * ids topologically adjacent to DataSetPointId (a "1-ring" or similar local neighborhood, depending
 * on the concrete subclass -- e.g. poly data cell-adjacency, or manifold-surface vertex rings).
 * Build() (implemented by subclasses) fills PointIds with the NPoints neighboring point ids and
 * sets IsBoundary if DataSetPointId lies on a mesh boundary. Instances are normally created and
 * owned in bulk by a vtkvmtkNeighborhoods collection, one per point of a data set, and consumed by
 * finite-difference/finite-element stencil classes (see vtkvmtkStencil) to compute derivative
 * weights.
 *
 * @sa
 * vtkvmtkNeighborhoods, vtkvmtkDataSetItem, vtkvmtkStencil
 */

#ifndef __vtkvmtkNeighborhood_h
#define __vtkvmtkNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkDataSetItem.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkNeighborhood : public vtkvmtkDataSetItem 
{
public:

  vtkTypeMacro(vtkvmtkNeighborhood,vtkvmtkDataSetItem);

  /**
   * Get whether DataSetPointId lies on a boundary of the data set (meaning, for concrete
   * subclasses, that it does not have a full/closed neighborhood). Valid only after Build() has
   * been called.
   */
  vtkGetMacro(IsBoundary,bool);

  /**
   * Get the number of neighboring point ids stored in this neighborhood. Valid only after Build()
   * has been called.
   */
  vtkIdType GetNumberOfPoints() {return this->NPoints;};

  /**
   * Get the i-th neighboring point id (0 <= i < GetNumberOfPoints()).
   */
  vtkIdType GetPointId(vtkIdType i) {return this->PointIds[i];};

  /**
   * Get a pointer into the internal point id array starting at index i, e.g. for passing a
   * contiguous run of ids to a VTK API expecting a raw vtkIdType array.
   */
  vtkIdType *GetPointer(vtkIdType i) {return this->PointIds+i;};

  /**
   * Build the neighborhood.
   */
  virtual void Build() override = 0;

  /**
   * Standard DeepCopy method.
   */
  virtual void DeepCopy(vtkvmtkItem *src) override;

protected:
  vtkvmtkNeighborhood();
  ~vtkvmtkNeighborhood();

  void ResizePointList(vtkIdType ptId, int size);

  vtkIdType NPoints;
  vtkIdType* PointIds;
  bool IsBoundary;

private:
  vtkvmtkNeighborhood(const vtkvmtkNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkNeighborhood&);  // Not implemented.
};

#endif

