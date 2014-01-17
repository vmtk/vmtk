/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataStencilFlowFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
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

#include "vtkvmtkPolyDataStencilFlowFilter.h"
#include "vtkvmtkStencils.h"
#include "vtkvmtkStencil.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkCellLocator.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataStencilFlowFilter);

vtkvmtkPolyDataStencilFlowFilter::vtkvmtkPolyDataStencilFlowFilter() 
{
  this->StencilType = VTK_VMTK_UMBRELLA_STENCIL;
  
  this->Stencils = NULL;
  this->NumberOfIterations = 100;
  this->RelaxationFactor = 0.1;
  this->MaximumDisplacement = VTK_VMTK_LARGE_DOUBLE;
  this->ProcessBoundary = 0;
  this->ConstrainOnSurface = 0;
}

vtkvmtkPolyDataStencilFlowFilter::~vtkvmtkPolyDataStencilFlowFilter()
{
  this->ReleaseStencils();
}

void vtkvmtkPolyDataStencilFlowFilter::ReleaseStencils()
{
  if (this->Stencils)
    {
    this->Stencils->UnRegister(this);
    this->Stencils = NULL;
    }
}

int vtkvmtkPolyDataStencilFlowFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
  output->GetFieldData()->PassData(input->GetFieldData());

  if (this->Stencils)
    {
    this->ReleaseStencils();
    }
  
  this->Stencils = vtkvmtkStencils::New();
  this->Stencils->SetItemType(this->StencilType);
  this->Stencils->SetDataSet(input);
  this->Stencils->ReallocateOnBuildOff();
  this->Stencils->WeightScalingOn();

  vtkCellLocator *cellLocator = NULL;
  double* weights = NULL;
  if (this->ConstrainOnSurface)
    {
    weights = new double[input->GetMaxCellSize()];
    cellLocator = vtkCellLocator::New();
    cellLocator->SetDataSet(input);
    cellLocator->BuildLocator();
    }
  
  vtkIdType numberOfPoints = input->GetNumberOfPoints();

  double displacement[3], stencilPoint[3], weight;
  double point[3], newPoint[3];

  vtkIdList* constrainCellIds = vtkIdList::New();
  constrainCellIds->SetNumberOfIds(input->GetNumberOfPoints());
  for (vtkIdType pointId=0; pointId<numberOfPoints; pointId++)
    {
    constrainCellIds->SetId(pointId,0);
    }
 
  for (int iteration=0; iteration<this->NumberOfIterations; iteration++)
    {
    this->Stencils->Build();
    for (vtkIdType pointId=0; pointId<numberOfPoints; pointId++)
      {
      vtkvmtkStencil* stencil = this->Stencils->GetStencil(pointId);
      if (stencil->GetIsBoundary() && (!this->ProcessBoundary))
        {
        continue;
        }      
      displacement[0] = displacement[1] = displacement[2] = 0.0;
      int numberOfStencilPoints = stencil->GetNumberOfPoints();
      for (int j=0; j<numberOfStencilPoints; j++)
        {
        output->GetPoint(stencil->GetPointId(j),stencilPoint);
        weight = stencil->GetWeight(j);
        displacement[0] += weight * stencilPoint[0];
        displacement[1] += weight * stencilPoint[1];
        displacement[2] += weight * stencilPoint[2];
        }
      weight = stencil->GetCenterWeight();
      output->GetPoint(pointId,point);
      displacement[0] -= weight * point[0];
      displacement[1] -= weight * point[1];
      displacement[2] -= weight * point[2];
      
      if (vtkMath::Norm(displacement) > this->MaximumDisplacement)
        {
        vtkMath::Normalize(displacement);
        displacement[0] *= this->MaximumDisplacement;
        displacement[1] *= this->MaximumDisplacement;
        displacement[2] *= this->MaximumDisplacement;
        }
      
      newPoint[0] = point[0] + this->RelaxationFactor * displacement[0];
      newPoint[1] = point[1] + this->RelaxationFactor * displacement[1];
      newPoint[2] = point[2] + this->RelaxationFactor * displacement[2];

      if (this->ConstrainOnSurface)
        {
        double closestPoint[3];
        vtkIdType cellId;
        int subId;
        double dist2, pcoords[3];
        if (input->GetCell(constrainCellIds->GetId(pointId))->EvaluatePosition(newPoint,closestPoint,subId,pcoords,dist2,weights)==0)
          {
          cellLocator->FindClosestPoint(newPoint,closestPoint,cellId,subId,dist2);
          constrainCellIds->SetId(pointId,cellId);
          }
        newPoint[0] = closestPoint[0];
        newPoint[1] = closestPoint[1];
        newPoint[2] = closestPoint[2];
        }
      
      output->GetPoints()->SetPoint(pointId,newPoint);
      }
    }

  constrainCellIds->Delete();
  
  if (weights)
  {
    delete[] weights;
    weights = NULL;
  }
  
  if (cellLocator)
    {
    cellLocator->Delete();
    cellLocator = NULL;
    }
  
  return 1;
}
