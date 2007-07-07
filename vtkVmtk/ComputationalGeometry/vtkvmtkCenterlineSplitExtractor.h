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
  vtkTypeRevisionMacro(vtkvmtkCenterlineSplitExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkCenterlineSplitExtractor *New();

  vtkSetVectorMacro(SplitPoint,double,3);
  vtkGetVectorMacro(SplitPoint,double,3);

  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  vtkSetMacro(GapLength,double);
  vtkGetMacro(GapLength,double);

  protected:
  vtkvmtkCenterlineSplitExtractor();
  ~vtkvmtkCenterlineSplitExtractor();  

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId);

  double SplitPoint[3];
  double Tolerance;
  double GapLength;

  private:
  vtkvmtkCenterlineSplitExtractor(const vtkvmtkCenterlineSplitExtractor&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSplitExtractor&);  // Not implemented.
};

#endif
