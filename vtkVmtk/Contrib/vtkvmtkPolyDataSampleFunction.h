/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataSampleFunction.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
// .NAME vtkvmtkPolyDataSampleFunction - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataSampleFunction_h
#define __vtkvmtkPolyDataSampleFunction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkImplicitFunction.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataSampleFunction : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataSampleFunction* New();
  vtkTypeMacro(vtkvmtkPolyDataSampleFunction,vtkPolyDataAlgorithm);

  // Description:
  // Specify the implicit function to use to generate data.
  virtual void SetImplicitFunction(vtkImplicitFunction*);
  vtkGetObjectMacro(ImplicitFunction,vtkImplicitFunction);

  vtkSetStringMacro(SampleArrayName);
  vtkGetStringMacro(SampleArrayName);

protected:
  vtkvmtkPolyDataSampleFunction();
  ~vtkvmtkPolyDataSampleFunction();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* SampleArrayName;

  vtkImplicitFunction *ImplicitFunction;


private:
  vtkvmtkPolyDataSampleFunction(const vtkvmtkPolyDataSampleFunction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSampleFunction&);  // Not implemented.
};

#endif

