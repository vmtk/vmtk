/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkLinearSystemSolver.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkLinearSystemSolver - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkLinearSystemSolver_h
#define __vtkvmtkLinearSystemSolver_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkLinearSystemSolver : public vtkObject
{
public:
  static vtkvmtkLinearSystemSolver* New();
  vtkTypeMacro(vtkvmtkLinearSystemSolver,vtkObject);

  vtkSetObjectMacro(LinearSystem,vtkvmtkLinearSystem);
  vtkGetObjectMacro(LinearSystem,vtkvmtkLinearSystem);

  vtkSetMacro(MaximumNumberOfIterations,int);
  vtkGetMacro(MaximumNumberOfIterations,int);

  vtkSetMacro(ConvergenceTolerance,double);
  vtkGetMacro(ConvergenceTolerance,double);

  virtual int Solve();

protected:
  vtkvmtkLinearSystemSolver();
  ~vtkvmtkLinearSystemSolver();

  vtkvmtkLinearSystem* LinearSystem;

  int MaximumNumberOfIterations;
  double ConvergenceTolerance;

  int NumberOfIterations;
  double Residual;

private:
  vtkvmtkLinearSystemSolver(const vtkvmtkLinearSystemSolver&);  // Not implemented.
  void operator=(const vtkvmtkLinearSystemSolver&);  // Not implemented.
};

#endif

