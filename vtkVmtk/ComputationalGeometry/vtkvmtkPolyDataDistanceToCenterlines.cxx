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

vtkCxxRevisionMacro(vtkvmtkPolyDataDistanceToCenterlines, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkPolyDataDistanceToCenterlines);

vtkvmtkPolyDataDistanceToCenterlines::vtkvmtkPolyDataDistanceToCenterlines() 
{
  this->DistanceToCenterlinesArrayName = NULL;

  this->Centerlines = NULL;
  this->CenterlineRadiusArrayName = NULL;

  this->UseRadiusInformation = 1;
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

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(this->Centerlines);
  tube->SetUseRadiusInformation(this->UseRadiusInformation);
  if (this->UseRadiusInformation)
    {
    tube->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);
    }

  double point[3], centerlinePoint[3];
  double distanceToCenterlines = 0.0;
  for (int i=0; i<numberOfInputPoints; i++)
    {
    input->GetPoint(i,point);
    tube->EvaluateFunction(point);
    tube->GetLastPolyBallCenter(centerlinePoint);
    distanceToCenterlines = sqrt(vtkMath::Distance2BetweenPoints(point,centerlinePoint));
    distanceToCenterlinesArray->SetComponent(i,0,distanceToCenterlines);
    }

  tube->Delete();
  distanceToCenterlinesArray->Delete();

  return 1;
}
