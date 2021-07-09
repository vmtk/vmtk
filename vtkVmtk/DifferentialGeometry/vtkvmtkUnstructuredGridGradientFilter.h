/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridGradientFilter.h,v $
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
// .NAME vtkvmtkUnstructuredGridGradientFilter - Compute the gradient of data stored within an unstructured grid mesh.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridGradientFilter_h
#define __vtkvmtkUnstructuredGridGradientFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridGradientFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridGradientFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridGradientFilter,vtkUnstructuredGridAlgorithm);

  vtkSetStringMacro(InputArrayName);
  vtkGetStringMacro(InputArrayName);

  vtkSetStringMacro(GradientArrayName);
  vtkGetStringMacro(GradientArrayName);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);

protected:
  vtkvmtkUnstructuredGridGradientFilter();
  ~vtkvmtkUnstructuredGridGradientFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* InputArrayName;
  char* GradientArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;
  int ComputeIndividualPartialDerivatives;

private:
  vtkvmtkUnstructuredGridGradientFilter(const vtkvmtkUnstructuredGridGradientFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridGradientFilter&);  // Not implemented.
};

#endif

