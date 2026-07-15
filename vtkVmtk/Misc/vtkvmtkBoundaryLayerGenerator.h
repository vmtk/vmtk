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
 * @class   vtkvmtkBoundaryLayerGenerator
 * @brief   Generates boundary layers of prismatic elements by warping a surface mesh.
 * @ingroup Misc
 *
 * This class builds a volumetric boundary layer mesh from an input triangulated (or quadratic
 * triangulated) surface mesh and a per-point warp vector field (typically the inward surface
 * normals). Each surface point is swept along its warp vector by a thickness that is either
 * constant (ConstantThickness), taken from the warp vector magnitude
 * (UseWarpVectorMagnitudeAsThickness), or read from a separate per-point thickness array
 * (LayerThicknessArrayName), producing one or more sublayers of wedge/hexahedron elements
 * (NumberOfSubLayers, SubLayerRatio) stacked from the original surface (outer) to the innermost
 * warped surface (inner). The sweep is performed incrementally in small substeps
 * (NumberOfSubsteps) with Laplacian-like relaxation (Relaxation) of the intermediate surfaces, and
 * self-intersecting ("tangled") elements are detected and iteratively corrected
 * (LocalCorrectionFactor) so that the resulting layer remains valid even on curved or converging
 * geometry.
 *
 * This is the filter behind the vmtkboundarylayer pype script, used to generate near-wall
 * prismatic boundary layer elements (e.g. from surface normals scaled by a boundary layer
 * thickness field) prior to volumetric (tetrahedral) meshing of the interior, so that CFD
 * simulations can resolve near-wall gradients. The generated cells, together with the optional
 * original surface and sidewall cells (IncludeSurfaceCells, IncludeSidewallCells), are tagged
 * with entity ids (CellEntityIdsArrayName) distinguishing inner surface, outer surface, sidewall,
 * and volume cells, so that downstream meshing/BC-assignment tools can identify each region. The
 * innermost warped surface is also made available separately through GetInnerSurface, typically
 * used as the surface to feed to a subsequent volumetric mesh generator.
 *
 * @sa
 * vtkvmtkPolyDataBoundaryExtractor
 */

#ifndef __vtkvmtkBoundaryLayerGenerator_h
#define __vtkvmtkBoundaryLayerGenerator_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkPoints;
class vtkUnsignedCharArray;
class vtkDataArray;

