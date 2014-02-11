/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBifurcationSections.cxx,v $
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

#include "vtkvmtkPolyDataBifurcationSections.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkCenterlineBifurcationVectors.h"
#include "vtkvmtkPolyDataBranchUtilities.h"
#include "vtkvmtkPolyDataBranchSections.h"


vtkStandardNewMacro(vtkvmtkPolyDataBifurcationSections);

vtkvmtkPolyDataBifurcationSections::vtkvmtkPolyDataBifurcationSections()
{
  this->GroupIdsArrayName = NULL;

  this->Centerlines = NULL;

  this->CenterlineRadiusArrayName = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->CenterlineTractIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->BifurcationSectionGroupIdsArrayName = NULL;
  this->BifurcationSectionBifurcationGroupIdsArrayName = NULL;
  this->BifurcationSectionPointArrayName = NULL;
  this->BifurcationSectionNormalArrayName = NULL;
  this->BifurcationSectionAreaArrayName = NULL;
  this->BifurcationSectionMinSizeArrayName = NULL;
  this->BifurcationSectionMaxSizeArrayName = NULL;
  this->BifurcationSectionShapeArrayName = NULL;
  this->BifurcationSectionClosedArrayName = NULL;
  this->BifurcationSectionOrientationArrayName = NULL;
  this->BifurcationSectionDistanceSpheresArrayName = NULL;

  this->NumberOfDistanceSpheres = 1;
}

vtkvmtkPolyDataBifurcationSections::~vtkvmtkPolyDataBifurcationSections()
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

  if (this->BifurcationSectionGroupIdsArrayName)
    {
    delete[] this->BifurcationSectionGroupIdsArrayName;
    this->BifurcationSectionGroupIdsArrayName = NULL;
    }

  if (this->BifurcationSectionBifurcationGroupIdsArrayName)
    {
    delete[] this->BifurcationSectionBifurcationGroupIdsArrayName;
    this->BifurcationSectionBifurcationGroupIdsArrayName = NULL;
    }

  if (this->BifurcationSectionPointArrayName)
    {
    delete[] this->BifurcationSectionPointArrayName;
    this->BifurcationSectionPointArrayName = NULL;
    }

  if (this->BifurcationSectionNormalArrayName)
    {
    delete[] this->BifurcationSectionNormalArrayName;
    this->BifurcationSectionNormalArrayName = NULL;
    }

  if (this->BifurcationSectionAreaArrayName)
    {
    delete[] this->BifurcationSectionAreaArrayName;
    this->BifurcationSectionAreaArrayName = NULL;
    }

  if (this->BifurcationSectionMinSizeArrayName)
    {
    delete[] this->BifurcationSectionMinSizeArrayName;
    this->BifurcationSectionMinSizeArrayName = NULL;
    }

  if (this->BifurcationSectionMaxSizeArrayName)
    {
    delete[] this->BifurcationSectionMaxSizeArrayName;
    this->BifurcationSectionMaxSizeArrayName = NULL;
    }

  if (this->BifurcationSectionShapeArrayName)
    {
    delete[] this->BifurcationSectionShapeArrayName;
    this->BifurcationSectionShapeArrayName = NULL;
    }

  if (this->BifurcationSectionClosedArrayName)
    {
    delete[] this->BifurcationSectionClosedArrayName;
    this->BifurcationSectionClosedArrayName = NULL;
    }

  if (this->BifurcationSectionOrientationArrayName)
    {
    delete[] this->BifurcationSectionOrientationArrayName;
    this->BifurcationSectionOrientationArrayName = NULL;
    }

  if (this->BifurcationSectionDistanceSpheresArrayName)
    {
    delete[] this->BifurcationSectionDistanceSpheresArrayName;
    this->BifurcationSectionDistanceSpheresArrayName = NULL;
    }
}

