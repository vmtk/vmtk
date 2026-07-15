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
 * @class   vtkvmtkPolyDataFELaplaceAssembler
 * @brief   Construct a Laplacian-based finite element calculation on a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataFELaplaceAssembler implements vtkvmtkFEAssembler::Build() to assemble the
 * piecewise-linear finite-element stiffness matrix of the surface Laplace-Beltrami operator over
 * every 2D (triangle) cell of DataSet: for each cell, Gauss quadrature (order QuadratureOrder,
 * inherited from vtkvmtkFEAssembler, via vtkvmtkGaussQuadrature / vtkvmtkFEShapeFunctions) is used to
 * integrate the dot product of shape function gradients, and the contributions are accumulated into
 * Matrix. No right-hand-side terms are added (the RHS is expected to be supplied by boundary
 * conditions, e.g. vtkvmtkDirichletBoundaryConditions). Solving the resulting linear system yields
 * the discrete harmonic function used, for instance, by vtkvmtkPolyDataHarmonicMappingFilter.
 *
 * @sa vtkvmtkFEAssembler, vtkvmtkPolyDataHarmonicMappingFilter, vtkvmtkPolyDataFEGradientAssembler
 */

#ifndef __vtkvmtkPolyDataFELaplaceAssembler_h
#define __vtkvmtkPolyDataFELaplaceAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFELaplaceAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkPolyDataFELaplaceAssembler* New();
  vtkTypeMacro(vtkvmtkPolyDataFELaplaceAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

protected:
  vtkvmtkPolyDataFELaplaceAssembler();
  ~vtkvmtkPolyDataFELaplaceAssembler();

private:
  vtkvmtkPolyDataFELaplaceAssembler(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFELaplaceAssembler&);  // Not implemented.
};

#endif
