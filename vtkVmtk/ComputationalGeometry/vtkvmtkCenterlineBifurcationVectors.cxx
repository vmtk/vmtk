/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBifurcationVectors.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineBifurcationVectors.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPolyLine.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkReferenceSystemUtilities.h"


vtkStandardNewMacro(vtkvmtkCenterlineBifurcationVectors);

vtkvmtkCenterlineBifurcationVectors::vtkvmtkCenterlineBifurcationVectors()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->TractIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->ReferenceSystems = NULL;

  this->ReferenceSystemGroupIdsArrayName = NULL;
  this->ReferenceSystemNormalArrayName = NULL;
  this->ReferenceSystemUpNormalArrayName = NULL;

  this->BifurcationVectorsArrayName = NULL;
  this->InPlaneBifurcationVectorsArrayName = NULL;
  this->OutOfPlaneBifurcationVectorsArrayName = NULL;
  this->BifurcationVectorsOrientationArrayName = NULL;
  this->BifurcationGroupIdsArrayName = NULL;

  this->InPlaneBifurcationVectorAnglesArrayName = NULL;
  this->OutOfPlaneBifurcationVectorAnglesArrayName = NULL;

  this->NormalizeBifurcationVectors = false;
}

vtkvmtkCenterlineBifurcationVectors::~vtkvmtkCenterlineBifurcationVectors()
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

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }

  if (this->TractIdsArrayName)
    {
    delete[] this->TractIdsArrayName;
    this->TractIdsArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->ReferenceSystems)
    {
    this->ReferenceSystems->Delete();
    }

  if (this->ReferenceSystemGroupIdsArrayName)
    {
    delete[] this->ReferenceSystemGroupIdsArrayName;
    this->ReferenceSystemGroupIdsArrayName = NULL;
    }

  if (this->ReferenceSystemNormalArrayName)
    {
    delete[] this->ReferenceSystemNormalArrayName;
    this->ReferenceSystemNormalArrayName = NULL;
    }

  if (this->ReferenceSystemUpNormalArrayName)
    {
    delete[] this->ReferenceSystemUpNormalArrayName;
    this->ReferenceSystemUpNormalArrayName = NULL;
    }

  if (this->BifurcationVectorsArrayName)
  {
    delete[] this->BifurcationVectorsArrayName;
    this->BifurcationVectorsArrayName = NULL;
  }

  if (this->InPlaneBifurcationVectorsArrayName)
  {
    delete[] this->InPlaneBifurcationVectorsArrayName;
    this->InPlaneBifurcationVectorsArrayName = NULL;
  }

  if (this->OutOfPlaneBifurcationVectorsArrayName)
  {
    delete[] this->OutOfPlaneBifurcationVectorsArrayName;
    this->OutOfPlaneBifurcationVectorsArrayName = NULL;
  }

  if (this->InPlaneBifurcationVectorAnglesArrayName)
  {
    delete[] this->InPlaneBifurcationVectorAnglesArrayName;
    this->InPlaneBifurcationVectorAnglesArrayName = NULL;
  }

  if (this->OutOfPlaneBifurcationVectorAnglesArrayName)
  {
    delete[] this->OutOfPlaneBifurcationVectorAnglesArrayName;
    this->OutOfPlaneBifurcationVectorAnglesArrayName = NULL;
  }

  if (this->BifurcationVectorsOrientationArrayName)
  {
    delete[] this->BifurcationVectorsOrientationArrayName;
    this->BifurcationVectorsOrientationArrayName = NULL;
  }
  
  if (this->BifurcationGroupIdsArrayName)
  {
    delete[] this->BifurcationGroupIdsArrayName;
    this->BifurcationGroupIdsArrayName = NULL;
  }
}

