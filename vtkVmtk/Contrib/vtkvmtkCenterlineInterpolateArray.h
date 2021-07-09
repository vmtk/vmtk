/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCenterlineInterpolateArray.h,v $
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
// .NAME vtkvmtkInterpolateCenterlineArray - Interpolate a point-based array from a set of provided values.
// .SECTION Description
// ..

#ifndef __vtkvmtkCenterlineInterpolateArray_h
#define __vtkvmtkCenterlineInterpolateArray_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkDataArray.h"
#include "vtkIdList.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkCenterlineInterpolateArray : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkCenterlineInterpolateArray* New();
  vtkTypeMacro(vtkvmtkCenterlineInterpolateArray,vtkPolyDataAlgorithm);

  //Description:
  //Default value to fill in when no data is available
  vtkSetMacro(DefaultValue,double);
  vtkGetMacro(DefaultValue,double);

  //Description:
  //Set/Get the values from which to interpolate
  vtkSetObjectMacro(Values,vtkDataArray);
  vtkGetObjectMacro(Values,vtkDataArray);

  //Description:
  //Set/Get the point ids corresponding to the provided values
  vtkSetObjectMacro(ValuesIds,vtkIdList);
  vtkGetObjectMacro(ValuesIds,vtkIdList);

  //Description:
  //Set/Get the name of the resulting array
  vtkSetStringMacro(InterpolatedArrayName);
  vtkGetStringMacro(InterpolatedArrayName);
  

protected:
  vtkvmtkCenterlineInterpolateArray();
  ~vtkvmtkCenterlineInterpolateArray();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  
  char *InterpolatedArrayName;

  double DefaultValue;
  vtkDataArray *Values;
  vtkIdList *ValuesIds;

private:
  vtkvmtkCenterlineInterpolateArray(const vtkvmtkCenterlineInterpolateArray&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineInterpolateArray&);  // Not implemented.
};

#endif

