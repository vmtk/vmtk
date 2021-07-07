/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataToUnstructuredGridFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataToUnstructuredGridFilter - create a mesh type object from a surface type object
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
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataToUnstructuredGridFilter *New();

protected:
  vtkvmtkPolyDataToUnstructuredGridFilter() {}
  ~vtkvmtkPolyDataToUnstructuredGridFilter() {}

  int FillInputPortInformation(int, vtkInformation *info) override;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  
private:
  vtkvmtkPolyDataToUnstructuredGridFilter(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
};

#endif