int vtkvmtkPolyDataBifurcationSections::RequestData(
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

  if (!this->BifurcationSectionGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionGroupIdsArrayName not specified");
    return 1;
    }

  if (!this->BifurcationSectionBifurcationGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionBifurcationGroupIdsArrayName not specified");
    return 1;
    }

  if (!this->BifurcationSectionPointArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionPointArrayName not specified");
    return 1;
    }

  if (!this->BifurcationSectionNormalArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionNormalArrayName not specified");
    return 1;
    }

  if (!this->BifurcationSectionAreaArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionAreaArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionMinSizeArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionMinSizeArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionMaxSizeArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionMaxSizeArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionShapeArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionShapeArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionClosedArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionClosedArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionOrientationArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionOrientationArrayName not specified");
    return 1;
    }

  if (!BifurcationSectionDistanceSpheresArrayName)
    {
    vtkErrorMacro(<<"BifurcationSectionDistanceSpheresArrayName not specified");
    return 1;
    }

  vtkIntArray* bifurcationSectionGroupIdsArray = vtkIntArray::New();
  bifurcationSectionGroupIdsArray->SetName(this->BifurcationSectionGroupIdsArrayName);

  vtkIntArray* bifurcationSectionBifurcationGroupIdsArray = vtkIntArray::New();
  bifurcationSectionBifurcationGroupIdsArray->SetName(this->BifurcationSectionBifurcationGroupIdsArrayName);

  vtkDoubleArray* bifurcationSectionPointArray = vtkDoubleArray::New();
  bifurcationSectionPointArray->SetName(this->BifurcationSectionPointArrayName);
  bifurcationSectionPointArray->SetNumberOfComponents(3);

  vtkDoubleArray* bifurcationSectionNormalArray = vtkDoubleArray::New();
  bifurcationSectionNormalArray->SetName(this->BifurcationSectionNormalArrayName);
  bifurcationSectionNormalArray->SetNumberOfComponents(3);

  vtkDoubleArray* bifurcationSectionAreaArray = vtkDoubleArray::New();
  bifurcationSectionAreaArray->SetName(this->BifurcationSectionAreaArrayName);

  vtkDoubleArray* bifurcationSectionShapeArray = vtkDoubleArray::New();
  bifurcationSectionShapeArray->SetName(this->BifurcationSectionShapeArrayName);

  vtkDoubleArray* bifurcationSectionMinSizeArray = vtkDoubleArray::New();
  bifurcationSectionMinSizeArray->SetName(this->BifurcationSectionMinSizeArrayName);

  vtkDoubleArray* bifurcationSectionMaxSizeArray = vtkDoubleArray::New();
  bifurcationSectionMaxSizeArray->SetName(this->BifurcationSectionMaxSizeArrayName);

  vtkIntArray* bifurcationSectionClosedArray = vtkIntArray::New();
  bifurcationSectionClosedArray->SetName(this->BifurcationSectionClosedArrayName);

  vtkIntArray* bifurcationSectionOrientationArray = vtkIntArray::New();
  bifurcationSectionOrientationArray->SetName(this->BifurcationSectionOrientationArrayName);
  
  vtkIntArray* bifurcationSectionDistanceSpheresArray = vtkIntArray::New();
  bifurcationSectionDistanceSpheresArray->SetName(this->BifurcationSectionDistanceSpheresArrayName);

  output->GetCellData()->AddArray(bifurcationSectionGroupIdsArray);
  output->GetCellData()->AddArray(bifurcationSectionBifurcationGroupIdsArray);
  output->GetCellData()->AddArray(bifurcationSectionPointArray);
  output->GetCellData()->AddArray(bifurcationSectionNormalArray);
  output->GetCellData()->AddArray(bifurcationSectionAreaArray);
  output->GetCellData()->AddArray(bifurcationSectionMinSizeArray);
  output->GetCellData()->AddArray(bifurcationSectionMaxSizeArray);
  output->GetCellData()->AddArray(bifurcationSectionShapeArray);
  output->GetCellData()->AddArray(bifurcationSectionClosedArray);
  output->GetCellData()->AddArray(bifurcationSectionOrientationArray);
  output->GetCellData()->AddArray(bifurcationSectionDistanceSpheresArray);

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputPolys = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);

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
    
    this->ComputeBifurcationSections(input,bifurcationGroupId,upStreamGroupIds,downStreamGroupIds,output);
    
    upStreamGroupIds->Delete();
    downStreamGroupIds->Delete();
    }

  blankedGroupIds->Delete();

  outputPoints->Delete();
  outputPolys->Delete();
  
  bifurcationSectionGroupIdsArray->Delete();
  bifurcationSectionBifurcationGroupIdsArray->Delete();
  bifurcationSectionPointArray->Delete();
  bifurcationSectionNormalArray->Delete();
  bifurcationSectionAreaArray->Delete();
  bifurcationSectionShapeArray->Delete();
  bifurcationSectionMinSizeArray->Delete();
  bifurcationSectionMaxSizeArray->Delete();
  bifurcationSectionClosedArray->Delete();
  bifurcationSectionOrientationArray->Delete();
  bifurcationSectionDistanceSpheresArray->Delete();

  return 1;
}

