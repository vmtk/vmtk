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
 * @class   vtkvmtkCenterlineSplitExtractor
 * @brief   Split a centerline into three tracts with the middle (blanked) tract being initialized between any two points on the centerline or being an arbitrary length starting at a particular point.
 * @ingroup ComputationalGeometry
 *
 * This is useful if you want to split a surface at an arbitrary location along a centerline in later processing.
 *
 * This is the filter behind the vmtkpointsplitextractor pype script. Two splitting strategies are
 * available (see SplittingMode): POINTANDGAP splits at the point on the centerline closest to
 * SplitPoint, opening a blanked gap of length GapLength around it; BETWEENPOINTS splits into a
 * blanked middle tract running between the points closest to SplitPoint and SplitPoint2.
 *
 * @sa
 * vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkCenterlineSplitExtractor_h
#define __vtkvmtkCenterlineSplitExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSplitExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSplitExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineSplitExtractor *New();

  ///@{
  /**
   * Set/Get the (x,y,z) location used to find the primary splitting point on the centerline (the
   * closest centerline point to SplitPoint is used). In POINTANDGAP mode this is the sole split
   * location; in BETWEENPOINTS mode it is one end of the blanked middle tract. Default: (0,0,0).
   */
  vtkSetVectorMacro(SplitPoint,double,3);
  vtkGetVectorMacro(SplitPoint,double,3);
  ///@}

  ///@{
  /**
   * Set/Get the distance tolerance used when locating centerline points coincident with SplitPoint/
   * SplitPoint2. Default: 1E-4.
   */
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);
  ///@}

  ///@{
  /**
   * Set/Get, in POINTANDGAP mode, the length of the blanked gap tract opened up around the split
   * point. Default: 1.0.
   */
  vtkSetMacro(GapLength,double);
  vtkGetMacro(GapLength,double);
  ///@}

  ///@{
  /**
   * Set/Get, in BETWEENPOINTS mode, the (x,y,z) location used to find the other end of the blanked
   * middle tract (the closest centerline point to SplitPoint2 is used). Default: (0,0,0).
   */
  vtkSetVectorMacro(SplitPoint2,double,3);
  vtkGetVectorMacro(SplitPoint2,double,3);
  ///@}

  ///@{
  /**
   * Set/Get the splitting strategy: POINTANDGAP (default) splits at SplitPoint, opening a blanked
   * gap of length GapLength around it; BETWEENPOINTS splits into a blanked middle tract running
   * between SplitPoint and SplitPoint2. See also SetSplittingModeToPointAndGap /
   * SetSplittingModeToBetweenPoints.
   */
  vtkSetMacro(SplittingMode,int);
  vtkGetMacro(SplittingMode,int);
  ///@}

  /**
   * Convenience method: set SplittingMode to split at SplitPoint with a GapLength-long gap (default).
   */
  void SetSplittingModeToPointAndGap()
  { this->SetSplittingMode(POINTANDGAP); }

  /**
   * Convenience method: set SplittingMode to split into a blanked tract between SplitPoint and
   * SplitPoint2.
   */
  void SetSplittingModeToBetweenPoints()
  { this->SetGroupingMode(BETWEENPOINTS); }

//BTX
  /**
   * Values for SplittingMode: strategy used to locate the split.
   */
  enum {
    POINTANDGAP,
    BETWEENPOINTS
  };
//ETX

  protected:
  vtkvmtkCenterlineSplitExtractor();
  ~vtkvmtkCenterlineSplitExtractor();  

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) override;
  virtual void ComputePointAndGapCenterlineSplitting(vtkPolyData* input, vtkIdType cellId);
  virtual void ComputeBetweenPointsCenterlineSplitting(vtkPolyData* input, vtkIdType cellId);

  double SplitPoint[3];
  double SplitPoint2[3];
  double Tolerance;
  double GapLength;

  int SplittingMode;

  private:
  vtkvmtkCenterlineSplitExtractor(const vtkvmtkCenterlineSplitExtractor&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSplitExtractor&);  // Not implemented.
};

#endif
