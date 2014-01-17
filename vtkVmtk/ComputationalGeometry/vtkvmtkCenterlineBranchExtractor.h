/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBranchExtractor.h,v $
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
  // .NAME vtkvmtkCenterlineBranchExtractor - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineBranchExtractor_h
#define __vtkvmtkCenterlineBranchExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBranchExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBranchExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkCenterlineBranchExtractor *New();
  
  protected:
  vtkvmtkCenterlineBranchExtractor();
  ~vtkvmtkCenterlineBranchExtractor();  

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId);
  virtual void GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts);

  private:
  vtkvmtkCenterlineBranchExtractor(const vtkvmtkCenterlineBranchExtractor&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBranchExtractor&);  // Not implemented.
};

#endif
