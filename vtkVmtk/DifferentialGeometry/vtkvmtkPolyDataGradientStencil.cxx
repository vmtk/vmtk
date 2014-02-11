/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataGradientStencil.cxx,v $
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

#include "vtkvmtkPolyDataGradientStencil.h"
#include "vtkPolyData.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataGradientStencil);

vtkvmtkPolyDataGradientStencil::vtkvmtkPolyDataGradientStencil()
{
  this->Area = 0.0;
  this->NumberOfComponents = 3;
}

void vtkvmtkPolyDataGradientStencil::ScaleWithArea()
{
  this->ScaleWithAreaFactor(1.0);
}

void vtkvmtkPolyDataGradientStencil::Gamma(double p0[3], double p1[3], double p2[3], double gamma[3])
{
  double p01[3], p12[3], p20[3];
  p01[0] = p0[0] - p1[0];
  p01[1] = p0[1] - p1[1];
  p01[2] = p0[2] - p1[2];
  p12[0] = p1[0] - p2[0];
  p12[1] = p1[1] - p2[1];
  p12[2] = p1[2] - p2[2];
  p20[0] = p2[0] - p0[0];
  p20[1] = p2[1] - p0[1];
  p20[2] = p2[2] - p0[2];
  
  double dot = vtkMath::Dot(p01,p12);
  gamma[0] = dot * p20[0];
  gamma[1] = dot * p20[1];
  gamma[2] = dot * p20[2];
}

void vtkvmtkPolyDataGradientStencil::Build()
{
  double point[3], point1[3], point2[3];
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
    firstId = 0;
    lastId = this->NPoints-2;
    }

  pdata->GetPoint(this->DataSetPointId,point);

  for (j=0; j<3*this->NPoints; j++)
    this->Weights[j] = 0.0;

  this->CenterWeight[0] = 0.0;
  this->CenterWeight[1] = 0.0;
  this->CenterWeight[2] = 0.0;
  
  double gamma[3];
  for (j=firstId; j<=lastId; j++)
    {
    pdata->GetPoint(this->PointIds[j],point1);
 
    int jplus = (j+1) % this->NPoints;
    pdata->GetPoint(this->PointIds[jplus],point2);

    double areaWeight = 1.0 / (4.0 * vtkvmtkMath::TriangleArea(point,point1,point2));
    
    this->Gamma(point,point1,point2,gamma); 
    this->CenterWeight[0] += gamma[0] * areaWeight;
    this->CenterWeight[1] += gamma[1] * areaWeight;
    this->CenterWeight[2] += gamma[2] * areaWeight;

    this->Gamma(point,point2,point1,gamma); 
    this->CenterWeight[0] += gamma[0] * areaWeight;
    this->CenterWeight[1] += gamma[1] * areaWeight;
    this->CenterWeight[2] += gamma[2] * areaWeight;

    this->Gamma(point1,point,point2,gamma); 
    this->Weights[3*j+0] += gamma[0] * areaWeight;
    this->Weights[3*j+1] += gamma[1] * areaWeight;
    this->Weights[3*j+2] += gamma[2] * areaWeight;

    this->Gamma(point1,point2,point,gamma); 
    this->Weights[3*j+0] += gamma[0] * areaWeight;
    this->Weights[3*j+1] += gamma[1] * areaWeight;
    this->Weights[3*j+2] += gamma[2] * areaWeight;

    this->Gamma(point2,point1,point,gamma); 
    this->Weights[3*jplus+0] += gamma[0] * areaWeight;
    this->Weights[3*jplus+1] += gamma[1] * areaWeight;
    this->Weights[3*jplus+2] += gamma[2] * areaWeight;

    this->Gamma(point2,point,point1,gamma); 
    this->Weights[3*jplus+0] += gamma[0] * areaWeight;
    this->Weights[3*jplus+1] += gamma[1] * areaWeight;
    this->Weights[3*jplus+2] += gamma[2] * areaWeight;
    }

  this->ComputeArea();
  this->ScaleWithArea();
}

