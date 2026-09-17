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

  This file is derived from vtkvmtkPolyDataCenterlineSections of SimVascular
  (https://github.com/SimVascular/SimVascular), provided under the following
  license:

  Copyright (c) Stanford University, The Regents of the University of
  California, and others.

  All Rights Reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

=========================================================================*/
/**
 * @class   vtkvmtkPolyDataCenterlineBranchSplitting
 * @brief   Split centerlines into branches and bifurcations and cut vessel sections along them.
 * @ingroup ComputationalGeometry
 *
 * Takes as input a closed vessel surface and the centerlines computed from it from a single inlet
 * to each outlet, such as the output of vtkvmtkPolyDataCenterlines, with the maximum inscribed
 * sphere radius at each point. This is the geometry processing needed to build reduced-order (0D
 * and 1D) blood flow models, originally developed for SimVascular.
 *
 * The centerlines are merged into a single tree without duplicated points, smoothed, and refined
 * next to each cap. A section of the surface is cut perpendicular to the centerline at each point,
 * as in vtkvmtkPolyDataCenterlineSections, and each section is classified as part of a single
 * branch or of a bifurcation region, from the number of centerlines passing through it
 * (vtkvmtkPolyDataBranchSections::CountBranchSectionCenterlines) and the number of branches of the
 * surface it touches (vtkvmtkPolyDataBranchSections::CountBranchSectionIds). The centerlines are
 * then split into branches and bifurcations, which are numbered, and the surface is labeled with
 * the branch or bifurcation each point belongs to.
 *
 * Results:
 * - Output: one section per centerline point, with the section properties of
 *   vtkvmtkPolyDataCenterlineSections and, as cell data, whether the section is in a bifurcation
 *   region (CenterlineSectionBifurcationArrayName) and the id of its centerline point
 *   (GlobalNodeIdArrayName). Sections are cut before points in bifurcation regions within a branch
 *   are removed from the centerlines, so these ids do not match the ids of the output centerlines
 *   when points were removed.
 * - Centerlines: replaced by the processed centerlines, a tree of two-point lines, with point data
 *   holding the section properties, the section normal (CenterlineSectionNormalArrayName), the
 *   branch id (BranchIdArrayName) or bifurcation id (BifurcationIdArrayName) of each point (-1 for
 *   the other kind), the distance along its branch or bifurcation (PathArrayName), which input
 *   centerlines pass through the point (CenterlineIdArrayName, one component per input centerline),
 *   and the point id (GlobalNodeIdArrayName).
 * - Surface: the input surface with point normals and the branch id and bifurcation id of each
 *   point, from the nearest centerline point of a branch and of a bifurcation in front of it, or -1
 *   if there is none. Points near a bifurcation have both.
 *
 * The filter replaces the content of the object set as Centerlines, so updating it again after
 * changing its parameters requires setting the original centerlines again.
 *
 * @par Provenance and credits
 * Ported from the vtkvmtkPolyDataCenterlineSections class of SimVascular
 * (https://github.com/SimVascular/SimVascular, Code/ThirdParty/vmtk/simvascular_vmtk, as of commit
 * 344727f3 of October 2025), SimVascular's extension of the VMTK class of the same name, which
 * computes the centerlines, branches and bifurcations used to generate 0D and 1D models, written by
 * Martin R. Pfaller, with changes by Luca Pegolotti and David W. Parker. The framework that uses it
 * is described in M. R. Pfaller, J. Pham, A. Verma, L. Pegolotti, N. M. Wilson, D. W. Parker,
 * W. Yang, A. L. Marsden, "Automated generation of 0D and 1D reduced-order models of
 * patient-specific blood flow", International Journal for Numerical Methods in Biomedical
 * Engineering, 38(10), e3639, 2022, https://doi.org/10.1002/cnm.3639. Compared with SimVascular's
 * implementation, it is a subclass of vtkvmtkPolyDataCenterlineSections instead of a replacement,
 * labels of the surface are copied to sections without interpolation, surface normals are always
 * computed with a consistent orientation, branch and bifurcation ids are kept with VTK 9.4 and
 * later, errors are reported through VTK, and progress is reported; branch and bifurcation ids,
 * paths, section normals and surface labels are otherwise the same.
 *
 * @sa
 * vtkvmtkPolyDataCenterlineSections, vtkvmtkPolyDataBranchSections, vtkvmtkPolyDataCenterlines
 */

#ifndef __vtkvmtkPolyDataCenterlineBranchSplitting_h
#define __vtkvmtkPolyDataCenterlineBranchSplitting_h

#include "vtkvmtkPolyDataCenterlineSections.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineBranchSplitting : public vtkvmtkPolyDataCenterlineSections
{
  public:
  vtkTypeMacro(vtkvmtkPolyDataCenterlineBranchSplitting,vtkvmtkPolyDataCenterlineSections);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataCenterlineBranchSplitting* New();

  /**
   * Get the input surface with point normals and the branch and bifurcation id of each point, after
   * the filter has been updated.
   */
  vtkGetObjectMacro(Surface,vtkPolyData);

  ///@{
  /**
   * Set/Get the name of the point data array of the input centerlines holding the maximum inscribed
   * sphere radius at each point. Default: "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data arrays of the output centerlines and surface holding the
   * branch id of each point, -1 for points in a bifurcation region. Default: "BranchId".
   */
  vtkSetStringMacro(BranchIdArrayName);
  vtkGetStringMacro(BranchIdArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data arrays of the output centerlines and surface holding the
   * bifurcation id of each point, -1 for points in a branch. Default: "BifurcationId".
   */
  vtkSetStringMacro(BifurcationIdArrayName);
  vtkGetStringMacro(BifurcationIdArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the output centerlines holding the distance of each
   * point along its branch or bifurcation from its first point. Default: "Path".
   */
  vtkSetStringMacro(PathArrayName);
  vtkGetStringMacro(PathArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the output centerlines that has one component for
   * each input centerline, 1 if that centerline passes through the point and 0 otherwise.
   * Default: "CenterlineId".
   */
  vtkSetStringMacro(CenterlineIdArrayName);
  vtkGetStringMacro(CenterlineIdArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the output centerlines holding the id of each point,
   * and of the output cell data array holding the id of the centerline point each section was cut
   * at. Default: "GlobalNodeId".
   */
  vtkSetStringMacro(GlobalNodeIdArrayName);
  vtkGetStringMacro(GlobalNodeIdArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the output centerlines holding the unit normal of
   * the section at each point, the centerline tangent. Default: "CenterlineSectionNormal".
   */
  vtkSetStringMacro(CenterlineSectionNormalArrayName);
  vtkGetStringMacro(CenterlineSectionNormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array and of the point data array of the output
   * centerlines holding, for each section, whether it is in a bifurcation region (1) or in a single
   * branch (0). Default: "CenterlineSectionBifurcation".
   */
  vtkSetStringMacro(CenterlineSectionBifurcationArrayName);
  vtkGetStringMacro(CenterlineSectionBifurcationArrayName);
  ///@}

  protected:
  vtkvmtkPolyDataCenterlineBranchSplitting();
  ~vtkvmtkPolyDataCenterlineBranchSplitting();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  bool ComputeSectionPlane(vtkIdType cellId, vtkIdType cellPointIndex, double origin[3], double normal[3]) override;
  void ExtractSection(vtkPolyData* input, double origin[3], double normal[3], vtkPolyData* section, bool & closed) override;
  void ProcessSection(vtkPolyData* input, vtkIdType pointId, vtkIdType sectionId, vtkPolyData* section, double origin[3], double normal[3], bool closed) override;

  /**
   * Merge the input centerlines into a tree of two-point lines, numbered from the inlet, with a
   * temporary branch and bifurcation classification of each point, and smooth each branch.
   */
  bool GenerateCleanCenterlines();

  /**
   * Compute the unit tangent at each centerline point. At caps, the normal of the nearest surface
   * point is used, pointing into the tree at the inlet and out of it at the outlets.
   */
  void ComputeTangents();

  /**
   * Insert a point halfway between each cap and its neighbor.
   */
  bool RefineCapPoints();

  /**
   * Label each surface point with the id in the centerline point data array labelArrayName of the
   * nearest centerline point in front of it for which the array otherArrayName is -1.
   */
  bool LabelSurface(const char* labelArrayName, const char* otherArrayName);

  /**
   * Correct the bifurcation classification of the sections: bifurcation regions touching a single
   * branch are removed, and single points left between bifurcations and next to caps are
   * reclassified.
   */
  bool CleanBifurcations();

  /**
   * Split the centerlines into branches and bifurcations, number them and compute the path along
   * each.
   */
  bool GroupCenterlines();

  /**
   * Extract the cells of the centerlines in bifurcation regions and in branches.
   */
  void SplitCenterlines(vtkPolyData* bifurcations, vtkPolyData* branches);

  /**
   * Number the connected regions of lines in labelArrayName, set otherArrayName to -1 and compute
   * the path along each region.
   */
  void LabelConnectedRegions(vtkPolyData* lines, const char* labelArrayName, const char* otherArrayName);

  vtkPolyData* Surface;

  char* RadiusArrayName;
  char* BranchIdArrayName;
  char* BifurcationIdArrayName;
  char* PathArrayName;
  char* CenterlineIdArrayName;
  char* GlobalNodeIdArrayName;
  char* CenterlineSectionNormalArrayName;
  char* CenterlineSectionBifurcationArrayName;

  int NumberOfCenterlines;

  private:
  vtkvmtkPolyDataCenterlineBranchSplitting(const vtkvmtkPolyDataCenterlineBranchSplitting&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineBranchSplitting&);  // Not implemented.

  class vtkInternals;
  vtkInternals* Internals;
};

#endif
