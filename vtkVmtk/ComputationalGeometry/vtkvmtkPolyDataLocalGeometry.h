/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataLocalGeometry.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataLocalGeometry - Compute shape-related quantities over a surface.
// .SECTION Description
// This class computes geodesic distance to centerlines and normalized tangency deviation distribution over a surface given the Voronoi diagram of its points, the ids of Voronoi diagram points which are poles of surface points, and the field of the geodesic distance from centerlines computed over the Voronoi diagram.
//
// Geodesic distance to centerlines field is computed as
// \f[ D_g(\mathbf{x}) = T(\mathbf{p}(\mathbf{x})) + |\mathbf{p}(\mathbf{x}) - \mathbf{x}| \f]
// where T(x) is the geodesic distance to centerlines field defined on the Voronoi diagram (computed by solving the Eikonal equation with unit speed over the Voronoi diagram by means of vtkNonManifoldFastMarching), and p(x) is the pole associated with surface point x.
//
// Normalized tangency deviation is a normalized measure of how much a surface point is far from being a tangency point to the evelope of maximal spheres defined on the centerlines (their envelope is the maximal inscribed cylinder), and it is computed as
// \f[\mathit{NTD}=\frac{T(\mathbf{p}(\mathbf{x}))}{D_g(\mathbf{x})}\f]
// This quantity ranges from 0 to 1, and it is independent from the radius of the maximal inscribed cylinder (on an elliptical base cylinder, NTD only depends on base ellipse eccentricity). 
//
// The description given here is particularly suited for the description of tubular surfaces in terms of centerlines, but this class can be used without this assumption. Whenever a geodesic distance field is computed over the Voronoi diagram associated with a shape from a subset of the Voronoi diagram itself (in this context, the centerlines), the surface geodesic distance field and the normalized tangency deviation can be defined, and this class can be used to compute them.
//
// .SECTION See Also
// vtkNonManifoldFastMarching vtkVoronoiDiagram3D

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataLocalGeometry *New();
  
  // Description:
  // TODO
  vtkSetMacro(ComputePoleVectors,int);
  vtkGetMacro(ComputePoleVectors,int);
  vtkBooleanMacro(ComputePoleVectors,int);

  // Description:
  // Turn on/off the computation of geodesic distance to centerlines.
  vtkSetMacro(ComputeGeodesicDistance,int);
  vtkGetMacro(ComputeGeodesicDistance,int);
  vtkBooleanMacro(ComputeGeodesicDistance,int);

  // Description:
  // Set/Get the name of the point data array where geodesic distance to centerlines is stored after execution.
  vtkSetStringMacro(GeodesicDistanceArrayName);
  vtkGetStringMacro(GeodesicDistanceArrayName);

  // Description:
  // Turn on/off the computation of normalized tangency deviation.
  vtkSetMacro(ComputeNormalizedTangencyDeviation,int);
  vtkGetMacro(ComputeNormalizedTangencyDeviation,int);
  vtkBooleanMacro(ComputeNormalizedTangencyDeviation,int);

  // Description:
  // Set/Get the name of the point data array where normalized tangency deviation is stored after execution.
  vtkSetStringMacro(NormalizedTangencyDeviationArrayName);
  vtkGetStringMacro(NormalizedTangencyDeviationArrayName);

  // Description:
  // TODO
  vtkSetMacro(ComputeEuclideanDistance,int);
  vtkGetMacro(ComputeEuclideanDistance,int);
  vtkBooleanMacro(ComputeEuclideanDistance,int);

  // Description:
  // TODO
  vtkSetStringMacro(EuclideanDistanceArrayName);
  vtkGetStringMacro(EuclideanDistanceArrayName);

  // Description:
  // TODO
  vtkSetMacro(ComputeCenterlineVectors,int);
  vtkGetMacro(ComputeCenterlineVectors,int);
  vtkBooleanMacro(ComputeCenterlineVectors,int);

  // Description:
  // TODO
  vtkSetStringMacro(CenterlineVectorsArrayName);
  vtkGetStringMacro(CenterlineVectorsArrayName);

  // Description:
  // TODO
  vtkSetMacro(ComputeCellIds,int);
  vtkGetMacro(ComputeCellIds,int);
  vtkBooleanMacro(ComputeCellIds,int);

  // Description:
  // TODO
  vtkSetStringMacro(CellIdsArrayName);
  vtkGetStringMacro(CellIdsArrayName);

  // Description:
  // TODO
  vtkSetMacro(ComputePCoords,int);
  vtkGetMacro(ComputePCoords,int);
  vtkBooleanMacro(ComputePCoords,int);

  vtkSetMacro(AdjustBoundaryValues,int);
  vtkGetMacro(AdjustBoundaryValues,int);
  vtkBooleanMacro(AdjustBoundaryValues,int);

  // Description:
  // TODO
  vtkSetStringMacro(PCoordsArrayName);
  vtkGetStringMacro(PCoordsArrayName);

  // Description:
  // Set/Get the Voronoi diagram associated with the input.
  vtkSetObjectMacro(VoronoiDiagram,vtkPolyData);
  vtkGetObjectMacro(VoronoiDiagram,vtkPolyData);

  // Description:
  // Set/Get the name of the Voronoi diagram point data array where geodesic distance to centerlines is stored.
  vtkSetStringMacro(VoronoiGeodesicDistanceArrayName);
  vtkGetStringMacro(VoronoiGeodesicDistanceArrayName);

  // Description:
  // TODO.
  vtkSetStringMacro(VoronoiPoleCenterlineVectorsArrayName);
  vtkGetStringMacro(VoronoiPoleCenterlineVectorsArrayName);

  // Description:
  // TODO.
  vtkSetStringMacro(VoronoiCellIdsArrayName);
  vtkGetStringMacro(VoronoiCellIdsArrayName);

  // Description:
  // TODO.
  vtkSetStringMacro(VoronoiPCoordsArrayName);
  vtkGetStringMacro(VoronoiPCoordsArrayName);

  // Description:
  // Set/Get the id list of the Voronoi diagram points which are the poles of surface points.
  vtkSetObjectMacro(PoleIds,vtkIdList);
  vtkGetObjectMacro(PoleIds,vtkIdList);

  // Description:
  // TODO
  vtkSetStringMacro(PoleVectorsArrayName);
  vtkGetStringMacro(PoleVectorsArrayName);

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
