/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkLASPACKLinearSystemSolver.cxx,v $
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

#include "vtkvmtkLASPACKLinearSystemSolver.h"
#include "vtkObjectFactory.h"

extern "C" 
{
#include "laspack/rtc.h"
#include "laspack/itersolv.h"
#include "laspack/qmatrix.h"
#include "laspack/vector.h"
#include "laspack/errhandl.h"
}

vtkCxxRevisionMacro(vtkvmtkLASPACKLinearSystemSolver, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkLASPACKLinearSystemSolver);

vtkvmtkLASPACKLinearSystemSolver::vtkvmtkLASPACKLinearSystemSolver()
{
  this->SolverType = VTK_VMTK_LASPACK_SOLVER_JACOBI;
  this->PreconditionerType = VTK_VMTK_LASPACK_PRECONDITIONER_NONE;
  this->Omega = 0.0;
}

int vtkvmtkLASPACKLinearSystemSolver::Solve()
{
  vtkvmtkSparseMatrix *system;
  vtkvmtkDoubleVector *solution, *rhs;

  if (this->Superclass::Solve()==-1)
    {
    return -1;
    }
  
  system = this->LinearSystem->GetA();
  rhs = this->LinearSystem->GetB();
  solution = this->LinearSystem->GetX();

  QMatrix A;
  Vector x, b;

  size_t Dim = system->GetNumberOfRows();

  Boolean Symmetry = False;
  Q_Constr(&A, "A", Dim, Symmetry, Rowws, Normal, True);
  V_Constr(&x, "x", Dim, Normal, True);
  V_Constr(&b, "b", Dim, Normal, True);

  int i, j;
  for (i=0; i<system->GetNumberOfRows(); i++)
    {
    vtkvmtkSparseMatrixRow* row = system->GetRow(i);
    Q_SetLen(&A, i+1, row->GetNumberOfElements()+1);
    for (j=0; j<row->GetNumberOfElements(); j++)
      {
      Q_SetEntry(&A, i+1, j, row->GetElementId(j)+1, row->GetElement(j));
      }
    Q_SetEntry(&A, i+1, row->GetNumberOfElements(), i+1, row->GetDiagonalElement());
    }

  for (i=0; i<rhs->GetNumberOfElements(); i++)
    {
    V_SetCmp(&b, i+1, rhs->GetElement(i));
    }

  V_SetAllCmp(&x, 0.0);

  PrecondProcType PrecondProc;

  switch (this->PreconditionerType)
    {
    case VTK_VMTK_LASPACK_PRECONDITIONER_NONE:
      PrecondProc = NULL;
      break;
    case VTK_VMTK_LASPACK_PRECONDITIONER_JACOBI:
      PrecondProc = JacobiPrecond;
      break;
    case VTK_VMTK_LASPACK_PRECONDITIONER_SSOR:
      PrecondProc = SSORPrecond;
      break;
    case VTK_VMTK_LASPACK_PRECONDITIONER_ILU:
      PrecondProc = ILUPrecond;
      break;
    default:
      PrecondProc = NULL;
    }

//   SetRTCAuxProc(PrintHistory);

//   Q_SetKer(&A, &x, NULL);
  SetRTCAccuracy(this->ConvergenceTolerance);

  switch (this->SolverType)
    {
    case VTK_VMTK_LASPACK_SOLVER_JACOBI:
      JacobiIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_SORFORW:
      SORForwIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_SORBACKW:
      SORBackwIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_SSOR:
      SSORIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_CHEBYSHEV:
      ChebyshevIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_CG:
      CGIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_CGN:
      CGNIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_GMRES:
      GMRESIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_BICG:
      BiCGIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_QMR:
      QMRIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_CGS:
      CGSIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    case VTK_VMTK_LASPACK_SOLVER_BICGSTAB:
      BiCGSTABIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
      break;
    default:
      JacobiIter(&A, &x, &b, this->MaximumNumberOfIterations, PrecondProc, this->Omega);
    }
  
  if (LASResult() != LASOK) 
    {
    printf("LASPack error: ");
    WriteLASErrDescr(stdout);
    return -1;
    }

  for (i=0; i<solution->GetNumberOfElements(); i++)
    {
    solution->SetElement(i,V_GetCmp(&x, i+1));
    }

  Q_Destr(&A);
  V_Destr(&x);
  V_Destr(&b);

  return 0;
} 

// void PrintHistory(int Iter, double rNorm, double bNorm, IterIdType IterId)
// {
//   printf("%3d. iteration ... accuracy = ", Iter);
//   if (!IsZero(bNorm))
//     {
//     printf("%12.5e\n", rNorm / bNorm);
//     }
//   else
//     {
//     printf("    ---\n");
//     }
// }
