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
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCapPolyData *New();

  ///@{
  /*! Set/Get the ids (into the list of open boundaries extracted from the input, in the order
      returned by vtkvmtkPolyDataBoundaryExtractor) of the boundaries to cap. If not set (default,
      NULL), every open boundary of the input surface is capped. */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /*! Set/Get the name of the cell data array used to tag the cells of the output with an integer
      id: the cells copied from the input all get CellEntityIdOffset, and each cap gets a distinct
      id of its own, (boundary index + 1 + CellEntityIdOffset) by default or whatever
      BoundaryCellEntityIds asks for. If the array already exists on the input, existing cell values
      are preserved and only the new cap cells are appended with the new tag. If left NULL (default),
      no cell entity id array is created. Commonly named "CellEntityIds". */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /*! Set/Get the base offset of the ids written into CellEntityIdsArrayName. It is both the id left
      on the cells copied from the input -- the wall, in a vascular surface -- and the base the cap
      ids count up from: the cap of the i-th processed boundary gets (i + 1 + CellEntityIdOffset),
      so the caps occupy the ids above the wall. A cap named by BoundaryCellEntityIds is the
      exception: it takes that id as it stands, with no offset applied. Default: 1. */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  ///@{
  /*! Set/Get the cell entity id to write into CellEntityIdsArrayName for the cap of each boundary,
      in place of the (boundary index + 1 + CellEntityIdOffset) that boundary's position in the list
      would give it. Indexed consistently with BoundaryIds -- entry i belongs to boundary i of the
      list of open boundaries extracted from the input. A boundary whose entry is negative or beyond
      the end of the list keeps the index-derived id, as does every boundary when this is not set
      (default, NULL). Requires CellEntityIdsArrayName to be set; ignored otherwise.

      The id given here is used as it stands: CellEntityIdOffset is not added to it. The offset still
      applies to everything it is not asked about, so the three can appear side by side in one
      output -- with an offset of 100 and entries [7, -1], the cells copied from the input get 100,
      the first cap gets 7, and the second, left to its position, gets 102. Choosing an offset that
      puts the derived ids clear of the chosen ones is what keeps a boundary the caller did not
      account for from colliding with one it did.

      The point of all this is that the boundary list is ordered by the boundary extractor, so a
      cap's id otherwise depends on how the input happens to be meshed and on which other boundaries
      exist, which is fragile for a caller that has to keep the same face numbering across runs. Note
      that the ids here are indices into *this* filter's extraction of *its own* input: a caller that
      grew flow extensions first has to carry its ids across that change with
      vtkvmtkPolyDataFlowExtensionsFilter::GetOutputBoundaryIds(), since extending a boundary
      replaces it and reorders the extraction. */
  vtkSetObjectMacro(BoundaryCellEntityIds,vtkIdList);
  vtkGetObjectMacro(BoundaryCellEntityIds,vtkIdList);
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
  vtkIdList* BoundaryCellEntityIds;
  int CellEntityIdOffset;

  double Displacement;
  double InPlaneDisplacement;
  vtkIdList* CapCenterIds;

  private:
  vtkvmtkCapPolyData(const vtkvmtkCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkCapPolyData&);  // Not implemented.
};

#endif
