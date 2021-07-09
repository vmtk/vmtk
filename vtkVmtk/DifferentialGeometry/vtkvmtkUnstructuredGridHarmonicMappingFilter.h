/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridHarmonicMappingFilter.h,v $
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
// .NAME vtkvmtkUnstructuredGridHarmonicMappingFilter - Construct a harmonic function over each vascular mesh segment used during mapping order to stretch the longitudinal metric to correctly account for the presence of insertion regions at bifurcations.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridHarmonicMappingFilter_h
#define __vtkvmtkUnstructuredGridHarmonicMappingFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridHarmonicMappingFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridHarmonicMappingFilter,vtkUnstructuredGridAlgorithm);

  vtkSetObjectMacro(BoundaryPointIds,vtkIdList);
  vtkGetObjectMacro(BoundaryPointIds,vtkIdList);

  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);

  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

protected:
  vtkvmtkUnstructuredGridHarmonicMappingFilter();
  ~vtkvmtkUnstructuredGridHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryPointIds;
  vtkDoubleArray* BoundaryValues;

  char* HarmonicMappingArrayName;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkUnstructuredGridHarmonicMappingFilter(const vtkvmtkUnstructuredGridHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridHarmonicMappingFilter&);  // Not implemented.
};

#endif

