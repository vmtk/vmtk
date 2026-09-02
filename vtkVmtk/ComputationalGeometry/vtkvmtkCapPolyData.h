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
/*! \class vtkvmtkCapPolyData
    \brief Close holes in a surface by creating a cap made of triangles sharing a common vertex at
    the boundary barycenter.
    \ingroup ComputationalGeometry

    This class closes the boundaries of a surface with a cap. Each cap is made of triangles sharing
    the boundary barycenter which is added to the data set. It is possible to retrieve the ids of the
    added points with GetCapCenterIds. Boundary barycenters can be displaced along boundary normals
    through the Displacement parameter. Since this class is used as a preprocessing step for Delaunay
    tessellation, displacement is meant to avoid the occurrences of degenerate tetrahedra on the caps.

    This is the simplest of the capping strategies used by the vmtksurfacecapper pype script (its
    "simple" Method); it is a fast, one-triangle-fan-per-hole capper meant to produce a closed,
    tetrahedralizable surface (e.g. before Delaunay-based volume meshing or before centerline
    extraction, both of which require a closed surface). Each open boundary of the input is capped
    independently; boundaries can be restricted with BoundaryIds. If CellEntityIdsArrayName is set,
    the newly created cap triangles are tagged with per-boundary ids in that cell data array, which
    downstream tools (mesh generators, boundary condition assignment) use to distinguish
    inlets/outlets/wall.

    \sa vtkvmtkPolyDataBoundaryExtractor, vtkvmtkBoundaryReferenceSystems
*/

#ifndef __vtkvmtkCapPolyData_h
#define __vtkvmtkCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCapPolyData *New();

  ///@{
  /*! Set/Get the ids of the boundaries to cap. If not set (default, NULL), every open boundary of
      the input surface is capped. A boundary's id is its label when BoundaryLabelsArrayName and
      BoundaryPointOrderArrayName are set and the input carries those arrays, and otherwise its
      position in the list of open boundaries extracted from the input, in the order returned by
      vtkvmtkPolyDataBoundaryExtractor. */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /*! Set/Get the name of the cell data array used to tag the newly created cap triangles with an
      integer id, one distinct value per capped boundary (offset by CellEntityIdOffset, then further
      offset by boundary index + 1). If the array already exists on the input, existing cell values
      are preserved and only the new cap cells are appended with the new tag. If left NULL (default),
      no cell entity id array is created. Commonly named "CellEntityIds". */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /*! Set/Get the base offset added to the ids written into CellEntityIdsArrayName. The id assigned
      to the cap of the i-th processed boundary is (i + 1 + CellEntityIdOffset). Default: 1. */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  ///@{
  /*! Set/Get the names of the point data arrays that carry the boundary labels of the input, as
      written by vtkvmtkPolyDataBoundaryLabeler. When both are set and the input carries arrays
      that still describe it, the boundaries are read from them instead of being extracted, and
      the cap of each boundary can be named through BoundaryCellEntityIds. The arrays are written
      on the output too, where the points of a capped boundary keep the label they had, as a
      record of the boundary the cap closed.

      Setting them also settles what a boundary id means everywhere else in this filter: with the
      labels in use a boundary's id is its label, and without them it is the boundary's position
      in the order the extractor returns, which is what it has always been. So BoundaryIds and
      BoundaryCellEntityIds are read the same way either way, and a caller that labels its surface
      gets ids that survive the filters in between without having to say so twice. */
  vtkSetStringMacro(BoundaryLabelsArrayName);
  vtkGetStringMacro(BoundaryLabelsArrayName);
  vtkSetStringMacro(BoundaryPointOrderArrayName);
  vtkGetStringMacro(BoundaryPointOrderArrayName);
  ///@}

  ///@{
  /*! Set/Get the id to write into CellEntityIdsArrayName for the cap of each boundary, indexed
      the way boundary ids are indexed everywhere else here (see BoundaryIds): entry i is the id
      given to the cap closing the boundary whose id is i.

      An id beyond the end of the array, or one whose entry is negative, is no entry at all: with
      the labels in use that cap keeps its boundary's label, which names it just as well, and
      without them the id its position would have given it. An output can carry ids from both
      rules at once.
      An id chosen here is used as it stands, with CellEntityIdOffset not added to it, while the
      offset still lands on the cells copied from the input and on any boundary left to its
      positional id.

      Where this earns its keep is with the boundary labels in use, because then the choice
      survives the filters in between: the label of a vessel end is carried by the surface's own
      point data, so a flow extension that replaces a boundary with a new one at the tip of its
      extension, or any filter that renumbers the points, leaves the answer to "which end is this"
      unchanged. */
  vtkSetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  vtkGetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  ///@}

  ///@{
  /*! Set/Get the displacement of boundary barycenters along the (outward-oriented) boundary normal,
      expressed as a fraction of the boundary's mean radius. A small positive value (default 0.1)
      lifts the cap center off the boundary plane so that Delaunay tetrahedralization of the capped
      surface does not produce degenerate (near-zero-volume) tetrahedra. */
  vtkSetMacro(Displacement,double);
  vtkGetMacro(Displacement,double);
  ///@}

  ///@{
  /*! Set/Get the in-plane displacement of boundary barycenters, expressed as a fraction of the
      boundary's mean radius, applied along one of the two directions perpendicular to the boundary
      normal. Like Displacement, this is used to break symmetry and avoid degenerate tetrahedra in a
      subsequent Delaunay tessellation. Default: 0.1. */
  vtkSetMacro(InPlaneDisplacement,double);
  vtkGetMacro(InPlaneDisplacement,double);
  ///@}

  /*! Get the ids, in the output poly data, of the newly inserted cap-center (barycenter) points --
      one per processed boundary, in the same order as the boundaries returned by
      vtkvmtkPolyDataBoundaryExtractor, and indexed consistently with BoundaryIds when the latter is
      set. Valid only after Update() has been called. */
  vtkGetObjectMacro(CapCenterIds,vtkIdList);

  protected:
  vtkvmtkCapPolyData();
  ~vtkvmtkCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;
  char* BoundaryLabelsArrayName;
  char* BoundaryPointOrderArrayName;
  vtkIdTypeArray* BoundaryCellEntityIds;

  double Displacement;
  double InPlaneDisplacement;
  vtkIdList* CapCenterIds;

  private:
  vtkvmtkCapPolyData(const vtkvmtkCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkCapPolyData&);  // Not implemented.
};

#endif
