/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshVorticity.h,v $
Language:  C++
Date:      $Date: 2006/07/27 08:28:36 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkMeshVorticity - Calculates vorticity from velocity gradients in a mesh.
// .SECTION Description
// .

#ifndef __vtkvmtkMeshVorticity_h
#define __vtkvmtkMeshVorticity_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshVorticity : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshVorticity,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkMeshVorticity *New();

  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
 
  vtkSetStringMacro(VorticityArrayName);
  vtkGetStringMacro(VorticityArrayName);
 
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  protected:
  vtkvmtkMeshVorticity();
  ~vtkvmtkMeshVorticity();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* VorticityArrayName;

  int ComputeIndividualPartialDerivatives;

  double ConvergenceTolerance;
  int QuadratureOrder;

  private:
  vtkvmtkMeshVorticity(const vtkvmtkMeshVorticity&);  // Not implemented.
  void operator=(const vtkvmtkMeshVorticity&);  // Not implemented.
};

#endif
