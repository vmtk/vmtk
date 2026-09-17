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
 * @class   vtkvmtkPolyDataBranchSections
 * @brief   Extract a vessel (cross) section n-spheres distance from from the start point of a branch.
 * @ingroup ComputationalGeometry
 *
 * Takes as input a surface and centerlines that have both already been split into branches. This
 * is the filter behind the vmtkbranchsections pype script; unlike
 * vtkvmtkPolyDataBifurcationSections (which cuts near bifurcations), this class cuts one section
 * per branch, at a fixed number of touching spheres from the branch's start (or end, if
 * ReverseDirection is on).
 *
 * @sa
 * vtkvmtkPolyDataBifurcationSections
 *
 *  The set of vessel sections contain the profile as well as the the following information about the section:
 *  - Branch Section Group Ids
 *  - Branch Section Bifurcation Group Ids
 *  - Branch Section Orientation
 *  - Branch Section Distance Spheres
 *  - Branch Section Point
 *  - Branch Section Normal
 *  - Branch Section Area
 *  - Branch Section Min Size
 *  - Branch Section Max Size
 *  - Branch Section Shape
 *  - Branch Section Closed
 */

#ifndef __vtkvmtkPolyDataBranchSections_h
#define __vtkvmtkPolyDataBranchSections_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBranchSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBranchSections,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataBranchSections* New();

  ///@{
  /**
   * Set/Get the name of the cell data array of the input surface holding the branch group id of each
   * cell. Required input.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the split, grouped centerlines corresponding to the input surface. Required input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the branch group id of each cell.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the id of the original, unsplit
   * centerline that each cell belongs to.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the tract id of each cell.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding, for each cell, whether it is a
   * "blanked" (redundant, overlapping) tract.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the cross-sectional area of each branch
   * section is stored.
   * Commonly named "BranchSectionArea".
   */
  vtkSetStringMacro(BranchSectionAreaArrayName);
  vtkGetStringMacro(BranchSectionAreaArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the minimum diameter of each branch section
   * is stored.
   * Commonly named "BranchSectionMinSize".
   */
  vtkSetStringMacro(BranchSectionMinSizeArrayName);
  vtkGetStringMacro(BranchSectionMinSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the maximum diameter of each branch section
   * is stored.
   * Commonly named "BranchSectionMaxSize".
   */
  vtkSetStringMacro(BranchSectionMaxSizeArrayName);
  vtkGetStringMacro(BranchSectionMaxSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the shape index of each branch section (the
   * ratio between its minimum and maximum diameter) is stored.
   * Commonly named "BranchSectionShape".
   */
  vtkSetStringMacro(BranchSectionShapeArrayName);
  vtkGetStringMacro(BranchSectionShapeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the group id (branch) that each section
   * belongs to is stored.
   * Commonly named "BranchSectionGroupIds".
   */
  vtkSetStringMacro(BranchSectionGroupIdsArrayName);
  vtkGetStringMacro(BranchSectionGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where, for each section, whether the cutting
   * plane fully closes off the vessel lumen (1) or not (0) is stored.
   * Commonly named "BranchSectionClosed".
   */
  vtkSetStringMacro(BranchSectionClosedArrayName);
  vtkGetStringMacro(BranchSectionClosedArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the number of touching spheres (see
   * NumberOfDistanceSpheres) each section was cut at is stored.
   * Commonly named "BranchSectionDistanceSpheres".
   */
  vtkSetStringMacro(BranchSectionDistanceSpheresArrayName);
  vtkGetStringMacro(BranchSectionDistanceSpheresArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the distance from the branch's start point (or end point, if ReverseDirection is on) at
   * which the section is cut, expressed in number of touching inscribed spheres (each sphere
   * touching the center of the previous one). Default: 1.
   */
  vtkSetMacro(NumberOfDistanceSpheres,int);
  vtkGetMacro(NumberOfDistanceSpheres,int);
  ///@}

  ///@{
  /**
   * Toggle measuring NumberOfDistanceSpheres from the end of each branch instead of its start.
   * Default: off.
   */
  vtkSetMacro(ReverseDirection,int);
  vtkGetMacro(ReverseDirection,int);
  vtkBooleanMacro(ReverseDirection,int);
  ///@}

  /**
   * Compute the area of a single cross-section polygon (typically one cell of the output).
   */
  static double ComputeBranchSectionArea(vtkPolyData* branchSection);

  /**
   * Compute the shape index (ratio of minimum to maximum diameter) of a single cross-section
   * polygon, also returning its center and the [min,max] diameter range in sizeRange.
   */
  static double ComputeBranchSectionShape(vtkPolyData* branchSection, double center[3], double sizeRange[2]);

  /**
   * Cut a surface with a plane (specified by origin and normal) and store the cross-section as a
   * single polygon cell in section.
   *
   * The name comes from this class cutting one branch at a time, but the input does not have to be
   * a cylinder or even tube-like: any polygonal surface can be cut, such as a whole vessel tree. When
   * the plane cuts the surface along several separate contours (for example across two vessels),
   * only the connected contour that contains the cut point closest to origin is kept. The polygon
   * points are ordered along the contour, so section can be passed directly to
   * ComputeBranchSectionArea and ComputeBranchSectionShape.
   *
   * closed is set to true if the contour is a closed loop. It is false if the contour is open, for
   * example when the plane crosses an open end of the surface; the polygon then closes the gap with
   * a straight edge, so its area and shape are not meaningful. If the plane does not cut the surface,
   * section is left empty (it has no cells) and closed is false; any previous content of section is
   * discarded.
   *
   * If idsArrayName is set, that point data array of cylinder, such as vessel or branch labels, is
   * copied to the points of section, so that CountBranchSectionIds can be used on the result. The
   * values are not interpolated: each section point takes the value of the nearer end point of the
   * cut surface edge it lies on, so the array only contains values that exist on the surface. No
   * other point data and no cell data is copied, and if cylinder has no such array, section has no
   * point data. Default: none.
   */
  static void ExtractCylinderSection(vtkPolyData* cylinder, double origin[3], double normal[3], vtkPolyData* section, bool & closed, const char* idsArrayName = nullptr);

  /**
   * Count how many centerlines pass through a vessel cross-section.
   *
   * centerlines is cut with the plane (specified by origin and normal), and the crossing points
   * that lie inside the first cell of section are counted. section must be the polygon returned by
   * ExtractCylinderSection for the same plane. Returns 0 if section has fewer than 3 points or no
   * centerline crosses the plane.
   *
   * The count tells whether a section cuts a single vessel or reaches into a bifurcation region:
   * a section perpendicular to a branch contains only that branch's centerline, while near a
   * bifurcation the section widens to include the centerlines of the child vessels as well. For
   * example, cutting a section at each centerline point with the centerline tangent as normal and
   * marking the points with a count other than 1 delineates the bifurcation regions along the
   * centerlines. Similarly, the count can confirm that a section used for area or diameter
   * measurement cuts only one vessel.
   *
   * Each crossing point is counted once, so centerlines must not overlap. Centerlines computed from
   * the inlet to each outlet run almost on top of each other along their shared segments, where a
   * section would count each of them; merge them into a single tree without duplicated segments
   * first. A count of 1 is not a guarantee that the section is outside a bifurcation region, since
   * an oblique plane may miss a nearby centerline; CountBranchSectionIds can be used as an
   * additional check.
   */
  static int CountBranchSectionCenterlines(vtkPolyData* section, vtkPolyData* centerlines, double origin[3], double normal[3]);

  /**
   * Count how many labeled regions of a surface a vessel cross-section touches.
   *
   * The distinct values of the point data array idsArrayName on the points of section are counted.
   * section must be extracted with ExtractCylinderSection, with the same idsArrayName, from a
   * surface whose points carry idsArrayName as integer labels, for example the id of the vessel or
   * branch each point belongs to. Values are converted to integers. Returns 0 if section has no such array.
   *
   * The count tells whether a section lies within a single labeled region: a section across one
   * vessel touches only the label of that vessel, while a section that runs across a boundary
   * between regions touches two or more. For example, together with CountBranchSectionCenterlines
   * it can classify sections cut along centerlines: a section is in a single branch if both counts
   * are 1. The two checks complement each other, since an
   * oblique section can run onto the wall of a neighboring vessel whose centerline it does not
   * contain. Similarly, the count can confirm that a section used for area or diameter measurement
   * lies within one labeled region.
   *
   * ExtractCylinderSection copies the labels to the section points without interpolation, so only
   * labels that exist on the surface are counted, even where regions with labels that are not
   * consecutive integers meet (for example 0 and 3).
   */
  static int CountBranchSectionIds(vtkPolyData* section, const char* idsArrayName);

  protected:
  vtkvmtkPolyDataBranchSections();
  ~vtkvmtkPolyDataBranchSections();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBranchSections(vtkPolyData* input, int groupId, vtkPolyData* output);

  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BranchSectionGroupIdsArrayName;
  char* BranchSectionAreaArrayName;
  char* BranchSectionMinSizeArrayName;
  char* BranchSectionMaxSizeArrayName;
  char* BranchSectionShapeArrayName;
  char* BranchSectionClosedArrayName;
  char* BranchSectionDistanceSpheresArrayName;

  int NumberOfDistanceSpheres;
  int ReverseDirection;

  private:
  vtkvmtkPolyDataBranchSections(const vtkvmtkPolyDataBranchSections&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBranchSections&);  // Not implemented.
};

#endif
