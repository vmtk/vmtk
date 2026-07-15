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
 * @class   vtkvmtkPolyDataCenterlines
 * @brief   Compute centerlines from surface.
 * @ingroup ComputationalGeometry
 *
 * Computes centerlines (with a maximum inscribed sphere radius at each point) between a set of
 * source and target boundary points on a closed surface, using the Voronoi-diagram-based method of
 * Antiga & Steinman: the surface is first Delaunay-tessellated (unless a precomputed
 * DelaunayTessellation is supplied), then its Voronoi diagram is extracted -- the Voronoi diagram of
 * points on a tubular surface approximates its medial axis, with the distance to the surface giving
 * the local maximum inscribed sphere radius. A single-source shortest-path search (Fast Marching /
 * Dijkstra-like propagation, weighted by CostFunction, typically favoring paths through the widest
 * part of the vessel) is then run on the Voronoi diagram from the source seed(s) to each target
 * seed, and the resulting paths become the output centerlines. This is the filter behind the
 * vmtkcenterlines pype script, the foundation of most other centerline-based analysis in vmtk.
 *
 * @sa
 * vtkvmtkVoronoiDiagram3D, vtkvmtkNonManifoldFastMarching, vtkvmtkSteepestDescentLineTracer
 */

#ifndef __vtkvmtkPolyDataCenterlines_h
#define __vtkvmtkPolyDataCenterlines_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkUnstructuredGrid.h"

