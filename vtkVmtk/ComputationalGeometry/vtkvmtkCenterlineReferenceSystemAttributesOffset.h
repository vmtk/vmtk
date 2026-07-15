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
 * @class   vtkvmtkCenterlineReferenceSystemAttributesOffset
 * @brief   Move the abscissa metric such that its zero point is at a bifurcation center and rotate parallel transport normals such that they are normal to the bifurcation plane.
 * @ingroup ComputationalGeometry
 *
 * Abscissa to be set to 0 at the bifurcation. We twist the entire parallel transport reference framing (rigidly) around the centerline so that the parallel transport normals are normal to the bifurcation plane at the bifurcation. this is useful for analyses that may be conducted later on comparing vessel torsion to the parallel transport normal
 *
 * Takes as input centerlines already carrying abscissa/parallel-transport-normal attributes (see
 * vtkvmtkCenterlineAttributesFilter) and a set of bifurcation reference systems (see
 * vtkvmtkCenterlineBifurcationReferenceSystems); this is the filter behind the
 * vmtkcenterlineoffsetattributes pype script.
 *
 * @sa
 * vtkvmtkCenterlineAttributesFilter, vtkvmtkCenterlineBifurcationReferenceSystems
 */

#ifndef __vtkvmtkCenterlineReferenceSystemAttributesOffset_h
#define __vtkvmtkCenterlineReferenceSystemAttributesOffset_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineReferenceSystemAttributesOffset : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineReferenceSystemAttributesOffset,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineReferenceSystemAttributesOffset* New();

  ///@{
  /**
   * Set/Get the name of the output point data array where the offset abscissa is stored. May be the
   * same as AbscissasArrayName to overwrite it in place, or a different name to preserve the
   * original alongside the offset one.
   * Commonly named "Abscissas".
   */
  vtkSetStringMacro(OffsetAbscissasArrayName);
  vtkGetStringMacro(OffsetAbscissasArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output 3-component point data array where the rotated (offset) parallel
   * transport normals are stored. May be the same as NormalsArrayName to overwrite it in place, or a
   * different name to preserve the original alongside the offset one.
   * Commonly named "ParallelTransportNormals", "OffsetNormals", "Normals", or "IoletNormals", depending on context.
   */
  vtkSetStringMacro(OffsetNormalsArrayName);
  vtkGetStringMacro(OffsetNormalsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input point data array holding the (unoffset) centerline abscissa, as
   * produced by vtkvmtkCenterlineAttributesFilter. Required input.
   * Commonly named "Abscissas".
   */
  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the input 3-component point data array holding the (unrotated) parallel
   * transport normals, as produced by vtkvmtkCenterlineAttributesFilter. Required input.
   * Commonly named "ParallelTransportNormals".
   */
  vtkSetStringMacro(NormalsArrayName);
  vtkGetStringMacro(NormalsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding the branch group id of
   * each cell. Required input; see vtkvmtkCenterlineSplittingAndGroupingFilter.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding the id of the original,
   * unsplit centerline that each cell belongs to.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the bifurcation reference systems (one vertex point per bifurcation, with a normal point
   * data array) computed by vtkvmtkCenterlineBifurcationReferenceSystems. Required input.
   */
  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the bifurcation plane
   * normal vector that the parallel transport normals are rotated to align with.
   * Commonly named "Normal".
   */
  vtkSetStringMacro(ReferenceSystemsNormalArrayName);
  vtkGetStringMacro(ReferenceSystemsNormalArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the group id that each
   * reference system (bifurcation) refers to.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(ReferenceSystemsGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemsGroupIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the group id of the bifurcation reference system to offset to. Default: 0. If explicitly
   * set to -1, the first reference system found in ReferenceSystems is used automatically, and the
   * resolved group id can be retrieved afterward with GetReferenceGroupId.
   */
  vtkSetMacro(ReferenceGroupId,int);
  vtkGetMacro(ReferenceGroupId,int);
  ///@}

  protected:
  vtkvmtkCenterlineReferenceSystemAttributesOffset();
  ~vtkvmtkCenterlineReferenceSystemAttributesOffset();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* OffsetAbscissasArrayName;
  char* OffsetNormalsArrayName;

  char* AbscissasArrayName;
  char* NormalsArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;

  vtkPolyData* ReferenceSystems;

  char* ReferenceSystemsNormalArrayName;
  char* ReferenceSystemsGroupIdsArrayName;

  int ReferenceGroupId;

  private:
  vtkvmtkCenterlineReferenceSystemAttributesOffset(const vtkvmtkCenterlineReferenceSystemAttributesOffset&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineReferenceSystemAttributesOffset&);  // Not implemented.
};

#endif
