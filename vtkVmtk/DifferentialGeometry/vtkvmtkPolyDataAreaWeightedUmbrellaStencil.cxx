/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataAreaWeightedUmbrellaStencil.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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

#include "vtkvmtkPolyDataAreaWeightedUmbrellaStencil.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataAreaWeightedUmbrellaStencil);

vtkvmtkPolyDataAreaWeightedUmbrellaStencil::vtkvmtkPolyDataAreaWeightedUmbrellaStencil()
{
  this->UseExtendedNeighborhood = 0;
}

void vtkvmtkPolyDataAreaWeightedUmbrellaStencil::Build()
{
  this->Superclass::Build();

  for (vtkIdType i=0; i<this->NPoints; i++)
    {
    this->Weights[i] = 1.0/double(this->NPoints);
    }

  vtkPolyData* pdata = vtkPolyData::SafeDownCast(this->DataSet);

  int j;
  for (j=0; j<this->NPoints; j++)
    {
    this->Weights[j] = 0.0;
    }
  
  double point[3], point1[3], point2[3];
//  double vector[3], vector1[3], vector2[3];
//  double dot = 0.0;
  
  pdata->GetPoint(this->DataSetPointId,point);

//  for (j=0; j<this->NPoints; j++)
//    {
//    pdata->GetPoint(this->PointIds[j],point1);
//    pdata->GetPoint(this->PointIds[(j+1)%this->NPoints],point2);
//    vector[0] = point2[0] - point1[0];
//    vector[1] = point2[1] - point1[1];
//    vector[2] = point2[2] - point1[2];
//    vector1[0] = point1[0] - point[0];
//    vector1[1] = point1[1] - point[1];
//    vector1[2] = point1[2] - point[2];
//    vector2[0] = point2[0] - point[0];
//    vector2[1] = point2[1] - point[1];
//    vector2[2] = point2[2] - point[2];
//    vtkMath::Normalize(vector);
//    vtkMath::Normalize(vector1);
//    vtkMath::Normalize(vector2);
//    dot = vtkMath::Dot(vector,vector1);
//    this->Weights[j] += vtkMath::Distance2BetweenPoints(point,point1) * dot * sqrt(1.0 - dot*dot) / 2.0;
//    if (!(this->IsBoundary || j==this->NPoints-1))
//      {
//      dot = vtkMath::Dot(vector,vector2);
//      this->Weights[(j+1)%this->NPoints] += vtkMath::Distance2BetweenPoints(point,point2) * dot * sqrt(1.0 - dot*dot) / 2.0;
//      }
  for (j=0; j<this->NPoints; j++)
    {
    pdata->GetPoint(this->PointIds[j],point1);
    pdata->GetPoint(this->PointIds[(j+1)%this->NPoints],point2);
    double area = vtkvmtkMath::TriangleArea(point,point1,point2);
    if (!(this->IsBoundary || j==this->NPoints-1))
      {
      this->Weights[j] += area;
      this->Weights[(j+1)%this->NPoints] += area;
      }
    }
 
  this->CenterWeight[0] = 0.0;
  for (j=0; j<this->NPoints; j++)
    {
    this->CenterWeight[0] += this->Weights[j];
    }

  this->Area = this->CenterWeight[0];

  this->ScaleWithArea();

  this->ChangeWeightSign();
}

void vtkvmtkPolyDataAreaWeightedUmbrellaStencil::ScaleWithArea()
{
  this->ScaleWithAreaFactor(1.0);
}
