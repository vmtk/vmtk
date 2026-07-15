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
 * @class   vtkvmtkSteepestDescentShooter
 * @brief   Experimental: for each seed point (e.g. a Voronoi pole), backtrace a steepest descent path over a non-manifold to the nearest point of a target polyline, recording where it lands.
 * @ingroup ComputationalGeometry
 *
 * This class is meant to be used after solving the Eikonal equation, with the target polyline
 * (typically the centerlines) as boundary condition, over a non-manifold polygonal domain (typically
 * a Voronoi diagram, via vtkNonManifoldFastMarching). For every seed, it follows the steepest descent
 * of the resulting scalar field (using the machinery of the base class vtkvmtkNonManifoldSteepestDescent)
 * until it reaches Target, and records, for each seed, the vector from seed to the landing point, the
 * target cell id/sub-id, and the parametric coordinate along the target cell. This is used e.g. to
 * associate each surface point's Voronoi pole to the closest point on the centerlines. Implement with
 * caution: this method is experimental and has known limitations when a path must cross multiple
 * Voronoi diagram "spikes" to reach the target.
 */

#ifndef __vtkvmtkSteepestDescentShooter_h
#define __vtkvmtkSteepestDescentShooter_h

#include "vtkvmtkNonManifoldSteepestDescent.h"
#include "vtkPolyData.h"
#include "vtkEdgeTable.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkSteepestDescentShooter : public vtkvmtkNonManifoldSteepestDescent
{
  public: 
  vtkTypeMacro(vtkvmtkSteepestDescentShooter,vtkvmtkNonManifoldSteepestDescent);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkSteepestDescentShooter *New();

  ///@{
  /**
   * Set/Get the target polyline (typically the centerlines) that seed backtraces are shot towards.
   * Required input.
   */
  vtkSetObjectMacro(Target,vtkPolyData);
  vtkGetObjectMacro(Target,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the ids, on the input non-manifold domain, of the seed points to backtrace from.
   * Required input.
   */
  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input domain used internally to track, for each
   * point, the edge of the shortest-path tree it was reached through (as produced by
   * vtkvmtkNonManifoldFastMarching).
   * Commonly named "EdgeArray".
   */
  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where, for each seed, the vector from the seed
   * to its landing point on Target is stored.
   * Commonly named "VoronoiPoleCenterlineVectors".
   */
  vtkSetStringMacro(TargetVectorsArrayName);
  vtkGetStringMacro(TargetVectorsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where, for each seed, the id of the Target cell
   * its backtrace path landed on is stored.
   * Commonly named "VoronoiCellIds".
   */
  vtkSetStringMacro(TargetCellIdsArrayName);
  vtkGetStringMacro(TargetCellIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where, for each seed, the parametric coordinate
   * along the landing Target cell is stored.
   */
  vtkSetStringMacro(TargetPCoordsArrayName);
  vtkGetStringMacro(TargetPCoordsArrayName);
  ///@}
 
  protected:
  vtkvmtkSteepestDescentShooter();
  ~vtkvmtkSteepestDescentShooter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void Backtrace(vtkPolyData* input, vtkIdType seedId);

  void FindNearestPolyLinePoint(double* currentPoint, double* lineClosestPoint, int &lineCellId, int &lineCellSubId, double &linePCoord);

  vtkPolyData* Target;
  vtkIdList* Seeds;

  vtkIntArray* TargetArray;
  vtkDataArray* EdgeArray;

  vtkDoubleArray* TargetVectors;
  vtkIntArray* TargetCellIds;
  vtkDoubleArray* TargetPCoords;

  char* EdgeArrayName;

  char* TargetVectorsArrayName;
  char* TargetCellIdsArrayName;
  char* TargetPCoordsArrayName;

  private:
  vtkvmtkSteepestDescentShooter(const vtkvmtkSteepestDescentShooter&);  // Not implemented.
  void operator=(const vtkvmtkSteepestDescentShooter&);  // Not implemented.
};

#endif
