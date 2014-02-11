/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkReferenceSystemUtilities.cxx,v $
Language:  C++
Date:      $Date: 2005/07/28 16:11:52 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkReferenceSystemUtilities.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkReferenceSystemUtilities);

vtkIdType vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(vtkPolyData* referenceSystems, const char* groupIdsArrayName, vtkIdType groupId)
{
  vtkDataArray* groupIdsArray = referenceSystems->GetPointData()->GetArray(groupIdsArrayName);
  int numberOfPoints = referenceSystems->GetNumberOfPoints();

  vtkIdType pointId = -1;
  for (int i=0; i<numberOfPoints; i++)
  {
    vtkIdType current_groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    if (current_groupId == groupId)
    {
      pointId = i;
      break;
    }
  }
  return pointId;
}
