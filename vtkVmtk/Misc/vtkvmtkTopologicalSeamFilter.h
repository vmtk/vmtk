/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkTopologicalSeamFilter.h,v $
Language:  C++
Date:      $Date: 2015/12/01 12:26:27 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkTopologicalSeamFilter - Create a feature edge on a surface from a point and implicit plane function.
  // .SECTION Description
  // .

#ifndef __vtkvmtkTopologicalSeamFilter_h
#define __vtkvmtkTopologicalSeamFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkImplicitFunction.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkTopologicalSeamFilter : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkTopologicalSeamFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkTopologicalSeamFilter *New();

  vtkSetVectorMacro(ClosestPoint,double,3);
  vtkGetVectorMacro(ClosestPoint,double,3);

  vtkSetStringMacro(SeamScalarsArrayName);
  vtkGetStringMacro(SeamScalarsArrayName);

  vtkSetObjectMacro(SeamFunction,vtkImplicitFunction);
  vtkGetObjectMacro(SeamFunction,vtkImplicitFunction);

  protected:
  vtkvmtkTopologicalSeamFilter();
  ~vtkvmtkTopologicalSeamFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double ClosestPoint[3];
  char* SeamScalarsArrayName;
  vtkImplicitFunction* SeamFunction;

  private:
  vtkvmtkTopologicalSeamFilter(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
  void operator=(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
};

#endif
