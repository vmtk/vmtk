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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkPolyDataDijkstraDistanceToPoints
 * @brief   Compute the geodesic (graph) distance from a set of seed points
 *          over a surface mesh using Dijkstra's algorithm.
 * @ingroup Contrib
 *
 * For each point of the input surface, vtkvmtkPolyDataDijkstraDistanceToPoints
 * computes the shortest path distance, along the mesh edges, to the closest
 * of a set of user-specified seed points (SeedIds), and outputs the result
 * as a point data array named DijkstraDistanceToPointsArrayName. The raw
 * Dijkstra distance is transformed as
 * (DistanceOffset + DistanceScale * rawDistance), clamped from below by
 * MinDistance and, if MaxDistance is positive, from above by MaxDistance
 * (a non-positive MaxDistance, the default, means unbounded).
 */

#ifndef __vtkvmtkPolyDataDijkstraDistanceToPoints_h
#define __vtkvmtkPolyDataDijkstraDistanceToPoints_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkIdList.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataDijkstraDistanceToPoints : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataDijkstraDistanceToPoints* New();
  vtkTypeMacro(vtkvmtkPolyDataDijkstraDistanceToPoints,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the ids of the input surface points to use as Dijkstra seeds,
   * i.e. the points from which the geodesic distance is measured.
   */
  vtkSetObjectMacro(SeedIds,vtkIdList);
  vtkGetObjectMacro(SeedIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the constant added to the raw Dijkstra distance before scaling
   * and clamping. Default: 0.
   */
  vtkSetMacro(DistanceOffset,double);
  vtkGetMacro(DistanceOffset,double);
  ///@}

  ///@{
  /**
   * Set/get the factor the raw Dijkstra distance is multiplied by (after
   * adding DistanceOffset). Default: 1.
   */
  vtkSetMacro(DistanceScale,double);
  vtkGetMacro(DistanceScale,double);
  ///@}

  ///@{
  /**
   * Set/get the lower bound the (offset and scaled) distance is clamped to.
   * Default: 0.
   */
  vtkSetMacro(MinDistance,double);
  vtkGetMacro(MinDistance,double);
  ///@}

  ///@{
  /**
   * Set/get the upper bound the (offset and scaled) distance is clamped to.
   * A non-positive value (the default, -1) means unbounded.
   */
  vtkSetMacro(MaxDistance,double);
  vtkGetMacro(MaxDistance,double);
  ///@}

  ///@{
  /**
   * Set/get the name of the output point data array holding the computed
   * distance values.
   * Commonly named "DijkstraDistanceToPoints".
   */
  vtkSetStringMacro(DijkstraDistanceToPointsArrayName);
  vtkGetStringMacro(DijkstraDistanceToPointsArrayName);
  ///@}

protected:
  vtkvmtkPolyDataDijkstraDistanceToPoints();
  ~vtkvmtkPolyDataDijkstraDistanceToPoints();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* DijkstraDistanceToPointsArrayName;

  vtkIdList* SeedIds;

  double DistanceOffset;
  double DistanceScale;
  double MinDistance;
  double MaxDistance;

private:
  vtkvmtkPolyDataDijkstraDistanceToPoints(const vtkvmtkPolyDataDijkstraDistanceToPoints&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDijkstraDistanceToPoints&);  // Not implemented.
};

#endif

