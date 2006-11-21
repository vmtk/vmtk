/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBranchSections.cxx,v $
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

#include "vtkvmtkPolyDataBranchSections.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkStripper.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkMath.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataBranchSections, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkvmtkPolyDataBranchSections);

vtkvmtkPolyDataBranchSections::vtkvmtkPolyDataBranchSections()
{
  this->GroupIdsArrayName = NULL;

  this->Centerlines = NULL;

  this->CenterlineRadiusArrayName = NULL;
  this->CenterlineGroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->CenterlineTractIdsArrayName = NULL;
  this->BlankingArrayName = NULL;

  this->BranchSectionGroupIdsArrayName = NULL;
  this->BranchSectionAreaArrayName = NULL;
  this->BranchSectionMinSizeArrayName = NULL;
  this->BranchSectionMaxSizeArrayName = NULL;
  this->BranchSectionShapeArrayName = NULL;
  this->BranchSectionClosedArrayName = NULL;
  this->BranchSectionDistanceSpheresArrayName = NULL;

  this->NumberOfDistanceSpheres = 1;
}

vtkvmtkPolyDataBranchSections::~vtkvmtkPolyDataBranchSections()
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

  if (this->BranchSectionGroupIdsArrayName)
    {
    delete[] this->BranchSectionGroupIdsArrayName;
    this->BranchSectionGroupIdsArrayName = NULL;
    }

  if (this->BranchSectionAreaArrayName)
    {
    delete[] this->BranchSectionAreaArrayName;
    this->BranchSectionAreaArrayName = NULL;
    }

  if (this->BranchSectionMinSizeArrayName)
    {
    delete[] this->BranchSectionMinSizeArrayName;
    this->BranchSectionMinSizeArrayName = NULL;
    }

  if (this->BranchSectionMaxSizeArrayName)
    {
    delete[] this->BranchSectionMaxSizeArrayName;
    this->BranchSectionMaxSizeArrayName = NULL;
    }

  if (this->BranchSectionShapeArrayName)
    {
    delete[] this->BranchSectionShapeArrayName;
    this->BranchSectionShapeArrayName = NULL;
    }

  if (this->BranchSectionClosedArrayName)
    {
    delete[] this->BranchSectionClosedArrayName;
    this->BranchSectionClosedArrayName = NULL;
    }

  if (this->BranchSectionDistanceSpheresArrayName)
    {
    delete[] this->BranchSectionDistanceSpheresArrayName;
    this->BranchSectionDistanceSpheresArrayName = NULL;
    }
}

