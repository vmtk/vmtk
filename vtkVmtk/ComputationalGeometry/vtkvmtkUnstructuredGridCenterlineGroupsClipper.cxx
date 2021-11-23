/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridCenterlineGroupsClipper.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.9 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridCenterlineGroupsClipper.h"
#include "vtkExecutive.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkClipDataSet.h"
#include "vtkvmtkAppendFilter.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridCenterlineGroupsClipper);

vtkvmtkUnstructuredGridCenterlineGroupsClipper::vtkvmtkUnstructuredGridCenterlineGroupsClipper()
{
  this->Centerlines = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineRadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->BlankingArrayName = NULL;
  this->CenterlineGroupIds = NULL;
  this->ClipAllCenterlineGroupIds = 0;
  this->CutoffRadiusFactor = VTK_VMTK_LARGE_DOUBLE;
  this->ClipValue = 0.0; 
  this->UseRadiusInformation = 1; 
 
  this->SetNumberOfOutputPorts(2);
  this->GenerateClippedOutput = 0;
  vtkUnstructuredGrid *output2 = vtkUnstructuredGrid::New();
  this->GetExecutive()->SetOutputData(1, output2);
  output2->Delete();
}

vtkvmtkUnstructuredGridCenterlineGroupsClipper::~vtkvmtkUnstructuredGridCenterlineGroupsClipper()
{
  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->CenterlineGroupIds)
    {
    this->CenterlineGroupIds->Delete();
    this->CenterlineGroupIds = NULL;
    }

  if (this->CenterlineGroupIdsArrayName)
    {
    delete[] this->CenterlineGroupIdsArrayName;
    this->CenterlineGroupIdsArrayName = NULL;
    }

  if (this->CenterlineRadiusArrayName)
    {
    delete[] this->CenterlineRadiusArrayName;
    this->CenterlineRadiusArrayName = NULL;
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
}

vtkUnstructuredGrid *vtkvmtkUnstructuredGridCenterlineGroupsClipper::GetClippedOutput()
{
  if (this->GetNumberOfOutputPorts() < 2)
    {
    return NULL;
    }

  return vtkUnstructuredGrid::SafeDownCast(this->GetExecutive()->GetOutputData(1));
}

