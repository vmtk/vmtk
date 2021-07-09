/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataManifoldNeighborhood.h,v $
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
// .NAME vtkvmtkPolyDataManifoldNeighborhood - Apply a neighborhood to the points of a manifold surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataManifoldNeighborhood_h
#define __vtkvmtkPolyDataManifoldNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
#include "vtkPolyData.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataManifoldNeighborhood : public vtkvmtkNeighborhood 
{
public:

  static vtkvmtkPolyDataManifoldNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataManifoldNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_POLYDATA_MANIFOLD_NEIGHBORHOOD;};

  // Description:
  // Build the neighborhood.
  virtual void Build() override;

protected:
  vtkvmtkPolyDataManifoldNeighborhood() {};
  ~vtkvmtkPolyDataManifoldNeighborhood() {};

private:
  vtkvmtkPolyDataManifoldNeighborhood(const vtkvmtkPolyDataManifoldNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataManifoldNeighborhood&);  // Not implemented.
};

#endif

