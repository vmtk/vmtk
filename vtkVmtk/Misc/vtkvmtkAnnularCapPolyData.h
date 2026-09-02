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
 * @class   vtkvmtkAnnularCapPolyData
 * @brief   Add annular caps between the boundaries of a walled surface.
 * @ingroup Misc
 *
 * This class closes the boundaries between the surfaces of a walled surface with caps. The
 * surfaces are required to be dense for the algorithm to produce legal caps.
 *
 * Unlike vtkvmtkCapPolyData (which caps each open boundary independently with a single-vertex
 * triangle fan), this filter is meant for surfaces that have two "parallel" boundaries bounding a
 * wall thickness (e.g. an inner lumen boundary and an outer wall boundary produced by offsetting
 * a vessel surface) -- it pairs up the closest boundaries by barycenter distance and connects them
 * with an annular strip of triangles, without inserting new points. This is one of the capping
 * strategies used by the vmtksurfacecapper pype script (its "annular" Method), producing a closed
 * surface out of a "walled" (double-boundary) input. Boundaries can be restricted with
 * BoundaryIds. If CellEntityIdsArrayName is set, the newly created cap triangles are tagged with
 * per-boundary-pair ids in that cell data array.
 *
 * @sa
 * vtkvmtkCapPolyData, vtkvmtkPolyDataBoundaryExtractor, vtkvmtkBoundaryReferenceSystems
 */

#ifndef __vtkvmtkAnnularCapPolyData_h
#define __vtkvmtkAnnularCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkAnnularCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkAnnularCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkAnnularCapPolyData *New();

  ///@{
  /**
   * Set/Get the ids (into the list of open boundaries extracted from the input, in the order
   * returned by vtkvmtkPolyDataBoundaryExtractor) of the boundaries to pair up and cap. If not
   * set (default, NULL), every open boundary of the input surface is considered, paired with its
   * closest (by barycenter distance) unpaired boundary. The number of boundaries considered must
   * be even.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array used to tag the newly created annular cap triangles
   * with an integer id, one distinct value per capped boundary pair (offset by
   * CellEntityIdOffset, then further offset by boundary index + 1). If the array already exists
   * on the input, existing cell values are preserved and only the new cap cells are appended with
   * the new tag. If left NULL (default), no cell entity id array is created.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the base offset added to the ids written into CellEntityIdsArrayName. The id assigned
   * to the cap connecting the i-th processed boundary pair is (i + 1 + CellEntityIdOffset).
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
   * the cap of each pair can be named through BoundaryCellEntityIds.
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
   * the way boundary ids are indexed everywhere else here (see BoundaryIds).
   *
   * A cap here closes a pair of boundaries rather than one, so it has two entries to choose
   * between: it takes whichever of the two has an entry, and when both do and they disagree it
   * takes the one of the lower boundary id and says so. Labelling the surface with
   * vtkvmtkPolyDataBoundaryLabeler in its Annular mode makes the lower of a pair the inner
   * boundary every time, so the cap is named after the vessel end rather than after whichever
   * boundary the pairing happened to find first.
   *
   * An id beyond the end of the array, or one whose entry is negative, is no entry at all. An id
   * chosen here is used as it stands, with CellEntityIdOffset not added to it.
   */
  vtkSetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  vtkGetObjectMacro(BoundaryCellEntityIds,vtkIdTypeArray);
  ///@}

  protected:
  vtkvmtkAnnularCapPolyData();
  ~vtkvmtkAnnularCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  /// The id to tag the cap closing a pair of boundaries with: the entry of whichever of the two
  /// has one, the lower boundary id winning a disagreement, or the positional id.
  vtkIdType PairCapCellEntityId(vtkIdType positionalId, vtkIdType boundaryId, vtkIdType partnerBoundaryId);

  vtkIdList* BoundaryIds;
  char* BoundaryLabelsArrayName;
  char* BoundaryPointOrderArrayName;
  vtkIdTypeArray* BoundaryCellEntityIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  private:
  vtkvmtkAnnularCapPolyData(const vtkvmtkAnnularCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkAnnularCapPolyData&);  // Not implemented.
};

#endif
