/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkLinearSystem.cxx,v $
  Language:  C++
  Date:      $Date: 2006/02/23 09:31:41 $
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

#include "vtkvmtkLinearSystem.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkLinearSystem);

vtkvmtkLinearSystem::vtkvmtkLinearSystem()
{
  this->A = NULL;
  this->X = NULL;
  this->B = NULL;
}

vtkvmtkLinearSystem::~vtkvmtkLinearSystem()
{
  if (this->A)
    {
    this->A->Delete();
    this->A = NULL;
    }

  if (this->X)
    {
    this->X->Delete();
    this->X = NULL;
    }

  if (this->B)
    {
    this->B->Delete();
    this->B = NULL;
    }
}

int vtkvmtkLinearSystem::CheckSystem()
{
  if (this->A==NULL)
    {
    vtkErrorMacro(<< "System matrix (A) not set!");
    return -1;
    }

  if (this->X==NULL)
    {
    vtkErrorMacro(<< "Unknown vector (x) not provided!");
    return -1;
    }

  if (this->B==NULL)
    {
    vtkErrorMacro(<< "Right-hand side vector (b) not set!");
    return -1;
    }

  if ((this->A->GetNumberOfRows()!=this->X->GetNumberOfElements())||(this->B->GetNumberOfElements()!=this->X->GetNumberOfElements()))
    {
    vtkErrorMacro(<< "System matrix size, unknown vector size and right-hand side vector size do not match!");
    return -1;
    }
  
  return 0;
}

