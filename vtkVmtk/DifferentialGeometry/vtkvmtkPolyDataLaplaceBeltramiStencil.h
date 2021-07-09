/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataLaplaceBeltramiStencil.h,v $
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
// .NAME vtkvmtkPolyDataLaplaceBeltramiStencil - Apply LB weighting to the neighborhood connections of a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataLaplaceBeltramiStencil_h
#define __vtkvmtkPolyDataLaplaceBeltramiStencil_h

#include "vtkObject.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataLaplaceBeltramiStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  vtkTypeMacro(vtkvmtkPolyDataLaplaceBeltramiStencil,vtkvmtkPolyDataManifoldStencil);

  void Build() override;
  void BuildBoundaryWeights(vtkIdType boundaryPointId, vtkIdType boundaryNeighborPointId, double &boundaryWeight, double &boundaryNeighborWeight);

protected:
  vtkvmtkPolyDataLaplaceBeltramiStencil();
  ~vtkvmtkPolyDataLaplaceBeltramiStencil() {};

private:
  vtkvmtkPolyDataLaplaceBeltramiStencil(const vtkvmtkPolyDataLaplaceBeltramiStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataLaplaceBeltramiStencil&);  // Not implemented.
};

#endif

