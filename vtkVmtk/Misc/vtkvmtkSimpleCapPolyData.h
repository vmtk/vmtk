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
 * @class   vtkvmtkSimpleCapPolyData
 * @brief   Close holes in a surface by filling each boundary loop with a single flat polygon.
 * @ingroup Misc
 *
 * This is the simplest and fastest of the capping strategies used by the vmtksurfacecapper
 * pype script (its "simple" Method): for every open boundary of the input, a single polygon
 * cell is inserted whose vertices are exactly the (unmodified) boundary points, in order --
 * no new points are added and the boundary is not displaced or smoothed. Each open boundary of
 * the input is capped independently; boundaries can be restricted with BoundaryIds. If
 * CellEntityIdsArrayName is set, the newly created cap polygon is tagged with a per-boundary id
 * in that cell data array, which downstream tools (mesh generators, boundary condition
 * assignment) use to distinguish inlets/outlets/wall. The output caps are polygons, not
 * necessarily triangles or planar for non-planar boundaries; they are commonly triangulated
 * afterwards with vtkTriangleFilter.
 *
 * @sa
 * vtkvmtkCapPolyData, vtkvmtkSmoothCapPolyData, vtkvmtkPolyDataBoundaryExtractor
 */

#ifndef __vtkvmtkSimpleCapPolyData_h
#define __vtkvmtkSimpleCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkSimpleCapPolyData : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSimpleCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkSimpleCapPolyData *New();

  ///@{
  /**
   * Set/Get the ids of the boundaries to cap. If not set (default, NULL), every open boundary of
   * the input surface is capped. A boundary's id is its label when BoundaryLabelsArrayName and
   * BoundaryPointOrderArrayName are set and the input carries those arrays, and otherwise its
   * position in the list of open boundaries extracted from the input, in the order returned by
   * vtkvmtkPolyDataBoundaryExtractor.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array used to tag the newly created cap polygons with an
   * integer id, one distinct value per capped boundary (offset by CellEntityIdOffset, then
   * further offset by boundary index + 1). If the array already exists on the input, existing
   * cell values are preserved and only the new cap cell is appended with the new tag. If left
   * NULL (default), no cell entity id array is created.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the base offset added to the ids written into CellEntityIdsArrayName. The id
   * assigned to the cap of the i-th processed boundary is (i + 1 + CellEntityIdOffset).
   * Default: 1.
   */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  ///@{
  /**
   * Set/Get the names of the point data arrays that carry the boundary labels of the input, as
   * written by vtkvmtkPolyDataBoundaryLabeler. When both are set and the input carries arrays
   * that still describe it, the boundaries are read from them instead of being extracted, and
   * the cap of each boundary can be named through BoundaryCellEntityIds. This filter adds no
   * point, so the arrays reach the output unchanged with the rest of the input's point data.
   *
   * Setting them also settles what a boundary id means everywhere else in this filter: with the
   * labels in use a boundary's id is its label, and without them it is the boundary's position
   * in the order the extractor returns, which is what it has always been.
   */
  vtkSetStringMacro(BoundaryLabelsArrayName);
  vtkGetStringMacro(BoundaryLabelsArrayName);
  vtkSetStringMacro(BoundaryPointOrderArrayName);
  vtkGetStringMacro(BoundaryPointOrderArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the id to write into CellEntityIdsArrayName for the cap of each boundary, indexed
   * the way boundary ids are indexed everywhere else here (see BoundaryIds): entry i is the id
   * given to the cap closing the boundary whose id is i.
   *
   * An id beyond the end of the array, or one whose entry is negative, is no entry at all: with
   * the labels in use that cap keeps its boundary's label, which names it just as well, and
   * without them the id its position would have given it.
   * An id chosen here is used as it stands, with CellEntityIdOffset not added to it, while the
   * offset still lands on the cells copied from the input and on any boundary left to its
   * positional id.
   */
  vtkSetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  vtkGetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  ///@}

  protected:
  vtkvmtkSimpleCapPolyData();
  ~vtkvmtkSimpleCapPolyData();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;
  char* BoundaryLabelsArrayName;
  char* BoundaryPointOrderArrayName;
  vtkIdTypeArray* BoundaryCellEntityIds;

  private:
  vtkvmtkSimpleCapPolyData(const vtkvmtkSimpleCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkSimpleCapPolyData&);  // Not implemented.
};

#endif
