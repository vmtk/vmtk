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
 * @class   vtkvmtkGaussQuadrature
 * @brief   Implements a finite element quadrature rule that approximates the definite integral of a function as a weighted sum of function values at specified points.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkGaussQuadrature computes, for a given VTK cell type and quadrature Order, the set of
 * parametric quadrature points (QuadraturePoints) and associated weights (QuadratureWeights) of a
 * Gauss-Legendre (or, for triangles/tetrahedra, Gauss-Jacobi-based) numerical integration rule
 * exact for polynomials up to the requested order. Initialize(cellType) (or the overload taking a
 * vtkCell) builds the rule appropriate for that cell's shape by combining 1D rules
 * (Initialize1DGauss / Initialize1DJacobi) via tensor products (TensorProductQuad,
 * TensorProductTriangle, TensorProductHexahedron, TensorProductWedge, TensorProductTetra); results
 * are cached and only recomputed if the cell type or order changes. This is the quadrature engine
 * used by vtkvmtkFEShapeFunctions / vtkvmtkFEAssembler subclasses to numerically integrate
 * element-level finite-element contributions.
 *
 * @sa
 * vtkvmtkFEShapeFunctions, vtkvmtkFEAssembler
 */

#ifndef __vtkvmtkGaussQuadrature_h
#define __vtkvmtkGaussQuadrature_h

#include "vtkObject.h"
#include "vtkvmtkWin32Header.h"

#include "vtkCell.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkGaussQuadrature : public vtkObject
{
public:
  vtkTypeMacro(vtkvmtkGaussQuadrature,vtkObject);
  static vtkvmtkGaussQuadrature* New();

  /**
   * Get the array of parametric quadrature point coordinates computed by the last Initialize()
   * call, one tuple per quadrature point (1, 2 or 3 components depending on cell dimensionality).
   */
  vtkGetObjectMacro(QuadraturePoints,vtkDoubleArray);

  /**
   * Get the array of quadrature weights computed by the last Initialize() call, one value per
   * quadrature point, in the same order as QuadraturePoints.
   */
  vtkGetObjectMacro(QuadratureWeights,vtkDoubleArray);

  ///@{
  /**
   * Set/get the order of the quadrature rule: the rule is exact for polynomials up to this degree.
   * Default: 1.
   */
  vtkSetMacro(Order,int);
  vtkGetMacro(Order,int);
  ///@}

  /**
   * Get the number of quadrature points produced by the last Initialize() call.
   */
  int GetNumberOfQuadraturePoints()
  {
    return this->QuadraturePoints->GetNumberOfTuples();
  }

  /**
   * Get a pointer to the parametric coordinates of the id-th quadrature point.
   */
  double* GetQuadraturePoint(vtkIdType id)
  {
    return this->QuadraturePoints->GetTuple(id);
  }

  /**
   * Copy the parametric coordinates of the id-th quadrature point into quadraturePoint.
   */
  void GetQuadraturePoint(vtkIdType id, double* quadraturePoint)
  {
    this->QuadraturePoints->GetTuple(id,quadraturePoint);
  }

  /**
   * Get the c-th parametric coordinate component of the id-th quadrature point.
   */
  double GetQuadraturePoint(vtkIdType id, int c)
  {
    return this->QuadraturePoints->GetComponent(id,c);
  }

  /**
   * Get the integration weight of the id-th quadrature point.
   */
  double GetQuadratureWeight(vtkIdType id)
  {
    return this->QuadratureWeights->GetValue(id);
  }

  /**
   * Build (or, if the cell type and Order match the previous call, reuse) the quadrature rule
   * appropriate for the given VTK cell type at the current Order. Supported cell types are line,
   * quadratic edge, quad, triangle, hexahedron, wedge and tetra, and their quadratic/biquadratic
   * variants.
   */
  void Initialize(vtkIdType cellType);

  /**
   * Convenience overload of Initialize(vtkIdType) that takes the cell type from cell.
   */
  void Initialize(vtkCell* cell)
  {
    this->Initialize(cell->GetCellType());
  }

  /**
   * Build a 1D Gauss-Legendre quadrature rule of the current Order on the reference interval
   * [-1,1], then rescale it to [0,1] via ScaleTo01(). Used internally as a building block for
   * tensor-product rules on higher-dimensional cells.
   */
  void Initialize1DGauss();

  /**
   * Build a 1D Gauss-Jacobi quadrature rule of the current Order, for Jacobi weight parameters
   * alpha and beta, on [0,1]. Used internally as a building block for tensor-product rules on
   * triangles and tetrahedra (which require a Jacobi-weighted rule to correctly integrate over a
   * simplex via a collapsed tensor-product mapping).
   */
  void Initialize1DJacobi(int alpha, int beta);

  /**
   * Rescale a 1D quadrature rule (points and weights) from the reference interval [-1,1] to [0,1].
   */
  void ScaleTo01();
 
protected:
  vtkvmtkGaussQuadrature();
  ~vtkvmtkGaussQuadrature();

  void TensorProductQuad(vtkvmtkGaussQuadrature* q1D);
  void TensorProductTriangle(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D);
  
  void TensorProductHexahedron(vtkvmtkGaussQuadrature* q1D);
  void TensorProductWedge(vtkvmtkGaussQuadrature* q1D, vtkvmtkGaussQuadrature* q2D);
  void TensorProductTetra(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D, vtkvmtkGaussQuadrature* jacB1D);
 
  vtkDoubleArray* QuadraturePoints;
  vtkDoubleArray* QuadratureWeights;
 
  int Order;
  int QuadratureType;
  vtkIdType CellType;
  int PreviousOrder;

private:  
  vtkvmtkGaussQuadrature(const vtkvmtkGaussQuadrature&);  // Not implemented.
  void operator=(const vtkvmtkGaussQuadrature&);  // Not implemented.

};

#endif
