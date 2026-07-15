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
 * @class   vtkvmtkItems
 * @brief   Control allocation and memory footprint of item instances.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkItems is an abstract, reference-counted, resizable array of vtkvmtkItem pointers (modeled
 * after VTK's own vtkCellArray/vtkPoints-style container classes). Allocate() creates and takes
 * ownership of numItems items (each instantiated through the subclass-provided
 * InstantiateNewItem() factory method), AllocateItem() replaces a single slot with a new item of a
 * possibly different type, and Resize()/Squeeze() grow or shrink the backing array as needed.
 * DeepCopy()/ShallowCopy() copy or share another collection's items. vtkvmtkDataSetItems is the
 * concrete subclass used to hold one item per point of a vtkDataSet.
 *
 * @sa
 * vtkvmtkItem, vtkvmtkDataSetItems, vtkvmtkNeighborhoods
 */

#ifndef __vtkvmtkItems_h
#define __vtkvmtkItems_h

#include "vtkObject.h"
#include "vtkvmtkItem.h"
#include "vtkDataSet.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkItems : public vtkObject 
{
public:

  vtkTypeMacro(vtkvmtkItems,vtkObject);

  ///@{
  /**
   * Set/get the item type (as understood by the subclass's InstantiateNewItem()) used by Allocate()
   * to create new items.
   */
  vtkSetMacro(ItemType,int);
  vtkGetMacro(ItemType,int);
  ///@}

  /**
   * Allocate the specified number of items (i.e., number of points) that will be built.
   */
  void Allocate(vtkIdType numItems, vtkIdType ext=1000);

  /**
   * Get the item stored at index id.
   */
  vtkvmtkItem* GetItem(vtkIdType id) {return this->Array[id];};

  ///@{
  /**
   * Set/get the number of valid items in the collection (i.e. MaxId + 1). Setting this only moves
   * the MaxId marker; it does not allocate or free storage.
   */
  void SetNumberOfItems(vtkIdType numberOfItems) { this->MaxId = numberOfItems - 1;};
  vtkIdType GetNumberOfItems() {return this->MaxId + 1;};
  ///@}

  /**
   * Reclaim any unused memory.
   */
  void Squeeze();

  /**
   * Reset to a state of no entries without freeing the memory.
   */
  void Reset();

  /**
   * Reset to a state of no entries freeing the memory.
   */
  void Initialize();

  /**
   * Releases the stencil array.
   */
  void ReleaseArray();

  /**
   * Replace the item at index i with a newly instantiated item of the given itemType, deleting the
   * previous item at that index.
   */
  void AllocateItem(vtkIdType i, vtkIdType itemType);

  /**
   * Standard DeepCopy method.
   */
  void DeepCopy(vtkvmtkItems *src);

  /**
   * Standard ShallowCopy method.
   */
  void ShallowCopy(vtkvmtkItems *src);

protected:
  vtkvmtkItems():Array(NULL),Size(0),MaxId(-1),Extend(1000) {};
  ~vtkvmtkItems();

  virtual vtkvmtkItem* InstantiateNewItem(int itemType) = 0;

  vtkvmtkItem** Array;   // pointer to data
  vtkIdType Size;       // allocated size of data
  vtkIdType MaxId;     // maximum index inserted thus far
  vtkIdType Extend;     // grow array by this point
  vtkvmtkItem** Resize(vtkIdType sz);  // function to resize data

  int ItemType;

private:
  vtkvmtkItems(const vtkvmtkItems&);  // Not implemented.
  void operator=(const vtkvmtkItems&);  // Not implemented.
};

#endif

