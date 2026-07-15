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
 * @class   vtkvmtkDataSetItems
 * @brief   Create a collection of data set items, one for every point within the data set.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkDataSetItems specializes vtkvmtkItems for the common case where the collection holds
 * exactly one vtkvmtkDataSetItem per point of a vtkDataSet. Build() allocates (if needed) one item
 * per point of DataSet, then, for every point, assigns DataSet and the point id to the
 * corresponding vtkvmtkDataSetItem and calls its Build() method. This is the base class of
 * vtkvmtkNeighborhoods, which uses it to build, for every point of an input data set, the
 * neighborhood item appropriate to the data set type (poly data, unstructured grid, manifold
 * surface, ...).
 *
 * @sa
 * vtkvmtkItems, vtkvmtkDataSetItem, vtkvmtkNeighborhoods
 */

#ifndef __vtkvmtkDataSetItems_h
#define __vtkvmtkDataSetItems_h

#include "vtkObject.h"
#include "vtkvmtkItems.h"
#include "vtkDataSet.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDataSetItems : public vtkvmtkItems 
{
public:

  vtkTypeMacro(vtkvmtkDataSetItems,vtkvmtkItems);

  ///@{
  /**
   * Set/get the data set whose points the item collection is built from. The reference is stored
   * but not registered (no reference counting), so the data set must be kept alive by the caller.
   */
/*   vtkSetObjectMacro(DataSet,vtkDataSet); */
/*   vtkGetObjectMacro(DataSet,vtkDataSet); */
  void SetDataSet(vtkDataSet* dataSet) {this->DataSet = dataSet;};
  vtkDataSet* GetDataSet() {return this->DataSet;};
  ///@}

  /**
   * Allocate (if not already allocated, or if ReallocateOnBuild is on) one item per point of
   * DataSet, then build each item in turn by assigning it DataSet and its point id and calling its
   * Build() method.
   */
  void Build();

  ///@{
  /**
   * Toggle whether Build() is forced to reallocate the item array even if it has already been
   * built for the current number of points. Propagated to each item's own ReallocateOnBuild flag.
   * Default: off.
   */
  vtkSetMacro(ReallocateOnBuild,int)
  vtkGetMacro(ReallocateOnBuild,int)
  vtkBooleanMacro(ReallocateOnBuild,int)
  ///@}

protected:
  vtkvmtkDataSetItems() {}
  ~vtkvmtkDataSetItems() {}

  vtkDataSet *DataSet;

  int ReallocateOnBuild;

private:
  vtkvmtkDataSetItems(const vtkvmtkDataSetItems&);  // Not implemented.
  void operator=(const vtkvmtkDataSetItems&);  // Not implemented.
};

#endif

