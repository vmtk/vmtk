/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSteepestDescentLineTracer.h,v $
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
// .NAME vtkvmtkSteepestDescentLineTracer - Trace the steepest descent line over a polygonal non-manifold.
// .SECTION Description
// This class takes in input a non-manifold surface made of convex polygons (such as a Voronoi diagram) on which a scalar field is defined (as a point data array) and produces steepest descent paths from an id list of seeds to a target, basically solving the ordinary differential equation
// \f[\frac{d\gamma(\tau)}{d\tau}=-\nabla T(\mathbf{x})\f]
// where \f$\gamma(\tau)\f$ is a path and T(x) is a scalar field defined over the polygonal non-manifold domain.
//
// The class uses the members of its base class vtkNonManifoldSteepestDescent to compute the steepest descent point at each step.
// The computed paths are polylines whose points always lie on input polygon edges.
//
// This class is meant to be used for backtracing centerlines after solving the Eikonal equation on the Voronoi diagram of a shape (by means of vtkNonManifoldFastMarching). If MergePaths is off, one polyline for each seed point is produced (the cell id of each polyline corresponds to the list id of input seeds). If MergePaths is on, polylines are merged if they intersect the same Voronoi diagram edge and their distance is below a user-defined tolerance. Actually, if a path visits an edge which has already been visited by a previous path, its endpoint is set to the previous path point, so that a T junction is produced. 
//
// The user can specify a point data array whose values are interpolated on path points and presented in output as point data. 
//
// If 1) EdgeArrayName and/or 2) EdgePCoordArrayName are provided, the output will contain 1) a 2-component vtkIntArray in which the point ids of the edges intersected by the paths are stored and 2) a 1-component vtkDoubleArray in which the parametric coordinate of the intersection is stored.
//
// .SECTION See Also
// vtkNonManifoldFastMarching vtkVoronoiDiagram3D

#ifndef __vtkvmtkSteepestDescentLineTracer_h
#define __vtkvmtkSteepestDescentLineTracer_h

#include "vtkvmtkNonManifoldSteepestDescent.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkSteepestDescentLineTracer : public vtkvmtkNonManifoldSteepestDescent
{
public:
  vtkTypeMacro(vtkvmtkSteepestDescentLineTracer,vtkvmtkNonManifoldSteepestDescent);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSteepestDescentLineTracer *New();

  // Description:
  // Set/Get the id of the seeds from which steepest descent starts.
  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);

  // Description:
  // Set/Get the id of the point on which steepest descent must terminate.
  vtkSetObjectMacro(Targets,vtkIdList);
  vtkGetObjectMacro(Targets,vtkIdList);

  // Description:
  // Set/Get the id of the point on which steepest descent has terminated for each seed point.
  vtkGetObjectMacro(HitTargets,vtkIdList);

  vtkSetMacro(StopOnTargets,int)
  vtkGetMacro(StopOnTargets,int)
  vtkBooleanMacro(StopOnTargets,int)

  // Description:
  // Set/Get the name of the point data array from which path point data is generated.
  vtkSetStringMacro(DataArrayName);
  vtkGetStringMacro(DataArrayName);

  // Description:
  // Set/Get the name of output point data array where the point ids of the edges intersected by the paths are going to be stored.
  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);

  // Description:
  // Set/Get the name of output point data array where the parametric coordinate of the intersection point with input edges are going to be stored.
  vtkSetStringMacro(EdgePCoordArrayName);
  vtkGetStringMacro(EdgePCoordArrayName);

  // Description:
  // Turn on/off merging paths if they intersect the same Voronoi edge. Paths are merged with T junctions in the same order as defined in the seed list.
  vtkSetMacro(MergePaths,int);
  vtkGetMacro(MergePaths,int);
  vtkBooleanMacro(MergePaths,int);

  // Description:
  // Set/Get the (absolute) tolerance with which two points intersecting the same Voronoi diagram edge are considered coincident. If set to a large value, two paths are merged if they intersect the same Voronoi diagram edge (default behaviour).
  vtkSetMacro(MergeTolerance,double);
  vtkGetMacro(MergeTolerance,double);

protected:
  vtkvmtkSteepestDescentLineTracer();
  ~vtkvmtkSteepestDescentLineTracer();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void Backtrace(vtkPolyData* input, vtkIdType seedId);

  vtkIdList* Seeds;
  vtkIdList* Targets;

  vtkIdList* HitTargets;

  int StopOnTargets;

  char* DataArrayName;

  char* EdgeArrayName;
  char* EdgePCoordArrayName;
  char* MinDistanceArrayName;

  int MergePaths;
  double MergeTolerance;

  vtkIntArray* Edges;
  vtkDoubleArray* EdgeParCoords;

  vtkIdList* EdgePointIds;
  vtkIntArray* CellIdsArray;
  vtkDoubleArray* PCoordsArray;
  vtkDoubleArray* MinDistanceArray;

  vtkDataArray* LineDataArray;

  int MergeWithExistingPaths;
  vtkPolyData* ExistingPaths;
  char* ExistingPathsEdgeArrayName;
  char* ExistingPathsEdgePCoordArrayName;
  vtkIntArray* ExistingPathsEdges;
  vtkDoubleArray* ExistingPathsEdgeParCoords;

  private:
  vtkvmtkSteepestDescentLineTracer(const vtkvmtkSteepestDescentLineTracer&);  // Not implemented.
  void operator=(const vtkvmtkSteepestDescentLineTracer&);  // Not implemented.
};

#endif