void vtkvmtkPolyDataBifurcationSections::ComputeBifurcationSections(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output)
{
  vtkDataArray* radiusArray = this->Centerlines->GetPointData()->GetArray(this->CenterlineRadiusArrayName);
  vtkPoints* outputPoints = output->GetPoints();
  vtkCellArray* outputPolys = output->GetPolys();
  vtkDoubleArray* bifurcationSectionPointArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionPointArrayName));
  vtkDoubleArray* bifurcationSectionNormalArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionNormalArrayName));
  vtkDoubleArray* bifurcationSectionAreaArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionAreaArrayName));
  vtkDoubleArray* bifurcationSectionMinSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionMinSizeArrayName));
  vtkDoubleArray* bifurcationSectionMaxSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionMaxSizeArrayName));
  vtkDoubleArray* bifurcationSectionShapeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionShapeArrayName));
  vtkIntArray* bifurcationSectionClosedArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionClosedArrayName));
  vtkIntArray* bifurcationSectionOrientationArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionOrientationArrayName));
  vtkIntArray* bifurcationSectionDistanceSpheresArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionDistanceSpheresArrayName));
  vtkIntArray* bifurcationSectionGroupIdsArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionGroupIdsArrayName));
  vtkIntArray* bifurcationSectionBifurcationGroupIdsArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BifurcationSectionBifurcationGroupIdsArrayName));
  
  vtkIdList* bifurcationSectionGroupIds = vtkIdList::New();
  vtkIntArray* bifurcationSectionOrientations = vtkIntArray::New();
  
  int numberOfUpStreamGroupIds = upStreamGroupIds->GetNumberOfIds();
  int numberOfDownStreamGroupIds = downStreamGroupIds->GetNumberOfIds();
  int i;
  for (i=0; i<numberOfUpStreamGroupIds; i++)
  {
    bifurcationSectionGroupIds->InsertNextId(upStreamGroupIds->GetId(i));
    bifurcationSectionOrientations->InsertNextValue(vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_UPSTREAM_ORIENTATION);
  }
  for (i=0; i<numberOfDownStreamGroupIds; i++)
  {
    bifurcationSectionGroupIds->InsertNextId(downStreamGroupIds->GetId(i));
    bifurcationSectionOrientations->InsertNextValue(vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_DOWNSTREAM_ORIENTATION);
  }

  int numberOfBifurcationSections = bifurcationSectionGroupIds->GetNumberOfIds();
  
  for (i=0; i<numberOfBifurcationSections; i++)
    {
    int bifurcationSectionGroupId = bifurcationSectionGroupIds->GetId(i);
    int bifurcationSectionOrientation = bifurcationSectionOrientations->GetValue(i);

    double averagePoint[3];
    averagePoint[0] = averagePoint[1] = averagePoint[2] = 0.0;

    double averageTangent[3];
    averageTangent[0] = averageTangent[1] = averageTangent[2] = 0.0;

    double weightSum = 0.0;

    int j;
    bool anyPoint = false;
    vtkIdList* groupCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,bifurcationSectionGroupId,groupCellIds);
    for (j=0; j<groupCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType cellId = groupCellIds->GetId(j);
      vtkCell* cell = this->Centerlines->GetCell(cellId);
      int numberOfCellPoints = cell->GetNumberOfPoints();
      
      vtkIdType touchingSubId = -1;
      double touchingPCoord = 0.0;
   
      vtkIdType pointSubId = -1;
      double pointPCoord = 0.0;
      bool forward = true;
 
      double lastPoint[3];
      double lastPointRadius = 0.0; 

      if (bifurcationSectionOrientation == vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_UPSTREAM_ORIENTATION)
      {
        pointSubId = numberOfCellPoints-2;
        pointPCoord = 1.0;
        forward = true;
        cell->GetPoints()->GetPoint(numberOfCellPoints-1,lastPoint);
        lastPointRadius = radiusArray->GetComponent(cell->GetPointId(numberOfCellPoints-1),0);
      }
      else if (bifurcationSectionOrientation == vtkvmtkCenterlineBifurcationVectors::VTK_VMTK_DOWNSTREAM_ORIENTATION)
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
    
      vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(this->Centerlines,this->CenterlineRadiusArrayName,cellId,pointSubId,pointPCoord,this->NumberOfDistanceSpheres,touchingSubId,touchingPCoord,forward);
      
      if (touchingSubId == -1)
        {
      	continue;
        }

      anyPoint = true;
      double touchingPoint[3];
      vtkvmtkCenterlineUtilities::InterpolatePoint(this->Centerlines,cellId,touchingSubId,touchingPCoord,touchingPoint);
      
      double touchingPoint0[3], touchingPoint1[3];
      this->Centerlines->GetCell(cellId)->GetPoints()->GetPoint(touchingSubId,touchingPoint0);
      this->Centerlines->GetCell(cellId)->GetPoints()->GetPoint(touchingSubId+1,touchingPoint1);

      double touchingPointTangent[3];
      touchingPointTangent[0] = touchingPoint1[0] - touchingPoint0[0];
      touchingPointTangent[1] = touchingPoint1[1] - touchingPoint0[1];
      touchingPointTangent[2] = touchingPoint1[2] - touchingPoint0[2];

      vtkMath::Normalize(touchingPointTangent);
      double touchingPointRadius = 0.0;
      vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->CenterlineRadiusArrayName,cellId,touchingSubId,touchingPCoord,touchingPointRadius);

      averagePoint[0] += touchingPointRadius * touchingPointRadius * touchingPoint[0];
      averagePoint[1] += touchingPointRadius * touchingPointRadius * touchingPoint[1];
      averagePoint[2] += touchingPointRadius * touchingPointRadius * touchingPoint[2];

      averageTangent[0] += touchingPointRadius * touchingPointRadius * touchingPointTangent[0];
      averageTangent[1] += touchingPointRadius * touchingPointRadius * touchingPointTangent[1];
      averageTangent[2] += touchingPointRadius * touchingPointRadius * touchingPointTangent[2];

      weightSum += touchingPointRadius * touchingPointRadius;
      }

    if (!anyPoint)
      {
      continue;
      }
      
    averagePoint[0] /= weightSum;
    averagePoint[1] /= weightSum;
    averagePoint[2] /= weightSum;

    averageTangent[0] /= weightSum;
    averageTangent[1] /= weightSum;
    averageTangent[2] /= weightSum;

    vtkMath::Normalize(averageTangent);

    //now cut branch with plane and get section. Compute section properties and store them.
    
    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,bifurcationSectionGroupId,false,cylinder);

    vtkPolyData* section = vtkPolyData::New();
    bool closed = false;
    vtkvmtkPolyDataBranchSections::ExtractCylinderSection(cylinder,averagePoint,averageTangent,section,closed);

    section->BuildCells();
    vtkPoints* sectionCellPoints = section->GetCell(0)->GetPoints();
    int numberOfSectionCellPoints = sectionCellPoints->GetNumberOfPoints();
    outputPolys->InsertNextCell(numberOfSectionCellPoints);
    for (j=0; j<numberOfSectionCellPoints; j++)
    {
      vtkIdType pointId = outputPoints->InsertNextPoint(sectionCellPoints->GetPoint(j));
      outputPolys->InsertCellPoint(pointId);
    }
    
    double area = vtkvmtkPolyDataBranchSections::ComputeBranchSectionArea(section);
    double sizeRange[2];
    double shape = vtkvmtkPolyDataBranchSections::ComputeBranchSectionShape(section,averagePoint,sizeRange);
    int closedValue = closed ? 1 : 0;

    bifurcationSectionGroupIdsArray->InsertNextValue(bifurcationSectionGroupId);
    bifurcationSectionBifurcationGroupIdsArray->InsertNextValue(bifurcationGroupId);
    bifurcationSectionPointArray->InsertNextTuple(averagePoint);
    bifurcationSectionNormalArray->InsertNextTuple(averageTangent);
    bifurcationSectionAreaArray->InsertNextValue(area);
    bifurcationSectionMinSizeArray->InsertNextValue(sizeRange[0]);
    bifurcationSectionMaxSizeArray->InsertNextValue(sizeRange[1]);
    bifurcationSectionShapeArray->InsertNextValue(shape);
    bifurcationSectionClosedArray->InsertNextValue(closedValue);
    bifurcationSectionOrientationArray->InsertNextValue(bifurcationSectionOrientation);
    bifurcationSectionDistanceSpheresArray->InsertNextValue(this->NumberOfDistanceSpheres);

    groupCellIds->Delete();
    cylinder->Delete();
    section->Delete();
    }

  bifurcationSectionGroupIds->Delete();
  bifurcationSectionOrientations->Delete();
}

void vtkvmtkPolyDataBifurcationSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
