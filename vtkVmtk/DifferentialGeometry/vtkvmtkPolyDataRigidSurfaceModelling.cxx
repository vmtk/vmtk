/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataRigidSurfaceModelling.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataRigidSurfaceModelling.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataRigidSurfaceModelling);

vtkvmtkPolyDataRigidSurfaceModelling::vtkvmtkPolyDataRigidSurfaceModelling()
{
}

vtkvmtkPolyDataRigidSurfaceModelling::~vtkvmtkPolyDataRigidSurfaceModelling()
{
}

int vtkvmtkPolyDataRigidSurfaceModelling::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkErrorMacro(<< "vtkvmtkPolyDataRigidSurfaceModelling hasn't been implemented yet.");
 
  return 1;
}

void vtkvmtkPolyDataRigidSurfaceModelling::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

