/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataManifoldStencil.cxx,v $
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

#include "vtkvmtkPolyDataManifoldStencil.h"
#include "vtkvmtkPolyDataManifoldExtendedNeighborhood.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"

#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkvmtkMath.h"
#include "vtkMath.h"



vtkvmtkPolyDataManifoldStencil::vtkvmtkPolyDataManifoldStencil()
  {
  this->Area = 0.0;
  this->NumberOfComponents = 1;
  this->UseExtendedNeighborhood = 1;
  }

void vtkvmtkPolyDataManifoldStencil::ComputeArea()
  {
  double point[3], point1[3], point2[3];
  vtkIdType j, numberOfTriangles;

  if (this->DataSet==NULL)
    {
    vtkErrorMacro(<<"No DataSet specified.");
    return;
    }

  this->DataSet->GetPoint(this->DataSetPointId,point);
  this->Area = 0.0;

  if (!this->IsBoundary)
    {
    numberOfTriangles = this->NPoints;
    }
  else
    {
    numberOfTriangles = this->NPoints-1;
    }

  for (j=0; j<numberOfTriangles; j++)
    {
    this->DataSet->GetPoint(this->PointIds[j],point1);
    this->DataSet->GetPoint(this->PointIds[(j+1)%this->NPoints],point2);
		
    this->Area += vtkvmtkMath::TriangleArea(point2,point,point1);
    }
  }

void vtkvmtkPolyDataManifoldStencil::ScaleWithAreaFactor(double factor)
  {
  double scale;

  if (this->Area<VTK_VMTK_DOUBLE_TOL)
    {
    scale = 2.0*VTK_VMTK_LARGE_DOUBLE;
    }
  else
    {
    scale = 1.0 / (this->Area * factor);
    }

  this->ScaleWeights(scale);
  }


void vtkvmtkPolyDataManifoldStencil::Build()
  {
  if (this->PointIds && this->Weights && !this->ReallocateOnBuild)
    {
    return;
    }
  
  vtkIdType i;
  vtkvmtkPolyDataManifoldNeighborhood *manifoldNeighborhood;

  if (this->UseExtendedNeighborhood)
    {
    manifoldNeighborhood = vtkvmtkPolyDataManifoldExtendedNeighborhood::New();
    }
  else
    {
    manifoldNeighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
    }
  manifoldNeighborhood->SetDataSet(this->DataSet);
  manifoldNeighborhood->SetDataSetPointId(this->DataSetPointId);
  manifoldNeighborhood->Build();

  this->NPoints = manifoldNeighborhood->GetNumberOfPoints();
  if (this->PointIds!=NULL)
    {
    delete[] this->PointIds;
    this->PointIds = NULL;
    }
  this->PointIds = new vtkIdType[this->NPoints];
  memcpy(this->PointIds,manifoldNeighborhood->GetPointer(0),this->NPoints*sizeof(vtkIdType));

  this->IsBoundary = manifoldNeighborhood->GetIsBoundary();

  if (this->Weights!=NULL)
    {
    delete[] this->Weights;
    this->Weights = NULL;
    }
  this->Weights = new double[this->NumberOfComponents*this->NPoints];
  for (i=0; i<this->NumberOfComponents*this->NPoints; i++)
    {
    this->Weights[i] = 0.0;
    }
  this->CenterWeight = new double[this->NumberOfComponents];
  for (i=0; i<this->NumberOfComponents; i++)
    {
    this->CenterWeight[i] = 0.0;
    }

  manifoldNeighborhood->Delete();
  }

void vtkvmtkPolyDataManifoldStencil::DeepCopy(vtkvmtkPolyDataManifoldStencil *src)
  {
  this->Superclass::DeepCopy(src);
  this->Area = src->Area;
  }
