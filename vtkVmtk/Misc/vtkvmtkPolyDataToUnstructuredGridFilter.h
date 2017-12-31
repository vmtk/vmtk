/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataToUnstructuredGridFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkPolyDataToUnstructuredGridFilter - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkPolyDataToUnstructuredGridFilter_h
#define __vtkvmtkPolyDataToUnstructuredGridFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataToUnstructuredGridFilter : public vtkUnstructuredGridAlgorithm
{
public:
  vtkTypeMacro(vtkvmtkPolyDataToUnstructuredGridFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static vtkvmtkPolyDataToUnstructuredGridFilter *New();

protected:
  vtkvmtkPolyDataToUnstructuredGridFilter() {}
  ~vtkvmtkPolyDataToUnstructuredGridFilter() {}

  int FillInputPortInformation(int, vtkInformation *info) VTK_OVERRIDE;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
  
private:
  vtkvmtkPolyDataToUnstructuredGridFilter(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
};

#endif


