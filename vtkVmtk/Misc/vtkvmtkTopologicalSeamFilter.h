/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkTopologicalSeamFilter.h,v $
Language:  C++
Date:      $Date: 2015/12/01 12:26:27 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkTopologicalSeamFilter - ...
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
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  double ClosestPoint[3];
  char* SeamScalarsArrayName;
  vtkImplicitFunction* SeamFunction;

  private:
  vtkvmtkTopologicalSeamFilter(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
  void operator=(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
};

#endif
