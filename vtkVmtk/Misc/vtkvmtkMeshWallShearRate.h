/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshWallShearRate.h,v $
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
// .NAME vtkvmtkMeshWallShearRate - calculates wall shear rate from velocity components in a mesh
// .SECTION Description
// .

#ifndef __vtkvmtkMeshWallShearRate_h
#define __vtkvmtkMeshWallShearRate_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshWallShearRate : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshWallShearRate,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkMeshWallShearRate *New();

  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
 
  vtkSetStringMacro(WallShearRateArrayName);
  vtkGetStringMacro(WallShearRateArrayName);
 
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);

  vtkSetMacro(UseFullStrainRateTensor,int);
  vtkGetMacro(UseFullStrainRateTensor,int);
  vtkBooleanMacro(UseFullStrainRateTensor,int);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  protected:
  vtkvmtkMeshWallShearRate();
  ~vtkvmtkMeshWallShearRate();  

  int FillInputPortInformation(int, vtkInformation *info) override;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* WallShearRateArrayName;

  int ComputeIndividualPartialDerivatives;

  double ConvergenceTolerance;
  int QuadratureOrder;
  int UseFullStrainRateTensor;

  private:
  vtkvmtkMeshWallShearRate(const vtkvmtkMeshWallShearRate&);  // Not implemented.
  void operator=(const vtkvmtkMeshWallShearRate&);  // Not implemented.
};

#endif
