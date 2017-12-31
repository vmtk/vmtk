/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataAreaWeightedUmbrellaStencil.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataAreaWeightedUmbrellaStencil - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataAreaWeightedUmbrellaStencil_h
#define __vtkvmtkPolyDataAreaWeightedUmbrellaStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkPolyDataManifoldStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataAreaWeightedUmbrellaStencil : public vtkvmtkPolyDataManifoldStencil
{
public:

  static vtkvmtkPolyDataAreaWeightedUmbrellaStencil *New();
  vtkTypeMacro(vtkvmtkPolyDataAreaWeightedUmbrellaStencil,vtkvmtkPolyDataManifoldStencil);

  virtual vtkIdType GetItemType() VTK_OVERRIDE {return VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL;};

  void Build() VTK_OVERRIDE;

protected:
  vtkvmtkPolyDataAreaWeightedUmbrellaStencil();
  ~vtkvmtkPolyDataAreaWeightedUmbrellaStencil() {};

  void ScaleWithArea() VTK_OVERRIDE;

private:
  vtkvmtkPolyDataAreaWeightedUmbrellaStencil(const vtkvmtkPolyDataAreaWeightedUmbrellaStencil&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataAreaWeightedUmbrellaStencil&);  // Not implemented.
};

#endif

