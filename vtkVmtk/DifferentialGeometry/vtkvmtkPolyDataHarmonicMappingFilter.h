/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataHarmonicMappingFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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
// .NAME vtkvmtkPolyDataHarmonicMappingFilter - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataHarmonicMappingFilter_h
#define __vtkvmtkPolyDataHarmonicMappingFilter_h

#include "vtkPolyDataToPolyDataFilter.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkIdList.h"
#include "vtkDoubleArray.h"
#include "vtkPolyDataAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataHarmonicMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataHarmonicMappingFilter* New();
  vtkTypeRevisionMacro(vtkvmtkPolyDataHarmonicMappingFilter,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(BoundaryPointIds,vtkIdList);
  vtkGetObjectMacro(BoundaryPointIds,vtkIdList);

  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);

  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

protected:
  vtkvmtkPolyDataHarmonicMappingFilter();
  ~vtkvmtkPolyDataHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  vtkIdList* BoundaryPointIds;
  vtkDoubleArray* BoundaryValues;

  char* HarmonicMappingArrayName;

  double ConvergenceTolerance;

private:
  vtkvmtkPolyDataHarmonicMappingFilter(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
};

#endif