class vtkPolyData;
class vtkPoints;
class vtkIdList;
class vtkDataArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlines : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataCenterlines,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataCenterlines *New();

  ///@{
  /**
   * Set/Get the ids, on the input surface, of the point(s) from which the centerline search starts.
   * Required input.
   */
  virtual void SetSourceSeedIds(vtkIdList*);
  vtkGetObjectMacro(SourceSeedIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the ids, on the input surface, of the point(s) that a centerline is computed to from
   * each source seed. Required input.
   */
  virtual void SetTargetSeedIds(vtkIdList*);
  vtkGetObjectMacro(TargetSeedIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the ids of cap-center points (e.g. as produced by vtkvmtkCapPolyData) on the input
   * surface. When set, these points are used to seed the Voronoi diagram computation more robustly
   * near capped boundaries.
   */
  virtual void SetCapCenterIds(vtkIdList*);
  vtkGetObjectMacro(CapCenterIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get a precomputed Delaunay tessellation of the input surface to use instead of computing one
   * internally (see GenerateDelaunayTessellation). After Update(), also usable to retrieve the
   * (possibly internally computed) tessellation.
   */
  vtkSetObjectMacro(DelaunayTessellation,vtkUnstructuredGrid);
  vtkGetObjectMacro(DelaunayTessellation,vtkUnstructuredGrid);
  ///@}

  ///@{
  /**
   * Set/Get a precomputed Voronoi diagram of the input surface to use instead of computing one
   * internally (see GenerateVoronoiDiagram; also requires PoleIds to be set). After Update(), also
   * usable to retrieve the (possibly internally computed) diagram.
   */
  vtkSetObjectMacro(VoronoiDiagram,vtkPolyData);
  vtkGetObjectMacro(VoronoiDiagram,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the "pole" ids of the Delaunay tessellation (points of the tessellation that are
   * vertices of the Voronoi diagram); required together with VoronoiDiagram when supplying a
   * precomputed diagram. After Update(), also usable to retrieve the internally computed pole ids.
   */
  vtkSetObjectMacro(PoleIds,vtkIdList);
  vtkGetObjectMacro(PoleIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where the maximum inscribed sphere radius is stored, on
   * both the output centerlines and the Voronoi diagram. Default: "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the cost function used to weight the shortest-path search on the Voronoi diagram,
   * expressed as a string in terms of R (the local maximum inscribed sphere radius); e.g. the
   * default "1/R" biases the path towards the widest part of the vessel (favoring centeredness), a
   * larger exponent such as "1/R2" biases it even more strongly, and "1" reduces to a plain geodesic
   * (shortest Euclidean path).
   */
  vtkSetStringMacro(CostFunction);
  vtkGetStringMacro(CostFunction);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the Voronoi diagram where the Eikonal (shortest-path
   * travel time) solution is stored.
   * Commonly named "EikonalSolution".
   */
  vtkSetStringMacro(EikonalSolutionArrayName);
  vtkGetStringMacro(EikonalSolutionArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the Voronoi diagram used internally to track, for
   * each point, the edge of the shortest-path tree it was reached through.
   * Commonly named "EdgeArray".
   */
  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the Voronoi diagram used internally to store the
   * parametric coordinate along the shortest-path tree edge (see EdgeArrayName) at which each point
   * was reached.
   * Commonly named "EdgePCoordArray".
   */
  vtkSetStringMacro(EdgePCoordArrayName);
  vtkGetStringMacro(EdgePCoordArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the Voronoi diagram where the evaluated CostFunction
   * value is stored at each point.
   * Commonly named "CostFunctionArray".
   */
  vtkSetStringMacro(CostFunctionArrayName);
  vtkGetStringMacro(CostFunctionArrayName);
  ///@}

  ///@{
  /**
   * Toggle flipping the orientation of the surface normals used when computing the Voronoi diagram.
   * Turn on if the resulting centerlines look degenerate/incorrect, which can indicate the input
   * surface has inward-pointing normals. Default: off.
   */
  vtkSetMacro(FlipNormals,int);
  vtkGetMacro(FlipNormals,int);
  vtkBooleanMacro(FlipNormals,int);
  ///@}

  ///@{
  /**
   * Toggle simplifying the Voronoi diagram before the shortest-path search, discarding points/cells
   * that don't contribute to the medial axis (see vtkvmtkSimplifyVoronoiDiagram). Speeds up
   * processing of large models at a small accuracy cost. Default: off.
   */
  vtkSetMacro(SimplifyVoronoi,int);
  vtkGetMacro(SimplifyVoronoi,int);
  vtkBooleanMacro(SimplifyVoronoi,int);
  ///@}

  ///@{
  /**
   * Toggle resampling the output centerlines to a uniform point spacing of ResamplingStepLength.
   * Default: off.
   */
  vtkSetMacro(CenterlineResampling,int);
  vtkGetMacro(CenterlineResampling,int);
  vtkBooleanMacro(CenterlineResampling,int);
  ///@}

  ///@{
  /**
   * Set/Get the point spacing used to resample the output centerlines when CenterlineResampling is
   * on. Default: 1.0.
   */
  vtkSetMacro(ResamplingStepLength,double);
  vtkGetMacro(ResamplingStepLength,double);
  ///@}

  ///@{
  /**
   * Toggle appending a short straight segment at each free end of the output centerlines, from the
   * seed point on the surface to the corresponding source/target seed's cap center (requires
   * CapCenterIds to be set). Default: off.
   */
  vtkSetMacro(AppendEndPointsToCenterlines,int);
  vtkGetMacro(AppendEndPointsToCenterlines,int);
  vtkBooleanMacro(AppendEndPointsToCenterlines,int);
  ///@}

  ///@{
  /**
   * Toggle computing the Delaunay tessellation internally from the input surface. Turn off and
   * supply DelaunayTessellation directly to reuse a tessellation computed elsewhere (e.g. to speed up
   * repeated centerline extractions on the same surface). Default: on.
   */
  vtkSetMacro(GenerateDelaunayTessellation,int);
  vtkGetMacro(GenerateDelaunayTessellation,int);
  vtkBooleanMacro(GenerateDelaunayTessellation,int);
  ///@}

  ///@{
  /**
   * Toggle computing the Voronoi diagram internally from the Delaunay tessellation. Turn off and
   * supply VoronoiDiagram and PoleIds directly to reuse a diagram computed elsewhere. Default: on.
   */
  vtkSetMacro(GenerateVoronoiDiagram,int);
  vtkGetMacro(GenerateVoronoiDiagram,int);
  vtkBooleanMacro(GenerateVoronoiDiagram,int);
  ///@}

  ///@{
  /**
   * Toggle stopping the shortest-path propagation as soon as all target seeds have been reached,
   * rather than continuing until the whole Voronoi diagram is visited. Speeds up processing when only
   * a few targets are needed out of a large model. Default: off.
   */
  vtkSetMacro(StopFastMarchingOnReachingTarget,int);
  vtkGetMacro(StopFastMarchingOnReachingTarget,int);
  vtkBooleanMacro(StopFastMarchingOnReachingTarget,int);
  ///@}

  ///@{
  /**
   * Set/Get the numerical tolerance used when computing the Delaunay tessellation (points closer
   * together than this are merged). Default: 1E-3.
   */
  vtkSetMacro(DelaunayTolerance,double);
  vtkGetMacro(DelaunayTolerance,double);
  ///@}


  protected:
  vtkvmtkPolyDataCenterlines();
  ~vtkvmtkPolyDataCenterlines();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void FindVoronoiSeeds(vtkUnstructuredGrid *delaunay, vtkIdList *boundaryBaricenterIds, vtkDataArray *normals, vtkIdList *seedIds);
  void AppendEndPoints(vtkPoints* endPointPairs);
  void ResampleCenterlines();
  void ReverseCenterlines();

  vtkIdList* SourceSeedIds;
  vtkIdList* TargetSeedIds;

  vtkIdList* CapCenterIds;

  vtkUnstructuredGrid* DelaunayTessellation;

  vtkPolyData* VoronoiDiagram;

  vtkIdList* PoleIds;

  char* RadiusArrayName;
  char* CostFunction;
  char* EikonalSolutionArrayName;
  char* EdgeArrayName;
  char* EdgePCoordArrayName;
  char* CostFunctionArrayName;

  int FlipNormals;
  int SimplifyVoronoi;
  int AppendEndPointsToCenterlines;
  int CenterlineResampling;

  double ResamplingStepLength;

  int GenerateVoronoiDiagram;
  int StopFastMarchingOnReachingTarget;
  int GenerateDelaunayTessellation;
  double DelaunayTolerance;

  private:
  vtkvmtkPolyDataCenterlines(const vtkvmtkPolyDataCenterlines&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlines&);  // Not implemented.
};

#endif
