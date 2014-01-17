/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataManifoldExtendedNeighborhood.h,v $
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
// .NAME vtkvmtkPolyDataManifoldExtendedNeighborhood - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataManifoldExtendedNeighborhood_h
#define __vtkvmtkPolyDataManifoldExtendedNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"
#include "vtkPolyData.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataManifoldExtendedNeighborhood : public vtkvmtkPolyDataManifoldNeighborhood
{
public:

  static vtkvmtkPolyDataManifoldExtendedNeighborhood *New();
  vtkTypeMacro(vtkvmtkPolyDataManifoldExtendedNeighborhood,vtkvmtkPolyDataManifoldNeighborhood);

  virtual vtkIdType GetItemType() {return VTK_VMTK_POLYDATA_MANIFOLD_EXTENDED_NEIGHBORHOOD;};

  // Description:
  // Build the neighborhood.
  virtual void Build();

protected:
  vtkvmtkPolyDataManifoldExtendedNeighborhood() {};
  ~vtkvmtkPolyDataManifoldExtendedNeighborhood() {};

private:
  vtkvmtkPolyDataManifoldExtendedNeighborhood(const vtkvmtkPolyDataManifoldExtendedNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataManifoldExtendedNeighborhood&);  // Not implemented.
};

#endif

