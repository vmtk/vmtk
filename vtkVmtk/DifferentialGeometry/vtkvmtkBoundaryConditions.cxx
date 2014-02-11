/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkBoundaryConditions.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:07:29 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkBoundaryConditions.h"
#include "vtkObjectFactory.h"



vtkvmtkBoundaryConditions::vtkvmtkBoundaryConditions()
  {
  this->LinearSystem = NULL;

  this->BoundaryNodes = NULL;
  this->BoundaryValues = NULL;
  }

vtkvmtkBoundaryConditions::~vtkvmtkBoundaryConditions()
{
  if (this->LinearSystem)
    {
    this->LinearSystem->Delete();
    this->LinearSystem = NULL;
    }

  if (this->BoundaryNodes)
    {
    this->BoundaryNodes->Delete();
    this->BoundaryNodes = NULL;
    }

  if (this->BoundaryValues)
    {
    this->BoundaryValues->Delete();
    this->BoundaryValues = NULL;
    }
}

void vtkvmtkBoundaryConditions::Apply()
  {
  if (this->LinearSystem==NULL)
    {
    vtkErrorMacro(<< "Linear system not set!");
    return;
    }

  if (this->BoundaryNodes==NULL)
    {
    vtkErrorMacro(<< "Boundary nodes not set!");
    return;
    }

  if (this->BoundaryValues==NULL)
    {
    vtkErrorMacro(<< "Boundary values not set!");
    return;
    }

  if (this->BoundaryNodes->GetNumberOfIds() != this->BoundaryValues->GetNumberOfTuples())
    {
    vtkErrorMacro(<< "Boundary nodes list and boundary values array do not have the same size!");
    return;
    }

  }

