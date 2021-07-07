/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSimplifyVoronoiDiagram.h,v $
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
// .NAME vtkvmtkSimplifyVoronoiDiagram - Remove non essential Voronoi polygon points (little spikes).
// .SECTION Description
// This class identifies and removes Voronoi polygon points if they are used by one cell and they are not poles. This helps to get rid of noisy Voronoi diagram parts induced by non smooth surface point distribution. This operation has no effect on the accuracy of the computation of centerlines and of surface related quantities.
// .SECTION See Also
// vtkVoronoiDiagram3D

#ifndef __vtkvmtkSimplifyVoronoiDiagram_h
#define __vtkvmtkSimplifyVoronoiDiagram_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#define VTK_VMTK_REMOVE_BOUNDARY_POINTS 0
#define VTK_VMTK_REMOVE_BOUNDARY_CELLS 1

class vtkCellArray;
class vtkCellTypes;
class vtkCellLinks;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkSimplifyVoronoiDiagram : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSimplifyVoronoiDiagram,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSimplifyVoronoiDiagram *New();

  // Set/Get id list of Voronoi diagram points to preserve.
  vtkSetObjectMacro(UnremovablePointIds,vtkIdList);
  vtkGetObjectMacro(UnremovablePointIds,vtkIdList);

  // Set/Get id list of Voronoi diagram cells to preserve.
  vtkSetObjectMacro(UnremovableCellIds,vtkIdList);
  vtkGetObjectMacro(UnremovableCellIds,vtkIdList);

  // Description:
  // Set/Get type of simplification.
  vtkSetMacro(Simplification,int);
  vtkGetMacro(Simplification,int);
  void SetSimplificationToRemoveBoundaryPoints() {
  this->SetSimplification(VTK_VMTK_REMOVE_BOUNDARY_POINTS);};
  void SetSimplificationToRemoveBoundaryCells() {
  this->SetSimplification(VTK_VMTK_REMOVE_BOUNDARY_CELLS);};

  vtkSetMacro(IncludeUnremovable,int);
  vtkGetMacro(IncludeUnremovable,int);
  vtkBooleanMacro(IncludeUnremovable,int);

  vtkSetMacro(OnePassOnly,int);
  vtkGetMacro(OnePassOnly,int);
  vtkBooleanMacro(OnePassOnly,int);

  protected:
  vtkvmtkSimplifyVoronoiDiagram();
  ~vtkvmtkSimplifyVoronoiDiagram();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  vtkIdType IsBoundaryEdge(vtkCellLinks* links, vtkIdType* edge);

  vtkIdList* UnremovablePointIds;
  vtkIdList* UnremovableCellIds;

  int Simplification;
  int IncludeUnremovable;
  int OnePassOnly;

  private:
  vtkvmtkSimplifyVoronoiDiagram(const vtkvmtkSimplifyVoronoiDiagram&);  // Not implemented.
  void operator=(const vtkvmtkSimplifyVoronoiDiagram&);  // Not implemented.
};

#endif
