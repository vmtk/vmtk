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
 * @class   vtkvmtkLinearToQuadraticMeshFilter
 * @brief   Converts linear mesh elements to quadratic mesh elements (optionally) by executing by projecting mid side nodes onto the surface and relaxing projection if Jacobian is negative, otherwise does not project nodes.
 * @ingroup Misc
 *
 * This filter takes an unstructured grid made of linear elements (tetrahedra, wedges, hexahedra,
 * and their bounding triangles/quads) and produces the corresponding second-order (quadratic)
 * elements by inserting a new mid-edge (and, for hexahedra/quads, mid-face and mid-volume) node
 * for every unique edge/face/volume, interpolating point data at the new nodes and copying cell
 * data unchanged. Mid-side nodes of boundary elements can optionally be snapped onto a
 * ReferenceSurface (e.g. the original, higher-resolution surface the mesh was generated from,
 * restricted to cells tagged with ProjectedCellEntityId in CellEntityIdsArrayName) rather than
 * left at the linear edge midpoint, so that the quadratic mesh better approximates curved
 * boundaries; because such projection can locally invert an element (a negative Jacobian),
 * projected nodes can be pulled back towards the linear midpoint (JacobianRelaxation) until the
 * element's Jacobian, sampled at QuadratureOrder Gauss points, is no longer detected as having
 * changed sign beyond NegativeJacobianTolerance.
 *
 * This is one of the filters behind the vmtklineartoquadratic pype script (used together with
 * vtkvmtkLinearToQuadraticSurfaceMeshFilter, which handles pure surface meshes), typically used to
 * upgrade a linear tetrahedral/hexahedral volume mesh to a curved, second-order mesh for
 * higher-order finite element analysis.
 *
 * @sa
 * vtkvmtkLinearToQuadraticSurfaceMeshFilter
 */

#ifndef __vtkvmtkLinearToQuadraticMeshFilter_h
#define __vtkvmtkLinearToQuadraticMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkCell.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearToQuadraticMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearToQuadraticMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkLinearToQuadraticMeshFilter *New();
  
  ///@{
  /**
   * Toggle converting linear wedges into 18-node biquadratic-quadratic wedges (VTK_BIQUADRATIC_QUADRATIC_WEDGE);
   * when off, wedges are converted into 15-node quadratic wedges (VTK_QUADRATIC_WEDGE) without
   * face nodes. Default: on.
   */
  vtkSetMacro(UseBiquadraticWedge,int);
  vtkGetMacro(UseBiquadraticWedge,int);
  vtkBooleanMacro(UseBiquadraticWedge,int);
  ///@}

  ///@{
  /**
   * Set/Get the target quadratic hexahedron element type by node count: 20 (VTK_QUADRATIC_HEXAHEDRON,
   * edge nodes only), 24 (VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON, edge + side-face nodes), or 27
   * (VTK_TRIQUADRATIC_HEXAHEDRON, edge + all face + volume nodes). Also controls whether bounding
   * quads are converted to 8-node (VTK_QUADRATIC_QUAD) or 9-node (VTK_BIQUADRATIC_QUAD) elements.
   * Default: 27.
   */
  vtkSetMacro(NumberOfNodesHexahedra,int);
  vtkGetMacro(NumberOfNodesHexahedra,int);
  ///@}

  ///@{
  /**
   * Set/Get the (typically higher-resolution) reference surface that newly inserted mid-side nodes
   * of boundary elements are snapped onto, instead of being left at the linear edge/face midpoint.
   * If NULL (default), no projection is performed. Only cells tagged with ProjectedCellEntityId in
   * CellEntityIdsArrayName are projected when both are set.
   */
  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input cell data array holding entity ids, used together with
   * ProjectedCellEntityId to restrict projection of mid-side nodes onto ReferenceSurface to a
   * specific subset of boundary cells (e.g. only the vessel wall, not caps/inlets/outlets). If
   * NULL (default), all boundary cells are eligible for projection.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the entity id (into CellEntityIdsArrayName) of the boundary cells whose mid-side nodes
   * should be projected onto ReferenceSurface. Ignored if CellEntityIdsArrayName is NULL. Default:
   * -1.
   */
  vtkSetMacro(ProjectedCellEntityId,int);
  vtkGetMacro(ProjectedCellEntityId,int);
  ///@}

  ///@{
  /**
   * Set/Get the order of the Gauss quadrature rule used to sample the Jacobian of each volume
   * element when checking whether projecting a mid-side node onto ReferenceSurface has inverted
   * (produced a sign change in the Jacobian of) the element. Default: 10.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

  ///@{
  /**
   * Set/Get the tolerance below which the product of the linear and quadratic element Jacobians,
   * sampled at each quadrature/node point, is considered a sign change (i.e. an inverted element).
   * Default: 0.0.
   */
  vtkSetMacro(NegativeJacobianTolerance,double);
  vtkGetMacro(NegativeJacobianTolerance,double);
  ///@}

  ///@{
  /**
   * Toggle relaxing (pulling back towards the linear edge/face midpoint) mid-side nodes that were
   * projected onto ReferenceSurface but whose projection would invert the adjacent volume element,
   * so that the final quadratic mesh has no negative Jacobians. Default: on.
   */
  vtkSetMacro(JacobianRelaxation,int);
  vtkGetMacro(JacobianRelaxation,int);
  vtkBooleanMacro(JacobianRelaxation,int);
  ///@}

  ///@{
  /**
   * Toggle a final pass, after all mid-side nodes have been placed, that re-checks every volume
   * element for a negative Jacobian (see NegativeJacobianTolerance) and emits an error for each one
   * still found, as a diagnostic on the quality of the output quadratic mesh. Default: off.
   */
  vtkSetMacro(TestFinalJacobians,int);
  vtkGetMacro(TestFinalJacobians,int);
  vtkBooleanMacro(TestFinalJacobians,int);
  ///@}

  protected:
  vtkvmtkLinearToQuadraticMeshFilter();
  ~vtkvmtkLinearToQuadraticMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  bool HasJacobianChangedSign(vtkCell* linearVolumeCell, vtkCell* quadraticVolumeCell);
  double ComputeJacobian(vtkCell* cell, double pcoords[3]);

  int UseBiquadraticWedge;

  int NumberOfNodesHexahedra;

  vtkPolyData* ReferenceSurface;

  char* CellEntityIdsArrayName;
  int ProjectedCellEntityId;
  int QuadratureOrder;
  double NegativeJacobianTolerance;

  int JacobianRelaxation;
  int TestFinalJacobians;

  private:
  vtkvmtkLinearToQuadraticMeshFilter(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
};

#endif
