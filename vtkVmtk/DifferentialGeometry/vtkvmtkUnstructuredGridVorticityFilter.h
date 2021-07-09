/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridVorticityFilter.h,v $
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
// .NAME vtkvmtkUnstructuredGridVorticityFilter - Calculates vorticity from velocity in a mesh.
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridVorticityFilter_h
#define __vtkvmtkUnstructuredGridVorticityFilter_h

#include "vtkvmtkWin32Header.h"
#include "vtkUnstructuredGridAlgorithm.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridVorticityFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridVorticityFilter* New();
  vtkTypeMacro(vtkvmtkUnstructuredGridVorticityFilter,vtkUnstructuredGridAlgorithm);

  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);

  vtkSetStringMacro(VorticityArrayName);
  vtkGetStringMacro(VorticityArrayName);

  vtkSetStringMacro(HelicityFactorArrayName);
  vtkGetStringMacro(HelicityFactorArrayName);

  vtkSetMacro(ComputeHelicityFactor,int);
  vtkGetMacro(ComputeHelicityFactor,int);
  vtkBooleanMacro(ComputeHelicityFactor,int);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

protected:
  vtkvmtkUnstructuredGridVorticityFilter();
  ~vtkvmtkUnstructuredGridVorticityFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* VorticityArrayName;
  char* HelicityFactorArrayName;
  int ComputeHelicityFactor;
  double ConvergenceTolerance;
  int QuadratureOrder;

private:
  vtkvmtkUnstructuredGridVorticityFilter(const vtkvmtkUnstructuredGridVorticityFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridVorticityFilter&);  // Not implemented.
};

#endif

