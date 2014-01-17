/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFELaplaceBeltramiStencil.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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

#include "vtkvmtkPolyDataFELaplaceBeltramiStencil.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataFELaplaceBeltramiStencil);

vtkvmtkPolyDataFELaplaceBeltramiStencil::vtkvmtkPolyDataFELaplaceBeltramiStencil()
{
  this->UseExtendedNeighborhood = 1;
}

void vtkvmtkPolyDataFELaplaceBeltramiStencil::ScaleWithArea()
  {
  this->ScaleWithAreaFactor(1.0/3.0);
  }
