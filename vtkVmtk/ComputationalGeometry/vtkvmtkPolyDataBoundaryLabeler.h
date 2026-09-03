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
 * @class   vtkvmtkPolyDataBoundaryLabeler
 * @brief   Give each open boundary of a surface a label that stays with it.
 * @ingroup ComputationalGeometry
 *
 * Extracts the open boundaries of the input surface and records them in two point data arrays,
 * so that later filters can tell the boundaries apart without extracting them again and without
 * relying on the extraction order staying put. See vtkvmtkBoundaryLabels for what the arrays
 * hold and why they are point data.
 *
 * This is meant to be the one place the boundaries of a surface are worked out. A boundary is
 * identified here, once, from geometry the caller already has -- the points it clicked, or the
 * planes it cut with -- and from then on the label is carried by the mesh rather than
 * recomputed, so vtkvmtkCapPolyData and vtkvmtkPolyDataFlowExtensionsFilter can be told which
 * vessel end is which without the caller having to translate ids across each of them.
 *
 * A boundary's label is the cell entity id its cap will carry. That is the whole point of the
 * numbering: a cap is asked for by the same number that names the vessel end it closes, so
 * nothing has to be translated between the point data that says which end a boundary is and the
 * cell data a solver reads its boundary conditions off. Labels therefore start above the wall,
 * at CellEntityIdOffset+1, rather than at zero -- see CellEntityIdOffset.
 *
 * The labeling modes differ only in where the labels come from:
 *
 * - BoundaryExtractionOrder gives boundary i the label i+1+CellEntityIdOffset, i being its
 *   position in the order vtkvmtkPolyDataBoundaryExtractor returns the boundaries in. These are
 *   exactly the ids vtkvmtkCapPolyData would have given the caps had it been left to number them
 *   itself, so a surface labeled this way is capped exactly as an unlabeled one is.
 * - ClosestToPlaneOrigin gives each plane's label to the boundary lying nearest its origin. The
 *   plane's normal is not used, so a caller with nothing but a position to point at a vessel end
 *   with can give the position as the origin and leave the normals unset.
 * - OnPlane gives a boundary a plane's label when every one of its points lies in that plane,
 *   within MaximumDistanceFromPlane. A boundary only partly in the plane is left alone rather
 *   than claimed, so a hole the surface already had cannot take the label of a cut. Where more
 *   than one boundary lies in the same plane, the one nearest its origin is taken and the rest
 *   are left to be labeled in their own right, so which of them it is does not come down to
 *   which the extractor happened to reach first.
 *
 * Both take a boundary only when it lies entirely within MaximumDistanceFromPlaneOrigin of the plane's
 * origin, which is what keeps a match local. A plane is infinite and a surface is not: without
 * that, a boundary on a different branch that happens to be coplanar with this cut, or one merely
 * nearest to it across a gap, would be claimed on the strength of a coincidence.
 * - MatchExistingLabels gives a boundary the label a strict majority of its own points already
 *   carry, and is how a surface is brought back into line after a filter that knew nothing about
 *   the arrays. Nothing is matched by distance: a boundary whose points do not agree -- a newly
 *   cut one, whose points are all new -- gets a fresh label rather than the label of whichever
 *   boundary happens to lie nearest. It matches the labels, not the geometry, and repairs
 *   neither: the boundaries are whatever the extractor finds in the mesh as it stands.
 *
 * In every mode a boundary that ends up with no label of its own is given a fresh one, above
 * every label already in use and never below CellEntityIdOffset+1, so no two boundaries ever
 * share a label and none of them is mistaken for the wall. Planes that matched no boundary are
 * reported in UnmatchedPlaneLabels rather than passed over in silence.
 *
 * This is the class to use. It uses vtkvmtkBoundaryLabels internally -- the names of the two
 * arrays and the value that means "not on a boundary" come from there, and that class is what
 * reads the arrays back into boundaries again -- but vtkvmtkBoundaryLabels only defines and reads
 * the format, and labels nothing itself.
 *
 * What makes going through this filter better than letting each filter work the boundaries out
 * for itself is that the boundary information ends up stored in the mesh. Downstream filters do
 * not extract the boundaries again: they read them from the arrays. That keeps them consistent,
 * since every filter is then looking at the same boundaries in the same order rather than at
 * whatever three separate extractions happen to agree on, and it saves the work of extracting
 * them over and over, which on a large surface is not free.
 *
 * @sa
 * vtkvmtkBoundaryLabels, vtkvmtkPolyDataBoundaryExtractor, vtkvmtkCapPolyData,
 * vtkvmtkPolyDataFlowExtensionsFilter
 */

