/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataSurfaceRemeshing.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataSurfaceRemeshing - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataSurfaceRemeshing_h
#define __vtkvmtkPolyDataSurfaceRemeshing_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkCellLocator;

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataSurfaceRemeshing : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataSurfaceRemeshing *New();
  vtkTypeRevisionMacro(vtkvmtkPolyDataSurfaceRemeshing,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  //BTX
  enum {
    SUCCESS = 0,
    NOT_EDGE,
    EDGE_ON_BOUNDARY,
    NON_MANIFOLD,
    NOT_TRIANGLES,
    DEGENERATE_TRIANGLES
  };

  enum {
    DO_CHANGE,
    DO_NOTHING
  };
  //ETX

protected:
  vtkvmtkPolyDataSurfaceRemeshing();
  ~vtkvmtkPolyDataSurfaceRemeshing();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int EdgeFlipConnectivityOptimizationIteration();
  int EdgeFlipIteration();
  int EdgeCollapseIteration();
  int EdgeSplitIteration();
  void PointRelocationIteration();

  int TestFlipEdgeValidity(vtkIdType pt1, vtkIdType pt2, vtkIdType cell1, vtkIdType cell2, vtkIdType pt3, vtkIdType pt4, double normalAngleTolerance);
  int TestConnectivityFlipEdge(vtkIdType pt1, vtkIdType pt2, double normalAngleTolerance);
  int TestDelaunayFlipEdge(vtkIdType pt1, vtkIdType pt2, double internalAngleTolerance, double normalAngleTolerance);
  int TestAspectRatioCollapseEdge(vtkIdType cellId, double aspectRatioThreshold, double maxArea, vtkIdType& pt1, vtkIdType& pt2);
  int TestAreaSplitEdge(vtkIdType cellId, double maxArea, vtkIdType& pt1, vtkIdType& pt2);

  int GetEdgeCellsAndOppositeEdge(vtkIdType pt1, vtkIdType pt2, vtkIdType& cell1, vtkIdType& cell2, vtkIdType& pt3, vtkIdType& pt4);

  int SplitEdge(vtkIdType pt1, vtkIdType pt2);
  int CollapseEdge(vtkIdType pt1, vtkIdType pt2);
  int FlipEdge(vtkIdType pt1, vtkIdType pt2);

  int SplitTriangle(vtkIdType cellId);
  int CollapseTriangle(vtkIdType cellId);

  void RelocatePoint(vtkIdType pointId, double relaxation);

  vtkPolyData* Mesh;
  vtkCellLocator* Locator;

  double AspectRatioThreshold;
  double MaxArea;
  double InternalAngleTolerance;
  double NormalAngleTolerance;
  double Relaxation;

private:
  vtkvmtkPolyDataSurfaceRemeshing(const vtkvmtkPolyDataSurfaceRemeshing&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSurfaceRemeshing&);  // Not implemented.
};

#endif

