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
 * @class   vtkvmtkUnstructuredGridCenterlineGroupsClipper
 * @brief   Split an unstructured grid into groups/branches corresponding to their proximity to the polyball line of a branched centerline.
 * @ingroup ComputationalGeometry
 *
 * The vtkUnstructuredGrid counterpart of vtkvmtkPolyDataCenterlineGroupsClipper: for each mesh
 * point, evaluates which branch group's polyball-line tube function (see vtkvmtkPolyBallLine) it is
 * closest to, using this as an implicit clipping function to cut a volumetric mesh along the
 * boundaries between branches. This is the filter behind the vmtkmeshbranchclipper pype script.
 *
 * @sa
 * vtkvmtkPolyDataCenterlineGroupsClipper, vtkvmtkPolyBallLine
 */

#ifndef __vtkvmtkUnstructuredGridCenterlineGroupsClipper_h
#define __vtkvmtkUnstructuredGridCenterlineGroupsClipper_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridCenterlineGroupsClipper : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkUnstructuredGridCenterlineGroupsClipper,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkUnstructuredGridCenterlineGroupsClipper *New();
  
  ///@{
  /**
   * Set/Get the split, grouped centerlines used to define the clipping. Required input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the subset of centerline group ids to clip the mesh against. If not set (default, NULL),
   * all group ids present in Centerlines are used (unless ClipAllCenterlineGroupIds restricts
   * further).
   */
  vtkSetObjectMacro(CenterlineGroupIds,vtkIdList);
  vtkGetObjectMacro(CenterlineGroupIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the branch group id of each cell.
   * Required input.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point. Used when UseRadiusInformation is on.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the group id assigned to each mesh cell is
   * stored.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding, for each cell, whether it is a
   * "blanked" (redundant, overlapping) tract -- blanked tracts are excluded when building the
   * per-group polyball clipping functions.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Toggle clipping against every centerline group id present in Centerlines, ignoring
   * CenterlineGroupIds. Default: off.
   */
  vtkSetMacro(ClipAllCenterlineGroupIds,int);
  vtkGetMacro(ClipAllCenterlineGroupIds,int);
  vtkBooleanMacro(ClipAllCenterlineGroupIds,int);
  ///@}

  ///@{
  /**
   * Toggle generating a second output containing the complementary (clipped-away) part of the mesh,
   * retrievable with GetClippedOutput(). Default: off.
   */
  vtkSetMacro(GenerateClippedOutput,int);
  vtkGetMacro(GenerateClippedOutput,int);
  vtkBooleanMacro(GenerateClippedOutput,int);
  ///@}

  /**
   * Get the complementary (clipped-away) mesh produced when GenerateClippedOutput is on.
   */
  vtkUnstructuredGrid* GetClippedOutput();

  ///@{
  /**
   * Set/Get a cutoff distance, expressed as a multiple of the local maximum inscribed sphere radius,
   * beyond which a mesh point is not considered close to a branch's polyball function. Default: a
   * very large value (effectively unbounded).
   */
  vtkSetMacro(CutoffRadiusFactor,double);
  vtkGetMacro(CutoffRadiusFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the implicit-function value at which the mesh is clipped against each branch's polyball
   * function. Default: 0.0.
   */
  vtkSetMacro(ClipValue,double);
  vtkGetMacro(ClipValue,double);
  ///@}

  ///@{
  /**
   * Toggle using the per-point sphere radii (CenterlineRadiusArrayName) when evaluating each
   * branch's polyball clipping function, so that the tube function's true tapered shape is used
   * rather than a plain distance-to-centerline test. Default: on.
   */
  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);
  ///@}

  protected:
  vtkvmtkUnstructuredGridCenterlineGroupsClipper();
  ~vtkvmtkUnstructuredGridCenterlineGroupsClipper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* Centerlines;

  vtkIdList* CenterlineGroupIds;

  char* CenterlineGroupIdsArrayName;
  char* CenterlineRadiusArrayName;

  char* GroupIdsArrayName;
  char* BlankingArrayName;

  int ClipAllCenterlineGroupIds;
  double CutoffRadiusFactor;
  double ClipValue;

  int GenerateClippedOutput;

  int UseRadiusInformation;

  private:
  vtkvmtkUnstructuredGridCenterlineGroupsClipper(const vtkvmtkUnstructuredGridCenterlineGroupsClipper&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridCenterlineGroupsClipper&);  // Not implemented.
};

#endif