class VTK_VMTK_MISC_EXPORT vtkvmtkBoundaryLayerGenerator : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkBoundaryLayerGenerator,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkBoundaryLayerGenerator *New();
  
  ///@{
  /**
   * Set/Get the name of the input point data vector array holding the direction (and, if
   * UseWarpVectorMagnitudeAsThickness is on, magnitude) along which each surface point is swept
   * to build the boundary layer. Typically the surface normals array. Must be set before Update()
   * is called. Commonly named "Normals".
   */
  vtkGetStringMacro(WarpVectorsArrayName);
  vtkSetStringMacro(WarpVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input point data scalar array holding the local boundary layer
   * thickness at each point. Used when neither ConstantThickness nor
   * UseWarpVectorMagnitudeAsThickness is on.
   */
  vtkGetStringMacro(LayerThicknessArrayName);
  vtkSetStringMacro(LayerThicknessArrayName);
  ///@}

  ///@{
  /**
   * Toggle using the magnitude of the WarpVectorsArrayName vectors as the local boundary layer
   * thickness, instead of a separate LayerThicknessArrayName array. Default: off.
   */
  vtkGetMacro(UseWarpVectorMagnitudeAsThickness,int);
  vtkSetMacro(UseWarpVectorMagnitudeAsThickness,int);
  vtkBooleanMacro(UseWarpVectorMagnitudeAsThickness,int);
  ///@}

  ///@{
  /**
   * Toggle using a single, uniform LayerThickness value for the whole boundary layer, instead of
   * a per-point thickness. Default: off.
   */
  vtkGetMacro(ConstantThickness,int);
  vtkSetMacro(ConstantThickness,int);
  vtkBooleanMacro(ConstantThickness,int);
  ///@}

  ///@{
  /**
   * Toggle inclusion, in the output unstructured grid, of the original surface cells (tagged with
   * InnerSurfaceCellEntityId on the innermost warped surface and OuterSurfaceCellEntityId on the
   * original outer surface). Default: off.
   */
  vtkGetMacro(IncludeSurfaceCells,int);
  vtkSetMacro(IncludeSurfaceCells,int);
  vtkBooleanMacro(IncludeSurfaceCells,int);
  ///@}

  ///@{
  /**
   * Toggle inclusion, in the output unstructured grid, of the quadrilateral sidewall cells
   * generated by sweeping the free edges of the input surface (edges shared by only one cell)
   * through the boundary layer, tagged with SidewallCellEntityId. Default: off.
   */
  vtkGetMacro(IncludeSidewallCells,int);
  vtkSetMacro(IncludeSidewallCells,int);
  vtkBooleanMacro(IncludeSidewallCells,int);
  ///@}

  ///@{
  /**
   * Toggle flipping the direction of the WarpVectorsArrayName vectors before sweeping, so that the
   * boundary layer grows on the opposite side of the surface (e.g. to switch between growing the
   * layer inward or outward). Default: off.
   */
  vtkGetMacro(NegateWarpVectors,int);
  vtkSetMacro(NegateWarpVectors,int);
  vtkBooleanMacro(NegateWarpVectors,int);
  ///@}

  ///@{
  /**
   * Set/Get the uniform boundary layer thickness used when ConstantThickness is on. Default: 1.0.
   */
  vtkGetMacro(LayerThickness,double);
  vtkSetMacro(LayerThickness,double);
  ///@}

  ///@{
  /**
   * Set/Get a multiplicative factor applied to the per-point values read from
   * LayerThicknessArrayName, scaling the overall boundary layer thickness up or down. Default:
   * 1.0.
   */
  vtkGetMacro(LayerThicknessRatio,double);
  vtkSetMacro(LayerThicknessRatio,double);
  ///@}

  ///@{
  /**
   * Set/Get the upper bound clamped onto the local boundary layer thickness at every point,
   * regardless of how it was computed. Default: VTK_VMTK_LARGE_DOUBLE (effectively unbounded).
   */
  vtkGetMacro(MaximumLayerThickness,double);
  vtkSetMacro(MaximumLayerThickness,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of prismatic sublayers the boundary layer is subdivided into, stacked from
   * the original (outer) surface to the innermost warped surface. Default: 1.
   */
  vtkGetMacro(NumberOfSubLayers,int);
  vtkSetMacro(NumberOfSubLayers,int);
  ///@}

  ///@{
  /**
   * Set/Get the thickness ratio between successive sublayers, moving away from the original
   * surface (a geometric growth/shrink factor applied sublayer to sublayer). Default: 1.0
   * (uniform sublayer thickness).
   */
  vtkGetMacro(SubLayerRatio,double);
  vtkSetMacro(SubLayerRatio,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of small incremental steps used to smoothly sweep the surface through the
   * boundary layer thickness, with relaxation applied at each step; higher values give a smoother,
   * more robust sweep on strongly curved/converging geometry at the cost of computation time.
   * Default: 2000.
   */
  vtkGetMacro(NumberOfSubsteps,int);
  vtkSetMacro(NumberOfSubsteps,int);
  ///@}

  ///@{
  /**
   * Set/Get the relaxation factor applied at each incremental warp substep to smooth the advancing
   * surface (see NumberOfSubsteps). Default: 0.01.
   */
  vtkGetMacro(Relaxation,double);
  vtkSetMacro(Relaxation,double);
  ///@}

  ///@{
  /**
   * Set/Get the amount of correction applied to the warp vectors of points found to produce
   * self-intersecting ("tangled") elements during the incremental sweep, in the iterative local
   * untangling procedure. Default: 0.45.
   */
  vtkGetMacro(LocalCorrectionFactor,double);
  vtkSetMacro(LocalCorrectionFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array used to tag every output cell (surface, sidewall, and
   * volume) with the entity id corresponding to its role (see InnerSurfaceCellEntityId,
   * OuterSurfaceCellEntityId, SidewallCellEntityId, VolumeCellEntityId). Must be set before
   * Update() is called.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the entity id written into CellEntityIdsArrayName for the innermost warped surface
   * cells (only relevant, and only added to the output, when IncludeSurfaceCells is on). Default:
   * 0.
   */
  vtkGetMacro(InnerSurfaceCellEntityId,int);
  vtkSetMacro(InnerSurfaceCellEntityId,int);
  ///@}

  ///@{
  /**
   * Set/Get the entity id written into CellEntityIdsArrayName for the original (outer) surface
   * cells (only relevant, and only added to the output, when IncludeSurfaceCells is on). Default:
   * 0.
   */
  vtkGetMacro(OuterSurfaceCellEntityId,int);
  vtkSetMacro(OuterSurfaceCellEntityId,int);
  ///@}

  ///@{
  /**
   * Set/Get the entity id written into CellEntityIdsArrayName for the sidewall cells generated by
   * sweeping the free edges of the input surface (only relevant, and only added to the output,
   * when IncludeSidewallCells is on). Default: 0.
   */
  vtkGetMacro(SidewallCellEntityId,int);
  vtkSetMacro(SidewallCellEntityId,int);
  ///@}

  ///@{
  /**
   * Set/Get the entity id written into CellEntityIdsArrayName for the prismatic volume cells that
   * make up the boundary layer. Default: 0.
   */
  vtkGetMacro(VolumeCellEntityId,int);
  vtkSetMacro(VolumeCellEntityId,int);
  ///@}

  /**
   * Get the innermost warped surface generated by sweeping the input surface through the full
   * boundary layer thickness, as an unstructured grid tagged with InnerSurfaceCellEntityId. Valid
   * only after Update() has been called; typically fed to a subsequent volumetric mesh generator.
   */
  vtkGetObjectMacro(InnerSurface,vtkUnstructuredGrid);

  protected:
  vtkvmtkBoundaryLayerGenerator();
  ~vtkvmtkBoundaryLayerGenerator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void BuildWarpVectors(vtkUnstructuredGrid* input);
  void IncrementalWarpPoints(vtkUnstructuredGrid* input, vtkPoints* basePoints, vtkPoints* warpedPoints, int substep, int numberOfSubsteps, double relaxation);
  void IncrementalWarpVectors(vtkUnstructuredGrid* input, int numberOfSubsteps, double relaxation);
  int CheckTangle(vtkUnstructuredGrid* input, vtkUnsignedCharArray* checkArray);
  void LocalUntangle(vtkUnstructuredGrid* input, vtkUnsignedCharArray* checkArray, double alpha); 
  void WarpPoints(vtkPoints* inputPoints, vtkPoints* warpedPoints, int subLayerId, bool quadratic);
  void UnwrapSublayers(vtkUnstructuredGrid* input, vtkPoints* outputPoints);

  vtkDataArray* WarpVectorsArray;
  vtkDataArray* LayerThicknessArray;

  int UseWarpVectorMagnitudeAsThickness;
  int ConstantThickness;

  char* WarpVectorsArrayName;
  char* LayerThicknessArrayName;

  double LayerThickness;
  double LayerThicknessRatio;
  double MaximumLayerThickness;

  int NumberOfSubLayers;
  int NumberOfSubsteps;
  double SubLayerRatio;

  int IncludeSurfaceCells;
  int IncludeSidewallCells;
  int NegateWarpVectors;

  vtkUnstructuredGrid* InnerSurface;

  char* CellEntityIdsArrayName;
  int InnerSurfaceCellEntityId;
  int OuterSurfaceCellEntityId;
  int SidewallCellEntityId;
  int VolumeCellEntityId;

  double Relaxation;
  double LocalCorrectionFactor;

  private:
  vtkvmtkBoundaryLayerGenerator(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryLayerGenerator&);  // Not implemented.
};

#endif
