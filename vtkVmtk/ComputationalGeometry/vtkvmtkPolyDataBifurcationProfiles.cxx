/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBifurcationProfiles.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
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

#include "vtkvmtkPolyDataBifurcationProfiles.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkConstants.h"
#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkCenterlineBifurcationVectors.h"
#include "vtkvmtkPolyDataBranchUtilities.h"

#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkvmtkBoundaryReferenceSystems.h"


vtkStandardNewMacro(vtkvmtkPolyDataBifurcationProfiles);

vtkvmtkPolyDataBifurcationProfiles::vtkvmtkPolyDataBifurcationProfiles()
{
  this->GroupIdsArrayName = NULL;

  this->Centerlines = NULL;

  this->CenterlineRadiusArrayName = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->CenterlineTractIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->BifurcationProfileGroupIdsArrayName = NULL;
  this->BifurcationProfileBifurcationGroupIdsArrayName = NULL;
  this->BifurcationProfileOrientationArrayName = NULL;
}

vtkvmtkPolyDataBifurcationProfiles::~vtkvmtkPolyDataBifurcationProfiles()
{
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

  if (this->CenterlineRadiusArrayName)
    {
    delete[] this->CenterlineRadiusArrayName;
    this->CenterlineRadiusArrayName = NULL;
    }

  if (this->CenterlineGroupIdsArrayName)
    {
    delete[] this->CenterlineGroupIdsArrayName;
    this->CenterlineGroupIdsArrayName = NULL;
    }

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }

  if (this->CenterlineTractIdsArrayName)
    {
    delete[] this->CenterlineTractIdsArrayName;
    this->CenterlineTractIdsArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->BifurcationProfileGroupIdsArrayName)
    {
    delete[] this->BifurcationProfileGroupIdsArrayName;
    this->BifurcationProfileGroupIdsArrayName = NULL;
    }

  if (this->BifurcationProfileBifurcationGroupIdsArrayName)
    {
    delete[] this->BifurcationProfileBifurcationGroupIdsArrayName;
    this->BifurcationProfileBifurcationGroupIdsArrayName = NULL;
    }

  if (this->BifurcationProfileOrientationArrayName)
    {
    delete[] this->BifurcationProfileOrientationArrayName;
    this->BifurcationProfileOrientationArrayName = NULL;
    }
}

