/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkLASPACKLinearSystemSolver.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkLASPACKLinearSystemSolver - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkLASPACKLinearSystemSolver_h
#define __vtkvmtkLASPACKLinearSystemSolver_h

#include "vtkObject.h"
#include "vtkvmtkLinearSystemSolver.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkLASPACKLinearSystemSolver : public vtkvmtkLinearSystemSolver
{
public:
  static vtkvmtkLASPACKLinearSystemSolver* New();
  vtkTypeRevisionMacro(vtkvmtkLASPACKLinearSystemSolver,vtkvmtkLinearSystemSolver);

  int Solve();

  vtkSetMacro(Omega,double);
  vtkGetMacro(Omega,double);

  vtkSetMacro(SolverType,int);
  vtkGetMacro(SolverType,int);
  void SetSolverTypeToJacobi()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_JACOBI); }
  void SetSolverTypeToSORForw()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_SORFORW); }
  void SetSolverTypeToSORBackw()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_SORBACKW); }
  void SetSolverTypeToSSOR()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_SSOR); }
  void SetSolverTypeToChebyshev()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_CHEBYSHEV); }
  void SetSolverTypeToCG()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_CG); }
  void SetSolverTypeToCGN()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_CGN); }
  void SetSolverTypeToGMRES()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_GMRES); }
  void SetSolverTypeToBiCG()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_BICG); }
  void SetSolverTypeToQMR()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_QMR); }
  void SetSolverTypeToCGS()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_CGS); }
  void SetSolverTypeToBiCGStab()
    { this->SetSolverType(VTK_VMTK_LASPACK_SOLVER_BICGSTAB); }

  vtkSetMacro(PreconditionerType,int);
  vtkGetMacro(PreconditionerType,int);
  void SetPreconditionerTypeToNone()
    { this->SetPreconditionerType(VTK_VMTK_LASPACK_PRECONDITIONER_NONE); }
  void SetPreconditionerTypeToJacobi()
    { this->SetPreconditionerType(VTK_VMTK_LASPACK_PRECONDITIONER_JACOBI); }
  void SetPreconditionerTypeToSSOR()
    { this->SetPreconditionerType(VTK_VMTK_LASPACK_PRECONDITIONER_SSOR); }
  void SetPreconditionerTypeToILU()
    { this->SetPreconditionerType(VTK_VMTK_LASPACK_PRECONDITIONER_ILU); }

  //BTX
  enum
    {
      VTK_VMTK_LASPACK_SOLVER_JACOBI,
      VTK_VMTK_LASPACK_SOLVER_SORFORW,
      VTK_VMTK_LASPACK_SOLVER_SORBACKW,
      VTK_VMTK_LASPACK_SOLVER_SSOR,
      VTK_VMTK_LASPACK_SOLVER_CHEBYSHEV,
      VTK_VMTK_LASPACK_SOLVER_CG,
      VTK_VMTK_LASPACK_SOLVER_CGN,
      VTK_VMTK_LASPACK_SOLVER_GMRES,
      VTK_VMTK_LASPACK_SOLVER_BICG,
      VTK_VMTK_LASPACK_SOLVER_QMR,
      VTK_VMTK_LASPACK_SOLVER_CGS,
      VTK_VMTK_LASPACK_SOLVER_BICGSTAB
    };
  //ETX
  
  //BTX
  enum
    {
      VTK_VMTK_LASPACK_PRECONDITIONER_NONE,
      VTK_VMTK_LASPACK_PRECONDITIONER_JACOBI,
      VTK_VMTK_LASPACK_PRECONDITIONER_SSOR,
      VTK_VMTK_LASPACK_PRECONDITIONER_ILU
    };
  //ETX

protected:
  vtkvmtkLASPACKLinearSystemSolver();
  ~vtkvmtkLASPACKLinearSystemSolver() {};

  int SolverType;

  int PreconditionerType;

  double Omega;

private:
  vtkvmtkLASPACKLinearSystemSolver(const vtkvmtkLASPACKLinearSystemSolver&);  // Not implemented.
  void operator=(const vtkvmtkLASPACKLinearSystemSolver&);  // Not implemented.
};

#endif

