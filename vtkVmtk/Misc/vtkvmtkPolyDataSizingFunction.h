/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataSizingFunction.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
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
  // .NAME vtkvmtkPolyDataSizingFunction - Add caps to boundaries.
  // .SECTION Description
  // This class constructs a sizing function for volume meshing on the basis 
  // on the input surface.

#ifndef __vtkvmtkPolyDataSizingFunction_h
#define __vtkvmtkPolyDataSizingFunction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataSizingFunction : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataSizingFunction,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkPolyDataSizingFunction *New();
  
  vtkSetStringMacro(SizingFunctionArrayName);
  vtkGetStringMacro(SizingFunctionArrayName);

  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  protected:
  vtkvmtkPolyDataSizingFunction();
  ~vtkvmtkPolyDataSizingFunction();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  char* SizingFunctionArrayName;
  double ScaleFactor;

  private:
  vtkvmtkPolyDataSizingFunction(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
};

#endif
