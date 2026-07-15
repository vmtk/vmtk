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
   * Cut cylinder with the plane through origin with the given normal, storing the resulting polygon
   * in section and whether it forms a single closed loop in closed.
   */
  static void ExtractCylinderSection(vtkPolyData* cylinder, double origin[3], double normal[3], vtkPolyData* section, bool & closed);

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
