/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridNeighborhood.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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
// .NAME vtkvmtkUnstructuredGridNeighborhood - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridNeighborhood_h
#define __vtkvmtkUnstructuredGridNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkUnstructuredGridNeighborhood *New();
  vtkTypeMacro(vtkvmtkUnstructuredGridNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() VTK_OVERRIDE {return VTK_VMTK_UNSTRUCTUREDGRID_NEIGHBORHOOD;};

  // Description:
  // Build the neighborhood.
  virtual void Build() VTK_OVERRIDE;

protected:
  vtkvmtkUnstructuredGridNeighborhood() {};
  ~vtkvmtkUnstructuredGridNeighborhood() {};

private:
  vtkvmtkUnstructuredGridNeighborhood(const vtkvmtkUnstructuredGridNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridNeighborhood&);  // Not implemented.
};

#endif

