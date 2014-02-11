/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineMetricFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/31 15:07:48 $
  Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataCenterlineMetricFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"



vtkvmtkPolyDataCenterlineMetricFilter::vtkvmtkPolyDataCenterlineMetricFilter() 
{
  this->MetricArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->GroupIdsArrayName = NULL;

  this->Centerlines = NULL;

  this->RadiusArrayName = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->CenterlineTractIdsArrayName = NULL;

  this->UseRadiusInformation = 1;
  this->IncludeBifurcations = 1;
}

vtkvmtkPolyDataCenterlineMetricFilter::~vtkvmtkPolyDataCenterlineMetricFilter()
{
  if (this->MetricArrayName)
    {
    delete[] this->MetricArrayName;
    this->MetricArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->CenterlineGroupIdsArrayName)
    {
    delete[] this->CenterlineGroupIdsArrayName;
    this->CenterlineGroupIdsArrayName = NULL;
    }

  if (this->CenterlineTractIdsArrayName)
    {
    delete[] this->CenterlineGroupIdsArrayName;
    this->CenterlineGroupIdsArrayName = NULL;
    }

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }
}

int vtkvmtkPolyDataCenterlineMetricFilter::RequestData(
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

  if (!this->MetricArrayName)
    {
    vtkErrorMacro(<<"MetricArrayName not set.");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetPointData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist.");
    return 1;
    }

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  if (this->UseRadiusInformation)
    {
    if (!this->RadiusArrayName)
      {
      vtkErrorMacro(<<"RadiusArrayName not set.");
      return 1;
      }

    vtkDataArray* radiusArray = this->Centerlines->GetPointData()->GetArray(this->RadiusArrayName);
    
    if (!radiusArray)
      {
      vtkErrorMacro(<<"RadiusArray with name specified does not exist.");
      return 1;
      }
    }

  if (this->IncludeBifurcations)
    {
    if (!this->BlankingArrayName)
      {
      vtkErrorMacro(<<"BlankingArrayName not set.");
      return 1;
      }

    vtkDataArray* blankingArray = this->Centerlines->GetCellData()->GetArray(this->BlankingArrayName);
    
    if (!blankingArray)
      {
      vtkErrorMacro(<<"BlankingArray with name specified does not exist.");
      return 1;
      }

    if (!this->CenterlineIdsArrayName)
      {
      vtkErrorMacro(<<"CenterlineIdsArrayName not set.");
      return 1;
      }

    vtkDataArray* centerlineIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineIdsArrayName);
    
    if (!centerlineIdsArray)
      {
      vtkErrorMacro(<<"CenterlineIdsArray with name specified does not exist.");
      return 1;
      }

    if (!this->CenterlineTractIdsArrayName)
      {
      vtkErrorMacro(<<"CenterlineTractIdsArrayName not set.");
      return 1;
      }

    vtkDataArray* centerlineTractIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineTractIdsArrayName);
    
    if (!centerlineTractIdsArray)
      {
      vtkErrorMacro(<<"CenterlineTractIdsArray with name specified does not exist.");
      return 1;
      }
    }
  
  if (!this->CenterlineGroupIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* centerlineGroupIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineGroupIdsArrayName);

  if (!centerlineGroupIdsArray)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArrayName with name specified does not exist.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkDoubleArray* metricArray = vtkDoubleArray::New();
  metricArray->SetName(this->MetricArrayName);
  metricArray->SetNumberOfComponents(1);
  metricArray->SetNumberOfTuples(numberOfInputPoints);
  metricArray->FillComponent(0,0.0);
  
  output->GetPointData()->AddArray(metricArray);

  vtkIdType groupId;
  for (int i=0; i<numberOfInputPoints; i++)
    {
    groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));
    this->EvaluateMetric(i,input->GetPoint(i),groupId,metricArray);
    }

  metricArray->Delete();

  return 1;
}
