/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDataSetItems.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkDataSetItems - ..
// .SECTION Description
// ..

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

/*   vtkSetObjectMacro(DataSet,vtkDataSet); */
/*   vtkGetObjectMacro(DataSet,vtkDataSet); */
  void SetDataSet(vtkDataSet* dataSet) {this->DataSet = dataSet;};
  vtkDataSet* GetDataSet() {return this->DataSet;};

  // Description:
  // Build the item array.
  void Build();

  vtkSetMacro(ReallocateOnBuild,int)
  vtkGetMacro(ReallocateOnBuild,int)
  vtkBooleanMacro(ReallocateOnBuild,int)

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

