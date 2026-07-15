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
 * @class   vtkvmtkSmoothCapPolyData
 * @brief   Close holes in a surface with a smooth, rounded (domed) cap built with concentric
 * thin-plate-spline rings.
 * @ingroup Misc
 *
 * This is the "smooth" capping strategy used by the vmtksurfacecapper pype script. For each
 * open boundary, a thin-plate-spline transform is fit between a flat unit disk (source
 * landmarks) and the actual boundary loop together with an "outer ring" of points offset
 * outward from the boundary, along the direction from each boundary point to the
 * area-weighted centroid of its neighboring input triangles, by a distance proportional to
 * ConstraintFactor and to the boundary's diagonal. This transform is then used to warp
 * NumberOfRings concentric circles from the unit disk into 3D, producing a bulged, rounded cap
 * whose shape near the rim follows the local shape of the surface (rather than a flat or
 * conical cap). Each open boundary of the input is capped independently; boundaries can be
 * restricted with BoundaryIds. If CellEntityIdsArrayName is set, the newly created cap cells
 * are tagged with per-boundary ids in that cell data array, which downstream tools (mesh
 * generators, boundary condition assignment) use to distinguish inlets/outlets/wall. The input
 * is expected to be triangulated.
 *
 * @sa
 * vtkvmtkCapPolyData, vtkvmtkSimpleCapPolyData, vtkvmtkPolyDataBoundaryExtractor
 */

#ifndef __vtkvmtkSmoothCapPolyData_h
#define __vtkvmtkSmoothCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkSmoothCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkSmoothCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkSmoothCapPolyData *New();

  ///@{
  /**
   * Set/Get the ids (into the list of open boundaries extracted from the input, in the order
   * returned by vtkvmtkPolyDataBoundaryExtractor) of the boundaries to cap. If not set (default,
   * NULL), every open boundary of the input surface is capped.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array used to tag the newly created cap cells with an
   * integer id, one distinct value per capped boundary (offset by CellEntityIdOffset, then
   * further offset by boundary index + 1). If the array already exists on the input, existing
   * cell values are preserved and only the new cap cells are appended with the new tag. If left
   * NULL (default), no cell entity id array is created.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the base offset added to the ids written into CellEntityIdsArrayName. The id
   * assigned to the cap cells of the i-th processed boundary is (i + 1 + CellEntityIdOffset).
   * Default: 1.
   */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  ///@{
  /**
   * Set/Get the factor controlling how much the cap bulges outward before curving in towards
   * the center, i.e. how strongly the shape of the input surface near the boundary influences
   * the shape of the cap. It scales the outward displacement (along the direction from each
   * boundary point to the area-weighted centroid of its neighboring triangles) used to build
   * the outer thin-plate-spline landmark ring, as a multiple of the boundary's diagonal length.
   * A value of 0 produces a flatter cap; larger values produce a more pronounced dome.
   * Default: 1.0.
   */
  vtkSetMacro(ConstraintFactor,double);
  vtkGetMacro(ConstraintFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of concentric point rings, interpolated by the thin-plate-spline
   * transform between the boundary loop and the cap's center, used to build the cap surface.
   * Higher values produce a smoother, more finely tessellated dome at the cost of more points
   * and cells. Default: 8.
   */
  vtkSetMacro(NumberOfRings,int);
  vtkGetMacro(NumberOfRings,int);
  ///@}

  protected:
  vtkvmtkSmoothCapPolyData();
  ~vtkvmtkSmoothCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  double ConstraintFactor;
  int NumberOfRings;

  private:
  vtkvmtkSmoothCapPolyData(const vtkvmtkSmoothCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkSmoothCapPolyData&);  // Not implemented.
};

#endif
