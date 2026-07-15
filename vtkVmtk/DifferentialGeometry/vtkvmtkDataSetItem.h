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
 * @class   vtkvmtkDataSetItem
 * @brief   Create an item instance associated with a data set and a particular point id.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkDataSetItem is an abstract vtkvmtkItem specialization that anchors the item to a specific
 * point (DataSetPointId) of a specific vtkDataSet (DataSet). Concrete subclasses (e.g.
 * vtkvmtkNeighborhood and its descendants) use Build() to compute, from DataSet and
 * DataSetPointId, whatever local geometric quantity the item represents (topological neighbors,
 * finite-difference stencil weights, etc.). Items of this kind are normally created and owned in
 * bulk by a matching vtkvmtkDataSetItems collection, one per point of the data set.
 *
 * @sa
 * vtkvmtkDataSetItems, vtkvmtkNeighborhood, vtkvmtkItem
 */

#ifndef __vtkvmtkDataSetItem_h
#define __vtkvmtkDataSetItem_h

#include "vtkObject.h"
#include "vtkvmtkItem.h"
#include "vtkDataSet.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDataSetItem : public vtkvmtkItem 
{
public:

  vtkTypeMacro(vtkvmtkDataSetItem,vtkvmtkItem);

  ///@{
  /**
   * Set/get the data set that this item refers to. The reference is stored but not registered
   * (no reference counting), so the data set must be kept alive by the caller for the lifetime of
   * this item.
   */
/*   vtkSetObjectMacro(DataSet,vtkDataSet); */
/*   vtkGetObjectMacro(DataSet,vtkDataSet); */
  void SetDataSet(vtkDataSet* dataSet) {this->DataSet = dataSet;};
  vtkDataSet* GetDataSet() {return this->DataSet;};
  ///@}

  ///@{
  /**
   * Set/get the id, within DataSet, of the point that this item is built around. Default: -1
   * (unset).
   */
  vtkSetMacro(DataSetPointId,vtkIdType);
  vtkGetMacro(DataSetPointId,vtkIdType);
  ///@}

  /**
   * Build the item.
   */
  virtual void Build() = 0;

  /**
   * Standard DeepCopy method.
   */
  virtual void DeepCopy(vtkvmtkItem *src) override;

  ///@{
  /**
   * Toggle whether Build() is forced to reallocate/recompute the item's internal data even if it
   * has already been built. Default: off.
   */
  vtkSetMacro(ReallocateOnBuild,int)
  vtkGetMacro(ReallocateOnBuild,int)
  vtkBooleanMacro(ReallocateOnBuild,int)
  ///@}

protected:
  vtkvmtkDataSetItem();
  ~vtkvmtkDataSetItem() {};

  vtkDataSet *DataSet;
  vtkIdType DataSetPointId;

  int ReallocateOnBuild;

private:
  vtkvmtkDataSetItem(const vtkvmtkDataSetItem&);  // Not implemented.
  void operator=(const vtkvmtkDataSetItem&);  // Not implemented.
};

#endif

