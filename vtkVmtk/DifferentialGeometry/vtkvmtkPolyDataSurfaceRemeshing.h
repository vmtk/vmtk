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
 * @class   vtkvmtkPolyDataSurfaceRemeshing
 * @brief   Remesh a triangulated surface into a mesh of more uniform, better-shaped ("quality")
 * triangles.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataSurfaceRemeshing iteratively improves a triangulated surface mesh through a
 * combination of edge flipping (topology/Delaunay optimization), edge collapse (removing
 * undersized or poorly shaped triangles), edge/triangle splitting (subdividing oversized
 * triangles), and point relocation (Laplacian-like smoothing, optionally reprojected onto the
 * original surface), aiming to bring every triangle's area close to a target element size while
 * keeping triangle aspect ratio and internal angles within acceptable bounds. Target element size
 * can be a single scalar (TargetArea, optionally scaled by TargetAreaFactor) or spatially varying,
 * read from a point data array (TargetAreaArrayName) when ElementSizeMode is set to
 * TARGET_AREA_ARRAY. Cell entity ids (CellEntityIdsArrayName) can be used to keep the boundaries
 * between differently-tagged regions (e.g. inlets/outlets/wall) fixed during remeshing, and
 * specific entities can be excluded from remeshing altogether (ExcludedEntityIds). This is the
 * filter behind the vmtksurfaceremeshing pype script, commonly used to prepare a surface with
 * well-shaped elements prior to volumetric mesh generation.
 *
 * @sa vtkvmtkPolyDataUmbrellaStencil, vtkvmtkCapPolyData
 */

#ifndef __vtkvmtkPolyDataSurfaceRemeshing_h
#define __vtkvmtkPolyDataSurfaceRemeshing_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"
#include "vtkIdList.h"

