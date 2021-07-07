/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMinHeap.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkMinHeap - Implementation of the min heap data structure.
// .SECTION Description
// This class is an implementation of the min heap data structure, used to handle a set of values in such a way that the retrieval of the minimum element takes constant time. A min heap is a complete binary tree where the value at each node is equal or less than the value at its children, and it is represented as an array where the children of a node stored at location k are at location 2k and 2k+1 (so that the parent of k is located at k/2). Keeping the min heap ordered after a value is updated or an id is inserted in teh heap takes O(log N). 
//
// In the present implementation, values are provided in a vtkDoubleArray, and element ids are inserted in the heap. Backpointers are used to access the heap by id. This class is optimized for working in conjunction with vtkNonManifoldFastMarching.
//
// For more insight see J.A. Sethian, Level Set Methods and Fast Marching Methods, Cambridge University Press, 2nd Edition, 1999.
// .SECTION Caveats
// Be sure to call Initialize() after defining MinHeapScalars.
// .SECTION See Also
// vtkNonManifoldFastMarching

#ifndef __vtkvmtkMinHeap_h
#define __vtkvmtkMinHeap_h

#include "vtkObject.h"
#include "vtkDoubleArray.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkMinHeap : public vtkObject
{
  public: 
  vtkTypeMacro(vtkvmtkMinHeap,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkMinHeap *New();
  
  // Description:
  // Initializes the heap to an empty state and prepares back pointers. Calls this method before using the min heap once MinHeapScalars have been defined.
  void Initialize();

  // Description:
  // Set/Get the array containing the values indexed by the min heap.
  vtkSetObjectMacro(MinHeapScalars,vtkDoubleArray);
  vtkGetObjectMacro(MinHeapScalars,vtkDoubleArray);

  // Description:
  // Get heap size.
  int GetSize();

  // Description:
  // Insert an index to a value in HeapScalars in the min heap.
  void InsertNextId(vtkIdType id);

  // Description:
  // Tells the min heap that the value indexed by id has changed in MinHeapScalars array.
  void UpdateId(vtkIdType id);

  // Description:
  // Gets the id of the minimum value in the min heap.
  vtkIdType GetMin();

  // Description:
  // Gets the id of the minimum value in the min heap and removes it from the min heap.
  vtkIdType RemoveMin();

  protected:
  vtkvmtkMinHeap();
  ~vtkvmtkMinHeap();  

  void Swap(vtkIdType loc0, vtkIdType loc1);
  int IsLeaf(vtkIdType loc);
  vtkIdType GetLeftChild(vtkIdType loc);
  vtkIdType GetRightChild(vtkIdType loc);
  vtkIdType GetParent(vtkIdType loc);
  void SiftUp(vtkIdType loc);
  void SiftDown(vtkIdType loc);
  vtkIdType RemoveAt(vtkIdType loc);

  vtkIdList* Heap;
  vtkIdList* BackPointers;

  vtkDoubleArray* MinHeapScalars;

  private:
  vtkvmtkMinHeap(const vtkvmtkMinHeap&);  // Not implemented.
  void operator=(const vtkvmtkMinHeap&);  // Not implemented.
};

#endif