#ifndef __vtkvmtkPolyDataBoundaryLabeler_h
#define __vtkvmtkPolyDataBoundaryLabeler_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkPoints.h"
#include "vtkvmtkWin32Header.h"

#include <vector>

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBoundaryLabeler : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkPolyDataBoundaryLabeler,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataBoundaryLabeler *New();

  //BTX
  enum
    {
    BOUNDARY_EXTRACTION_ORDER,
    CLOSEST_TO_PLANE_ORIGIN,
    ON_PLANE,
    MATCH_EXISTING_LABELS
    };
  //ETX

  ///@{
  /**
   * Set/Get the name of the point data array where each boundary point's label is written.
   * Defaults to vtkvmtkBoundaryLabels::GetDefaultBoundaryLabelsArrayName().
   */
  vtkSetStringMacro(BoundaryLabelsArrayName);
  vtkGetStringMacro(BoundaryLabelsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where each boundary point's index within its own
   * boundary is written. Defaults to
   * vtkvmtkBoundaryLabels::GetDefaultBoundaryPointOrderArrayName().
   */
  vtkSetStringMacro(BoundaryPointOrderArrayName);
  vtkGetStringMacro(BoundaryPointOrderArrayName);
  ///@}

  ///@{
  /**
   * Set/Get where the labels come from. See the class documentation.
   */
  vtkSetClampMacro(LabelingMode,int,BOUNDARY_EXTRACTION_ORDER,MATCH_EXISTING_LABELS);
  vtkGetMacro(LabelingMode,int);
  void SetLabelingModeToBoundaryExtractionOrder() { this->SetLabelingMode(BOUNDARY_EXTRACTION_ORDER); }
  void SetLabelingModeToClosestToPlaneOrigin() { this->SetLabelingMode(CLOSEST_TO_PLANE_ORIGIN); }
  void SetLabelingModeToOnPlane() { this->SetLabelingMode(ON_PLANE); }
  void SetLabelingModeToMatchExistingLabels() { this->SetLabelingMode(MATCH_EXISTING_LABELS); }
  ///@}

  ///@{
  /**
   * Set/Get the cell entity id of the wall, which the labels are numbered above. Default 1, the
   * same value vtkvmtkCapPolyData's own CellEntityIdOffset defaults to.
   *
   * A label is the cell entity id of the cap that will close its boundary, and the cappers leave
   * CellEntityIdOffset on the cells they copied from their input. So the first id a cap may take
   * is CellEntityIdOffset+1: this is what keeps a label off the wall, and off the 0 the volume
   * elements of a mesh carry. Set it to whatever the capper this surface will go to is set to.
   */
  vtkSetMacro(CellEntityIdOffset,vtkIdType);
  vtkGetMacro(CellEntityIdOffset,vtkIdType);
  ///@}

  ///@{
  /**
   * Set/Get whether the input is a walled surface, whose open boundaries come in pairs -- an
   * inner boundary and the outer one facing it across the wall thickness.
   *
   * With this off (default) every boundary is labelled on its own, and the two boundaries of a
   * wall end up with unrelated labels. With it on the boundaries are paired by barycenter
   * distance, the inner boundary of each pair is labelled as it would have been anyway, and its
   * outer partner takes that label plus AnnularOuterBoundaryOffset. Of a pair, the inner is
   * whichever has the smaller mean radius.
   *
   * The point of the arrangement is that the two labels of a wall are then related by a known
   * constant, and the inner one is always the lower of the two: a filter that caps the annulus
   * between them names its cap after the lower id, and so always names it after the vessel end
   * the caller meant, whichever way round the pair was found. An odd boundary with no partner
   * keeps the label it was given.
   */
  vtkSetMacro(Annular,bool);
  vtkGetMacro(Annular,bool);
  vtkBooleanMacro(Annular,bool);
  ///@}

  ///@{
  /**
   * Set/Get what is added to an inner boundary's label to give its outer partner's, when
   * Annular is on. Default 1000.
   *
   * It has to be larger than the highest label the surface will ever carry, or an outer
   * boundary's label collides with some other boundary's inner one.
   */
  vtkSetMacro(AnnularOuterBoundaryOffset,vtkIdType);
  vtkGetMacro(AnnularOuterBoundaryOffset,vtkIdType);
  ///@}

  ///@{
  /**
   * Set/Get the origins of the planes, one per plane. A point is a plane whose normal is never
   * asked for, so this is also where a caller with only positions to offer puts them.
   */
  vtkSetObjectMacro(PlaneOrigins,vtkPoints);
  vtkGetObjectMacro(PlaneOrigins,vtkPoints);
  ///@}

  ///@{
  /**
   * Set/Get the normals of the planes, one 3-component tuple per plane. They need not be unit
   * length. Required by the OnPlane mode and ignored by the others.
   */
  vtkSetObjectMacro(PlaneNormals,vtkDoubleArray);
  vtkGetObjectMacro(PlaneNormals,vtkDoubleArray);
  ///@}

  ///@{
  /**
   * Set/Get the label to give the boundary each plane matches. Entry i belongs to plane i. When
   * not set, plane i is taken to carry the label i+1+CellEntityIdOffset.
   *
   * A label set here is taken as it stands, so a caller choosing its own is the one responsible
   * for keeping them above the wall: a label of CellEntityIdOffset or below names a cap that
   * cannot be told from the wall it is set into.
   */
  vtkSetObjectMacro(PlaneLabels,vtkIdList);
  vtkGetObjectMacro(PlaneLabels,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get how far any point of a boundary may be from a plane for that boundary to count as
   * lying in it. Used by the OnPlane mode, which requires a positive value: a plane is infinite,
   * so with no limit every boundary lies in every plane.
   */
  vtkSetMacro(MaximumDistanceFromPlane,double);
  vtkGetMacro(MaximumDistanceFromPlane,double);
  ///@}

  ///@{
  /**
   * Set/Get how far any point of a boundary may be from a plane's origin for that boundary to be
   * a candidate for that plane's label. A boundary is considered only when it lies entirely
   * within this distance -- one point of it beyond and it is passed over, however well the rest
   * of it fits -- which is what confines a match to the neighbourhood of the origin. Zero or less
   * puts no limit on it. Used by both the ClosestToPlaneOrigin and the OnPlane modes.
   */
  vtkSetMacro(MaximumDistanceFromPlaneOrigin,double);
  vtkGetMacro(MaximumDistanceFromPlaneOrigin,double);
  ///@}

  /**
   * Number of open boundaries found in the input, valid after Update().
   */
  vtkGetMacro(NumberOfBoundaries,vtkIdType);

  /**
   * The label given to each boundary, in the order the boundaries were extracted, valid after
   * Update(). Use it to relate a label back to a positional boundary id, for a filter that has
   * not been told the array names.
   */
  vtkGetObjectMacro(BoundaryLabels,vtkIdList);

  /**
   * The labels of the planes that claimed no boundary, valid after Update(). A plane lands here
   * when nothing was near enough, when the boundary it would have claimed had already been taken
   * by an earlier plane, or when its boundary is not there at all -- a cut that removed another
   * cut's boundary, say.
   */
  vtkGetObjectMacro(UnmatchedPlaneLabels,vtkIdList);

  protected:
  vtkvmtkPolyDataBoundaryLabeler();
  ~vtkvmtkPolyDataBoundaryLabeler() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  /// Pair the boundaries by barycenter distance and give each outer boundary its inner
  /// partner's label plus AnnularOuterBoundaryOffset. See Annular.
  void LabelAnnularPairs(vtkPolyData* boundaries, std::vector<vtkIdType>& labels);

  char* BoundaryLabelsArrayName;
  char* BoundaryPointOrderArrayName;

  int LabelingMode;
  vtkIdType CellEntityIdOffset;
  bool Annular;
  vtkIdType AnnularOuterBoundaryOffset;

  vtkPoints* PlaneOrigins;
  vtkDoubleArray* PlaneNormals;
  vtkIdList* PlaneLabels;
  double MaximumDistanceFromPlane;
  double MaximumDistanceFromPlaneOrigin;

  vtkIdType NumberOfBoundaries;
  vtkIdList* BoundaryLabels;
  vtkIdList* UnmatchedPlaneLabels;

  private:
  vtkvmtkPolyDataBoundaryLabeler(const vtkvmtkPolyDataBoundaryLabeler&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBoundaryLabeler&);  // Not implemented.
};

#endif
