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
 * @class   vtkvmtkPolyDataBifurcationSections
 * @brief   Extract a set of vessel (cross) sections n-spheres away (upstream and downstream) from a bifurctation.
 * @ingroup ComputationalGeometry
 *
 * Takes as input a surface and centerlines that have both already been split into branches. This
 * is the filter behind the vmtkbifurcationsections pype script.
 *
 * @sa
 * vtkvmtkPolyDataBranchSections, vtkvmtkPolyDataBifurcationProfiles
 *
 * The set of vessel sections contain the profile as well as the the following information about the section:
 * - Bifurcation Section Group Ids
 * - Bifurcation Section Bifurcation Group Ids
 * - Bifurcation Section Orientation
 * - Bifurcation Section Distance Spheres
 * - Bifurcation Section Point
 * - Bifurcation Section Normal
 * - Bifurcation Section Area
 * - Bifurcation Section Min Size
 * - Bifurcation Section Max Size
 * - Bifurcation Section Shape
 * - Bifurcation Section Closed
 */

#ifndef __vtkvmtkPolyDataBifurcationSections_h
#define __vtkvmtkPolyDataBifurcationSections_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBifurcationSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBifurcationSections,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataBifurcationSections* New();

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
   * Set/Get the name of the output cell data array where the (x,y,z) point at which each section was
   * cut is stored.
   * Commonly named "BifurcationSectionPoint".
   */
  vtkSetStringMacro(BifurcationSectionPointArrayName);
  vtkGetStringMacro(BifurcationSectionPointArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the cutting plane normal of each section is
   * stored.
   * Commonly named "BifurcationSectionNormal".
   */
  vtkSetStringMacro(BifurcationSectionNormalArrayName);
  vtkGetStringMacro(BifurcationSectionNormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the cross-sectional area of each section is
   * stored.
   * Commonly named "BifurcationSectionArea".
   */
  vtkSetStringMacro(BifurcationSectionAreaArrayName);
  vtkGetStringMacro(BifurcationSectionAreaArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the minimum diameter of each section is
   * stored.
   * Commonly named "BifurcationSectionMinSize".
   */
  vtkSetStringMacro(BifurcationSectionMinSizeArrayName);
  vtkGetStringMacro(BifurcationSectionMinSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the maximum diameter of each section is
   * stored.
   * Commonly named "BifurcationSectionMaxSize".
   */
  vtkSetStringMacro(BifurcationSectionMaxSizeArrayName);
  vtkGetStringMacro(BifurcationSectionMaxSizeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the shape index of each section (the ratio
   * between its minimum and maximum diameter) is stored.
   * Commonly named "BifurcationSectionShape".
   */
  vtkSetStringMacro(BifurcationSectionShapeArrayName);
  vtkGetStringMacro(BifurcationSectionShapeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the group id that each section belongs to
   * is stored.
   * Commonly named "BifurcationSectionGroupIds".
   */
  vtkSetStringMacro(BifurcationSectionGroupIdsArrayName);
  vtkGetStringMacro(BifurcationSectionGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the group id of the bifurcation that each
   * section belongs to is stored.
   * Commonly named "BifurcationSectionBifurcationGroupIds".
   */
  vtkSetStringMacro(BifurcationSectionBifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationSectionBifurcationGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the orientation of each section is stored:
   * 0 if it lies upstream of its bifurcation, 1 if downstream.
   * Commonly named "BifurcationSectionOrientation".
   */
  vtkSetStringMacro(BifurcationSectionOrientationArrayName);
  vtkGetStringMacro(BifurcationSectionOrientationArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the number of touching spheres (see
   * NumberOfDistanceSpheres) each section was cut at is stored.
   * Commonly named "BifurcationSectionDistanceSpheres".
   */
  vtkSetStringMacro(BifurcationSectionDistanceSpheresArrayName);
  vtkGetStringMacro(BifurcationSectionDistanceSpheresArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where, for each section, whether the cutting
   * plane fully closes off the vessel lumen (1) or not (0, e.g. it also intersects a nearby
   * bifurcation or free end) is stored.
   * Commonly named "BifurcationSectionClosed".
   */
  vtkSetStringMacro(BifurcationSectionClosedArrayName);
  vtkGetStringMacro(BifurcationSectionClosedArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the distance from each bifurcation at which sections are cut, expressed in number of
   * touching inscribed spheres (each sphere touching the center of the previous one). Default: 1.
   */
  vtkSetMacro(NumberOfDistanceSpheres,int);
  vtkGetMacro(NumberOfDistanceSpheres,int);
  ///@}

  protected:
  vtkvmtkPolyDataBifurcationSections();
  ~vtkvmtkPolyDataBifurcationSections();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBifurcationSections(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output);
  
  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BifurcationSectionGroupIdsArrayName;
  char* BifurcationSectionBifurcationGroupIdsArrayName;
  char* BifurcationSectionOrientationArrayName;
  char* BifurcationSectionDistanceSpheresArrayName;
  char* BifurcationSectionPointArrayName;
  char* BifurcationSectionNormalArrayName;
  char* BifurcationSectionAreaArrayName;
  char* BifurcationSectionMinSizeArrayName;
  char* BifurcationSectionMaxSizeArrayName;
  char* BifurcationSectionShapeArrayName;
  char* BifurcationSectionClosedArrayName;

  int NumberOfDistanceSpheres;

  private:
  vtkvmtkPolyDataBifurcationSections(const vtkvmtkPolyDataBifurcationSections&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBifurcationSections&);  // Not implemented.
};

#endif
