/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkEmptyNeighborhood.cxx,v $
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

#include "vtkvmtkEmptyNeighborhood.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkEmptyNeighborhood);

void vtkvmtkEmptyNeighborhood::Build()
  {
  this->NPoints = 0;

  if (this->PointIds!=NULL)
    {
    delete[] this->PointIds;
    this->PointIds = NULL;
    }
  }


