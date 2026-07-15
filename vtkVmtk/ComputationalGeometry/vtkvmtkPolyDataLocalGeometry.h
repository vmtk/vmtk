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
 * @class   vtkvmtkPolyDataLocalGeometry
 * @brief   Compute shape-related quantities over a surface.
 * @ingroup ComputationalGeometry
 *
 * This class computes geodesic distance to centerlines and normalized tangency deviation distribution over a surface given the Voronoi diagram of its points, the ids of Voronoi diagram points which are poles of surface points, and the field of the geodesic distance from centerlines computed over the Voronoi diagram.
 *
 * Geodesic distance to centerlines field is computed as
 * \f[ D_g(\mathbf{x}) = T(\mathbf{p}(\mathbf{x})) + |\mathbf{p}(\mathbf{x}) - \mathbf{x}| \f]
 * where T(x) is the geodesic distance to centerlines field defined on the Voronoi diagram (computed by solving the Eikonal equation with unit speed over the Voronoi diagram by means of vtkNonManifoldFastMarching), and p(x) is the pole associated with surface point x.
 *
 * Normalized tangency deviation is a normalized measure of how much a surface point is far from being a tangency point to the evelope of maximal spheres defined on the centerlines (their envelope is the maximal inscribed cylinder), and it is computed as
 * \f[\mathit{NTD}=\frac{T(\mathbf{p}(\mathbf{x}))}{D_g(\mathbf{x})}\f]
 * This quantity ranges from 0 to 1, and it is independent from the radius of the maximal inscribed cylinder (on an elliptical base cylinder, NTD only depends on base ellipse eccentricity).
 *
 * The description given here is particularly suited for the description of tubular surfaces in terms of centerlines, but this class can be used without this assumption. Whenever a geodesic distance field is computed over the Voronoi diagram associated with a shape from a subset of the Voronoi diagram itself (in this context, the centerlines), the surface geodesic distance field and the normalized tangency deviation can be defined, and this class can be used to compute them.
 *
 *
 * @sa
 * vtkNonManifoldFastMarching vtkVoronoiDiagram3D
 */

