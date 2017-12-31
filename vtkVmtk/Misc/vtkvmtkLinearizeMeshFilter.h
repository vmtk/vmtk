/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearizeMeshFilter.h,v $
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
  // .NAME vtkvmtkLinearizeMeshFilter - Converts linear elements to quadratic.
  // .SECTION Description
  // ...

#ifndef __vtkvmtkLinearizeMeshFilter_h
#define __vtkvmtkLinearizeMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearizeMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearizeMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkLinearizeMeshFilter *New();
  
  vtkSetMacro(CleanOutput,int);
  vtkGetMacro(CleanOutput,int);
  vtkBooleanMacro(CleanOutput,int);

  protected:
  vtkvmtkLinearizeMeshFilter();
  ~vtkvmtkLinearizeMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  int CleanOutput;

  private:
  vtkvmtkLinearizeMeshFilter(const vtkvmtkLinearizeMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearizeMeshFilter&);  // Not implemented.
};

#endif
