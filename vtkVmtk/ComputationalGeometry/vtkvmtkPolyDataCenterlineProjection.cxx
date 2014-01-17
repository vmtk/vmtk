/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineProjection.cxx,v $
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

#include "vtkvmtkPolyDataCenterlineProjection.h"
#include "vtkPointData.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineProjection);

vtkvmtkPolyDataCenterlineProjection::vtkvmtkPolyDataCenterlineProjection() 
{
  this->Centerlines = NULL;
  this->CenterlineRadiusArrayName = NULL;

  this->UseRadiusInformation = 1;
}

vtkvmtkPolyDataCenterlineProjection::~vtkvmtkPolyDataCenterlineProjection()
{
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

int vtkvmtkPolyDataCenterlineProjection::RequestData(
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

  vtkPointData* outputPointData = output->GetPointData();
  vtkPointData* referencePointData = this->Centerlines->GetPointData();

  outputPointData->InterpolateAllocate(referencePointData,numberOfInputPoints);

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(this->Centerlines);
  tube->SetUseRadiusInformation(this->UseRadiusInformation);
  if (this->UseRadiusInformation)
    {
    tube->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);
    }

  double point[3];
  for (int i=0; i<numberOfInputPoints; i++)
    {
    input->GetPoint(i,point);
    tube->EvaluateFunction(point);
    vtkIdType cellId = tube->GetLastPolyBallCellId();
    vtkIdType subId = tube->GetLastPolyBallCellSubId();
    double pcoord = tube->GetLastPolyBallCellPCoord();
    vtkCell* cell = this->Centerlines->GetCell(cellId);
//  FIXME: for some reason EvaluateLocation fails on the PolyLine - weights are funny. Copy from the closest subId for the moment.
//    double pcoords[3];
//    pcoords[0] = pcoord;
//    pcoords[1] = pcoords[2] = 0.0;
//    double* weights = new double[cell->GetNumberOfPoints()];
//    double locationPoint[3];
//    cell->EvaluateLocation(subId,pcoords,locationPoint,weights);
//    outputPointData->InterpolatePoint(referencePointData,i,cell->GetPointIds(),weights); 
//    delete[] weights;
    if (pcoord < 0.5)
      {
      outputPointData->CopyData(referencePointData,cell->GetPointId(subId),i);
      }
    else
      {
      outputPointData->CopyData(referencePointData,cell->GetPointId(subId+1),i);
      }
    }

  tube->Delete();

  return 1;
}
