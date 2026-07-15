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
 * @class   vtkvmtkFEShapeFunctions
 * @brief   Provides a set of common utility functions (calculate jacobian, interpolation function, and interpolation function derivatives) used during assembly of certain finite element solutions.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkFEShapeFunctions evaluates, for a given vtkCell, the isoparametric finite-element shape
 * functions (interpolation functions Phi), their gradients with respect to physical (x,y,z)
 * coordinates (DPhi), and the Jacobian of the parametric-to-physical mapping (Jacobians), at a set
 * of parametric points (typically Gauss quadrature points, see vtkvmtkGaussQuadrature). Initialize()
 * precomputes and caches these quantities for every point in pcoords so that GetPhi/GetDPhi/
 * GetJacobian can be queried cheaply during element assembly. The static
 * GetInterpolationFunctions/GetInterpolationDerivs/ComputeJacobian methods dispatch to the
 * appropriate VTK cell type's own interpolation routines and can be used standalone, without an
 * Initialize() call, for a single evaluation point. Supported cell types include line, quad,
 * triangle, hexahedron, wedge and tetra, and their quadratic/biquadratic variants.
 *
 * @sa
 * vtkvmtkGaussQuadrature, vtkvmtkFEAssembler
 */

#ifndef __vtkvmtkFEShapeFunctions_h
#define __vtkvmtkFEShapeFunctions_h

#include "vtkObject.h"
#include "vtkvmtkWin32Header.h"

#include "vtkCell.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkFEShapeFunctions : public vtkObject
{
public:
  vtkTypeMacro(vtkvmtkFEShapeFunctions,vtkObject);
  static vtkvmtkFEShapeFunctions* New();

  /**
   * Precompute and cache the shape function values (Phi), their physical-space gradients (DPhi),
   * and the mapping Jacobian (Jacobians) at every parametric point held in pcoords, for the given
   * cell. Subsequent GetPhi/GetDPhi/GetJacobian calls index into these cached arrays using the
   * point index id (0-based, in the order of pcoords) and, for Phi/DPhi, the cell-local node index
   * i (0 to cell->GetNumberOfPoints()-1).
   */
  void Initialize(vtkCell* cell, vtkDoubleArray* pcoords);

  /**
   * Get the cached value of the i-th node's shape function at the id-th parametric point
   * (previously computed by Initialize()).
   */
  double GetPhi(vtkIdType id, vtkIdType i)
  { return this->Phi->GetValue(id*this->NumberOfCellPoints+i); }

  /**
   * Get a pointer to the cached 3-component physical-space gradient of the i-th node's shape
   * function at the id-th parametric point (previously computed by Initialize()).
   */
  double* GetDPhi(vtkIdType id, vtkIdType i)
  { return this->DPhi->GetTuple(id*this->NumberOfCellPoints+i); }

  /**
   * Copy the cached 3-component physical-space gradient of the i-th node's shape function at the
   * id-th parametric point (previously computed by Initialize()) into dphi.
   */
  void GetDPhi(vtkIdType id, vtkIdType i, double* dphi)
  { this->DPhi->GetTuple(id*this->NumberOfCellPoints+i,dphi); }

  /**
   * Get the c-th component of the cached physical-space gradient of the i-th node's shape function
   * at the id-th parametric point (previously computed by Initialize()).
   */
  double GetDPhi(vtkIdType id, vtkIdType i, int c)
  { return this->DPhi->GetComponent(id*this->NumberOfCellPoints+i,c); }

  /**
   * Get the cached Jacobian of the parametric-to-physical mapping at the i-th parametric point
   * (previously computed by Initialize()).
   */
  double GetJacobian(vtkIdType i)
  { return this->Jacobians->GetValue(i); }

  /**
   * Evaluate, into sf, the shape function (interpolation function) values of cell at the parametric
   * coordinates pcoords. Dispatches to the appropriate vtkCell subclass's InterpolationFunctions
   * method based on cell->GetCellType(); unsupported cell types emit a warning.
   */
  static void GetInterpolationFunctions(vtkCell* cell, double* pcoords, double* sf);

  /**
   * Evaluate, into derivs, the parametric-space derivatives of cell's shape functions at the
   * parametric coordinates pcoords. Dispatches to the appropriate vtkCell subclass's
   * InterpolationDerivs (or InterpolateDerivs) method based on cell->GetCellType(); unsupported
   * cell types emit a warning.
   */
  static void GetInterpolationDerivs(vtkCell* cell, double* pcoords, double* derivs);

  /**
   * Compute and return the Jacobian of the parametric-to-physical mapping of cell at the parametric
   * coordinates pcoords, for 2D or 3D cells. A negative Jacobian (inverted/degenerate element) is
   * reported (optionally, when the corresponding compile-time macro is enabled) and its absolute
   * value is returned.
   */
  static double ComputeJacobian(vtkCell* cell, double* pcoords);

protected:
  vtkvmtkFEShapeFunctions();
  ~vtkvmtkFEShapeFunctions();
 
  static void ComputeInverseJacobianMatrix2D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[2][3]);
  static void ComputeInverseJacobianMatrix3D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[3][3]);

  vtkDoubleArray* Phi;
  vtkDoubleArray* DPhi;
  vtkDoubleArray* Jacobians;
  vtkIdType NumberOfCellPoints;

private:  
  vtkvmtkFEShapeFunctions(const vtkvmtkFEShapeFunctions&);  // Not implemented.
  void operator=(const vtkvmtkFEShapeFunctions&);  // Not implemented.

};

#endif
