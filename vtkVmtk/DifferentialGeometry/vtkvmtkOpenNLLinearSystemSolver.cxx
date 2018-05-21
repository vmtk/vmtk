/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkOpenNLLinearSystemSolver.cxx,v $
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

#include "vtkvmtkOpenNLLinearSystemSolver.h"
#include "vtkObjectFactory.h"

extern "C" 
{
#include "nl.h"
}


vtkStandardNewMacro(vtkvmtkOpenNLLinearSystemSolver);

vtkvmtkOpenNLLinearSystemSolver::vtkvmtkOpenNLLinearSystemSolver()
{
  this->SolverType = VTK_VMTK_OPENNL_SOLVER_CG;
  this->PreconditionerType = VTK_VMTK_OPENNL_PRECONDITIONER_NONE;
  this->Omega = 0.0;
}

int vtkvmtkOpenNLLinearSystemSolver::Solve()
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

  nlNewContext();

  nlSolverParameteri(NL_SOLVER,NL_CG);

  switch (this->SolverType)
    {
    case VTK_VMTK_OPENNL_SOLVER_CG:
      nlSolverParameteri(NL_SOLVER,NL_CG);
      break;
    case VTK_VMTK_OPENNL_SOLVER_BICGSTAB:
      nlSolverParameteri(NL_SOLVER,NL_BICGSTAB);
      break;
    case VTK_VMTK_OPENNL_SOLVER_GMRES:
      nlSolverParameteri(NL_SOLVER,NL_GMRES);
      break;
    }
 
  switch (this->PreconditionerType)
    {
    case VTK_VMTK_OPENNL_PRECONDITIONER_NONE:
      nlSolverParameteri(NL_PRECONDITIONER,NL_PRECOND_NONE);
      break;
    case VTK_VMTK_OPENNL_PRECONDITIONER_JACOBI:
      nlSolverParameteri(NL_PRECONDITIONER,NL_PRECOND_JACOBI);
      break;
    case VTK_VMTK_OPENNL_PRECONDITIONER_SSOR:
      nlSolverParameteri(NL_PRECONDITIONER,NL_PRECOND_SSOR);
      break;
    }

  nlSolverParameteri(NL_NB_VARIABLES,rhs->GetNumberOfElements());
  nlSolverParameteri(NL_LEAST_SQUARES,NL_FALSE);
  nlSolverParameteri(NL_MAX_ITERATIONS,this->MaximumNumberOfIterations);
  nlSolverParameterd(NL_THRESHOLD,this->ConvergenceTolerance);

  nlBegin(NL_SYSTEM);
  nlBegin(NL_MATRIX);

  int i, j;
  for (i=0; i<system->GetNumberOfRows(); i++)
    {
    nlRowParameterd(NL_RIGHT_HAND_SIDE,-rhs->GetElement(i));
    vtkvmtkSparseMatrixRow* row = system->GetRow(i);
    nlBegin(NL_ROW);
    for (j=0; j<row->GetNumberOfElements(); j++)
      {
      nlCoefficient(row->GetElementId(j),row->GetElement(j));
      }
    nlCoefficient(i,row->GetDiagonalElement());
    nlEnd(NL_ROW);
    }

  nlEnd(NL_MATRIX);
  nlEnd(NL_SYSTEM);
  nlSolve();

  for (i=0; i<solution->GetNumberOfElements(); i++)
    {
    solution->SetElement(i,nlGetVariable(i));
    }

  nlDeleteContext(nlGetCurrent());

  return 0;
} 