int vtkvmtkPolyDataBifurcationProfiles::RequestData(
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

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetPointData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  if (!this->CenterlineRadiusArrayName)
    {
    vtkErrorMacro(<<"CenterlineRadiusArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineRadiusArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName);

  if (!centerlineRadiusArray)
    {
    vtkErrorMacro(<<"CenterlineRadiusArray with name specified does not exist");
    return 1;
    }

  if (!this->CenterlineGroupIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineGroupIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineGroupIdsArrayName);

  if (!centerlineGroupIdsArray)
    {
    vtkErrorMacro(<<"CenterlineGroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  if (!centerlineIdsArray)
    {
    vtkErrorMacro(<<"CenterlineIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->CenterlineTractIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineTractIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineTractIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineTractIdsArrayName);

  if (!centerlineTractIdsArray)
    {
    vtkErrorMacro(<<"CenterlineTractIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<<"BlankingArrayName not specified");
    return 1;
    }

  vtkDataArray* blankingArray = this->Centerlines->GetCellData()->GetArray(this->BlankingArrayName);

  if (!blankingArray)
    {
    vtkErrorMacro(<<"BlankingArray with name specified does not exist");
    return 1;
    }

  if (!this->BifurcationProfileGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BifurcationProfileGroupIdsArrayName not specified");
    return 1;
    }

  if (!this->BifurcationProfileBifurcationGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BifurcationProfileBifurcationGroupIdsArrayName not specified");
    return 1;
    }

  if (!BifurcationProfileOrientationArrayName)
    {
    vtkErrorMacro(<<"BifurcationProfileOrientationArrayName not specified");
    return 1;
    }

  vtkIntArray* bifurcationProfileGroupIdsArray = vtkIntArray::New();
  bifurcationProfileGroupIdsArray->SetName(this->BifurcationProfileGroupIdsArrayName);

  vtkIntArray* bifurcationProfileBifurcationGroupIdsArray = vtkIntArray::New();
  bifurcationProfileBifurcationGroupIdsArray->SetName(this->BifurcationProfileBifurcationGroupIdsArrayName);

  vtkIntArray* bifurcationProfileOrientationArray = vtkIntArray::New();
  bifurcationProfileOrientationArray->SetName(this->BifurcationProfileOrientationArrayName);
  
  output->GetCellData()->AddArray(bifurcationProfileGroupIdsArray);
  output->GetCellData()->AddArray(bifurcationProfileBifurcationGroupIdsArray);
  output->GetCellData()->AddArray(bifurcationProfileOrientationArray);

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputLines = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetLines(outputLines);

  vtkIdList* blankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetBlankedGroupsIdList(this->Centerlines,this->CenterlineGroupIdsArrayName,this->BlankingArrayName,blankedGroupIds);
  int i;
  for (i=0; i<blankedGroupIds->GetNumberOfIds(); i++)
  {
    vtkIdType bifurcationGroupId = blankedGroupIds->GetId(i);

    //compute quantities and insert proper values in arrays
    vtkIdList* upStreamGroupIds = vtkIdList::New();
    vtkIdList* downStreamGroupIds = vtkIdList::New();
    
    vtkvmtkCenterlineUtilities::FindAdjacentCenterlineGroupIds(this->Centerlines,this->CenterlineGroupIdsArrayName,this->CenterlineIdsArrayName,this->CenterlineTractIdsArrayName,bifurcationGroupId,upStreamGroupIds,downStreamGroupIds);
    
    this->ComputeBifurcationProfiles(input,bifurcationGroupId,upStreamGroupIds,downStreamGroupIds,output);
    
    upStreamGroupIds->Delete();
    downStreamGroupIds->Delete();
    }

  blankedGroupIds->Delete();

  outputPoints->Delete();
  outputLines->Delete();
  
  bifurcationProfileGroupIdsArray->Delete();
  bifurcationProfileBifurcationGroupIdsArray->Delete();
  bifurcationProfileOrientationArray->Delete();

  return 1;
}

void vtkvmtkPolyDataBifurcationProfiles::ComputeBifurcationProfiles(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output)
{
  vtkDataArray* radiusArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName);
  vtkPoints* outputPoints = output->GetPoints();
  vtkCellArray* outputLines = output->GetLines();
  vtkIntArray* bifurcationProfileOrientationArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationProfileOrientationArrayName));
  vtkIntArray* bifurcationProfileGroupIdsArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationProfileGroupIdsArrayName));
  vtkIntArray* bifurcationProfileBifurcationGroupIdsArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationProfileBifurcationGroupIdsArrayName));
  
  vtkIdList* bifurcationProfileGroupIds = vtkIdList::New();
  vtkIntArray* bifurcationProfileOrientations = vtkIntArray::New();
  
  int numberOfUpStreamGroupIds = upStreamGroupIds->GetNumberOfIds();
  int numberOfDownStreamGroupIds = downStreamGroupIds->GetNumberOfIds();
  int i;
  for (i=0; i<numberOfUpStreamGroupIds; i++)
  {
    bifurcationProfileGroupIds->InsertNextId(upStreamGroupIds->GetId(i));
    bifurcationProfileOrientations->InsertNextValue(vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_UPSTREAM_ORIENTATION);
  }
  for (i=0; i<numberOfDownStreamGroupIds; i++)
  {
    bifurcationProfileGroupIds->InsertNextId(downStreamGroupIds->GetId(i));
    bifurcationProfileOrientations->InsertNextValue(vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_DOWNSTREAM_ORIENTATION);
  }

  int numberOfBifurcationProfiles = bifurcationProfileGroupIds->GetNumberOfIds();
  
  for (i=0; i<numberOfBifurcationProfiles; i++)
    {
    int bifurcationProfileGroupId = bifurcationProfileGroupIds->GetId(i);
    int bifurcationProfileOrientation = bifurcationProfileOrientations->GetValue(i);

    double averagePoint[3];
    averagePoint[0] = averagePoint[1] = averagePoint[2] = 0.0;

    double weightSum = 0.0;

    int j;
    vtkIdList* groupCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,bifurcationProfileGroupId,groupCellIds);
    for (j=0; j<groupCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType cellId = groupCellIds->GetId(j);
      vtkCell* cell = this->Centerlines->GetCell(cellId);
      int numberOfCellPoints = cell->GetNumberOfPoints();
      
      double lastPoint[3];
      double lastPointRadius = 0.0; 

      if (bifurcationProfileOrientation == vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_UPSTREAM_ORIENTATION)
      {
        cell->GetPoints()->GetPoint(numberOfCellPoints-1,lastPoint);
        lastPointRadius = radiusArray->GetComponent(cell->GetPointId(numberOfCellPoints-1),0);
      }
      else if (bifurcationProfileOrientation == vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_DOWNSTREAM_ORIENTATION)
      {
        cell->GetPoints()->GetPoint(0,lastPoint);
        lastPointRadius = radiusArray->GetComponent(cell->GetPointId(0),0);
      }
      else
      {
        vtkErrorMacro("Error: invalid BifurcationVectorOrientation");
        return;
      }

      averagePoint[0] += lastPointRadius * lastPointRadius * lastPoint[0];
      averagePoint[1] += lastPointRadius * lastPointRadius * lastPoint[1];
      averagePoint[2] += lastPointRadius * lastPointRadius * lastPoint[2];
 
      weightSum += lastPointRadius * lastPointRadius;
      }

    averagePoint[0] /= weightSum;
    averagePoint[1] /= weightSum;
    averagePoint[2] /= weightSum;

    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,bifurcationProfileGroupId,false,cylinder);

    vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
