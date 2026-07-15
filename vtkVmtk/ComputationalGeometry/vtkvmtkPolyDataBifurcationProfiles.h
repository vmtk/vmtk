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
 * @class   vtkvmtkPolyDataBifurcationProfiles
 * @brief   Compute bifurcation profiles (the bifurcation splitting lines) from an input surface and grouped and split centerlines.
 * @ingroup ComputationalGeometry
 *
 * Used to extract the bifurcation group, the orientation, and label the group as either coming from an upstream source or leading to a downstream centerline branch.
 *
 * Takes as input a surface and centerlines that have both already been split into branches (see
 * vtkvmtkCenterlineSplittingAndGroupingFilter, vtkvmtkPolyDataCenterlineGroupsClipper). This is the
 * filter behind the vmtkbifurcationprofiles pype script.
 *
 * @sa
 * vtkvmtkPolyDataBifurcationSections, vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkPolyDataBifurcationProfiles_h
#define __vtkvmtkPolyDataBifurcationProfiles_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBifurcationProfiles : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBifurcationProfiles,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataBifurcationProfiles* New();

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
   * Set/Get the name of the output cell data array where the group id that each bifurcation profile
   * belongs to is stored.
   * Commonly named "BifurcationProfileGroupIds".
   */
  vtkSetStringMacro(BifurcationProfileGroupIdsArrayName);
  vtkGetStringMacro(BifurcationProfileGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the group id of the bifurcation that each
   * profile belongs to is stored.
   * Commonly named "BifurcationProfileBifurcationGroupIds".
   */
  vtkSetStringMacro(BifurcationProfileBifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationProfileBifurcationGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the orientation of each profile is stored:
   * 0 if the profile lies upstream of its bifurcation, 1 if downstream.
   * Commonly named "BifurcationProfileOrientation".
   */
  vtkSetStringMacro(BifurcationProfileOrientationArrayName);
  vtkGetStringMacro(BifurcationProfileOrientationArrayName);
  ///@}

  protected:
  vtkvmtkPolyDataBifurcationProfiles();
  ~vtkvmtkPolyDataBifurcationProfiles();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBifurcationProfiles(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output);
  
  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BifurcationProfileGroupIdsArrayName;
  char* BifurcationProfileBifurcationGroupIdsArrayName;
  char* BifurcationProfileOrientationArrayName;

  private:
  vtkvmtkPolyDataBifurcationProfiles(const vtkvmtkPolyDataBifurcationProfiles&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBifurcationProfiles&);  // Not implemented.
};

#endif
