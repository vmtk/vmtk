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
 * @class   vtkvmtkBoundaryLabels
 * @brief   Boundaries of a surface, stored in the surface's own point data.
 * @ingroup ComputationalGeometry
 *
 * A boundary has no identity of its own in these filters: it is referred to by its position in
 * the list vtkvmtkPolyDataBoundaryExtractor returns, and that order falls out of the input's
 * cell and point numbering. Two filters agree on which boundary is which only for as long as
 * they are handed the very same mesh, so any filter that renumbers points, merges them, or
 * rebuilds the surface silently permutes the ids, and a caller that carried an id across it
 * ends up pointing at the wrong vessel end.
 *
 * This class stores the whole extraction result in two point data arrays instead, so that the
 * answer travels with the mesh:
 *
 * - the labels array holds, on each point of a boundary, the label of the boundary it belongs
 *   to, and InvalidBoundaryLabel on every other point. This is the boundary's identity.
 * - the point order array holds that point's index within its boundary, 0..n-1, and
 *   InvalidBoundaryLabel elsewhere. This is the ring's order and its winding direction.
 *
 * Both are written as vtkIntArray rather than the vtkIdTypeArray they would otherwise be. VTK
 * takes an array of vtkIdType to be point ids, which cannot be interpolated onto a point that
 * did not exist before, so it drops such an array outright rather than carry it: passing a
 * labeled surface through vtkClipPolyData loses vtkIdTypeArray point data and keeps every
 * other numeric type. Since surviving that is the whole point of storing the boundaries this
 * way, they are ints. They are read back through vtkDataArray and rounded, so an array a filter
 * has passed through as some other type is read just the same.
 *
 * Neither array records a point id, so nothing in them is invalidated by renumbering. Merging
 * coincident points keeps them (both copies of a boundary point carry the same pair), moving
 * points keeps them, and appending points leaves the existing entries alone. Contrast a stored
 * copy of the extractor's output, whose point scalars are input point ids and so mean nothing
 * once the mesh they indexed has changed.
 *
 * A labels array on its own would not be enough to skip the extraction: it says which points
 * belong to a boundary but not in what order, and the cap triangle fan and the flow extension
 * rings both need the ordered ring.
 *
 * GetBoundaries() rebuilds, from the two arrays alone, a vtkPolyData in the form
 * vtkvmtkPolyDataBoundaryExtractor produces, so a filter can use it in place of extracting.
 *
 * This class does not label anything: it is the format and the reader, not the writer. To put
 * labels on a surface use vtkvmtkPolyDataBoundaryLabeler, which is the filter that works the
 * boundaries out and writes the two arrays, and which is what a caller normally wants. What lives
 * here is what both ends of that arrangement have to agree on -- the default array names, the
 * value that means "not on a boundary", and the reading of the arrays back into boundaries -- so
 * that vtkvmtkCapPolyData and vtkvmtkPolyDataFlowExtensionsFilter can consume what the labeler
 * wrote without either of them owning the format. Use it directly to read the boundaries of a
 * surface that is already labeled, and to check whether a surface carries the arrays at all.
 *
 * @sa
 * vtkvmtkPolyDataBoundaryLabeler, vtkvmtkPolyDataBoundaryExtractor, vtkvmtkCapPolyData,
 * vtkvmtkPolyDataFlowExtensionsFilter
 */

#ifndef __vtkvmtkBoundaryLabels_h
#define __vtkvmtkBoundaryLabels_h

#include "vtkObject.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkBoundaryLabels : public vtkObject
{
  public:
  vtkTypeMacro(vtkvmtkBoundaryLabels,vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkBoundaryLabels *New();

  /**
   * Name the filters use for the labels array unless told otherwise.
   */
  static const char* GetDefaultBoundaryLabelsArrayName() { return "BoundaryLabels"; }

  /**
   * Name the filters use for the point order array unless told otherwise.
   */
  static const char* GetDefaultBoundaryPointOrderArrayName() { return "BoundaryPointOrder"; }

  /**
   * Value both arrays carry on a point that is not on a boundary. Any negative value is read
   * the same way, so a label is always non-negative.
   */
  static vtkIdType GetInvalidBoundaryLabel() { return -1; }

  /**
   * Whether surface carries both arrays, sized to its points. Says nothing about whether their
   * contents still describe the surface: only GetBoundaries() can answer that.
   */
  static bool HasBoundaryLabels(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName);

  /**
   * Rebuild the boundaries of surface from the two arrays, without extracting them.
   *
   * boundaries is filled in the form vtkvmtkPolyDataBoundaryExtractor produces - the boundary
   * points copied into their own vtkPoints, one polyline cell per boundary, and point scalars
   * holding the id, in surface, of each of those points - so that it can be used wherever the
   * extractor's output is. Boundaries come out in ascending label order, and boundaryLabels, if
   * given, receives the label of each cell of boundaries in turn.
   *
   * Returns false, leaving boundaries empty, when the arrays are missing or no longer describe
   * surface, in which case the caller should extract the boundaries instead. What is checked is
   * that every label owns a whole run of point order values, 0..n-1, with none missing, none
   * repeated, and at least three points - enough to catch an array left over from an earlier
   * mesh, a merge that collapsed two points of a ring onto one, and the fractional values a
   * clipping filter leaves behind when it interpolates the arrays onto the points it creates.
   * A boundary whose points were all removed simply disappears, which is not an error.
   */
  static bool GetBoundaries(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName, vtkPolyData* boundaries, vtkIdList* boundaryLabels);

  /**
   * The boundaries of surface, read from the labels when both array names are given and the
   * arrays still describe it, and extracted with vtkvmtkPolyDataBoundaryExtractor otherwise.
   *
   * boundaries is filled either way, in the form the extractor produces, so a filter can work
   * from it without knowing which of the two happened. boundaryLabels holds the label of each
   * boundary when the labels were used and is emptied when they were not, and the return value
   * says which -- and so says what a boundary's id means: its label with the labels in use, and
   * its position in the extraction order without them.
   *
   * A surface that names the arrays but no longer carries ones describing it falls back to
   * extraction, which renames every cap a caller had chosen an id for. Pass the calling filter
   * as warningSource to have that said rather than passed over.
   */
  static bool GetOrExtractBoundaries(vtkPolyData* surface, const char* boundaryLabelsArrayName, const char* boundaryPointOrderArrayName, vtkPolyData* boundaries, vtkIdList* boundaryLabels, vtkObject* warningSource = nullptr);

  protected:
  vtkvmtkBoundaryLabels() {}
  ~vtkvmtkBoundaryLabels() override {}

  private:
  vtkvmtkBoundaryLabels(const vtkvmtkBoundaryLabels&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryLabels&);  // Not implemented.
};

#endif
