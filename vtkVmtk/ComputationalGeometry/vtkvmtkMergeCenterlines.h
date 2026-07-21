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
 * @class   vtkvmtkMergeCenterlines
 * @brief   Combine multiple centerlines which lie within the same branch of a split and grouped centerline.
 * @ingroup ComputationalGeometry
 *
 * If multiple centerlines exist within the same branch, each centerlines point locations are averaged (euclidean interpolation) to result in a branch with only one centerline.
 *
 * Takes as input split, grouped centerlines (see vtkvmtkCenterlineSplittingAndGroupingFilter) and
 * outputs one merged, resampled line per branch group. This is the filter behind the
 * vmtkcenterlinemerge pype script.
 *
 * @sa
 * vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkMergeCenterlines_h
#define __vtkvmtkMergeCenterlines_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkMergeCenterlines : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMergeCenterlines,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMergeCenterlines *New();
  
  ///@{
  /**
   * Set/Get the name of the point data array of the input centerlines holding the maximum inscribed
   * sphere radius at each point. Required input.
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
   * Set/Get the name of the cell data array of the input centerlines holding the id of the original,
   * unsplit centerline that each cell belongs to.
   * Commonly named "CenterlineIds".
   */
  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding the tract id of each
   * cell.
   * Commonly named "TractIds".
   */
  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the input centerlines holding, for each cell, whether
   * it is a "blanked" (redundant, overlapping) tract.
   * Commonly named "Blanking".
   */
  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the point spacing used to resample each merged branch line. If left at its default
   * (0.0), no resampling is performed and the merged line uses the union of all input point
   * abscissas.
   */
  vtkSetMacro(ResamplingStepLength,double);
  vtkGetMacro(ResamplingStepLength,double);
  ///@}

  ///@{
  /**
   * Toggle whether blanked groups (see BlankingArrayName) are also merged and included in the
   * output, in addition to non-blanked branch groups. Default: on.
   */
  vtkSetMacro(MergeBlanked,int);
  vtkGetMacro(MergeBlanked,int);
  vtkBooleanMacro(MergeBlanked,int);
  ///@}

  protected:
  vtkvmtkMergeCenterlines();
  ~vtkvmtkMergeCenterlines();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* TractIdsArrayName;
  char* BlankingArrayName;

  double ResamplingStepLength;
  int MergeBlanked;

  private:
  vtkvmtkMergeCenterlines(const vtkvmtkMergeCenterlines&);  // Not implemented.
  void operator=(const vtkvmtkMergeCenterlines&);  // Not implemented.
};

#endif
