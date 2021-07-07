/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkEmptyNeighborhood.h,v $
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
// .NAME vtkvmtkEmptyNeighborhood - Create an an empty neighborhood instance.
// .SECTION Description
// ..

#ifndef __vtkvmtkEmptyNeighborhood_h
#define __vtkvmtkEmptyNeighborhood_h

#include "vtkObject.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkNeighborhood.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkEmptyNeighborhood : public vtkvmtkNeighborhood 
{
public:
  static vtkvmtkEmptyNeighborhood* New();
  vtkTypeMacro(vtkvmtkEmptyNeighborhood,vtkvmtkNeighborhood);

  virtual vtkIdType GetItemType() override {return VTK_VMTK_EMPTY_NEIGHBORHOOD;};

  // Description:
  // Build the stencil.
  void Build() override;

protected:
  vtkvmtkEmptyNeighborhood() {};
  ~vtkvmtkEmptyNeighborhood() {};

private:
  vtkvmtkEmptyNeighborhood(const vtkvmtkEmptyNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkEmptyNeighborhood&);  // Not implemented.
};

#endif

