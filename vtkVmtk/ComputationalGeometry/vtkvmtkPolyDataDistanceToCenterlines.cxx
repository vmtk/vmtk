/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDistanceToCenterlines.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/31 15:07:48 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataDistanceToCenterlines.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataDistanceToCenterlines);

vtkvmtkPolyDataDistanceToCenterlines::vtkvmtkPolyDataDistanceToCenterlines() 
{
  this->DistanceToCenterlinesArrayName = NULL;

  this->Centerlines = NULL;
  this->CenterlineRadiusArrayName = NULL;

  this->UseRadiusInformation = 1;
  this->EvaluateTubeFunction = 0;
  this->EvaluateCenterlineRadius = 0;
  this->ProjectPointArrays = 0;
}

vtkvmtkPolyDataDistanceToCenterlines::~vtkvmtkPolyDataDistanceToCenterlines()
{
  if (this->DistanceToCenterlinesArrayName)
    {
    delete[] this->DistanceToCenterlinesArrayName;
    this->DistanceToCenterlinesArrayName = NULL;
    }

  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->CenterlineRadiusArrayName)
    {
    delete[] this->CenterlineRadiusArrayName;
    this->CenterlineRadiusArrayName = NULL;
    }
}

int vtkvmtkPolyDataDistanceToCenterlines::RequestData(
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

  if (!this->DistanceToCenterlinesArrayName)
    {
    vtkErrorMacro(<<"DistanceToCenterlinesArrayName not set.");
    return 1;
    }

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  if (this->EvaluateTubeFunction || this->EvaluateCenterlineRadius)
    {
    this->UseRadiusInformation = 1;
    }

  if (this->UseRadiusInformation)
    {
    if (!this->CenterlineRadiusArrayName)
      {
      vtkErrorMacro(<<"CenterlineRadiusArrayName not set.");
      return 1;
      }

    vtkDataArray* centerlineRadiusArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName);
    
    if (!centerlineRadiusArray)
      {
      vtkErrorMacro(<<"CenterlineRadiusArrayName with name specified does not exist.");
      return 1;
      }
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkDoubleArray* distanceToCenterlinesArray = vtkDoubleArray::New();
  distanceToCenterlinesArray->SetName(this->DistanceToCenterlinesArrayName);
  distanceToCenterlinesArray->SetNumberOfComponents(1);
  distanceToCenterlinesArray->SetNumberOfTuples(numberOfInputPoints);
  distanceToCenterlinesArray->FillComponent(0,0.0);
  
  output->GetPointData()->AddArray(distanceToCenterlinesArray);

  vtkDoubleArray* surfaceCenterlineRadiusArray = NULL;
  if (this->EvaluateCenterlineRadius)
    {
    surfaceCenterlineRadiusArray = vtkDoubleArray::New();
    surfaceCenterlineRadiusArray->SetName(this->CenterlineRadiusArrayName);
    surfaceCenterlineRadiusArray->SetNumberOfComponents(1);
    surfaceCenterlineRadiusArray->SetNumberOfTuples(numberOfInputPoints);
    surfaceCenterlineRadiusArray->FillComponent(0,0.0);
    
    output->GetPointData()->AddArray(surfaceCenterlineRadiusArray);
    }

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(this->Centerlines);
  tube->SetUseRadiusInformation(this->UseRadiusInformation);
  if (this->UseRadiusInformation)
    {
    tube->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);
    }

  if (this->ProjectPointArrays) 
    {
    output->GetPointData()->InterpolateAllocate(this->Centerlines->GetPointData(),numberOfInputPoints);
    }

  double point[3], centerlinePoint[3];
  double distanceToCenterlines = 0.0;
  for (int i=0; i<numberOfInputPoints; i++)
    {
    input->GetPoint(i,point);
    double tubeFunctionValue = tube->EvaluateFunction(point);
    if (this->EvaluateTubeFunction)
      {
      distanceToCenterlines = tubeFunctionValue;
      }
    else
      {
      tube->GetLastPolyBallCenter(centerlinePoint);
      distanceToCenterlines = sqrt(vtkMath::Distance2BetweenPoints(point,centerlinePoint));
      }
    distanceToCenterlinesArray->SetComponent(i,0,distanceToCenterlines);
    if (this->EvaluateCenterlineRadius)
      {
      surfaceCenterlineRadiusArray->SetComponent(i,0,tube->GetLastPolyBallCenterRadius());
      }

    if (this->ProjectPointArrays) 
      {
      vtkIdType cellId = tube->GetLastPolyBallCellId();
      vtkIdType subId = tube->GetLastPolyBallCellSubId();
      vtkIdType pcoord = tube->GetLastPolyBallCellPCoord();
      vtkCell* cell = this->Centerlines->GetCell(cellId);
      vtkIdType pointId0 = cell->GetPointId(subId);
      vtkIdType pointId1 = cell->GetPointId(subId+1);
      output->GetPointData()->InterpolateEdge(this->Centerlines->GetPointData(),i,pointId0,pointId1,pcoord);
      }
    }

  tube->Delete();
  distanceToCenterlinesArray->Delete();
  if (surfaceCenterlineRadiusArray)
    {
    surfaceCenterlineRadiusArray->Delete();
    }

  return 1;
}
