/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearSystemSolver.cxx,v $
Language:  C++
Date:      $Date: 2005/03/04 11:07:29 $
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

#include "vtkvmtkLinearSystemSolver.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkLinearSystemSolver);

vtkvmtkLinearSystemSolver::vtkvmtkLinearSystemSolver()
{
  this->LinearSystem = NULL;

  this->MaximumNumberOfIterations = VTK_VMTK_LARGE_INTEGER;
  this->ConvergenceTolerance = 1E-4;

  this->NumberOfIterations = 0;
  this->Residual = VTK_VMTK_LARGE_DOUBLE;
}

vtkvmtkLinearSystemSolver::~vtkvmtkLinearSystemSolver()
{
  if (this->LinearSystem)
    {
    this->LinearSystem->Delete();
    this->LinearSystem = NULL;
    }
}

int vtkvmtkLinearSystemSolver::Solve()
{
  if (this->LinearSystem==NULL)
    {
    vtkErrorMacro(<< "Linear system not set!");
    return -1;
    }

  if (this->LinearSystem->CheckSystem()==-1) 
    {
    return -1; 
    }

  return 0;
}