int vtkvmtkCenterlineBifurcationVectors::RequestData(
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

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineIdsArray = input->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  if (!centerlineIdsArray)
    {
    vtkErrorMacro(<<"CenterlineIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->TractIdsArrayName)
    {
    vtkErrorMacro(<<"TractIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* tractIdsArray = input->GetCellData()->GetArray(this->TractIdsArrayName);

  if (!tractIdsArray)
    {
    vtkErrorMacro(<<"TractIdsArray with name specified does not exist");
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

  if (!this->ReferenceSystemGroupIdsArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemGroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* referenceSystemGroupIdsArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemGroupIdsArrayName);

  if (!referenceSystemGroupIdsArray)
    {
    vtkErrorMacro(<<"ReferenceSystemGroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->ReferenceSystemNormalArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemNormalArrayName not specified");
    return 1;
    }

  vtkDataArray* referenceSystemNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemNormalArrayName);

  if (!referenceSystemNormalArray)
    {
    vtkErrorMacro(<<"ReferenceSystemNormalArray with name specified does not exist");
    return 1;
    }

  if (!this->ReferenceSystemUpNormalArrayName)
    {
    vtkErrorMacro(<<"ReferenceSystemUpNormalArrayName not specified");
    return 1;
    }

  vtkDataArray* referenceSystemUpNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemUpNormalArrayName);

  if (!referenceSystemUpNormalArray)
    {
    vtkErrorMacro(<<"ReferenceSystemUpNormalArray with name specified does not exist");
    return 1;
    }

  if (!this->BifurcationVectorsArrayName)
    {
    vtkErrorMacro(<<"BifurcationVectorsArrayName not specified");
    return 1;
    }

  if (!this->InPlaneBifurcationVectorsArrayName)
    {
    vtkErrorMacro(<<"InPlaneBifurcationVectorsArrayName not specified");
    return 1;
    }

  if (!this->OutOfPlaneBifurcationVectorsArrayName)
    {
    vtkErrorMacro(<<"OutOfPlaneBifurcationVectorsArrayName not specified");
    return 1;
    }

  if (!this->BifurcationVectorsOrientationArrayName)
    {
    vtkErrorMacro(<<"BifurcationVectorsOrientationArrayName not specified");
    return 1;
    }

  if (!this->BifurcationGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BifurcationGroupIdsArrayName not specified");
    return 1;
    }

  if (!this->InPlaneBifurcationVectorAnglesArrayName)
    {
    vtkErrorMacro(<<"InPlaneBifurcationVectorAnglesArrayName not specified");
    return 1;
    }

  if (!this->OutOfPlaneBifurcationVectorAnglesArrayName)
    {
    vtkErrorMacro(<<"OutOfPlaneBifurcationVectorAnglesArrayName not specified");
    return 1;
    }

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputVerts = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetVerts(outputVerts);

  vtkDoubleArray* bifurcationVectorsArray = vtkDoubleArray::New();
  bifurcationVectorsArray->SetName(this->BifurcationVectorsArrayName);
  bifurcationVectorsArray->SetNumberOfComponents(3);
  
  vtkDoubleArray* inPlaneBifurcationVectorsArray = vtkDoubleArray::New();
  inPlaneBifurcationVectorsArray->SetName(this->InPlaneBifurcationVectorsArrayName);
  inPlaneBifurcationVectorsArray->SetNumberOfComponents(3);
  
  vtkDoubleArray* outOfPlaneBifurcationVectorsArray = vtkDoubleArray::New();
  outOfPlaneBifurcationVectorsArray->SetName(this->OutOfPlaneBifurcationVectorsArrayName);
  outOfPlaneBifurcationVectorsArray->SetNumberOfComponents(3);

  vtkDoubleArray* inPlaneBifurcationVectorAnglesArray = vtkDoubleArray::New();
  inPlaneBifurcationVectorAnglesArray->SetName(this->InPlaneBifurcationVectorAnglesArrayName);
  inPlaneBifurcationVectorAnglesArray->SetNumberOfComponents(1);
  
  vtkDoubleArray* outOfPlaneBifurcationVectorAnglesArray = vtkDoubleArray::New();
  outOfPlaneBifurcationVectorAnglesArray->SetName(this->OutOfPlaneBifurcationVectorAnglesArrayName);
  outOfPlaneBifurcationVectorAnglesArray->SetNumberOfComponents(1);

  vtkIntArray* bifurcationVectorsOrientationArray = vtkIntArray::New();
  bifurcationVectorsOrientationArray->SetName(this->BifurcationVectorsOrientationArrayName);
  bifurcationVectorsOrientationArray->SetNumberOfComponents(1);

  vtkIntArray* bifurcationVectorsGroupIdsArray = vtkIntArray::New();
  bifurcationVectorsGroupIdsArray->SetName(this->GroupIdsArrayName);
  bifurcationVectorsGroupIdsArray->SetNumberOfComponents(1);

  vtkIntArray* bifurcationVectorsBifurcationGroupIdsArray = vtkIntArray::New();
  bifurcationVectorsBifurcationGroupIdsArray->SetName(this->BifurcationGroupIdsArrayName);
  bifurcationVectorsBifurcationGroupIdsArray->SetNumberOfComponents(1);

  output->GetPointData()->AddArray(bifurcationVectorsArray);
  output->GetPointData()->AddArray(inPlaneBifurcationVectorsArray);
  output->GetPointData()->AddArray(outOfPlaneBifurcationVectorsArray);
  output->GetPointData()->AddArray(inPlaneBifurcationVectorAnglesArray);
  output->GetPointData()->AddArray(outOfPlaneBifurcationVectorAnglesArray);
  output->GetPointData()->AddArray(bifurcationVectorsOrientationArray);
  output->GetPointData()->AddArray(bifurcationVectorsGroupIdsArray);
  output->GetPointData()->AddArray(bifurcationVectorsBifurcationGroupIdsArray);

  vtkIdList* blankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetBlankedGroupsIdList(input,this->GroupIdsArrayName,this->BlankingArrayName,blankedGroupIds);
  int i;
  for (i=0; i<blankedGroupIds->GetNumberOfIds(); i++)
  {
    vtkIdType bifurcationGroupId = blankedGroupIds->GetId(i);
    
    vtkIdList* groupBifurcationVectorsGroupIds = vtkIdList::New();
    vtkIntArray* groupBifurcationVectorsOrientation = vtkIntArray::New();
    vtkDoubleArray* groupBifurcationVectors = vtkDoubleArray::New();
    vtkPoints* groupBifurcationVectorsPoints = vtkPoints::New();
    vtkDoubleArray* groupInPlaneBifurcationVectors = vtkDoubleArray::New();
    vtkDoubleArray* groupOutOfPlaneBifurcationVectors = vtkDoubleArray::New();
    vtkDoubleArray* groupInPlaneBifurcationVectorAngles = vtkDoubleArray::New();
    vtkDoubleArray* groupOutOfPlaneBifurcationVectorAngles = vtkDoubleArray::New();

    this->ComputeBifurcationVectors(input,bifurcationGroupId,groupBifurcationVectorsGroupIds,groupBifurcationVectorsOrientation,groupBifurcationVectors,groupBifurcationVectorsPoints);
    this->ComputeBifurcationVectorComponents(bifurcationGroupId,groupBifurcationVectors,groupInPlaneBifurcationVectors,groupOutOfPlaneBifurcationVectors);
    this->ComputeBifurcationVectorAngles(bifurcationGroupId,groupBifurcationVectors,groupInPlaneBifurcationVectors,groupOutOfPlaneBifurcationVectors,groupInPlaneBifurcationVectorAngles,groupOutOfPlaneBifurcationVectorAngles);
    
    int j;
    for (j=0; j<groupBifurcationVectorsGroupIds->GetNumberOfIds(); j++)
    {
      vtkIdType pointId = outputPoints->InsertNextPoint(groupBifurcationVectorsPoints->GetPoint(j));
      outputVerts->InsertNextCell(1);
      outputVerts->InsertCellPoint(pointId);

      bifurcationVectorsArray->InsertNextTuple(groupBifurcationVectors->GetTuple(j));
      inPlaneBifurcationVectorsArray->InsertNextTuple(groupInPlaneBifurcationVectors->GetTuple(j));
      outOfPlaneBifurcationVectorsArray->InsertNextTuple(groupOutOfPlaneBifurcationVectors->GetTuple(j));
      inPlaneBifurcationVectorAnglesArray->InsertNextTuple(groupInPlaneBifurcationVectorAngles->GetTuple(j));
      outOfPlaneBifurcationVectorAnglesArray->InsertNextTuple(groupOutOfPlaneBifurcationVectorAngles->GetTuple(j));
      bifurcationVectorsOrientationArray->InsertNextValue(groupBifurcationVectorsOrientation->GetValue(j));
      bifurcationVectorsGroupIdsArray->InsertNextValue(groupBifurcationVectorsGroupIds->GetId(j));
      bifurcationVectorsBifurcationGroupIdsArray->InsertNextValue(bifurcationGroupId);
    }
    
    groupBifurcationVectorsGroupIds->Delete();
    groupBifurcationVectorsOrientation->Delete();
    groupBifurcationVectors->Delete();
    groupBifurcationVectorsPoints->Delete();
    groupInPlaneBifurcationVectors->Delete();
    groupOutOfPlaneBifurcationVectors->Delete();
    groupInPlaneBifurcationVectorAngles->Delete();
    groupOutOfPlaneBifurcationVectorAngles->Delete();
  }

  blankedGroupIds->Delete();

  outputPoints->Delete();
  outputVerts->Delete();

  bifurcationVectorsArray->Delete();
  inPlaneBifurcationVectorsArray->Delete();
  outOfPlaneBifurcationVectorsArray->Delete();
  inPlaneBifurcationVectorAnglesArray->Delete();
  outOfPlaneBifurcationVectorAnglesArray->Delete();
  bifurcationVectorsOrientationArray->Delete();
  bifurcationVectorsGroupIdsArray->Delete();
  bifurcationVectorsBifurcationGroupIdsArray->Delete();

  return 1;
}

void vtkvmtkCenterlineBifurcationVectors::ComputeBifurcationVectors(vtkPolyData* input, int bifurcationGroupId, vtkIdList* bifurcationVectorsGroupIds, vtkIntArray* bifurcationVectorsOrientation, vtkDoubleArray* bifurcationVectors, vtkPoints* bifurcationVectorsPoints)
{
  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);

  bifurcationVectorsGroupIds->Initialize();
  bifurcationVectorsOrientation->Initialize();

  vtkIdList* upStreamGroupIds = vtkIdList::New();
  vtkIdList* downStreamGroupIds = vtkIdList::New();
  
  vtkvmtkCenterlineUtilities::FindAdjacentCenterlineGroupIds(input,this->GroupIdsArrayName,this->CenterlineIdsArrayName,this->TractIdsArrayName,bifurcationGroupId,upStreamGroupIds,downStreamGroupIds);

  int numberOfUpStreamGroupIds = upStreamGroupIds->GetNumberOfIds();
  int numberOfDownStreamGroupIds = downStreamGroupIds->GetNumberOfIds();

  int i;
  for (i=0; i<numberOfUpStreamGroupIds; i++)
  {
    bifurcationVectorsGroupIds->InsertNextId(upStreamGroupIds->GetId(i));
    bifurcationVectorsOrientation->InsertNextValue(VTK_VMTK_UPSTREAM_ORIENTATION);
  }
  for (i=0; i<numberOfDownStreamGroupIds; i++)
  {
    bifurcationVectorsGroupIds->InsertNextId(downStreamGroupIds->GetId(i));
    bifurcationVectorsOrientation->InsertNextValue(VTK_VMTK_DOWNSTREAM_ORIENTATION);
  }
 
  upStreamGroupIds->Delete();
  downStreamGroupIds->Delete();

  bifurcationVectors->Initialize();
  bifurcationVectors->SetNumberOfComponents(3);

  int numberOfBifurcationVectors = bifurcationVectorsGroupIds->GetNumberOfIds();
  
  for (i=0; i<numberOfBifurcationVectors; i++)
    {
    int bifurcationVectorGroupId = bifurcationVectorsGroupIds->GetId(i);
    int bifurcationVectorOrientation = bifurcationVectorsOrientation->GetValue(i);
    double averageLastPoint[3], averageTouchingPoint[3];
    averageLastPoint[0] = averageLastPoint[1] = averageLastPoint[2] = 0.0;
    averageTouchingPoint[0] = averageTouchingPoint[1] = averageTouchingPoint[2] = 0.0;
    double lastPointWeightSum = 0.0;
    double touchingPointWeightSum = 0.0;
    vtkIdList* groupCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(input,this->GroupIdsArrayName,bifurcationVectorGroupId,groupCellIds);
    for (int j=0; j<groupCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType cellId = groupCellIds->GetId(j);
      vtkCell* cell = input->GetCell(cellId);
      int numberOfCellPoints = cell->GetNumberOfPoints();
      
      vtkIdType touchingSubId = -1;
      double touchingPCoord = 0.0;
   
      vtkIdType pointSubId = -1;
      double pointPCoord = 0.0;
      bool forward = true;
 
      double lastPoint[3];
      double lastPointRadius = 0.0; 

      if (bifurcationVectorOrientation == VTK_VMTK_UPSTREAM_ORIENTATION)
      {
        pointSubId = numberOfCellPoints-2;
        pointPCoord = 1.0;
        forward = true;
        cell->GetPoints()->GetPoint(numberOfCellPoints-1,lastPoint);
        lastPointRadius = radiusArray->GetComponent(cell->GetPointId(numberOfCellPoints-1),0);
      }
      else if (bifurcationVectorOrientation == VTK_VMTK_DOWNSTREAM_ORIENTATION)
      {
        pointSubId = 0;
        pointPCoord = 0.0;
        forward = false;
        cell->GetPoints()->GetPoint(0,lastPoint);
        lastPointRadius = radiusArray->GetComponent(cell->GetPointId(0),0);
      }
      else
      {
        vtkErrorMacro("Error: invalid BifurcationVectorOrientation");
        return;
      }
      
      vtkvmtkCenterlineSphereDistance::FindTouchingSphereCenter(input,this->RadiusArrayName,cellId,pointSubId,pointPCoord,touchingSubId,touchingPCoord,forward);

      if (touchingSubId == -1)
        {
        touchingSubId = numberOfCellPoints-2;
        touchingPCoord = 1.0;
        }

      //accumulate last point and touching point (weighted with radius^2)

      averageLastPoint[0] += lastPointRadius * lastPointRadius * lastPoint[0];
      averageLastPoint[1] += lastPointRadius * lastPointRadius * lastPoint[1];
      averageLastPoint[2] += lastPointRadius * lastPointRadius * lastPoint[2];

      lastPointWeightSum += lastPointRadius * lastPointRadius;
      double touchingPoint[3], touchingPointRadius;
      vtkvmtkCenterlineUtilities::InterpolatePoint(input,cellId,touchingSubId,touchingPCoord,touchingPoint);
      vtkvmtkCenterlineUtilities::InterpolateTuple(input,this->RadiusArrayName,cellId,touchingSubId,touchingPCoord,&touchingPointRadius);
      
      averageTouchingPoint[0] += touchingPointRadius * touchingPointRadius * touchingPoint[0];
      averageTouchingPoint[1] += touchingPointRadius * touchingPointRadius * touchingPoint[1];
      averageTouchingPoint[2] += touchingPointRadius * touchingPointRadius * touchingPoint[2];

      touchingPointWeightSum += touchingPointRadius * touchingPointRadius;
      }
    groupCellIds->Delete();

    averageLastPoint[0] /= lastPointWeightSum;
    averageLastPoint[1] /= lastPointWeightSum;
    averageLastPoint[2] /= lastPointWeightSum;

    averageTouchingPoint[0] /= touchingPointWeightSum;
    averageTouchingPoint[1] /= touchingPointWeightSum;
    averageTouchingPoint[2] /= touchingPointWeightSum;
    
    //build and store bifurcationVector for this group

    double bifurcationVector[3], bifurcationVectorPoint[3];

    if (bifurcationVectorOrientation == VTK_VMTK_UPSTREAM_ORIENTATION)
      {
      bifurcationVector[0] = averageLastPoint[0] - averageTouchingPoint[0];
      bifurcationVector[1] = averageLastPoint[1] - averageTouchingPoint[1];
      bifurcationVector[2] = averageLastPoint[2] - averageTouchingPoint[2];
      bifurcationVectorPoint[0] = averageTouchingPoint[0]; 
      bifurcationVectorPoint[1] = averageTouchingPoint[1]; 
      bifurcationVectorPoint[2] = averageTouchingPoint[2]; 
      }
    else if (bifurcationVectorOrientation == VTK_VMTK_DOWNSTREAM_ORIENTATION)
      {
      bifurcationVector[0] = averageTouchingPoint[0] - averageLastPoint[0];
      bifurcationVector[1] = averageTouchingPoint[1] - averageLastPoint[1];
      bifurcationVector[2] = averageTouchingPoint[2] - averageLastPoint[2];
      bifurcationVectorPoint[0] = averageLastPoint[0]; 
      bifurcationVectorPoint[1] = averageLastPoint[1]; 
      bifurcationVectorPoint[2] = averageLastPoint[2]; 
      }
    else
      {
      vtkErrorMacro("Error: invalid BifurcationVectorOrientation");
      return;
      }

    if (this->NormalizeBifurcationVectors)
      {
      vtkMath::Normalize(bifurcationVector);
      }

    bifurcationVectors->InsertNextTuple(bifurcationVector);
    bifurcationVectorsPoints->InsertNextPoint(bifurcationVectorPoint);
    }
}

void vtkvmtkCenterlineBifurcationVectors::ComputeBifurcationVectorComponents(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors)
{
  // get the reference system, get the normal, project the angles.

  vtkDataArray* referenceSystemNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemNormalArrayName);
  vtkDataArray* referenceSystemUpNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemUpNormalArrayName);

  inPlaneBifurcationVectors->Initialize();
  inPlaneBifurcationVectors->SetNumberOfComponents(3);
  outOfPlaneBifurcationVectors->Initialize();
  outOfPlaneBifurcationVectors->SetNumberOfComponents(3);
  
  int referenceSystemPointId = vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(this->ReferenceSystems,this->ReferenceSystemGroupIdsArrayName,bifurcationGroupId); 
  
  double bifurcationPlaneNormal[3];
  referenceSystemNormalArray->GetTuple(referenceSystemPointId,bifurcationPlaneNormal);

  double bifurcationUpNormal[3];
  referenceSystemUpNormalArray->GetTuple(referenceSystemPointId,bifurcationUpNormal);

  double bifurcationVector[3], inPlaneBifurcationVector[3], outOfPlaneBifurcationVector[3];

  int numberOfBifurcationVectors = bifurcationVectors->GetNumberOfTuples();

  int j;
  for (j=0; j<numberOfBifurcationVectors; j++)
    {
    bifurcationVectors->GetTuple(j,bifurcationVector);

    double dot = vtkMath::Dot(bifurcationVector,bifurcationPlaneNormal);

    outOfPlaneBifurcationVector[0] = dot * bifurcationPlaneNormal[0];
    outOfPlaneBifurcationVector[1] = dot * bifurcationPlaneNormal[1];
    outOfPlaneBifurcationVector[2] = dot * bifurcationPlaneNormal[2];

    inPlaneBifurcationVector[0] = bifurcationVector[0] - outOfPlaneBifurcationVector[0];
    inPlaneBifurcationVector[1] = bifurcationVector[1] - outOfPlaneBifurcationVector[1];
    inPlaneBifurcationVector[2] = bifurcationVector[2] - outOfPlaneBifurcationVector[2];
  
    inPlaneBifurcationVectors->InsertNextTuple(inPlaneBifurcationVector);
    outOfPlaneBifurcationVectors->InsertNextTuple(outOfPlaneBifurcationVector);
    }
}

