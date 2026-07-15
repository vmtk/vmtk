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
 * @class   vtkvmtkSimplifyVoronoiDiagram
 * @brief   Remove non essential Voronoi polygon points (little spikes).
 * @ingroup ComputationalGeometry
 *
 * This class identifies and removes Voronoi polygon points if they are used by one cell and they are not poles. This helps to get rid of noisy Voronoi diagram parts induced by non smooth surface point distribution. This operation has no effect on the accuracy of the computation of centerlines and of surface related quantities.
 *
 * @sa
 * vtkVoronoiDiagram3D
 */

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
  static vtkvmtkSimplifyVoronoiDiagram *New();
  vtkTypeMacro(vtkvmtkSimplifyVoronoiDiagram, vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the ids of Voronoi diagram points that must never be removed, regardless of whether they
   * would otherwise qualify for removal (used when Simplification is VTK_VMTK_REMOVE_BOUNDARY_POINTS,
   * typically to protect the Voronoi poles). If IncludeUnremovable is off, cells referencing only
   * points not in this list may still be entirely removed.
   */
  vtkSetObjectMacro(UnremovablePointIds,vtkIdList);
  vtkGetObjectMacro(UnremovablePointIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the ids of Voronoi diagram cells that must never be removed, regardless of whether they
   * would otherwise qualify for removal (used when Simplification is VTK_VMTK_REMOVE_BOUNDARY_CELLS).
   */
  vtkSetObjectMacro(UnremovableCellIds,vtkIdList);
  vtkGetObjectMacro(UnremovableCellIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get type of simplification.
   */
  vtkSetMacro(Simplification,int);
  vtkGetMacro(Simplification,int);
  void SetSimplificationToRemoveBoundaryPoints() {
  this->SetSimplification(VTK_VMTK_REMOVE_BOUNDARY_POINTS);};
  void SetSimplificationToRemoveBoundaryCells() {
  this->SetSimplification(VTK_VMTK_REMOVE_BOUNDARY_CELLS);};
  ///@}

  ///@{
  /**
   * Toggle whether points/cells listed in UnremovablePointIds/UnremovableCellIds (and the cells built
   * from them) are kept in the output (on) or dropped entirely once simplification has otherwise
   * converged (off). Only relevant when at least one such id was supplied. Default: on.
   */
  vtkSetMacro(IncludeUnremovable,int);
  vtkGetMacro(IncludeUnremovable,int);
  vtkBooleanMacro(IncludeUnremovable,int);
  ///@}

  ///@{
  /**
   * Toggle limiting the simplification to a single pass over the Voronoi diagram cells, instead of
   * iterating until no more points/cells qualify for removal. Default: off (iterate to convergence).
   */
  vtkSetMacro(OnePassOnly,int);
  vtkGetMacro(OnePassOnly,int);
  vtkBooleanMacro(OnePassOnly,int);
  ///@}

protected:
  vtkvmtkSimplifyVoronoiDiagram();
  ~vtkvmtkSimplifyVoronoiDiagram() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  vtkIdType IsBoundaryEdge(vtkCellLinks* links, vtkIdType* edge);

  vtkIdList* UnremovablePointIds;
  vtkIdList* UnremovableCellIds;

  int Simplification;
  int IncludeUnremovable;
  int OnePassOnly;

private:
  vtkvmtkSimplifyVoronoiDiagram(const vtkvmtkSimplifyVoronoiDiagram&) = delete;
  void operator=(const vtkvmtkSimplifyVoronoiDiagram&) = delete;
};

#endif