#ifndef __vtkvmtkPolyDataLocalGeometry_h
#define __vtkvmtkPolyDataLocalGeometry_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataLocalGeometry : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataLocalGeometry,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataLocalGeometry *New();
  
  ///@{
  /**
   * Turn on/off the computation, for every surface point, of the vector from that point to its
   * Voronoi pole (the point of VoronoiDiagram identified by PoleIds), stored in PoleVectorsArrayName.
   */
  vtkSetMacro(ComputePoleVectors,int);
  vtkGetMacro(ComputePoleVectors,int);
  vtkBooleanMacro(ComputePoleVectors,int);
  ///@}

  ///@{
  /**
   * Turn on/off the computation of geodesic distance to centerlines.
   */
  vtkSetMacro(ComputeGeodesicDistance,int);
  vtkGetMacro(ComputeGeodesicDistance,int);
  vtkBooleanMacro(ComputeGeodesicDistance,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where geodesic distance to centerlines is stored after
   * execution.
   * Commonly named "GeodesicDistance".
   */
  vtkSetStringMacro(GeodesicDistanceArrayName);
  vtkGetStringMacro(GeodesicDistanceArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off the computation of normalized tangency deviation.
   */
  vtkSetMacro(ComputeNormalizedTangencyDeviation,int);
  vtkGetMacro(ComputeNormalizedTangencyDeviation,int);
  vtkBooleanMacro(ComputeNormalizedTangencyDeviation,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where normalized tangency deviation is stored after
   * execution.
   * Commonly named "NTD".
   */
  vtkSetStringMacro(NormalizedTangencyDeviationArrayName);
  vtkGetStringMacro(NormalizedTangencyDeviationArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off the computation, for every surface point, of the Euclidean distance to the
   * corresponding point on the centerlines (surface point's pole, offset by the pole's
   * VoronoiPoleCenterlineVectorsArrayName vector), stored in EuclideanDistanceArrayName.
   */
  vtkSetMacro(ComputeEuclideanDistance,int);
  vtkGetMacro(ComputeEuclideanDistance,int);
  vtkBooleanMacro(ComputeEuclideanDistance,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where the Euclidean distance to centerlines is stored
   * after execution, when ComputeEuclideanDistance is on.
   * Commonly named "EuclideanDistance".
   */
  vtkSetStringMacro(EuclideanDistanceArrayName);
  vtkGetStringMacro(EuclideanDistanceArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off the computation, for every surface point, of the vector from that point to the
   * corresponding point on the centerlines, stored in CenterlineVectorsArrayName.
   */
  vtkSetMacro(ComputeCenterlineVectors,int);
  vtkGetMacro(ComputeCenterlineVectors,int);
  vtkBooleanMacro(ComputeCenterlineVectors,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array where the vector from each surface point to
   * its corresponding centerline point is stored after execution, when ComputeCenterlineVectors is on.
   * Commonly named "CenterlineVectors".
   */
  vtkSetStringMacro(CenterlineVectorsArrayName);
  vtkGetStringMacro(CenterlineVectorsArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off copying, for every surface point, the id of the centerline cell (and sub-id) closest
   * to its Voronoi pole (read from VoronoiCellIdsArrayName on VoronoiDiagram) into CellIdsArrayName.
   */
  vtkSetMacro(ComputeCellIds,int);
  vtkGetMacro(ComputeCellIds,int);
  vtkBooleanMacro(ComputeCellIds,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 2-component point data array where the centerline cell id and sub-id
   * closest to each surface point are stored after execution, when ComputeCellIds is on.
   * Commonly named "CellIdsArray".
   */
  vtkSetStringMacro(CellIdsArrayName);
  vtkGetStringMacro(CellIdsArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off copying, for every surface point, the parametric coordinate along the closest
   * centerline cell (read from VoronoiPCoordsArrayName on VoronoiDiagram) into PCoordsArrayName.
   */
  vtkSetMacro(ComputePCoords,int);
  vtkGetMacro(ComputePCoords,int);
  vtkBooleanMacro(ComputePCoords,int);
  ///@}

  ///@{
  /**
   * Turn on/off replacing the computed quantities (pole vectors, geodesic distance, Euclidean
   * distance, centerline vectors) at points lying on an open boundary of the input surface with a
   * simpler, purely geometric radial vector/distance from the boundary barycenter. This avoids
   * artifacts near boundaries, where the geodesic-distance and steepest-descent computations on the
   * Voronoi diagram are less reliable because the neighborhood is incomplete. Default: off.
   */
  vtkSetMacro(AdjustBoundaryValues,int);
  vtkGetMacro(AdjustBoundaryValues,int);
  vtkBooleanMacro(AdjustBoundaryValues,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where the parametric coordinate along the closest
   * centerline cell is stored after execution, when ComputePCoords is on.
   * Commonly named "PCoordsArray".
   */
  vtkSetStringMacro(PCoordsArrayName);
  vtkGetStringMacro(PCoordsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the Voronoi diagram associated with the input.
   */
  vtkSetObjectMacro(VoronoiDiagram,vtkPolyData);
  vtkGetObjectMacro(VoronoiDiagram,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the Voronoi diagram point data array where geodesic distance to centerlines is
   * stored.
   * Commonly named "VoronoiGeodesicDistance".
   */
  vtkSetStringMacro(VoronoiGeodesicDistanceArrayName);
  vtkGetStringMacro(VoronoiGeodesicDistanceArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of VoronoiDiagram giving, for each Voronoi point, the
   * vector towards the corresponding point on the centerlines (as computed e.g. by
   * vtkvmtkSteepestDescentShooter::SetTargetVectorsArrayName). Required when ComputeEuclideanDistance
   * or ComputeCenterlineVectors is on.
   * Commonly named "VoronoiPoleCenterlineVectors".
   */
  vtkSetStringMacro(VoronoiPoleCenterlineVectorsArrayName);
  vtkGetStringMacro(VoronoiPoleCenterlineVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of VoronoiDiagram giving, for each Voronoi point, the id
   * (and sub-id) of the closest centerline cell (as computed e.g. by
   * vtkvmtkSteepestDescentShooter::SetTargetCellIdsArrayName). Required when ComputeCellIds is on.
   * Commonly named "VoronoiCellIds".
   */
  vtkSetStringMacro(VoronoiCellIdsArrayName);
  vtkGetStringMacro(VoronoiCellIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of VoronoiDiagram giving, for each Voronoi point, the
   * parametric coordinate along the closest centerline cell. Required when ComputePCoords is on.
   * Commonly named "VoronoiPCoords".
   */
  vtkSetStringMacro(VoronoiPCoordsArrayName);
  vtkGetStringMacro(VoronoiPCoordsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the id list of the Voronoi diagram points which are the poles of surface points.
   */
  vtkSetObjectMacro(PoleIds,vtkIdList);
  vtkGetObjectMacro(PoleIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array where the vector from each surface point to
   * its Voronoi pole is stored after execution, when ComputePoleVectors is on.
   * Commonly named "PoleVectors".
   */
  vtkSetStringMacro(PoleVectorsArrayName);
  vtkGetStringMacro(PoleVectorsArrayName);
  ///@}

  protected:
  vtkvmtkPolyDataLocalGeometry();
  ~vtkvmtkPolyDataLocalGeometry();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void AdjustBoundaryQuantities(vtkPolyData* output);

  int ComputePoleVectors;
  int ComputeGeodesicDistance;
  int ComputeNormalizedTangencyDeviation;
  int ComputeEuclideanDistance;
  int ComputeCenterlineVectors;
  int ComputeCellIds;
  int ComputePCoords;

  int AdjustBoundaryValues;

  char* PoleVectorsArrayName;
  char* GeodesicDistanceArrayName;
  char* NormalizedTangencyDeviationArrayName;
  char* EuclideanDistanceArrayName;
  char* CenterlineVectorsArrayName;
  char* CellIdsArrayName;
  char* PCoordsArrayName;

  char* VoronoiGeodesicDistanceArrayName;
  char* VoronoiPoleCenterlineVectorsArrayName;
  char* VoronoiCellIdsArrayName;
  char* VoronoiPCoordsArrayName;
  vtkPolyData* VoronoiDiagram;
  vtkIdList* PoleIds;

  private:
  vtkvmtkPolyDataLocalGeometry(const vtkvmtkPolyDataLocalGeometry&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataLocalGeometry&);  // Not implemented.
};

#endif
