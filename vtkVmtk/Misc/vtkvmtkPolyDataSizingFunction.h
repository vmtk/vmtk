/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataSizingFunction.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataSizingFunction - constructs a sizing function for volume meshing on the basis of input surface
// .SECTION Description
// ...

#ifndef __vtkvmtkPolyDataSizingFunction_h
#define __vtkvmtkPolyDataSizingFunction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataSizingFunction : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataSizingFunction,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataSizingFunction *New();
  
  vtkSetStringMacro(SizingFunctionArrayName);
  vtkGetStringMacro(SizingFunctionArrayName);

  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  protected:
  vtkvmtkPolyDataSizingFunction();
  ~vtkvmtkPolyDataSizingFunction();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* SizingFunctionArrayName;
  double ScaleFactor;

  private:
  vtkvmtkPolyDataSizingFunction(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
};

#endif
