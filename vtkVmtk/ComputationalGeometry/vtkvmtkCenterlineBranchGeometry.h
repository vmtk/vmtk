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
 * @class   vtkvmtkCenterlineBranchGeometry
 * @brief   Computes the length, curvature, torsion, and tortuosity for each branch within a split centerline.
 * @ingroup ComputationalGeometry
 *
 * The length, curvature, torsion, and tortuosity metrics are scalar quantities which are identical for each point / tract that makes up a branch. They are cell data which are attached to all cells in the branch.
 *
 * A laplacian smoothing filter can be applied to the line if the computation appears to be unstable (as we are using second derivatives and such here).
 *
 * Unlike vtkvmtkCenterlineGeometry (which computes point-wise differential geometry for raw,
 * unsplit centerlines), this class computes one aggregate value per branch (group of cells sharing
 * a GroupIdsArrayName value), optionally subsampling each branch's line to a coarser, more robust
 * point spacing before computing curvature/torsion. This is the filter behind the
 * vmtkbranchgeometry pype script, run after splitting/grouping.
 *
 * @sa
 * vtkvmtkCenterlineGeometry, vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkCenterlineBranchGeometry_h
#define __vtkvmtkCenterlineBranchGeometry_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBranchGeometry : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBranchGeometry,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineBranchGeometry* New();

  ///@{
  /**
   * Set/Get the name of the point data array of the input centerlines holding the maximum inscribed
   * sphere radius at each point. Required when SphereSubsampling is on.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
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
   * Set/Get the name of the cell data array of the input centerlines holding, for each cell, whether
   * it is a "blanked" (redundant, overlapping) tract -- blanked cells are excluded from the geometry
   * computation for each branch.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the arc length of each branch is stored.
   * Commonly named "Length".
   */
  vtkSetStringMacro(LengthArrayName);
  vtkGetStringMacro(LengthArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the (length-weighted average) curvature of
   * each branch is stored.
   * Commonly named "Curvature".
   */
  vtkSetStringMacro(CurvatureArrayName);
  vtkGetStringMacro(CurvatureArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the (length-weighted average) torsion of
   * each branch is stored.
   * Commonly named "Torsion".
   */
  vtkSetStringMacro(TorsionArrayName);
  vtkGetStringMacro(TorsionArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output cell data array where the tortuosity of each branch is stored,
   * computed as (branch length / chord length) - 1.
   * Commonly named "Tortuosity".
   */
  vtkSetStringMacro(TortuosityArrayName);
  vtkGetStringMacro(TortuosityArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the minimum point spacing enforced when LineSubsampling is on: points closer together
   * than this along a branch are merged before computing curvature/torsion, to stabilize the
   * second-derivative estimates. Default: 0.1.
   */
  vtkSetMacro(MinSubsamplingSpacing,double);
  vtkGetMacro(MinSubsamplingSpacing,double);
  ///@}

  ///@{
  /**
   * Set/Get the relaxation factor used by the optional Laplacian smoothing pass applied to each
   * branch's points before curvature/torsion are computed (see LineSmoothing). Default: 0.01.
   */
  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of Laplacian smoothing iterations applied to each branch's points before
   * curvature/torsion are computed (see LineSmoothing). Default: 100.
   */
  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);
  ///@}

  ///@{
  /**
   * Toggle Laplacian smoothing of each branch's points prior to computing curvature and torsion, to
   * stabilize the second-derivative estimates on noisy or piecewise-linear centerlines. Default: off.
   */
  vtkSetMacro(LineSmoothing,int);
  vtkGetMacro(LineSmoothing,int);
  vtkBooleanMacro(LineSmoothing,int);
  ///@}

  ///@{
  /**
   * Toggle subsampling of each branch's points to (approximately) MinSubsamplingSpacing before the
   * geometry computation. Default: on.
   */
  vtkSetMacro(LineSubsampling,int);
  vtkGetMacro(LineSubsampling,int);
  vtkBooleanMacro(LineSubsampling,int);
  ///@}

  ///@{
  /**
   * Toggle an alternative subsampling strategy that walks along each branch skipping ahead by the
   * local maximum inscribed sphere radius (from RadiusArrayName) at each step, rather than a fixed
   * spacing. Default: on.
   */
  vtkSetMacro(SphereSubsampling,int);
  vtkGetMacro(SphereSubsampling,int);
  vtkBooleanMacro(SphereSubsampling,int);
  ///@}

  protected:
  vtkvmtkCenterlineBranchGeometry();
  ~vtkvmtkCenterlineBranchGeometry();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double ComputeGroupLength(vtkPolyData* input, int branchGroupId);
  double ComputeGroupCurvature(vtkPolyData* input, int branchGroupId);
  double ComputeGroupTorsion(vtkPolyData* input, int branchGroupId);
  double ComputeGroupTortuosity(vtkPolyData* input, int branchGroupId, double groupLength);

  static void SubsampleLine(vtkPoints* linePoints, vtkPoints* subsampledLinePoints, double minSpacing = 0.01);
  void SphereSubsampleLine(vtkPolyData* input, vtkIdType cellId, vtkPoints* subsampledLinePoints);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* BlankingArrayName;

  char* LengthArrayName;
  char* CurvatureArrayName;
  char* TorsionArrayName;
  char* TortuosityArrayName;

  int LineSubsampling;
  int SphereSubsampling;
  int LineSmoothing;

  double MinSubsamplingSpacing;
  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineBranchGeometry(const vtkvmtkCenterlineBranchGeometry&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBranchGeometry&);  // Not implemented.
};

#endif
