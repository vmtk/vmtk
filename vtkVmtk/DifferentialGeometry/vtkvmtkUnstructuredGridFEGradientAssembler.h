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
 * @class   vtkvmtkUnstructuredGridFEGradientAssembler
 * @brief   Construct a gradient based finite element calculation on a mesh.
 * @ingroup DifferentialGeometry
 *
 * Assembles, over a vtkUnstructuredGrid, the finite-element mass matrix and right-hand side vector
 * needed to L2-project either the full gradient (AssemblyMode = VTKVMTK_GRADIENTASSEMBLY) or a
 * single partial derivative along Direction (VTKVMTK_PARTIALDERIVATIVEASSEMBLY) of the scalar point
 * data array named ScalarsArrayName (component ScalarsComponent). Used internally by
 * vtkvmtkUnstructuredGridGradientFilter; solving the assembled system (e.g. with
 * vtkvmtkOpenNLLinearSystemSolver) yields the smoothed nodal gradient.
 *
 * @sa vtkvmtkFEAssembler, vtkvmtkUnstructuredGridGradientFilter
 */

#ifndef __vtkvmtkUnstructuredGridFEGradientAssembler_h
#define __vtkvmtkUnstructuredGridFEGradientAssembler_h

#include "vtkvmtkFEAssembler.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridFEGradientAssembler : public vtkvmtkFEAssembler
{
public:

  static vtkvmtkUnstructuredGridFEGradientAssembler* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridFEGradientAssembler,vtkvmtkFEAssembler);

  /**
   * Assemble the mass matrix and RHS vector (see AssemblyMode).
   */
  virtual void Build() override;

  ///@{
  /**
   * Set/Get the name of the point data array whose gradient/partial derivative is assembled for.
   */
  vtkSetStringMacro(ScalarsArrayName);
  vtkGetStringMacro(ScalarsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the component of ScalarsArrayName to use, for multi-component input arrays. Default: 0.
   */
  vtkSetMacro(ScalarsComponent,int);
  vtkGetMacro(ScalarsComponent,int);
  ///@}

  ///@{
  /**
   * Set/Get the coordinate direction (0=x, 1=y, 2=z) whose partial derivative is assembled for when
   * AssemblyMode is VTKVMTK_PARTIALDERIVATIVEASSEMBLY.
   */
  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);
  ///@}

  ///@{
  /**
   * Set/Get whether Build() assembles the full gradient (VTKVMTK_GRADIENTASSEMBLY, default) or a
   * single partial derivative along Direction (VTKVMTK_PARTIALDERIVATIVEASSEMBLY). See also
   * SetAssemblyModeToGradient / SetAssemblyModeToPartialDerivative.
   */
  vtkSetMacro(AssemblyMode,int);
  vtkGetMacro(AssemblyMode,int);
  ///@}
  /**
   * Convenience method: set AssemblyMode to assemble the full gradient (default).
   */
  void SetAssemblyModeToGradient()
  { this->SetAssemblyMode(VTKVMTK_GRADIENTASSEMBLY); }
  /**
   * Convenience method: set AssemblyMode to assemble a single partial derivative along Direction.
   */
  void SetAssemblyModeToPartialDerivative()
  { this->SetAssemblyMode(VTKVMTK_PARTIALDERIVATIVEASSEMBLY); }

//BTX
  /**
   * Values for AssemblyMode.
   */
  enum {
    VTKVMTK_GRADIENTASSEMBLY,
    VTKVMTK_PARTIALDERIVATIVEASSEMBLY
  };
//ETX

protected:
  vtkvmtkUnstructuredGridFEGradientAssembler();
  ~vtkvmtkUnstructuredGridFEGradientAssembler();

  void BuildGradient();
  void BuildPartialDerivative();

  char* ScalarsArrayName;
  int ScalarsComponent;
  int AssemblyMode;
  int Direction;

private:
  vtkvmtkUnstructuredGridFEGradientAssembler(const vtkvmtkUnstructuredGridFEGradientAssembler&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridFEGradientAssembler&);  // Not implemented.
};

#endif
