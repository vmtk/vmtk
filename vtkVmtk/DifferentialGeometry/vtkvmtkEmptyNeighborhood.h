/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkEmptyNeighborhood
 * @brief   Create an empty neighborhood instance.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkEmptyNeighborhood is the trivial vtkvmtkNeighborhood implementation: Build() always
 * results in zero points (NPoints = 0, PointIds freed). It is used as a placeholder item type
 * (VTK_VMTK_EMPTY_NEIGHBORHOOD), e.g. by vtkvmtkNeighborhoods, for points that should not have a
 * real neighborhood computed.
 *
 * @sa
 * vtkvmtkNeighborhood, vtkvmtkNeighborhoods
 */

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

  /**
   * Get the item type identifier, VTK_VMTK_EMPTY_NEIGHBORHOOD.
   */
  virtual vtkIdType GetItemType() override {return VTK_VMTK_EMPTY_NEIGHBORHOOD;};

  /**
   * Build the stencil.
   */
  void Build() override;

protected:
  vtkvmtkEmptyNeighborhood() {};
  ~vtkvmtkEmptyNeighborhood() {};

private:
  vtkvmtkEmptyNeighborhood(const vtkvmtkEmptyNeighborhood&);  // Not implemented.
  void operator=(const vtkvmtkEmptyNeighborhood&);  // Not implemented.
};

#endif

