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
 * @class   vtkvmtkPolyDataHarmonicMappingFilter
 * @brief   Construct a harmonic function over each vascular surface segment used during mapping order to stretch the longitudinal metric to correctly account for the presence of insertion regions at bifurcations.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataHarmonicMappingFilter solves a discrete Laplace (harmonic) boundary value problem
 * on the input surface: the linear system (either assembled with stencils, via vtkvmtkStencils using
 * the finite-element Laplace-Beltrami stencil, or with true piecewise-linear finite elements, via
 * vtkvmtkPolyDataFELaplaceAssembler, according to AssemblyMode) is subjected to Dirichlet boundary
 * conditions (vtkvmtkDirichletBoundaryConditions) that pin the points listed in BoundaryPointIds to
 * the corresponding values in BoundaryValues, then solved with vtkvmtkOpenNLLinearSystemSolver
 * (conjugate gradient, tolerance ConvergenceTolerance). The resulting harmonic scalar field is
 * written to the output as a point data array named HarmonicMappingArrayName. This is the core
 * computation behind vtkvmtkPolyDataCylinderHarmonicMappingFilter, and more generally is used to
 * build smooth surface parameterizations (e.g. longitudinal position along a vessel segment) that
 * remain well behaved near non-planar boundaries such as bifurcation insertion regions.
 *
 * @sa vtkvmtkPolyDataCylinderHarmonicMappingFilter, vtkvmtkPolyDataFELaplaceAssembler,
 *     vtkvmtkOpenNLLinearSystemSolver, vtkvmtkStencils
 */

#ifndef __vtkvmtkPolyDataHarmonicMappingFilter_h
#define __vtkvmtkPolyDataHarmonicMappingFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPolyDataAlgorithm.h"

#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataHarmonicMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataHarmonicMappingFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the ids, into the input poly data, of the points on which a Dirichlet boundary
   * condition is imposed (the corresponding values are taken from BoundaryValues, matched by
   * index). Must be set, together with BoundaryValues, before Update() is called.
   */
  vtkSetObjectMacro(BoundaryPointIds,vtkIdList);
  vtkGetObjectMacro(BoundaryPointIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the harmonic function value imposed at each point listed in BoundaryPointIds (matched
   * by index). Must be set, together with BoundaryPointIds, before Update() is called.
   */
  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data array where the computed harmonic scalar field is stored on
   * the output. Must be set before Update() is called.
   * Commonly named "HarmonicMapping".
   */
  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/get the convergence tolerance of the iterative linear solver (see
   * vtkvmtkOpenNLLinearSystemSolver) used to solve the harmonic problem. Default: 1E-6.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/get how the discrete Laplace system is assembled: VTK_VMTK_ASSEMBLY_STENCILS uses the
   * finite-element Laplace-Beltrami stencil (via vtkvmtkStencils); VTK_VMTK_ASSEMBLY_FINITEELEMENTS
   * (the default) uses true piecewise-linear finite elements (via
   * vtkvmtkPolyDataFELaplaceAssembler, with quadrature order QuadratureOrder).
   */
  vtkSetMacro(AssemblyMode,int);
  vtkGetMacro(AssemblyMode,int);
  void SetAssemblyModeToStencils()
  { this->SetAssemblyMode(VTK_VMTK_ASSEMBLY_STENCILS); }
   void SetAssemblyModeToFiniteElements()
  { this->SetAssemblyMode(VTK_VMTK_ASSEMBLY_FINITEELEMENTS); }
  ///@}

  ///@{
  /**
   * Set/get the order of the Gauss quadrature rule used to integrate the finite-element matrix when
   * AssemblyMode is VTK_VMTK_ASSEMBLY_FINITEELEMENTS. Default: 1.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}


//BTX
  enum 
    {
    VTK_VMTK_ASSEMBLY_STENCILS,
    VTK_VMTK_ASSEMBLY_FINITEELEMENTS
    };
//ETX

protected:
  vtkvmtkPolyDataHarmonicMappingFilter();
  ~vtkvmtkPolyDataHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryPointIds;
  vtkDoubleArray* BoundaryValues;

  char* HarmonicMappingArrayName;
  double ConvergenceTolerance;
  int AssemblyMode;
  int QuadratureOrder;

private:
  vtkvmtkPolyDataHarmonicMappingFilter(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
};

#endif

