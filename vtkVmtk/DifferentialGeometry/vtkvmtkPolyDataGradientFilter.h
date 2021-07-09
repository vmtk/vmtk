/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataGradientFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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
// .NAME vtkvmtkPolyDataGradientFilter - Compute the gradient of data stored within an polydata surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataGradientFilter_h
#define __vtkvmtkPolyDataGradientFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPolyDataAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataGradientFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataGradientFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataGradientFilter,vtkPolyDataAlgorithm);

  vtkSetStringMacro(InputArrayName);
  vtkGetStringMacro(InputArrayName);

  vtkSetStringMacro(GradientArrayName);
  vtkGetStringMacro(GradientArrayName);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

protected:
  vtkvmtkPolyDataGradientFilter();
  ~vtkvmtkPolyDataGradientFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* InputArrayName;
  char* GradientArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkPolyDataGradientFilter(const vtkvmtkPolyDataGradientFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataGradientFilter&);  // Not implemented.
};

#endif