int vtkvmtkPolyDataBranchSections::RequestData(
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

  if (!this->BranchSectionGroupIdsArrayName)
    {
    vtkErrorMacro(<<"BranchSectionGroupIdsArrayName not specified");
    return 1;
    }

  if (!this->BranchSectionAreaArrayName)
    {
    vtkErrorMacro(<<"BranchSectionAreaArrayName not specified");
    return 1;
    }

  if (!BranchSectionMinSizeArrayName)
    {
    vtkErrorMacro(<<"BranchSectionMinSizeArrayName not specified");
    return 1;
    }

  if (!BranchSectionMaxSizeArrayName)
    {
    vtkErrorMacro(<<"BranchSectionMaxSizeArrayName not specified");
    return 1;
    }

  if (!BranchSectionShapeArrayName)
    {
    vtkErrorMacro(<<"BranchSectionShapeArrayName not specified");
    return 1;
    }

  if (!BranchSectionClosedArrayName)
    {
    vtkErrorMacro(<<"BranchSectionClosedArrayName not specified");
    return 1;
    }

  if (!BranchSectionDistanceSpheresArrayName)
    {
    vtkErrorMacro(<<"BranchSectionDistanceSpheresArrayName not specified");
    return 1;
    }

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputPolys = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);

  vtkIntArray* branchSectionGroupIdsArray = vtkIntArray::New();
  branchSectionGroupIdsArray->SetName(this->BranchSectionGroupIdsArrayName);

  vtkDoubleArray* branchSectionAreaArray = vtkDoubleArray::New();
  branchSectionAreaArray->SetName(this->BranchSectionAreaArrayName);

  vtkDoubleArray* branchSectionShapeArray = vtkDoubleArray::New();
  branchSectionShapeArray->SetName(this->BranchSectionShapeArrayName);

  vtkDoubleArray* branchSectionMinSizeArray = vtkDoubleArray::New();
  branchSectionMinSizeArray->SetName(this->BranchSectionMinSizeArrayName);

  vtkDoubleArray* branchSectionMaxSizeArray = vtkDoubleArray::New();
  branchSectionMaxSizeArray->SetName(this->BranchSectionMaxSizeArrayName);

  vtkIntArray* branchSectionClosedArray = vtkIntArray::New();
  branchSectionClosedArray->SetName(this->BranchSectionClosedArrayName);

  vtkIntArray* branchSectionDistanceSpheresArray = vtkIntArray::New();
  branchSectionDistanceSpheresArray->SetName(this->BranchSectionDistanceSpheresArrayName);

  output->GetCellData()->AddArray(branchSectionGroupIdsArray);
  output->GetCellData()->AddArray(branchSectionAreaArray);
  output->GetCellData()->AddArray(branchSectionMinSizeArray);
  output->GetCellData()->AddArray(branchSectionMaxSizeArray);
  output->GetCellData()->AddArray(branchSectionShapeArray);
  output->GetCellData()->AddArray(branchSectionClosedArray);
  output->GetCellData()->AddArray(branchSectionDistanceSpheresArray);
  
  vtkIdList* nonBlankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetNonBlankedGroupsIdList(this->Centerlines,this->CenterlineGroupIdsArrayName,this->BlankingArrayName,nonBlankedGroupIds);
  int i;
  for (i=0; i<nonBlankedGroupIds->GetNumberOfIds(); i++)
  {
    vtkIdType groupId = nonBlankedGroupIds->GetId(i);

    this->ComputeBranchSections(input,groupId,output);
  }

  nonBlankedGroupIds->Delete();

  outputPoints->Delete();
  outputPolys->Delete();

  branchSectionGroupIdsArray->Delete();
  branchSectionAreaArray->Delete();
  branchSectionMinSizeArray->Delete();
  branchSectionMaxSizeArray->Delete();
  branchSectionShapeArray->Delete();
  branchSectionClosedArray->Delete();
  branchSectionDistanceSpheresArray->Delete();
  
  return 1;
}