#if (VTK_MAJOR_VERSION <= 5)
    boundaryExtractor->SetInput(cylinder);
#else
    boundaryExtractor->SetInputData(cylinder);
#endif
    boundaryExtractor->Update();

    vtkPolyData* profile = boundaryExtractor->GetOutput();

    int numberOfCells = profile->GetNumberOfCells();
    int closestCellId = -1;
    double minDistance = VTK_VMTK_LARGE_DOUBLE;
    for (j=0; j<numberOfCells; j++)
      {
      double barycenter[3];
      vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(profile->GetCell(j)->GetPoints(),barycenter);
      double barycenterDistance = vtkMath::Distance2BetweenPoints(barycenter,averagePoint);
      if (barycenterDistance < minDistance)
        {
        closestCellId = j;
        minDistance = barycenterDistance;
        }
      }

    vtkPoints* profileCellPoints = profile->GetCell(closestCellId)->GetPoints();
    int numberOfProfileCellPoints = profileCellPoints->GetNumberOfPoints();
    outputLines->InsertNextCell(numberOfProfileCellPoints);
    for (j=0; j<numberOfProfileCellPoints; j++)
      {
      vtkIdType pointId = outputPoints->InsertNextPoint(profileCellPoints->GetPoint(j));
      outputLines->InsertCellPoint(pointId);
      }
    
    bifurcationProfileGroupIdsArray->InsertNextValue(bifurcationProfileGroupId);
    bifurcationProfileBifurcationGroupIdsArray->InsertNextValue(bifurcationGroupId);
    bifurcationProfileOrientationArray->InsertNextValue(bifurcationProfileOrientation);

    groupCellIds->Delete();
    cylinder->Delete();
    boundaryExtractor->Delete();
    }

  bifurcationProfileGroupIds->Delete();
  bifurcationProfileOrientations->Delete();
}

void vtkvmtkPolyDataBifurcationProfiles::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
