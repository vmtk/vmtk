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
 * @class   vtkvmtkEmptyStencil
 * @brief   Create a stencil that does not apply any weighting to the neighborhoods of the data set.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkEmptyStencil is the trivial vtkvmtkStencil implementation: Build() frees any existing
 * point ids, weights, and center weight and leaves the stencil with zero points (unless
 * ReallocateOnBuild is off and the stencil was already built, in which case Build() is a no-op). It
 * is used as a placeholder item type (VTK_VMTK_EMPTY_STENCIL) for points that should not have real
 * finite-difference/finite-element weights computed.
 *
 * @sa
 * vtkvmtkStencil, vtkvmtkEmptyNeighborhood
 */

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

  /**
   * Build the stencil.
   */
  void Build() override;

  /**
   * Get the item type identifier, VTK_VMTK_EMPTY_STENCIL.
   */
  virtual vtkIdType GetItemType() override {return VTK_VMTK_EMPTY_STENCIL;};

protected:
  vtkvmtkEmptyStencil() {};
  ~vtkvmtkEmptyStencil() {};

private:
  vtkvmtkEmptyStencil(const vtkvmtkEmptyStencil&);  // Not implemented.
  void operator=(const vtkvmtkEmptyStencil&);  // Not implemented.
};

#endif

