/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkItem.h,v $
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
// .NAME vtkvmtkItem - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkItem_h
#define __vtkvmtkItem_h

#include "vtkObject.h"
#include "vtkDataSet.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkItem : public vtkObject 
{
public:

  vtkTypeMacro(vtkvmtkItem,vtkObject);

  virtual vtkIdType GetItemType() = 0;

  // Description:
  // Standard DeepCopy method.  Since this object contains no reference
  // to other objects, there is no ShallowCopy.
  virtual void DeepCopy(vtkvmtkItem *src);

protected:
  vtkvmtkItem() {};
  ~vtkvmtkItem() {};

private:
  vtkvmtkItem(const vtkvmtkItem&);  // Not implemented.
  void operator=(const vtkvmtkItem&);  // Not implemented.
};

#endif

