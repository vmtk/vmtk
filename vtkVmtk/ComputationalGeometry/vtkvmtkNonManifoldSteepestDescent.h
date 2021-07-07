/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkNonManifoldSteepestDescent.h,v $
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
// .NAME vtkvmtkNonManifoldSteepestDescent - Abstract class for steepest descent on a polygonal non-manifold.
// .SECTION Description
// This class is an abstract filter used as base class for performing steepest descent on a non-manifold surface made of convex polygons (such as the Voronoi diagram) on the basis of a given scalar field. Steepest descent is performed on the edges of input polygons with a first order approximation.
//
// .SECTION See Also
// vtkSteepestDescentLineTracer vtkSurfaceToCenterlines vtkVoronoiDiagram3D

#ifndef __vtkvmtkNonManifoldSteepestDescent_h
#define __vtkvmtkNonManifoldSteepestDescent_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkDataArray.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#define VTK_VMTK_DOWNWARD 0
#define VTK_VMTK_UPWARD 1

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkNonManifoldSteepestDescent : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkNonManifoldSteepestDescent,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkNonManifoldSteepestDescent *New();

  // Description:
  // Set/Get the name of the point data array used as the descent scalar field.
  vtkSetStringMacro(DescentArrayName);
  vtkGetStringMacro(DescentArrayName);

  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);
  void SetDirectionToDownward() 
  {this->SetDirection(VTK_VMTK_DOWNWARD); }
  void SetDirectionToUpward() 
  {this->SetDirection(VTK_VMTK_UPWARD); }

  protected:
  vtkvmtkNonManifoldSteepestDescent();
  ~vtkvmtkNonManifoldSteepestDescent();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  // Description:
  // Compute the steepest descent point in terms of edge (point id pair) and parametric coordinate on edge. It takes in input a starting point expressed in terms of edge (point id pair) and parametric coordinate on edge. It returns the descent value.
  double GetSteepestDescent(vtkPolyData* input, vtkIdType* edge, double s, vtkIdType* steepestDescentEdge, double &steepestDescentS);
  double GetSteepestDescentInCell(vtkPolyData* input, vtkIdType cellId, vtkIdType* edge, double s, vtkIdType* steepestDescentEdge, double &steepestDescentS, double &steepestDescentLength);

  vtkDataArray* DescentArray;
  char* DescentArrayName;

  int NumberOfEdgeSubdivisions;
  int Direction;

  private:
  vtkvmtkNonManifoldSteepestDescent(const vtkvmtkNonManifoldSteepestDescent&);  // Not implemented.
  void operator=(const vtkvmtkNonManifoldSteepestDescent&);  // Not implemented.
};

#endif

