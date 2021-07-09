/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataNeighborhood.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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
// .NAME vtkvmtkPolyDataNeighborhood - Create a neighborhood for points on a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataNeighborhood_h
#define __vtkvmtkPolyDataNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkPolyDataNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_POLYDATA_NEIGHBORHOOD;};

  // Description:
  // Build the neighborhood.
  virtual void Build() override;

protected:
  vtkvmtkPolyDataNeighborhood() {};
  ~vtkvmtkPolyDataNeighborhood() {};

private:
  vtkvmtkPolyDataNeighborhood(const vtkvmtkPolyDataNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataNeighborhood&);  // Not implemented.
};

#endif

