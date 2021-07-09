/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataHarmonicMappingFilter.h,v $
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
// .NAME vtkvmtkPolyDataHarmonicMappingFilter - Construct a harmonic function over each vascular surface segment used during mapping order to stretch the longitudinal metric to correctly account for the presence of insertion regions at bifurcations.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataHarmonicMappingFilter_h
#define __vtkvmtkPolyDataHarmonicMappingFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkPolyDataAlgorithm.h"

#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataHarmonicMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataHarmonicMappingFilter,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(BoundaryPointIds,vtkIdList);
  vtkGetObjectMacro(BoundaryPointIds,vtkIdList);

  vtkSetObjectMacro(BoundaryValues,vtkDoubleArray);
  vtkGetObjectMacro(BoundaryValues,vtkDoubleArray);

  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(AssemblyMode,int);
  vtkGetMacro(AssemblyMode,int);
  void SetAssemblyModeToStencils()
  { this->SetAssemblyMode(VTK_VMTK_ASSEMBLY_STENCILS); }
   void SetAssemblyModeToFiniteElements()
  { this->SetAssemblyMode(VTK_VMTK_ASSEMBLY_FINITEELEMENTS); }

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);


//BTX
  enum 
    {
    VTK_VMTK_ASSEMBLY_STENCILS,
    VTK_VMTK_ASSEMBLY_FINITEELEMENTS
    };
//ETX

protected:
  vtkvmtkPolyDataHarmonicMappingFilter();
  ~vtkvmtkPolyDataHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryPointIds;
  vtkDoubleArray* BoundaryValues;

  char* HarmonicMappingArrayName;
  double ConvergenceTolerance;
  int AssemblyMode;
  int QuadratureOrder;

private:
  vtkvmtkPolyDataHarmonicMappingFilter(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataHarmonicMappingFilter&);  // Not implemented.
};

#endif