int vtkvmtkUnstructuredGridCenterlineGroupsClipper::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray* centerlineGroupIdsArray;
  vtkIntArray* blankingArray;
  vtkIntArray* groupIdsArray;

  if (!this->Centerlines)
    {
    vtkErrorMacro(<< "Centerlines not set.");
    return 1;
    }

  if (!this->ClipAllCenterlineGroupIds && !this->CenterlineGroupIds)
    {
    vtkErrorMacro(<< "CenterlineGroupIds not set.");
    return 1;
    }

  if (!this->CenterlineGroupIdsArrayName)
    {
    vtkErrorMacro(<< "CenterlineGroupIdsArrayName not set.");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<< "GroupIdsArrayName not set.");
    return 1;
    }

  centerlineGroupIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineGroupIdsArrayName);

  if (!centerlineGroupIdsArray)
    {
    vtkErrorMacro(<< "CenterlineGroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<< "BlankingArrayName not set.");
    return 1;
    }

  blankingArray = vtkIntArray::SafeDownCast(this->Centerlines->GetCellData()->GetArray(this->BlankingArrayName));

  if (!blankingArray)
    {
    vtkErrorMacro(<< "BlankingArrayName with name specified does not exist");
    return 1;
    }

  if (!this->CenterlineRadiusArrayName)
    {
    vtkErrorMacro(<< "CenterlineRadiusArrayName not set.");
    return 1;
    }

  if (!this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName))
    {
    vtkErrorMacro(<< "CenterlineRadiusArray with name specified does not exist");
    return 1;
    }

  if (this->Centerlines->GetNumberOfCells() == 1)
    {
    output->DeepCopy(input);
    groupIdsArray = vtkIntArray::New();
    groupIdsArray->SetName(this->GroupIdsArrayName);
    groupIdsArray->SetNumberOfTuples(output->GetNumberOfPoints());
    groupIdsArray->FillComponent(0,centerlineGroupIdsArray->GetComponent(0,0));
    output->GetPointData()->AddArray(groupIdsArray);
    groupIdsArray->Delete();
    return 1;
    }
  
  vtkvmtkAppendFilter* appendBranches = vtkvmtkAppendFilter::New();
  appendBranches->MergeDuplicatePointsOff();
  vtkvmtkAppendFilter* appendClippedOutput = NULL;
  if (this->GenerateClippedOutput)
    {
    appendClippedOutput = vtkvmtkAppendFilter::New();
    appendClippedOutput->MergeDuplicatePointsOff();
    }

  // for each group, compute the clipping array, clip, add group ids array and append.

  vtkvmtkPolyBallLine* groupTubes = vtkvmtkPolyBallLine::New();
  groupTubes->SetInput(this->Centerlines);
  groupTubes->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);
  groupTubes->SetUseRadiusInformation(this->UseRadiusInformation);

  vtkvmtkPolyBallLine* nonGroupTubes = vtkvmtkPolyBallLine::New();
  nonGroupTubes->SetInput(this->Centerlines);
  nonGroupTubes->SetPolyBallRadiusArrayName(this->CenterlineRadiusArrayName);
  nonGroupTubes->SetUseRadiusInformation(this->UseRadiusInformation);

  int numberOfPoints = input->GetNumberOfPoints();

  const char clippingArrayName[] = "ClippingArray";

  vtkDoubleArray* clippingArray = vtkDoubleArray::New();
  clippingArray->SetNumberOfComponents(1);
  clippingArray->SetNumberOfTuples(numberOfPoints);
  clippingArray->FillComponent(0,0.0);
  clippingArray->SetName(clippingArrayName);

  vtkUnstructuredGrid* clippingInput = vtkUnstructuredGrid::New();
  clippingInput->DeepCopy(input);
  clippingInput->GetPointData()->AddArray(clippingArray);
  clippingInput->GetPointData()->SetActiveScalars(clippingArrayName);

  vtkIdList* groupTubesGroupIds = vtkIdList::New();
  vtkIdList* nonGroupTubesGroupIds = vtkIdList::New();

  double point[3];
  double groupTubeValue, nonGroupTubeValue, tubeDifferenceValue;

  vtkIdType groupId;

  vtkIdList* centerlineGroupIds = vtkIdList::New();
 
  int i;
  if (this->ClipAllCenterlineGroupIds)
    {
    for (i=0; i<centerlineGroupIdsArray->GetNumberOfTuples(); i++)
      {
      if (blankingArray->GetValue(i) == 1)
        {
        continue;
        }
      centerlineGroupIds->InsertUniqueId(static_cast<vtkIdType>(vtkMath::Round(centerlineGroupIdsArray->GetComponent(i,0))));
      }
    }
  else
    {
    centerlineGroupIds->DeepCopy(this->CenterlineGroupIds);
    }

  for (i=0; i<centerlineGroupIds->GetNumberOfIds(); i++)
    {
    groupId = centerlineGroupIds->GetId(i);

    groupTubesGroupIds->Initialize();
    nonGroupTubesGroupIds->Initialize();

    for (int j=0; j<this->Centerlines->GetNumberOfCells(); j++)
      {
      if (blankingArray->GetValue(j) == 1)
        {
        continue;
        }
      if (static_cast<vtkIdType>(vtkMath::Round(centerlineGroupIdsArray->GetComponent(j,0))) == groupId)
        {
        groupTubesGroupIds->InsertNextId(j);
        }
      else
        {
        nonGroupTubesGroupIds->InsertNextId(j);
        }
      }

    if ((groupTubesGroupIds->GetNumberOfIds() == 0) || (nonGroupTubesGroupIds->GetNumberOfIds() == 0))
      {
      continue;
      }

    groupTubes->SetInputCellIds(groupTubesGroupIds);
    nonGroupTubes->SetInputCellIds(nonGroupTubesGroupIds);

    for (int k=0; k<numberOfPoints; k++)
      {
      input->GetPoint(k,point);
      groupTubeValue = groupTubes->EvaluateFunction(point);
      if (groupTubeValue > this->CutoffRadiusFactor * this->CutoffRadiusFactor - 1)
        {
        groupTubeValue = VTK_VMTK_LARGE_DOUBLE;
        }
      nonGroupTubeValue = nonGroupTubes->EvaluateFunction(point);
      tubeDifferenceValue = nonGroupTubeValue - groupTubeValue;
      clippingArray->SetValue(k,tubeDifferenceValue);
      }

    vtkClipDataSet* clipper = vtkClipDataSet::New();
#if (VTK_MAJOR_VERSION <= 5)
    clipper->SetInput(clippingInput);
#else
    clipper->SetInputData(clippingInput);
#endif
    clipper->SetValue(this->ClipValue);
    clipper->GenerateClipScalarsOff();
    clipper->SetGenerateClippedOutput(this->GenerateClippedOutput);
    clipper->Update();

    if (clipper->GetOutput()->GetNumberOfPoints()==0)
      {
      clipper->Delete();
      continue;
      }

    vtkUnstructuredGrid* clippedBranch = vtkUnstructuredGrid::New();
    clippedBranch->DeepCopy(clipper->GetOutput());

    if (this->GenerateClippedOutput)
      {
      vtkUnstructuredGrid* clippedOutputBranch = vtkUnstructuredGrid::New();
      clippedOutputBranch->DeepCopy(clipper->GetClippedOutput());
#if (VTK_MAJOR_VERSION <= 5)
      appendClippedOutput->AddInput(clippedOutputBranch);
#else
      appendClippedOutput->AddInputData(clippedOutputBranch);
#endif
      clippedOutputBranch->Delete();
      }
    
    groupIdsArray = vtkIntArray::New();
    groupIdsArray->SetName(this->GroupIdsArrayName);
    groupIdsArray->SetNumberOfTuples(clippedBranch->GetNumberOfPoints());
    groupIdsArray->FillComponent(0,groupId);

    clippedBranch->GetPointData()->AddArray(groupIdsArray);
#if (VTK_MAJOR_VERSION <= 5)
    appendBranches->AddInput(clippedBranch);
#else
    appendBranches->AddInputData(clippedBranch);
#endif
    groupIdsArray->Delete();
    clippedBranch->Delete();
    clipper->Delete();
    }

  appendBranches->Update();

  output->DeepCopy(appendBranches->GetOutput());

  if (this->GenerateClippedOutput)
    {
    appendClippedOutput->Update();
    this->GetClippedOutput()->DeepCopy(appendClippedOutput->GetOutput());
    appendClippedOutput->Delete();
    }

  clippingArray->Delete();
  clippingInput->Delete();
  appendBranches->Delete();
  groupTubes->Delete();
  nonGroupTubes->Delete();
  groupTubesGroupIds->Delete();
  nonGroupTubesGroupIds->Delete();
  centerlineGroupIds->Delete();

  return 1;
}

void vtkvmtkUnstructuredGridCenterlineGroupsClipper::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
