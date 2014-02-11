/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBranchGeometry.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
Version:   $Revision: 1.12 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineBranchGeometry.h"
#include "vtkvmtkCenterlineGeometry.h"
#include "vtkvmtkCenterlineSmoothing.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkvmtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"

// #define SPIRAL_TEST


vtkStandardNewMacro(vtkvmtkCenterlineBranchGeometry);

vtkvmtkCenterlineBranchGeometry::vtkvmtkCenterlineBranchGeometry()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->LengthArrayName = NULL;
  this->CurvatureArrayName = NULL; 
  this->TorsionArrayName = NULL;
  this->TortuosityArrayName = NULL;

  this->MinSubsamplingSpacing = 0.1;
  this->SmoothingFactor = 0.01;
  this->NumberOfSmoothingIterations = 10;

  this->LineSubsampling = 1;
  this->SphereSubsampling = 1;
  this->LineSmoothing = 0;
}

vtkvmtkCenterlineBranchGeometry::~vtkvmtkCenterlineBranchGeometry()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->LengthArrayName)
    {
    delete[] this->LengthArrayName;
    this->LengthArrayName = NULL;
    }

  if (this->CurvatureArrayName)
    {
    delete[] this->CurvatureArrayName;
    this->CurvatureArrayName = NULL;
    }

  if (this->TorsionArrayName)
    {
    delete[] this->TorsionArrayName;
    this->TorsionArrayName = NULL;
    }

  if (this->TortuosityArrayName)
    {
    delete[] this->TortuosityArrayName;
    this->TortuosityArrayName = NULL;
    }
}

int vtkvmtkCenterlineBranchGeometry::RequestData(
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

  if (!this->RadiusArrayName)
    {
    vtkErrorMacro(<<"RadiusArrayName not specified");
    return 1;
    }

  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);

  if (!radiusArray)
    {
    vtkErrorMacro(<<"RadiusArray with name specified does not exist");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<<"BlankingArrayName not specified");
    return 1;
    }

  vtkDataArray* blankingArray = input->GetCellData()->GetArray(this->BlankingArrayName);

  if (!blankingArray)
    {
    vtkErrorMacro(<<"BlankingArray with name specified does not exist");
    return 1;
    }

  if (!this->LengthArrayName)
    {
    vtkErrorMacro(<<"LengthArrayName not specified");
    return 1;
    }

  if (!this->CurvatureArrayName)
    {
    vtkErrorMacro(<<"CurvatureArrayName not specified");
    return 1;
    }

  if (!this->TorsionArrayName)
    {
    vtkErrorMacro(<<"TorsionArrayName not specified");
    return 1;
    }

  if (!this->TortuosityArrayName)
    {
    vtkErrorMacro(<<"TortuosityArrayName not specified");
    return 1;
    }

  vtkDoubleArray* lengthArray = vtkDoubleArray::New();
  lengthArray->SetName(this->LengthArrayName);

  vtkDoubleArray* curvatureArray = vtkDoubleArray::New();
  curvatureArray->SetName(this->CurvatureArrayName);

  vtkDoubleArray* torsionArray = vtkDoubleArray::New();
  torsionArray->SetName(this->TorsionArrayName);

  vtkDoubleArray* tortuosityArray = vtkDoubleArray::New();
  tortuosityArray->SetName(this->TortuosityArrayName);

  vtkDoubleArray* branchGroupIdsArray = vtkDoubleArray::New();
  branchGroupIdsArray->SetName(this->GroupIdsArrayName);

  output->GetPointData()->AddArray(lengthArray);
  output->GetPointData()->AddArray(curvatureArray);
  output->GetPointData()->AddArray(torsionArray);
  output->GetPointData()->AddArray(tortuosityArray);
  output->GetPointData()->AddArray(branchGroupIdsArray);

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputVerts = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetVerts(outputVerts);

  vtkIdList* nonBlankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetNonBlankedGroupsIdList(input,this->GroupIdsArrayName,this->BlankingArrayName,nonBlankedGroupIds);
  for (int i=0; i<nonBlankedGroupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = nonBlankedGroupIds->GetId(i);
            
    double groupLength = this->ComputeGroupLength(input,groupId);
    double groupCurvature = this->ComputeGroupCurvature(input,groupId);
    double groupTorsion = this->ComputeGroupTorsion(input,groupId);
    double groupTortuosity = this->ComputeGroupTortuosity(input,groupId,groupLength);

    branchGroupIdsArray->InsertNextTuple1(groupId);
    lengthArray->InsertNextTuple1(groupLength);
    curvatureArray->InsertNextTuple1(groupCurvature);
    torsionArray->InsertNextTuple1(groupTorsion);
    tortuosityArray->InsertNextTuple1(groupTortuosity);

    double zeroPoint[3];
    zeroPoint[0] = zeroPoint[1] = zeroPoint[2] = 0.0;
    vtkIdType pointId = outputPoints->InsertNextPoint(zeroPoint);
    outputVerts->InsertNextCell(1);
    outputVerts->InsertCellPoint(pointId);
    }
  nonBlankedGroupIds->Delete();

  outputPoints->Delete();
  outputVerts->Delete();

  lengthArray->Delete();
  curvatureArray->Delete();
  torsionArray->Delete();
  tortuosityArray->Delete();
  branchGroupIdsArray->Delete();

  return 1;
}

