/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkEmptyStencil.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
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

#include "vtkvmtkEmptyStencil.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkEmptyStencil);

void vtkvmtkEmptyStencil::Build()
  {
  if (this->PointIds && !this->ReallocateOnBuild)
    {
    return;
    }
  
  this->NPoints = 0;

  if (this->PointIds!=NULL)
    {
    delete[] this->PointIds;
    this->PointIds = NULL;
    }
  if (this->Weights!=NULL)
    {
    delete[] this->Weights;
    this->Weights = NULL;
    }
  if (this->CenterWeight!=NULL)
    {
    delete[] this->CenterWeight;
    this->CenterWeight = NULL;
    }
  }


