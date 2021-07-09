/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter.h,v $
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
// .NAME vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter - Wrap a harmonic function over a cylinders for every group within a surface.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter_h
#define __vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter,vtkPolyDataAlgorithm);

  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

protected:
  vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter();
  ~vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* HarmonicMappingArrayName;
  char* GroupIdsArrayName;

private:
  vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter(const vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter&);  // Not implemented.
};

#endif