double vtkvmtkCenterlineBranchGeometry::ComputeGroupLength(vtkPolyData* input, int branchGroupId)
{
  double groupLength = 0.0;

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  int numberOfCells = input->GetNumberOfCells();

  double lengthWeightSum = 0.0;

  for (int i=0; i<numberOfCells; i++)
    {
    int groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));

    if (groupId != branchGroupId)
      {
      continue;
      }

    int numberOfCellPoints = input->GetCell(i)->GetNumberOfPoints();

    double length = 0.0;

    double point0[3], point1[3];
    for (int j=0; j<numberOfCellPoints-1; j++)
      {
      input->GetCell(i)->GetPoints()->GetPoint(j,point0);
      input->GetCell(i)->GetPoints()->GetPoint(j+1,point1);

      length += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
      }

    groupLength += length;
    lengthWeightSum += 1.0;
    }

  groupLength /= lengthWeightSum;
  
  return groupLength;
}

double vtkvmtkCenterlineBranchGeometry::ComputeGroupCurvature(vtkPolyData* input, int branchGroupId)
{
  double groupCurvature = 0.0;

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  int numberOfCells = input->GetNumberOfCells();

  double groupCurvatureWeightSum = 0.0;

  for (int i=0; i<numberOfCells; i++)
    {
    int groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));

    if (groupId != branchGroupId)
      {
      continue;
      }

    vtkPoints* linePoints = vtkPoints::New();
#ifndef SPIRAL_TEST
    linePoints->DeepCopy(input->GetCell(i)->GetPoints());
#else
    int numberOfSectors = 5;
    for (int j=0; j<numberOfSectors; j++)
      {
      double point[3];
      point[0] = cos(j*2.0*vtkMath::Pi()/numberOfSectors);
      point[1] = sin(j*2.0*vtkMath::Pi()/numberOfSectors);
      point[2] = j*(2.0*vtkMath::Pi()/numberOfSectors);
      linePoints->InsertNextPoint(point);
      }
#endif

    if (this->LineSmoothing)
      {
      vtkPoints* smoothLinePoints = vtkPoints::New();
      vtkvmtkCenterlineSmoothing::SmoothLine(linePoints,smoothLinePoints,this->NumberOfSmoothingIterations,this->SmoothingFactor);
      linePoints->DeepCopy(smoothLinePoints);
      smoothLinePoints->Delete();
      }

    if (this->LineSubsampling)
      {
      vtkPoints* subsampledLinePoints = vtkPoints::New();
      if (this->SphereSubsampling)
        {
        this->SphereSubsampleLine(input,i,subsampledLinePoints);
        }
      else
        {
        this->SubsampleLine(linePoints,subsampledLinePoints,this->MinSubsamplingSpacing);
        }
      linePoints->DeepCopy(subsampledLinePoints);
      subsampledLinePoints->Delete();
      }

    vtkDoubleArray* curvatureArray = vtkDoubleArray::New();
    double curvature = vtkvmtkCenterlineGeometry::ComputeLineCurvature(linePoints,curvatureArray);

    curvatureArray->Delete();

    groupCurvature += curvature;
    groupCurvatureWeightSum += 1.0;
    }

  groupCurvature /= groupCurvatureWeightSum;

