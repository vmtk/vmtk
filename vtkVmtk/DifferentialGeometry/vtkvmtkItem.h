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
 * @class   vtkvmtkItem
 * @brief   Serves as the base class for constructing neighborhoods and stencils from a set of points.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkItem is the abstract root of vmtk's differential-geometry "item" hierarchy (see
 * vtkvmtkDataSetItem, vtkvmtkNeighborhood, vtkvmtkStencil): a lightweight, individually
 * reference-counted object representing one per-point piece of local data (a topological
 * neighborhood, a finite-difference/finite-element stencil, etc.), meant to be held in bulk by a
 * matching vtkvmtkItems collection. GetItemType() identifies the concrete item type so that
 * collections can instantiate the right subclass, and DeepCopy() is the standard mechanism for
 * copying one item's data into another of the same type.
 *
 * @sa
 * vtkvmtkItems, vtkvmtkDataSetItem, vtkvmtkNeighborhood
 */

#ifndef __vtkvmtkItem_h
#define __vtkvmtkItem_h

#include "vtkObject.h"
#include "vtkDataSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkItem : public vtkObject 
{
public:

  vtkTypeMacro(vtkvmtkItem,vtkObject);

  /**
   * Get an identifier for the concrete item type (one of the VTK_VMTK_*_NEIGHBORHOOD /
   * VTK_VMTK_*_STENCIL constants), used by owning vtkvmtkItems collections to instantiate the
   * correct subclass.
   */
  virtual vtkIdType GetItemType() = 0;

  /**
   * Standard DeepCopy method.  Since this object contains no reference to other objects, there is no
   * ShallowCopy.
   */
  virtual void DeepCopy(vtkvmtkItem *src);

protected:
  vtkvmtkItem() {};
  ~vtkvmtkItem() {};

private:
  vtkvmtkItem(const vtkvmtkItem&);  // Not implemented.
  void operator=(const vtkvmtkItem&);  // Not implemented.
};

#endif

