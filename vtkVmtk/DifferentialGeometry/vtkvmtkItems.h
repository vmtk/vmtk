/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkItems.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkItems - ..
// .SECTION Description
// ..

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

  vtkSetMacro(ItemType,int);
  vtkGetMacro(ItemType,int);

  // Description:
  // Allocate the specified number of items (i.e., number of points) that
  // will be built.
  void Allocate(vtkIdType numItems, vtkIdType ext=1000);

  vtkvmtkItem* GetItem(vtkIdType id) {return this->Array[id];};

  void SetNumberOfItems(vtkIdType numberOfItems) { this->MaxId = numberOfItems - 1;};
  vtkIdType GetNumberOfItems() {return this->MaxId + 1;};

  // Description:
  // Reclaim any unused memory.
  void Squeeze();

  // Description:
  // Reset to a state of no entries without freeing the memory.
  void Reset();

  // Description:
  // Reset to a state of no entries freeing the memory.
  void Initialize();

  // Description:
  // Releases the stencil array.
  void ReleaseArray();

  void AllocateItem(vtkIdType i, vtkIdType itemType);

  // Description:
  // Standard DeepCopy method.
  void DeepCopy(vtkvmtkItems *src);

  // Description:
  // Standard ShallowCopy method.
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

