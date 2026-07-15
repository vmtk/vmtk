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
 * @class   vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter
 * @brief   Compute a per-branch-boundary longitudinal (abscissa) Dirichlet condition, expressed in the reference system frame, for downstream harmonic mapping.
 * @ingroup ComputationalGeometry
 *
 * Given a branched surface (GroupIdsArrayName), its centerlines and the branch reference systems
 * (typically from vtkvmtkCenterlineBranchExtractor / vtkvmtkCenterlineBifurcationReferenceSystems),
 * this filter computes, for every branch, a longitudinal metric value to assign to each of its two
 * open boundaries and writes it into BoundaryMetricArrayName. If a boundary sits at a bifurcation, the
 * assigned value is the radius-squared-weighted average centerline abscissa of the reference system
 * origin over all centerlines that cross that bifurcation; if the boundary is a free end (no adjacent
 * bifurcation), the extremal centerline abscissa of the branch itself is used instead. Each branch
 * must be topologically a cylinder (exactly two open boundaries). The resulting point data array is
 * used as the boundary condition for vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter, followed by
 * vtkvmtkPolyDataStretchMappingFilter, in the vmtkbranchmapping pype script, which builds a
 * longitudinal/circular parametrization of the surface for patching and statistical mapping.
 *
 * @sa
 * vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter, vtkvmtkPolyDataStretchMappingFilter,
 * vtkvmtkReferenceSystemUtilities, vtkvmtkPolyDataBranchUtilities
 */

#ifndef __vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter_h
#define __vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class vtkDataArray;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/Get the name of the output point data array where the computed boundary metric (longitudinal
   * abscissa value assigned to each branch boundary) is stored.
   * Commonly named "BoundaryMetric".
   */
  vtkSetStringMacro(BoundaryMetricArrayName);
  vtkGetStringMacro(BoundaryMetricArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the input surface holding the branch group id of each
   * point. Required input.
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
   * Set/Get the name of the point data array of Centerlines holding the longitudinal abscissa at each
   * point, used to compute the boundary metric value.
   * Commonly named "Abscissas".
   */
  vtkSetStringMacro(CenterlineAbscissasArrayName);
  vtkGetStringMacro(CenterlineAbscissasArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point, used to weight the averaging of reference system abscissas at a
   * bifurcation.
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
   * centerline that each cell belongs to. Used, together with CenterlineTractIdsArrayName, to find
   * the branch groups adjacent to each bifurcation.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of Centerlines holding the tract id of each cell. Used,
   * together with CenterlineIdsArrayName, to find the branch groups adjacent to each bifurcation.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the poly data whose points are the origins of the per-branch reference systems (typically
   * the output of vtkvmtkCenterlineBranchExtractor / vtkvmtkCenterlineBifurcationReferenceSystems).
   * Required input.
   */
  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of ReferenceSystems holding the branch group id that
   * each reference system origin belongs to.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(ReferenceSystemGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemGroupIdsArrayName);
  ///@}

protected:
  vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter();
  ~vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* BoundaryMetricArrayName;

  char* GroupIdsArrayName;

  vtkPolyData* Centerlines;
  char* CenterlineAbscissasArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;

  vtkPolyData* ReferenceSystems;
  char* ReferenceSystemGroupIdsArrayName;

private:
  vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter(const vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter&);  // Not implemented.
};

#endif