void vtkvmtkPolyDataBranchSections::ComputeBranchSections(vtkPolyData* input, int groupId, vtkPolyData* output)
{
  vtkPoints* branchSectionPoints = output->GetPoints();
  vtkCellArray* branchSectionPolys = output->GetPolys();

  vtkIntArray* branchSectionGroupIdsArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionGroupIdsArrayName));
  vtkDoubleArray* branchSectionAreaArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionAreaArrayName));
  vtkDoubleArray* branchSectionMinSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionMinSizeArrayName));
  vtkDoubleArray* branchSectionMaxSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionMaxSizeArrayName));
  vtkDoubleArray* branchSectionShapeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionShapeArrayName));
  vtkIntArray* branchSectionClosedArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionClosedArrayName));
  vtkIntArray* branchSectionDistanceSpheresArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->BranchSectionDistanceSpheresArrayName));

  int i, j;

  for (i=0; ; i++)
    {
    double averagePoint[3];
    averagePoint[0] = averagePoint[1] = averagePoint[2] = 0.0;

    double averageTangent[3];
    averageTangent[0] = averageTangent[1] = averageTangent[2] = 0.0;

    double weightSum = 0.0;

    int totalNumberOfSpheres = i * this->NumberOfDistanceSpheres;
    bool anyPoint = false;
    
    vtkIdList* groupCellIds = vtkIdList::New();
    vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(this->Centerlines,this->CenterlineGroupIdsArrayName,groupId,groupCellIds);
    for (j=0; j<groupCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType centerlineCellId = groupCellIds->GetId(j);
      vtkPoints* centerlineCellPoints = this->Centerlines->GetCell(centerlineCellId)->GetPoints();

      vtkIdType firstSubId = 0;
      double firstPCoord = 0.0;
      vtkIdType touchingSubId = -1;
      double touchingPCoord = 0.0;
      vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(this->Centerlines,this->CenterlineRadiusArrayName,centerlineCellId,firstSubId,firstPCoord,totalNumberOfSpheres,touchingSubId,touchingPCoord,false);
      
      if (touchingSubId == -1)
        {
      	continue;
        }

      anyPoint = true;
      double touchingPoint[3];
      vtkvmtkCenterlineUtilities::InterpolatePoint(this->Centerlines,centerlineCellId,touchingSubId,touchingPCoord,touchingPoint);

      double touchingPoint0[3], touchingPoint1[3];
      centerlineCellPoints->GetPoint(touchingSubId,touchingPoint0);
      centerlineCellPoints->GetPoint(touchingSubId+1,touchingPoint1);

      double touchingPointTangent[3];
      touchingPointTangent[0] = touchingPoint1[0] - touchingPoint0[0];
      touchingPointTangent[1] = touchingPoint1[1] - touchingPoint0[1];
      touchingPointTangent[2] = touchingPoint1[2] - touchingPoint0[2];

      vtkMath::Normalize(touchingPointTangent);
      double touchingPointRadius = 0.0;
      vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->CenterlineRadiusArrayName,centerlineCellId,touchingSubId,touchingPCoord,touchingPointRadius);

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
      break;
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
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,groupId,false,cylinder);

    vtkPolyData* section = vtkPolyData::New();
    bool closed = false;
    this->ExtractCylinderSection(cylinder,averagePoint,averageTangent,section,closed);

    section->BuildCells();
    vtkPoints* sectionCellPoints = section->GetCell(0)->GetPoints();
    int numberOfSectionCellPoints = sectionCellPoints->GetNumberOfPoints();
    branchSectionPolys->InsertNextCell(numberOfSectionCellPoints);
    int k;
    for (k=0; k<numberOfSectionCellPoints; k++)
    {
      vtkIdType branchPointId = branchSectionPoints->InsertNextPoint(sectionCellPoints->GetPoint(k));
      branchSectionPolys->InsertCellPoint(branchPointId);
    }
    
    double area = this->ComputeBranchSectionArea(section);
    double sizeRange[2];
    double shape = this->ComputeBranchSectionShape(section,averagePoint,sizeRange);

    branchSectionGroupIdsArray->InsertNextValue(groupId);
    branchSectionAreaArray->InsertNextValue(area);
    branchSectionMinSizeArray->InsertNextValue(sizeRange[0]);
    branchSectionMaxSizeArray->InsertNextValue(sizeRange[1]);
    branchSectionShapeArray->InsertNextValue(shape);
    branchSectionClosedArray->InsertNextValue(closed);
    branchSectionDistanceSpheresArray->InsertNextValue(totalNumberOfSpheres);

    groupCellIds->Delete();
    cylinder->Delete();
    section->Delete();
    }  
}