class vtkCellLocator;
class vtkIntArray;

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataSurfaceRemeshing : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataSurfaceRemeshing *New();
  vtkTypeMacro(vtkvmtkPolyDataSurfaceRemeshing,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the maximum triangle aspect ratio (Frobenius-norm based, as used internally by
   * TestAspectRatioCollapseEdge) tolerated before an edge of the triangle becomes a candidate for
   * collapsing. Default: 1.2.
   */
  vtkSetMacro(AspectRatioThreshold,double);
  vtkGetMacro(AspectRatioThreshold,double);
  ///@}

  ///@{
  /**
   * Set/get the tolerance, in radians, used when testing whether flipping an edge would improve
   * the minimum internal angle of the two triangles sharing it (edge-flip optimization). Default:
   * 0.0.
   */
  vtkSetMacro(InternalAngleTolerance,double);
  vtkGetMacro(InternalAngleTolerance,double);
  ///@}

  ///@{
  /**
   * Set/get the maximum angle, in radians, allowed between the normals of the two triangles
   * sharing an edge for that edge to remain a candidate for flipping or collapsing; edges bounding
   * a sharp feature (larger normal angle) are preserved. Default: 0.2.
   */
  vtkSetMacro(NormalAngleTolerance,double);
  vtkGetMacro(NormalAngleTolerance,double);
  ///@}

  ///@{
  /**
   * Set/get the maximum change in surface normal orientation, in radians, tolerated when
   * collapsing an edge; if collapsing would tilt the normal of any triangle around the surviving
   * point by more than this angle (protecting sharp geometric features from being flattened), the
   * collapse is rejected. Default: 0.5.
   */
  vtkSetMacro(CollapseAngleThreshold,double);
  vtkGetMacro(CollapseAngleThreshold,double);
  ///@}

  ///@{
  /**
   * Set/get the relaxation factor used when relocating interior points towards the centroid of
   * their one-ring neighborhood (Laplacian-like smoothing pass run as part of each remeshing
   * iteration). Default: 0.5.
   */
  vtkSetMacro(Relaxation,double);
  vtkGetMacro(Relaxation,double);
  ///@}

  ///@{
  /**
   * Set/get the uniform target triangle area used when ElementSizeMode is TARGET_AREA; triangles
   * larger than this (scaled by TriangleSplitFactor) are split, and edges of triangles
   * significantly smaller than this (scaled by MinAreaFactor) are candidates for collapsing.
   * Default: 1.0.
   */
  vtkSetMacro(TargetArea,double);
  vtkGetMacro(TargetArea,double);
  ///@}

  ///@{
  /**
   * Set/get a multiplicative factor applied to the per-triangle target area read from
   * TargetAreaArrayName when ElementSizeMode is TARGET_AREA_ARRAY (has no effect in TARGET_AREA
   * mode). Default: 1.0.
   */
  vtkSetMacro(TargetAreaFactor,double);
  vtkGetMacro(TargetAreaFactor,double);
  ///@}

  ///@{
  /**
   * Set/get the multiplier applied to the local target area to decide when a triangle should be
   * split: a triangle becomes a split candidate once its area exceeds TriangleSplitFactor times
   * the target area. Default: 1.0.
   */
  vtkSetMacro(TriangleSplitFactor,double);
  vtkGetMacro(TriangleSplitFactor,double);
  ///@}

  ///@{
  /**
   * Set/get the multiplier applied to the local target area below which a triangle's shortest
   * edge becomes a candidate for collapsing, in addition to the AspectRatioThreshold test.
   * Default: 0.5.
   */
  vtkSetMacro(MinAreaFactor,double);
  vtkGetMacro(MinAreaFactor,double);
  ///@}

  ///@{
  /**
   * Set/get the hard upper bound clamped onto the computed local target triangle area, regardless
   * of ElementSizeMode. Default: VTK_VMTK_LARGE_FLOAT (effectively unbounded).
   */
  vtkSetMacro(MaxArea,double);
  vtkGetMacro(MaxArea,double);
  ///@}

  ///@{
  /**
   * Set/get the hard lower bound clamped onto the computed local target triangle area, regardless
   * of ElementSizeMode. Default: 0.0.
   */
  vtkSetMacro(MinArea,double);
  vtkGetMacro(MinArea,double);
  ///@}

  ///@{
  /**
   * Set/get the number of outer remeshing iterations (edge flip/collapse/split/point-relocation
   * passes) performed. Default: 10.
   */
  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the number of edge-flip connectivity-optimization passes performed (improving vertex
   * valence / Delaunay-ness) within each main remeshing iteration. Default: 20.
   */
  vtkSetMacro(NumberOfConnectivityOptimizationIterations,int);
  vtkGetMacro(NumberOfConnectivityOptimizationIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data array holding a spatially varying target triangle area,
   * used when ElementSizeMode is TARGET_AREA_ARRAY.
   * Commonly named "TargetArea".
   */
  vtkSetStringMacro(TargetAreaArrayName);
  vtkGetStringMacro(TargetAreaArrayName);
  ///@}

  ///@{
  /**
   * Set/get how the local target triangle area is determined: TARGET_AREA uses the uniform value
   * TargetArea (times TargetAreaFactor); TARGET_AREA_ARRAY interpolates the per-point array named
   * by TargetAreaArrayName at the triangle center (also scaled by TargetAreaFactor). Convenience
   * setters SetElementSizeModeToTargetArea() and SetElementSizeModeToTargetAreaArray() are
   * provided. Default: TARGET_AREA.
   */
  vtkSetMacro(ElementSizeMode,int);
  vtkGetMacro(ElementSizeMode,int);
  ///@}
  void SetElementSizeModeToTargetArea()
  { this->SetElementSizeMode(TARGET_AREA); }
  void SetElementSizeModeToTargetAreaArray()
  { this->SetElementSizeMode(TARGET_AREA_ARRAY); }

  ///@{
  /**
   * Toggle whether edges lying on an open boundary of the surface (or, when
   * CellEntityIdsArrayName is set, on the boundary between two differently-tagged entities) are
   * protected from flipping, collapsing, or splitting. When on, an EDGE_ON_BOUNDARY test always
   * blocks the corresponding operation. Default: off.
   */
  vtkSetMacro(PreserveBoundaryEdges,int);
  vtkGetMacro(PreserveBoundaryEdges,int);
  vtkBooleanMacro(PreserveBoundaryEdges,int);
  ///@}

  ///@{
  /**
   * Set/get the name of the cell data array of the input holding an integer entity id per cell
   * (e.g. distinguishing inlet/outlet/wall regions, such as produced by vtkvmtkCapPolyData). When
   * set, the boundary between cells with different entity ids is treated like a mesh boundary
   * and, together with ExcludedEntityIds, controls which triangles participate in remeshing.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/get the list of CellEntityIdsArrayName entity ids whose triangles are left untouched by
   * remeshing (skipped by all flip/collapse/split/relocation operations). Has no effect if
   * CellEntityIdsArrayName is not set. If not set (default, NULL), no entities are excluded.
   */
  vtkSetObjectMacro(ExcludedEntityIds,vtkIdList);
  vtkGetObjectMacro(ExcludedEntityIds,vtkIdList);
  ///@}

  //BTX
  enum {
    SUCCESS = 0,
    EDGE_ON_BOUNDARY,
    EDGE_BETWEEN_ENTITIES,
    EDGE_LOCKED,
    NOT_EDGE,
    NON_MANIFOLD,
    NOT_TRIANGLES,
    DEGENERATE_TRIANGLES,
    TRIANGLE_LOCKED
  };

  enum {
    DO_CHANGE,
    DO_NOTHING
  };

  enum {
    TARGET_AREA,
    TARGET_AREA_ARRAY
  };

  enum {
    RELOCATE_SUCCESS,
    RELOCATE_FAILURE
  };

  enum {
    INTERNAL_POINT,
    POINT_ON_BOUNDARY,
    NO_NEIGHBORS
  };
  //ETX

protected:
  vtkvmtkPolyDataSurfaceRemeshing();
  ~vtkvmtkPolyDataSurfaceRemeshing();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void BuildEntityBoundary(vtkPolyData* input, vtkPolyData* entityBoundary);

  int EdgeFlipConnectivityOptimizationIteration();
  int EdgeFlipIteration();
  int EdgeCollapseIteration();
  int TriangleSplitIteration();
  int EdgeSplitIteration();
  int PointRelocationIteration(bool projectToSurface=true);

  int TestFlipEdgeValidity(vtkIdType pt1, vtkIdType pt2, vtkIdType cell1, vtkIdType cell2, vtkIdType pt3, vtkIdType pt4);
  int TestConnectivityFlipEdge(vtkIdType pt1, vtkIdType pt2);
  int TestDelaunayFlipEdge(vtkIdType pt1, vtkIdType pt2);
  int TestAspectRatioCollapseEdge(vtkIdType cellId, vtkIdType& pt1, vtkIdType& pt2);
  int TestTriangleSplit(vtkIdType cellId);
  int TestAreaSplitEdge(vtkIdType cellId, vtkIdType& pt1, vtkIdType& pt2);
  
  int IsElementExcluded(vtkIdType cellId);
  int GetEdgeCellsAndOppositeEdge(vtkIdType pt1, vtkIdType pt2, vtkIdType& cell1, vtkIdType& cell2, vtkIdType& pt3, vtkIdType& pt4);

  int SplitEdge(vtkIdType pt1, vtkIdType pt2);
  int CollapseEdge(vtkIdType pt1, vtkIdType pt2);
  int FlipEdge(vtkIdType pt1, vtkIdType pt2);

  int SplitTriangle(vtkIdType cellId);
  int CollapseTriangle(vtkIdType cellId);

  int RelocatePoint(vtkIdType pointId, bool projectToSurface);

  int IsPointOnBoundary(vtkIdType pointId);
  int IsPointOnEntityBoundary(vtkIdType pointId);

  int GetNumberOfBoundaryEdges(vtkIdType cellId);

  double ComputeTriangleTargetArea(vtkIdType cellId);

  int FindOneRingNeighbors(vtkIdType pointId, vtkIdList* neighborIds);

  vtkPolyData* Mesh;
  vtkPolyData* InputBoundary;
  vtkPolyData* InputEntityBoundary;
  vtkCellLocator* Locator;
  vtkCellLocator* BoundaryLocator;
  vtkCellLocator* EntityBoundaryLocator;
  vtkIntArray* CellEntityIdsArray;
  vtkDataArray* TargetAreaArray;
  vtkIdList* ExcludedEntityIds;

  double AspectRatioThreshold;
  double InternalAngleTolerance;
  double NormalAngleTolerance;
  double CollapseAngleThreshold;
  double Relaxation;
  int NumberOfConnectivityOptimizationIterations;
  int NumberOfIterations;

  int PreserveBoundaryEdges;

  int ElementSizeMode;
  double TargetArea;
  double TargetAreaFactor;
  double MaxArea;
  double MinArea;
  double MinAreaFactor;
  double TriangleSplitFactor;
  char* TargetAreaArrayName;

  char* CellEntityIdsArrayName;

private:
  vtkvmtkPolyDataSurfaceRemeshing(const vtkvmtkPolyDataSurfaceRemeshing&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSurfaceRemeshing&);  // Not implemented.
};

#endif

