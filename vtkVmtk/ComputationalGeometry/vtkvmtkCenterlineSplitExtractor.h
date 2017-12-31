/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSplitExtractor.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCenterlineSplitExtractor - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineSplitExtractor_h
#define __vtkvmtkCenterlineSplitExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSplitExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSplitExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

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

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) VTK_OVERRIDE;
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