void vtkvmtkCenterlineBifurcationVectors::ComputeBifurcationVectorAngles(int bifurcationGroupId, vtkDoubleArray* bifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectors, vtkDoubleArray* outOfPlaneBifurcationVectors, vtkDoubleArray* inPlaneBifurcationVectorAngles, vtkDoubleArray* outOfPlaneBifurcationVectorAngles)
{
  vtkDataArray* referenceSystemNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemNormalArrayName);
  vtkDataArray* referenceSystemUpNormalArray = this->ReferenceSystems->GetPointData()->GetArray(this->ReferenceSystemUpNormalArrayName);

  int referenceSystemPointId = vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(this->ReferenceSystems,this->ReferenceSystemGroupIdsArrayName,bifurcationGroupId); 
  
  double bifurcationPlaneNormal[3], bifurcationUpNormal[3];
  
  referenceSystemNormalArray->GetTuple(referenceSystemPointId,bifurcationPlaneNormal);
  referenceSystemUpNormalArray->GetTuple(referenceSystemPointId,bifurcationUpNormal);
  
  int numberOfBifurcationVectors = bifurcationVectors->GetNumberOfTuples();
  int i;
  for (i=0; i<numberOfBifurcationVectors; i++)
  {
    double bifurcationVector[3], inPlaneBifurcationVector[3], outOfPlaneBifurcationVector[3];

    bifurcationVectors->GetTuple(i,bifurcationVector);
    inPlaneBifurcationVectors->GetTuple(i,inPlaneBifurcationVector);
    outOfPlaneBifurcationVectors->GetTuple(i,outOfPlaneBifurcationVector);
  
    vtkMath::Normalize(bifurcationVector);
    vtkMath::Normalize(inPlaneBifurcationVector);
    
    double inPlaneBifurcationVectorAngle = vtkvmtkMath::AngleBetweenNormals(inPlaneBifurcationVector,bifurcationUpNormal);
    double cross[3];
    vtkMath::Cross(inPlaneBifurcationVector,bifurcationUpNormal,cross);
    if (vtkMath::Dot(cross,bifurcationPlaneNormal) < 0.0)
      {         
      inPlaneBifurcationVectorAngle *= -1.0;         
      }
      
    double outOfPlaneBifurcationVectorAngle = vtkMath::Pi()/2.0 - vtkvmtkMath::AngleBetweenNormals(bifurcationVector,bifurcationPlaneNormal);
   
    inPlaneBifurcationVectorAngles->InsertNextValue(inPlaneBifurcationVectorAngle);
    outOfPlaneBifurcationVectorAngles->InsertNextValue(outOfPlaneBifurcationVectorAngle);
  }
}

void vtkvmtkCenterlineBifurcationVectors::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
