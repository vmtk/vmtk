/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkConstrainedLaplacianPolyDataFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .NAME vtkvmtkConstrainedLaplacianPolyDataFilter - smooth a surface mesh by constraining the motion of selected point ids. 
// .SECTION Description
// vtkvmtkConstrainedLaplacianPolyDataFilter

#ifndef __vtkvmtkConstrainedLaplacianPolyDataFilter_h
#define __vtkvmtkConstrainedLaplacianPolyDataFilter_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkIdList;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkConstrainedLaplacianPolyDataFilter : public vtkPolyDataToPolyDataFilter
{
  public: 
  vtkTypeMacro(vtkvmtkConstrainedLaplacianPolyDataFilter,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkConstrainedLaplacianPolyDataFilter *New();

  vtkSetMacro(Convergence, double);
  vtkGetMacro(Convergence, double);

  vtkSetMacro(RelaxationFactor, double);
  vtkGetMacro(RelaxationFactor, double);

  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  vtkSetMacro(BoundarySmoothing, int);
  vtkGetMacro(BoundarySmoothing, int);
  vtkBooleanMacro(BoundarySmoothing, int);

  virtual void SetConstrainedPointIds(vtkIdList *);
  vtkGetObjectMacro(ConstrainedPointIds, vtkIdList);

  protected:
  vtkvmtkConstrainedLaplacianPolyDataFilter();
  ~vtkvmtkConstrainedLaplacianPolyDataFilter();  

  void Execute();

  vtkIdList *ConstrainedPointIds;
  
  double Convergence;
  double RelaxationFactor;
  int NumberOfIterations;
  int BoundarySmoothing;

  private:
  vtkvmtkConstrainedLaplacianPolyDataFilter(const vtkvmtkConstrainedLaplacianPolyDataFilter&);  // Not implemented.
  void operator=(const vtkvmtkConstrainedLaplacianPolyDataFilter&);  // Not implemented.
};

#endif