void vtkvmtkPolyDataBranchSections::ExtractCylinderSection(vtkPolyData* cylinder, double origin[3], double normal[3], vtkPolyData* section, bool & closed)
{
  vtkPlane* plane = vtkPlane::New();
  plane->SetOrigin(origin);
  plane->SetNormal(normal);

  vtkCutter* cutter = vtkCutter::New();
  cutter->SetInput(cylinder);
  cutter->SetCutFunction(plane);
  cutter->GenerateCutScalarsOn();
  cutter->SetValue(0,0.0);
  cutter->Update();

  vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
  cleaner->SetInput(cutter->GetOutput());
  cleaner->Update();

  if (cleaner->GetOutput()->GetNumberOfPoints() == 0)
    {
    return;
    }

  vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
  connectivityFilter->SetInput(cleaner->GetOutput());
  connectivityFilter->SetExtractionModeToClosestPointRegion();
  connectivityFilter->SetClosestPoint(origin);
  connectivityFilter->Update();

  section->DeepCopy(connectivityFilter->GetOutput());
  section->Update();

  // TODO: manually reconstruct single cell line from connectivity output

  if (section->GetNumberOfCells() == 0)
    {
    return;
    }

  section->BuildCells();
  section->BuildLinks();

  // find first point

  int numberOfLinePoints = section->GetNumberOfPoints();

  unsigned short ncells;
  vtkIdType* cells;
  vtkIdType npts, *pts;

  int numberOfSingleCellPoints = 0;
  vtkIdType firstPointId = -1;

  for (int i=0; i<numberOfLinePoints; i++)
    {
    section->GetPointCells(i,ncells,cells);
    if (ncells == 1)
      {
      ++numberOfSingleCellPoints;
      firstPointId = i;
      }
    }

  if (numberOfSingleCellPoints == 0)
    {
    firstPointId = section->GetCell(0)->GetPointId(0);
    }

  vtkIdList* polygonPointIds = vtkIdList::New();
  polygonPointIds->InsertNextId(firstPointId);

  bool done = false;
  vtkIdType pointId = firstPointId;

  closed = false;

  vtkIdType cellId = -1;
  while (!done)
    {
    section->GetPointCells(pointId,ncells,cells);
    if (ncells == 1)
      {
      if (pointId == firstPointId)
        {
        cellId = cells[0];
        }
      else
        {
        done = true;
        break;
        }
      }
    else if (ncells == 2)
      {
      if (cells[0] == cellId)
        {
        cellId = cells[1];
        }
      else
        {
        cellId = cells[0];
        }
      }

    section->GetCellPoints(cellId,npts,pts);

    if (pts[0] == pointId)
      {
      pointId = pts[1];
      }
    else
      {
      pointId = pts[0];
      }

    if (pointId == firstPointId)
      {
      closed = true;
      done = true;
      break;
      }

    polygonPointIds->InsertNextId(pointId);
    }

  section->GetLines()->Reset();
  section->GetPolys()->Reset();

  section->GetPolys()->InsertNextCell(polygonPointIds);
  section->Update();

  cutter->Delete();
  connectivityFilter->Delete();
  polygonPointIds->Delete();
}

double vtkvmtkPolyDataBranchSections::ComputeBranchSectionArea(vtkPolyData* branchSection)
{
  branchSection->BuildCells();
  
  if (branchSection->GetNumberOfCells() == 0)
    {
    return 0.0;
    }

  vtkPolygon* sectionPolygon = vtkPolygon::SafeDownCast(branchSection->GetCell(0));

  vtkIdList* trianglePointIds = vtkIdList::New();

  sectionPolygon->Triangulate(trianglePointIds);

  int numberOfTriangles = trianglePointIds->GetNumberOfIds() / 3;

  double polygonArea = 0.0;

  for (int i=0; i<numberOfTriangles; i++)
    {
    vtkIdType pointId0 = trianglePointIds->GetId(3*i);
    vtkIdType pointId1 = trianglePointIds->GetId(3*i+1);
    vtkIdType pointId2 = trianglePointIds->GetId(3*i+2);

    double point0[3], point1[3], point2[3];

    sectionPolygon->GetPoints()->GetPoint(pointId0,point0);
    sectionPolygon->GetPoints()->GetPoint(pointId1,point1);
    sectionPolygon->GetPoints()->GetPoint(pointId2,point2);

    double triangleArea = vtkTriangle::TriangleArea(point0,point1,point2);

    polygonArea += triangleArea;
    }

  trianglePointIds->Delete();

  return polygonArea;
}

double vtkvmtkPolyDataBranchSections::ComputeBranchSectionShape(vtkPolyData* branchSection, double center[3], double sizeRange[2])
{
  branchSection->BuildCells();
  
  if (branchSection->GetNumberOfCells() == 0)
    {
    sizeRange[0] = sizeRange[1] = 0.0;
    return 0.0;
    }

  vtkPolygon* sectionPolygon = vtkPolygon::SafeDownCast(branchSection->GetCell(0));

  int numberOfSectionPolygonPoints = sectionPolygon->GetNumberOfPoints();

  double minDistance = VTK_VMTK_LARGE_DOUBLE;
  double maxDistance = 0.0;

  for (int i=0; i<numberOfSectionPolygonPoints; i++)
    {
    double point[3];
    sectionPolygon->GetPoints()->GetPoint(i,point);
    double distance = sqrt(vtkMath::Distance2BetweenPoints(point,center));

    if (distance > maxDistance)
      {
      maxDistance = distance;
      }

    if (distance < minDistance)
      {
      minDistance = distance;
      }
    }

  sizeRange[0] = minDistance;
  sizeRange[1] = maxDistance;

  double sectionShape = minDistance / maxDistance;

  return sectionShape;
}

void vtkvmtkPolyDataBranchSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