//   cout<<"Group curvature:"<<groupCurvature<<endl;

  return groupCurvature;
}

double vtkvmtkCenterlineBranchGeometry::ComputeGroupTorsion(vtkPolyData* input, int branchGroupId)
{
  double groupTorsion = 0.0;

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  int numberOfCells = input->GetNumberOfCells();

  double groupTorsionWeightSum = 0.0;

  for (int i=0; i<numberOfCells; i++)
    {
    int groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));

    if (groupId != branchGroupId)
      {
      continue;
      }

    vtkPoints* linePoints = vtkPoints::New();
#ifndef SPIRAL_TEST
    linePoints->DeepCopy(input->GetCell(i)->GetPoints());
#else
    int numberOfSectors = 5;
    for (int j=0; j<numberOfSectors; j++)
      {
      double point[3];
      point[0] = cos(j*2.0*vtkMath::Pi()/numberOfSectors);
      point[1] = sin(j*2.0*vtkMath::Pi()/numberOfSectors);
      point[2] = j*(2.0*vtkMath::Pi()/numberOfSectors);
      linePoints->InsertNextPoint(point);
      }
#endif

    if (this->LineSmoothing)
      {
      vtkPoints* smoothLinePoints = vtkPoints::New();
      vtkvmtkCenterlineSmoothing::SmoothLine(linePoints,smoothLinePoints,this->NumberOfSmoothingIterations,this->SmoothingFactor);
      linePoints->DeepCopy(smoothLinePoints);
      smoothLinePoints->Delete();
      }

    if (this->LineSubsampling)
      {
      vtkPoints* subsampledLinePoints = vtkPoints::New();
      if (this->SphereSubsampling)
        {
        this->SphereSubsampleLine(input,i,subsampledLinePoints);
        }
      else
        {
        this->SubsampleLine(linePoints,subsampledLinePoints,this->MinSubsamplingSpacing);
        }
      linePoints->DeepCopy(subsampledLinePoints);
      subsampledLinePoints->Delete();
      }

    vtkDoubleArray* torsionArray = vtkDoubleArray::New();
    double torsion = vtkvmtkCenterlineGeometry::ComputeLineTorsion(linePoints,torsionArray);

    torsionArray->Delete();

    linePoints->Delete();

    groupTorsion += torsion;
    groupTorsionWeightSum += 1.0;
    }

  groupTorsion /= groupTorsionWeightSum;

//   cout<<"Group torsion:"<<groupTorsion<<endl;
  
  return groupTorsion;
}


void vtkvmtkCenterlineBranchGeometry::SubsampleLine(vtkPoints* linePoints, vtkPoints* subsampledLinePoints, double minSpacing)
{
  int numberOfPoints = linePoints->GetNumberOfPoints();

  double point0[3];
  double point1[3];

  double spacing = 0.0;
  linePoints->GetPoint(0,point0);
  subsampledLinePoints->InsertNextPoint(point0);
  for (int j=1; j<numberOfPoints-1; j++)
    {
    linePoints->GetPoint(j,point1);

    spacing += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));

    if (spacing < minSpacing)
      {
      continue;
      }
    
    subsampledLinePoints->InsertNextPoint(point1);
    linePoints->GetPoint(j,point0);
    spacing = 0.0;
    }

  linePoints->GetPoint(numberOfPoints-1,point1);
  
  spacing = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));

  if (spacing<minSpacing)
    {
    int numberOfSubsampledPoints = subsampledLinePoints->GetNumberOfPoints();
    subsampledLinePoints->InsertPoint(numberOfSubsampledPoints-1,point1);
    }
  else
    {
    subsampledLinePoints->InsertNextPoint(point1);
    }
}

