/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshLambda2.h,v $
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
  // .NAME vtkvmtkMeshLambda2 - Compute the Lambda2 surface for a given flow field which allow for the visualization of fluid vortex cores. 
  // .SECTION Description
  // .

#ifndef __vtkvmtkMeshLambda2_h
#define __vtkvmtkMeshLambda2_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshLambda2 : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshLambda2,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkMeshLambda2 *New();

  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
 
  vtkSetStringMacro(Lambda2ArrayName);
  vtkGetStringMacro(Lambda2ArrayName);
 
  vtkSetMacro(ComputeIndividualPartialDerivatives,int);
  vtkGetMacro(ComputeIndividualPartialDerivatives,int);
  vtkBooleanMacro(ComputeIndividualPartialDerivatives,int);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  vtkSetMacro(ForceBoundaryToNegative,int);
  vtkGetMacro(ForceBoundaryToNegative,int);
  vtkBooleanMacro(ForceBoundaryToNegative,int);

  protected:
  vtkvmtkMeshLambda2();
  ~vtkvmtkMeshLambda2();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* VelocityArrayName;
  char* Lambda2ArrayName;

  int ComputeIndividualPartialDerivatives;
  double ConvergenceTolerance;
  int QuadratureOrder;
  int ForceBoundaryToNegative;

  private:
  vtkvmtkMeshLambda2(const vtkvmtkMeshLambda2&);  // Not implemented.
  void operator=(const vtkvmtkMeshLambda2&);  // Not implemented.
};

#endif
