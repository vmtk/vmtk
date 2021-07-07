/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSplitExtractor.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineSplitExtractor - Split a centerline into three tracts with the middle (blanked) tract being initialized between any two points on the centerline or being an arbitrary length starting at a particular point.  
// .SECTION Description
// This is useful if you want to split a surface at an arbitrary location along a centerline in later processing. 

#ifndef __vtkvmtkCenterlineSplitExtractor_h
#define __vtkvmtkCenterlineSplitExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSplitExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSplitExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineSplitExtractor *New();

  vtkSetVectorMacro(SplitPoint,double,3);
  vtkGetVectorMacro(SplitPoint,double,3);

  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  vtkSetMacro(GapLength,double);
  vtkGetMacro(GapLength,double);

  vtkSetVectorMacro(SplitPoint2,double,3);
  vtkGetVectorMacro(SplitPoint2,double,3);

  vtkSetMacro(SplittingMode,int);
  vtkGetMacro(SplittingMode,int);
  void SetSplittingModeToPointAndGap()
  { this->SetSplittingMode(POINTANDGAP); }
  void SetSplittingModeToBetweenPoints()
  { this->SetGroupingMode(BETWEENPOINTS); }

//BTX
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
