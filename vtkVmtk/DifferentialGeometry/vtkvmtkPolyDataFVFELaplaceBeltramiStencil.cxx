/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFVFELaplaceBeltramiStencil.cxx,v $
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

#include "vtkvmtkPolyDataFVFELaplaceBeltramiStencil.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataFVFELaplaceBeltramiStencil);

vtkvmtkPolyDataFVFELaplaceBeltramiStencil::vtkvmtkPolyDataFVFELaplaceBeltramiStencil()
{
  this->UseExtendedNeighborhood = 1;
}
  
void vtkvmtkPolyDataFVFELaplaceBeltramiStencil::ComputeArea(vtkPolyData *data, vtkIdType pointId)
  {
  double point[3], point1[3], point2[3];
  vtkIdType j, numberOfTriangles;
  int triangleObtusity;

  data->GetPoint(pointId,point);
  this->Area = 0.0;

  if (!this->IsBoundary)
    numberOfTriangles = this->NPoints;
  else
    numberOfTriangles = this->NPoints-1;

  for (j=0; j<numberOfTriangles; j++)
    {
    data->GetPoint(this->PointIds[j],point1);
    
    data->GetPoint(this->PointIds[(j+1)%this->NPoints],point2);
    
    triangleObtusity = vtkvmtkMath::IsTriangleObtuse(point2,point,point1);
    
    if (triangleObtusity==VTK_VMTK_NON_OBTUSE)
      this->Area += vtkvmtkMath::VoronoiSectorArea(point2,point,point1);
    else if (triangleObtusity==VTK_VMTK_OBTUSE_IN_POINT)
      this->Area += vtkvmtkMath::TriangleArea(point2,point,point1)/2.0;
    else if (triangleObtusity==VTK_VMTK_OBTUSE_NOT_IN_POINT)
      this->Area += vtkvmtkMath::TriangleArea(point2,point,point1)/4.0;
    }
  }

void vtkvmtkPolyDataFVFELaplaceBeltramiStencil::ScaleWithArea()
  {
//  this->ScaleWithAreaFactor(1.0);
  this->ScaleWithAreaFactor(1.0/3.0);
  }


