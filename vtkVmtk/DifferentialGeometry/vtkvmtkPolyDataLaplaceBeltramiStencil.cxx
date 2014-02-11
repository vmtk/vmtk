/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataLaplaceBeltramiStencil.cxx,v $
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

#include "vtkvmtkPolyDataLaplaceBeltramiStencil.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"



vtkvmtkPolyDataLaplaceBeltramiStencil::vtkvmtkPolyDataLaplaceBeltramiStencil()
  {
  this->NumberOfComponents = 1;
  }

void vtkvmtkPolyDataLaplaceBeltramiStencil::BuildBoundaryWeights(vtkIdType boundaryPointId, vtkIdType boundaryNeighborPointId, double &boundaryWeight, double &boundaryNeighborWeight)
  {
  double point[3], point1[3], point2[3];
  double vector1[3], vector2[3];
  double triangleArea;

  if (!this->IsBoundary)
    return;

  this->DataSet->GetPoint(this->DataSetPointId,point);
  this->DataSet->GetPoint(boundaryPointId,point1);
  this->DataSet->GetPoint(boundaryNeighborPointId,point2);

  vector1[0] = point1[0] - point[0];
  vector1[1] = point1[1] - point[1];
  vector1[2] = point1[2] - point[2];

  vector2[0] = point2[0] - point[0];
  vector2[1] = point2[1] - point[1];
  vector2[2] = point2[2] - point[2];

  triangleArea = vtkvmtkMath::TriangleArea(point,point1,point2);

  boundaryWeight = vtkMath::Distance2BetweenPoints(point,point2);
  boundaryNeighborWeight = vtkMath::Dot(vector1,vector2);
  
  if (triangleArea<VTK_VMTK_DOUBLE_TOL)
    {
    if (fabs(boundaryWeight)<VTK_VMTK_DOUBLE_TOL)
      boundaryWeight = 0.0;
    else
      boundaryWeight = VTK_VMTK_LARGE_DOUBLE;
    
    if (fabs(boundaryNeighborWeight)<VTK_VMTK_DOUBLE_TOL)
      boundaryNeighborWeight = 0.0;
    else
      boundaryNeighborWeight = VTK_VMTK_LARGE_DOUBLE;
    }
  else
    {
    boundaryWeight /= 4.0 * triangleArea;
    boundaryNeighborWeight /= 4.0 * triangleArea;
    }
  }

void vtkvmtkPolyDataLaplaceBeltramiStencil::Build()
  {
  double point[3], point1[3], point2[3];
  double boundaryWeight, boundaryNeighborWeight;
  double cotangent;
  vtkIdType j, firstId, lastId;

  this->Superclass::Build();

  vtkPolyData* pdata = vtkPolyData::SafeDownCast(this->DataSet);

  if (!this->IsBoundary)
    {
    firstId = 0;
    lastId = this->NPoints-1;
    }
  else
    {
    firstId = 1;
    lastId = this->NPoints-3;
    }

  pdata->GetPoint(this->DataSetPointId,point);

  for (j=0; j<this->NPoints; j++)
    this->Weights[j] = 0.0;

  for (j=firstId; j<=lastId; j++)
    {
    pdata->GetPoint(this->PointIds[j],point1);
    
    pdata->GetPoint(this->PointIds[(j+1)%this->NPoints],point2);

    cotangent = vtkvmtkMath::Cotangent(point,point2,point1);
    this->Weights[j] += cotangent / 2.0;

    cotangent = vtkvmtkMath::Cotangent(point,point1,point2);
    this->Weights[(j+1)%this->NPoints] += cotangent / 2.0;
    }

  if (this->IsBoundary)
    {
    this->BuildBoundaryWeights(this->PointIds[0],this->PointIds[1],boundaryWeight,boundaryNeighborWeight);
    this->Weights[0] += boundaryWeight;
    this->Weights[1] += boundaryNeighborWeight;

    this->BuildBoundaryWeights(this->PointIds[this->NPoints-1],this->PointIds[this->NPoints-2],boundaryWeight,boundaryNeighborWeight);
    this->Weights[this->NPoints-1] += boundaryWeight;
    this->Weights[this->NPoints-2] += boundaryNeighborWeight;
    }

  this->CenterWeight[0] = 0.0;
  for (j=0; j<this->NPoints; j++)
    {
    this->CenterWeight[0] += this->Weights[j];
    }

  this->ComputeArea();
  this->ScaleWithArea();
  this->ChangeWeightSign();
  }

