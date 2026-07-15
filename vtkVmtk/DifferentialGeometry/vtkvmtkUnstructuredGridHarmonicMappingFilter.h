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
 * @class   vtkvmtkUnstructuredGridHarmonicMappingFilter
 * @brief   Construct a harmonic function over each vascular mesh segment used during mapping order to stretch the longitudinal metric to correctly account for the presence of insertion regions at bifurcations.
 * @ingroup DifferentialGeometry
 *
 * Solves the Laplace equation (assembled with vtkvmtkUnstructuredGridFELaplaceAssembler, Gauss
 * quadrature order QuadratureOrder) over the input mesh with Dirichlet boundary conditions given by
 * BoundaryValues at the points listed in BoundaryPointIds, writing the resulting harmonic scalar
 * field into the point data array named HarmonicMappingArrayName. The linear system is solved
 * iteratively to ConvergenceTolerance.
 *
 * @sa vtkvmtkUnstructuredGridFELaplaceAssembler, vtkvmtkPolyDataHarmonicMappingFilter
 */

#ifndef __vtkvmtkUnstructuredGridHarmonicMappingFilter_h
#define __vtkvmtkUnstructuredGridHarmonicMappingFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridHarmonicMappingFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridHarmonicMappingFilter,vtkUnstructuredGridAlgorithm);

  ///@{
  /**
   * Set/Get the ids of the mesh points where a Dirichlet boundary value is imposed (parallel to
   * BoundaryValues). Required input.
   */
  vtkSetObjectMacro(BoundaryPointIds,vtkIdList);
  vtkGetObjectMacro(BoundaryPointIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the Dirichlet boundary values imposed at the points listed in BoundaryPointIds
   * (parallel array). Required input.
   */
  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the computed harmonic scalar field is
   * stored.
   * Commonly named "HarmonicMapping".
   */
  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the convergence tolerance of the iterative linear solver used to solve the Laplace
   * system.
   */
  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get the order of the Gauss quadrature rule used to integrate the finite-element matrices in
   * vtkvmtkUnstructuredGridFELaplaceAssembler.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

protected:
  vtkvmtkUnstructuredGridHarmonicMappingFilter();
  ~vtkvmtkUnstructuredGridHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryPointIds;
  vtkDoubleArray* BoundaryValues;

  char* HarmonicMappingArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkUnstructuredGridHarmonicMappingFilter(const vtkvmtkUnstructuredGridHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridHarmonicMappingFilter&);  // Not implemented.
};

#endif

