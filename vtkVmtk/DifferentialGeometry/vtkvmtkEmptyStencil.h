/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkEmptyStencil.h,v $
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
// .NAME vtkvmtkEmptyStencil - create a stencil but do not apply a weighting to the neighborhoods of the data set.
// .SECTION Description
// ..

#ifndef __vtkvmtkEmptyStencil_h
#define __vtkvmtkEmptyStencil_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkStencil.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkEmptyStencil : public vtkvmtkStencil 
{
public:
  static vtkvmtkEmptyStencil* New();
  vtkTypeMacro(vtkvmtkEmptyStencil,vtkvmtkStencil);

  // Description:
  // Build the stencil.
  void Build() override;

  virtual vtkIdType GetItemType() override {return VTK_VMTK_EMPTY_STENCIL;};

protected:
  vtkvmtkEmptyStencil() {};
  ~vtkvmtkEmptyStencil() {};

private:
  vtkvmtkEmptyStencil(const vtkvmtkEmptyStencil&);  // Not implemented.
  void operator=(const vtkvmtkEmptyStencil&);  // Not implemented.
};

#endif

