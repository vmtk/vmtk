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
 * @class   vtkvmtkPolyDataFEGradientAssembler
 * @brief   Construct a gradient based finite element calculation on a surface.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataFEGradientAssembler implements vtkvmtkFEAssembler::Build() to assemble, over every
 * 2D (triangle) cell of DataSet, the finite-element mass matrix and the right-hand-side vector of the
 * L2 projection of the surface gradient of a scalar point data array (ScalarsArrayName /
 * ScalarsComponent) onto the mesh's piecewise-linear basis functions. Gauss quadrature (order
 * QuadratureOrder, inherited from vtkvmtkFEAssembler) is used to integrate shape functions and their
 * derivatives (via vtkvmtkGaussQuadrature / vtkvmtkFEShapeFunctions). Because the projected gradient
 * is a 3-component vector, three copies of the same mass matrix (block-diagonal) and three RHS
 * components are assembled at once (see Initialize(3) in Build()). Solving the resulting linear
 * system (e.g. with vtkvmtkOpenNLLinearSystemSolver) yields a smoothed nodal gradient field; this is
 * the assembler used internally by vtkvmtkPolyDataGradientFilter.
 *
 * @sa vtkvmtkFEAssembler, vtkvmtkPolyDataGradientFilter, vtkvmtkPolyDataFELaplaceAssembler
 */

#ifndef __vtkvmtkPolyDataFEGradientAssembler_h
#define __vtkvmtkPolyDataFEGradientAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataFEGradientAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkPolyDataFEGradientAssembler* New();
  vtkTypeMacro(vtkvmtkPolyDataFEGradientAssembler,vtkvmtkFEAssembler);

  virtual void Build() override;

  ///@{
  /**
   * Set/get the name of the point data scalar array whose surface gradient is assembled by
   * Build(). Must be set, and the array must exist on DataSet, before calling Build().
   */
  vtkSetStringMacro(ScalarsArrayName);
  vtkGetStringMacro(ScalarsArrayName);
  ///@}

  ///@{
  /**
   * Set/get the component of the ScalarsArrayName array (for multi-component arrays) whose gradient
   * is computed. Default: 0.
   */
  vtkSetMacro(ScalarsComponent,int);
  vtkGetMacro(ScalarsComponent,int);
  ///@}

protected:
  vtkvmtkPolyDataFEGradientAssembler();
  ~vtkvmtkPolyDataFEGradientAssembler();

  char* ScalarsArrayName;
  int ScalarsComponent;

private:
  vtkvmtkPolyDataFEGradientAssembler(const vtkvmtkPolyDataFEGradientAssembler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFEGradientAssembler&);  // Not implemented.
};

#endif
