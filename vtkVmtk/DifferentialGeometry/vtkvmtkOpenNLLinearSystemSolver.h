/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkOpenNLLinearSystemSolver.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkOpenNLLinearSystemSolver - Solve a linear system of equations using open NL.
// .SECTION Description
// There are a number of different solver types that can be used:
//    - VTK_VMTK_OPENNL_SOLVER_BICGSTAB = Biconjugate gradient stabilized method
//    - VTK_VMTK_OPENNL_SOLVER_GMRES = Generalized minimal residual method
//    - VTK_VMTK_OPENNL_SOLVER_CG = Iterative conjugate gradient solver

#ifndef __vtkvmtkOpenNLLinearSystemSolver_h
#define __vtkvmtkOpenNLLinearSystemSolver_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystemSolver.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkOpenNLLinearSystemSolver : public vtkvmtkLinearSystemSolver
{
public:
  static vtkvmtkOpenNLLinearSystemSolver* New();
  vtkTypeMacro(vtkvmtkOpenNLLinearSystemSolver,vtkvmtkLinearSystemSolver);

  int Solve() override;

  vtkSetMacro(SolverType,int);
  vtkGetMacro(SolverType,int);
  void SetSolverTypeToCG()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_CG); }
  void SetSolverTypeToGMRES()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_GMRES); }
  void SetSolverTypeToBiCGStab()
    { this->SetSolverType(VTK_VMTK_OPENNL_SOLVER_BICGSTAB); }

  vtkSetMacro(PreconditionerType,int);
  vtkGetMacro(PreconditionerType,int);
  void SetPreconditionerTypeToNone()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_NONE); }
  void SetPreconditionerTypeToJacobi()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI); }
  void SetPreconditionerTypeToSSOR()
    { this->SetPreconditionerType(VTK_VMTK_OPENNL_PRECONDITIONER_SSOR); }

  vtkSetMacro(Omega,double);
  vtkGetMacro(Omega,double);

  //BTX
  enum
    {
      VTK_VMTK_OPENNL_SOLVER_CG,
      VTK_VMTK_OPENNL_SOLVER_GMRES,
      VTK_VMTK_OPENNL_SOLVER_BICGSTAB
    };
  //ETX
  
  //BTX
  enum
    {
      VTK_VMTK_OPENNL_PRECONDITIONER_NONE,
      VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI,
      VTK_VMTK_OPENNL_PRECONDITIONER_SSOR
    };
  //ETX

protected:
  vtkvmtkOpenNLLinearSystemSolver();
  ~vtkvmtkOpenNLLinearSystemSolver() {};

  int SolverType;
  int PreconditionerType;
  double Omega;

private:
  vtkvmtkOpenNLLinearSystemSolver(const vtkvmtkOpenNLLinearSystemSolver&);  // Not implemented.
  void operator=(const vtkvmtkOpenNLLinearSystemSolver&);  // Not implemented.
};

#endif