void vtkvmtkCenterlineBranchGeometry::SphereSubsampleLine(vtkPolyData* input, vtkIdType cellId, vtkPoints* subsampledLinePoints)
{
  vtkIdType subId = 0;
  double pcoord = 0.0;

  vtkIdType touchingSubId = 0;
  double touchingPCoord = 0.0;

  double point[3];
  double point0[3];
  double point1[3];

  subsampledLinePoints->InsertNextPoint(input->GetCell(cellId)->GetPoints()->GetPoint(0));

  while (touchingSubId != -1)
    {
    vtkvmtkCenterlineSphereDistance::FindTouchingSphereCenter(input, this->RadiusArrayName, cellId, subId, pcoord, touchingSubId, touchingPCoord, false);

    if (touchingSubId == -1)
      {
      break;
      }

    input->GetCell(cellId)->GetPoints()->GetPoint(touchingSubId,point0);
    input->GetCell(cellId)->GetPoints()->GetPoint(touchingSubId+1,point1);

    point[0] = (1.0 - touchingPCoord) * point0[0] + touchingPCoord * point1[0];
    point[1] = (1.0 - touchingPCoord) * point0[1] + touchingPCoord * point1[1];
    point[2] = (1.0 - touchingPCoord) * point0[2] + touchingPCoord * point1[2];

    subsampledLinePoints->InsertNextPoint(point);

    subId = touchingSubId;
    pcoord = touchingPCoord;
    }  

  subsampledLinePoints->InsertNextPoint(input->GetCell(cellId)->GetPoints()->GetPoint(input->GetCell(cellId)->GetNumberOfPoints()-1));

}

double vtkvmtkCenterlineBranchGeometry::ComputeGroupTortuosity(vtkPolyData* input, int branchGroupId, double groupLength)
{
  double groupTortuosity = 0.0;

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);
  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);

  int numberOfCells = input->GetNumberOfCells();

  double averageFirstPoint[3], averageLastPoint[3];
  averageFirstPoint[0] = averageFirstPoint[1] = averageFirstPoint[2] = 0.0;
  averageLastPoint[0] = averageLastPoint[1] = averageLastPoint[2] = 0.0;
  double firstPointWeightSum = 0.0;
  double lastPointWeightSum = 0.0;

  for (int i=0; i<numberOfCells; i++)
    {
    int groupId = static_cast<int>(groupIdsArray->GetComponent(i,0));

    if (groupId != branchGroupId)
      {
      continue;
      }

    int numberOfCellPoints = input->GetCell(i)->GetNumberOfPoints();

    double firstPoint[3], lastPoint[3];

    input->GetCell(i)->GetPoints()->GetPoint(0,firstPoint);
    input->GetCell(i)->GetPoints()->GetPoint(numberOfCellPoints-1,lastPoint);

    double firstPointRadius = radiusArray->GetComponent(input->GetCell(i)->GetPointId(0),0);
    double lastPointRadius = radiusArray->GetComponent(input->GetCell(i)->GetPointId(numberOfCellPoints-1),0);

    averageFirstPoint[0] += firstPointRadius * firstPointRadius * firstPoint[0];
    averageFirstPoint[1] += firstPointRadius * firstPointRadius * firstPoint[1];
    averageFirstPoint[2] += firstPointRadius * firstPointRadius * firstPoint[2];

    firstPointWeightSum += firstPointRadius * firstPointRadius;

    averageLastPoint[0] += lastPointRadius * lastPointRadius * lastPoint[0];
    averageLastPoint[1] += lastPointRadius * lastPointRadius * lastPoint[1];
    averageLastPoint[2] += lastPointRadius * lastPointRadius * lastPoint[2];

    lastPointWeightSum += lastPointRadius * lastPointRadius;
    }

  averageFirstPoint[0] /= firstPointWeightSum;
  averageFirstPoint[1] /= firstPointWeightSum;
  averageFirstPoint[2] /= firstPointWeightSum;

  averageLastPoint[0] /= lastPointWeightSum;
  averageLastPoint[1] /= lastPointWeightSum;
  averageLastPoint[2] /= lastPointWeightSum;

  groupTortuosity = groupLength / sqrt(vtkMath::Distance2BetweenPoints(averageFirstPoint,averageLastPoint)) - 1.0;
  
  return groupTortuosity;
}

void vtkvmtkCenterlineBranchGeometry::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

