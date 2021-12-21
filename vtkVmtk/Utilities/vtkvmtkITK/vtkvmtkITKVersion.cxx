/*=========================================================================

Program:   VMTK 
Module:    vtkvmtkITKVersion
Language:  C++
Date:      $Date: 2021/12/16 $
Version:   $Revision: 1.5 $

  Copyright (c) Kurt Sansom, Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkITKVersion.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkvmtkITKVersion);

void vtkvmtkITKVersion::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
