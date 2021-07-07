/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAppendFilter.h,v $
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
// .NAME vtkvmtkAppendFilter - merges points, point data, and cell data.
// .SECTION Description
// The RequestData method implementation is heavily based on the same method in vtkAppendFilter.
// vtkAppendFilter is covered by the following (VTK) copyright notice.
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkAppendFilter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkvmtkAppendFilter_h
#define __vtkvmtkAppendFilter_h

#include "vtkAppendFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkDataSetCollection;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkAppendFilter : public vtkAppendFilter
{
public:
  static vtkvmtkAppendFilter *New();

  vtkTypeMacro(vtkvmtkAppendFilter,vtkAppendFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(MergeDuplicatePoints,int);
  vtkGetMacro(MergeDuplicatePoints,int);
  vtkBooleanMacro(MergeDuplicatePoints,int);

protected:
  vtkvmtkAppendFilter();
  ~vtkvmtkAppendFilter();

  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int MergeDuplicatePoints;

private:
  vtkvmtkAppendFilter(const vtkvmtkAppendFilter&);  // Not implemented.
  void operator=(const vtkvmtkAppendFilter&);  // Not implemented.
};

#endif
