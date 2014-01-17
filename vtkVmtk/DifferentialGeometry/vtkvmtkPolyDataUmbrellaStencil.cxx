/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataUmbrellaStencil.cxx,v $
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

#include "vtkvmtkPolyDataUmbrellaStencil.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataUmbrellaStencil);

vtkvmtkPolyDataUmbrellaStencil::vtkvmtkPolyDataUmbrellaStencil()
{
  this->UseExtendedNeighborhood = 0;
}
  
void vtkvmtkPolyDataUmbrellaStencil::Build()
  {
  this->Superclass::Build();

  this->Area = 1.0;
  for (vtkIdType i=0; i<this->NPoints; i++)
    {
    this->Weights[i] = 1.0/double(this->NPoints);
    }
  this->CenterWeight[0] = 1.0;
  this->ChangeWeightSign();
  }

