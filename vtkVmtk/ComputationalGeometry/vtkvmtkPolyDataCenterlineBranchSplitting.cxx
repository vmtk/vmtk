/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

  This file is derived from vtkvmtkPolyDataCenterlineSections of SimVascular
  (https://github.com/SimVascular/SimVascular), provided under the following
  license:

  Copyright (c) Stanford University, The Regents of the University of
  California, and others.

  All Rights Reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

=========================================================================*/

#include "vtkvmtkPolyDataCenterlineBranchSplitting.h"
#include "vtkvmtkPolyDataBranchSections.h"

#include "vtkAppendFilter.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCleanPolyData.h"
#include "vtkConnectivityFilter.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointDataToCellData.h"
#include "vtkPointLocator.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkThreshold.h"
#include "vtkUnstructuredGrid.h"

#include <algorithm>
#include <set>
#include <vector>

vtkStandardNewMacro(vtkvmtkPolyDataCenterlineBranchSplitting);

namespace
{
// Arrays used only while the filter runs
const char* BranchIdTmpArrayName = "BranchIdTmp";
const char* BifurcationIdTmpArrayName = "BifurcationIdTmp";
const char* RemoveArrayName = "CenterlineSectionRemove";
const char* RegionIdArrayName = "RegionId";

// Values of the bifurcation classification arrays
const int Undefined = -1;
const int Branch = 0;
const int Bifurcation = 1;
const int BifurcationDownstream = 2;

// The cells of each point of lines. Adjacency is computed here rather than
// with vtkPolyData links, which hold a reference back to the polydata.
std::vector<std::vector<vtkIdType>> GetPointCells(vtkPolyData* lines)
{
  std::vector<std::vector<vtkIdType>> pointCells(lines->GetNumberOfPoints());
  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType cellId=0; cellId<lines->GetNumberOfCells(); cellId++)
    {
    lines->GetCellPoints(cellId,cellPointIds);
    for (vtkIdType i=0; i<cellPointIds->GetNumberOfIds(); i++)
      {
      pointCells[cellPointIds->GetId(i)].push_back(cellId);
      }
    }
  return pointCells;
}

bool IsOnePiece(vtkPolyData* polyData)
{
  vtkNew<vtkConnectivityFilter> connectivity;
  connectivity->SetInputData(polyData);
  connectivity->SetExtractionModeToAllRegions();
  connectivity->Update();
  return connectivity->GetNumberOfExtractedRegions() == 1;
}

void BuildPointLocator(vtkPolyData* polyData, vtkPointLocator* locator)
{
  vtkNew<vtkPolyData> points;
  points->SetPoints(polyData->GetPoints());
  locator->SetDataSet(points);
  locator->BuildLocator();
}

// The lines of an unstructured grid, such as the output of vtkThreshold, as
// polydata with the same points, point order, cell order and attributes
void LinesToPolyData(vtkUnstructuredGrid* grid, vtkPolyData* polyData)
{
  vtkNew<vtkPolyData> lines;
  vtkNew<vtkPoints> points;
  if (grid->GetPoints())
    {
    points->ShallowCopy(grid->GetPoints());
    }
  lines->SetPoints(points);
  vtkNew<vtkCellArray> cells;
  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType cellId=0; cellId<grid->GetNumberOfCells(); cellId++)
    {
    grid->GetCellPoints(cellId,cellPointIds);
    cells->InsertNextCell(cellPointIds);
    }
  lines->SetLines(cells);
  lines->GetPointData()->ShallowCopy(grid->GetPointData());
  lines->GetCellData()->ShallowCopy(grid->GetCellData());
  polyData->DeepCopy(lines);
}

// Start of each step in the progress of the whole filter, from approximate
// running times
const double PROGRESS_NORMALS = 0.0;
const double PROGRESS_CLEAN = 0.01;
const double PROGRESS_LABEL_PRELIMINARY = 0.02;
const double PROGRESS_SECTIONS = 0.12;
const double PROGRESS_GROUP = 0.88;
const double PROGRESS_LABEL = 0.89;

// While it exists, maps progress from 0 to 1 onto the part of the progress
// range of algorithm from start to end, and sets the progress text
class ProgressStep
{
  public:
  ProgressStep(vtkAlgorithm* algorithm, const char* text, double start, double end)
    : Algorithm(algorithm), Shift(algorithm->GetProgressShift()), Scale(algorithm->GetProgressScale())
    {
    algorithm->SetProgressText(text);
    algorithm->SetProgressShiftScale(this->Shift + this->Scale * start, this->Scale * (end - start));
    algorithm->UpdateProgress(0.0);
    }
  ~ProgressStep()
    {
    this->Algorithm->SetProgressShiftScale(this->Shift,this->Scale);
    }
  ProgressStep(const ProgressStep&) = delete;
  ProgressStep& operator=(const ProgressStep&) = delete;

  private:
  vtkAlgorithm* Algorithm;
  double Shift;
  double Scale;
};

// vtkConnectivityFilter stores region ids in an implicit array since VTK 9.4,
// which vtkAppendFilter does not merge with the explicit arrays of other
// inputs. Replace it with an explicit array of the given name.
void RenameRegionIds(vtkDataSetAttributes* attributes, const char* name)
{
  vtkNew<vtkIdTypeArray> regionIds;
  regionIds->DeepCopy(attributes->GetArray(RegionIdArrayName));
  regionIds->SetName(name);
  attributes->RemoveArray(RegionIdArrayName);
  attributes->AddArray(regionIds);
}
}

class vtkvmtkPolyDataCenterlineBranchSplitting::vtkInternals
{
  public:
  // Number of cells of each centerline point while sections are computed
  std::vector<vtkIdType> NumberOfPointCells;
  vtkIntArray* SectionBifurcation = nullptr;
  vtkIntArray* SectionGlobalNodeId = nullptr;
  vtkIntArray* CenterlineBifurcation = nullptr;
  vtkDataArray* CenterlineNormal = nullptr;
};

vtkvmtkPolyDataCenterlineBranchSplitting::vtkvmtkPolyDataCenterlineBranchSplitting()
{
  this->SetCenterlineSectionAreaArrayName("CenterlineSectionArea");
  this->SetCenterlineSectionMinSizeArrayName("CenterlineSectionMinSize");
  this->SetCenterlineSectionMaxSizeArrayName("CenterlineSectionMaxSize");
  this->SetCenterlineSectionShapeArrayName("CenterlineSectionShape");
  this->SetCenterlineSectionClosedArrayName("CenterlineSectionClosed");

  this->Surface = vtkPolyData::New();

  this->RadiusArrayName = nullptr;
  this->BranchIdArrayName = nullptr;
  this->BifurcationIdArrayName = nullptr;
  this->PathArrayName = nullptr;
  this->CenterlineIdArrayName = nullptr;
  this->GlobalNodeIdArrayName = nullptr;
  this->CenterlineSectionNormalArrayName = nullptr;
  this->CenterlineSectionBifurcationArrayName = nullptr;

  this->SetRadiusArrayName("MaximumInscribedSphereRadius");
  this->SetBranchIdArrayName("BranchId");
  this->SetBifurcationIdArrayName("BifurcationId");
  this->SetPathArrayName("Path");
  this->SetCenterlineIdArrayName("CenterlineId");
  this->SetGlobalNodeIdArrayName("GlobalNodeId");
  this->SetCenterlineSectionNormalArrayName("CenterlineSectionNormal");
  this->SetCenterlineSectionBifurcationArrayName("CenterlineSectionBifurcation");

  this->NumberOfCenterlines = 0;

  this->Internals = new vtkInternals;
}

vtkvmtkPolyDataCenterlineBranchSplitting::~vtkvmtkPolyDataCenterlineBranchSplitting()
{
  this->Surface->Delete();
  this->Surface = nullptr;

  this->SetRadiusArrayName(nullptr);
  this->SetBranchIdArrayName(nullptr);
  this->SetBifurcationIdArrayName(nullptr);
  this->SetPathArrayName(nullptr);
  this->SetCenterlineIdArrayName(nullptr);
  this->SetGlobalNodeIdArrayName(nullptr);
  this->SetCenterlineSectionNormalArrayName(nullptr);
  this->SetCenterlineSectionBifurcationArrayName(nullptr);

  delete this->Internals;
}

int vtkvmtkPolyDataCenterlineBranchSplitting::RequestData(
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
    return 0;
    }

  const char* arrayNames[] = {this->CenterlineSectionAreaArrayName, this->CenterlineSectionMinSizeArrayName,
    this->CenterlineSectionMaxSizeArrayName, this->CenterlineSectionShapeArrayName, this->CenterlineSectionClosedArrayName,
    this->RadiusArrayName, this->BranchIdArrayName, this->BifurcationIdArrayName, this->PathArrayName,
    this->CenterlineIdArrayName, this->GlobalNodeIdArrayName, this->CenterlineSectionNormalArrayName,
    this->CenterlineSectionBifurcationArrayName};
  for (const char* arrayName : arrayNames)
    {
    if (!arrayName)
      {
      vtkErrorMacro(<<"An array name is not specified");
      return 0;
      }
    }

  if (!this->Centerlines->GetPointData()->GetArray(this->RadiusArrayName))
    {
    vtkErrorMacro(<<"Centerlines have no point data array named " << this->RadiusArrayName);
    return 0;
    }

  if (input->GetNumberOfPolys() == 0)
    {
    vtkErrorMacro(<<"Input surface has no polygons");
    return 0;
    }

  {
  ProgressStep step(this,"Computing surface normals",PROGRESS_NORMALS,PROGRESS_CLEAN);
  // vtkPolyDataNormals keeps normals the surface already has, which may point
  // inward, instead of computing oriented ones
  vtkNew<vtkPolyData> surface;
  surface->ShallowCopy(input);
  surface->GetPointData()->SetActiveNormals(nullptr);

  vtkNew<vtkPolyDataNormals> surfaceNormals;
  surfaceNormals->SetInputData(surface);
  surfaceNormals->SplittingOff();
  surfaceNormals->AutoOrientNormalsOn();
  surfaceNormals->ComputePointNormalsOn();
  surfaceNormals->ConsistencyOn();
  surfaceNormals->Update();
  this->Surface->DeepCopy(surfaceNormals->GetOutput());
  }
  if (this->AbortRequested())
    {
    return 1;
    }

  {
  ProgressStep step(this,"Merging and smoothing centerlines",PROGRESS_CLEAN,PROGRESS_LABEL_PRELIMINARY);
  if (!this->GenerateCleanCenterlines())
    {
    return 0;
    }

  this->ComputeTangents();

  if (!this->RefineCapPoints())
    {
    return 0;
    }
  }
  if (this->AbortRequested())
    {
    return 1;
    }

  // Whether each centerline point is in a bifurcation region, as found from
  // its section. Points that get no section count as bifurcation.
  vtkNew<vtkIntArray> centerlineBifurcation;
  centerlineBifurcation->SetName(this->CenterlineSectionBifurcationArrayName);
  centerlineBifurcation->SetNumberOfValues(this->Centerlines->GetNumberOfPoints());
  centerlineBifurcation->Fill(Bifurcation);
  this->Centerlines->GetPointData()->AddArray(centerlineBifurcation);

  vtkNew<vtkIntArray> sectionBifurcation;
  sectionBifurcation->SetName(this->CenterlineSectionBifurcationArrayName);
  output->GetCellData()->AddArray(sectionBifurcation);
  vtkNew<vtkIntArray> sectionGlobalNodeId;
  sectionGlobalNodeId->SetName(this->GlobalNodeIdArrayName);
  output->GetCellData()->AddArray(sectionGlobalNodeId);

  {
  ProgressStep step(this,"Labeling surface",PROGRESS_LABEL_PRELIMINARY,PROGRESS_SECTIONS);
  // Preliminary labels of the surface, which tell whether a section runs
  // across a boundary between branches
  if (!this->LabelSurface(BranchIdTmpArrayName,BifurcationIdTmpArrayName))
    {
    return 0;
    }
  }
  if (this->AbortRequested())
    {
    return 1;
    }

  std::vector<std::vector<vtkIdType>> pointCells = GetPointCells(this->Centerlines);
  this->Internals->NumberOfPointCells.resize(pointCells.size());
  for (size_t i=0; i<pointCells.size(); i++)
    {
    this->Internals->NumberOfPointCells[i] = static_cast<vtkIdType>(pointCells[i].size());
    }
  this->Internals->SectionBifurcation = sectionBifurcation;
  this->Internals->SectionGlobalNodeId = sectionGlobalNodeId;
  this->Internals->CenterlineBifurcation = centerlineBifurcation;
  this->Internals->CenterlineNormal = this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionNormalArrayName);

  {
  ProgressStep step(this,"Computing centerline sections",PROGRESS_SECTIONS,PROGRESS_GROUP);
  this->ComputeCenterlineSections(this->Surface,output);
  }

  this->Internals->NumberOfPointCells.clear();
  this->Internals->SectionBifurcation = nullptr;
  this->Internals->SectionGlobalNodeId = nullptr;
  this->Internals->CenterlineBifurcation = nullptr;
  this->Internals->CenterlineNormal = nullptr;

  if (this->AbortRequested())
    {
    return 1;
    }

  {
  ProgressStep step(this,"Grouping branches and bifurcations",PROGRESS_GROUP,PROGRESS_LABEL);
  if (!this->CleanBifurcations())
    {
    return 0;
    }

  if (!this->GroupCenterlines())
    {
    return 0;
    }
  }
  if (this->AbortRequested())
    {
    return 1;
    }

  {
  ProgressStep step(this,"Labeling surface",PROGRESS_LABEL,1.0);
  if (!this->LabelSurface(this->BranchIdArrayName,this->BifurcationIdArrayName) ||
      !this->LabelSurface(this->BifurcationIdArrayName,this->BranchIdArrayName))
    {
    return 0;
    }
  }

  this->Centerlines->GetPointData()->RemoveArray(BranchIdTmpArrayName);
  this->Centerlines->GetPointData()->RemoveArray(BifurcationIdTmpArrayName);
  this->Surface->GetPointData()->RemoveArray(BranchIdTmpArrayName);

  this->UpdateProgress(1.0);
  return 1;
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::GenerateCleanCenterlines()
{
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputData(this->Centerlines);
  cleaner->PointMergingOn();
  cleaner->Update();
  vtkPolyData* centerlines = cleaner->GetOutput();

  this->NumberOfCenterlines = centerlines->GetNumberOfCells();
  if (this->NumberOfCenterlines == 0)
    {
    vtkErrorMacro(<<"Centerlines have no cells");
    return false;
    }

  if (!IsOnePiece(centerlines))
    {
    vtkErrorMacro(<<"Centerlines consist of more than one piece");
    return false;
    }

  const vtkIdType numberOfPoints = centerlines->GetNumberOfPoints();
  vtkDataArray* inputRadius = centerlines->GetPointData()->GetArray(this->RadiusArrayName);

  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> lines;

  vtkNew<vtkDoubleArray> radius;
  radius->SetName(this->RadiusArrayName);
  radius->SetNumberOfValues(numberOfPoints);
  radius->Fill(0.0);

  vtkNew<vtkIntArray> globalNodeId;
  globalNodeId->SetName(this->GlobalNodeIdArrayName);
  globalNodeId->SetNumberOfValues(numberOfPoints);
  globalNodeId->Fill(0);

  vtkNew<vtkIntArray> centerlineId;
  centerlineId->SetName(this->CenterlineIdArrayName);
  centerlineId->SetNumberOfComponents(this->NumberOfCenterlines);
  centerlineId->SetNumberOfTuples(numberOfPoints);
  centerlineId->Fill(0);

  // Number the points in the order the centerlines reach them, starting from
  // the inlet, and connect each new point to the previous one on its
  // centerline. Points shared with an earlier centerline are not repeated.
  std::vector<vtkIdType> newPointIds(numberOfPoints,-1);
  newPointIds[0] = points->InsertNextPoint(centerlines->GetPoint(0));
  radius->SetValue(0,inputRadius->GetTuple1(0));

  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType c=0; c<this->NumberOfCenterlines; c++)
    {
    centerlines->GetCellPoints(c,cellPointIds);
    if (cellPointIds->GetNumberOfIds() == 0 || newPointIds[cellPointIds->GetId(0)] < 0)
      {
      vtkErrorMacro(<<"Centerlines must all start from the same inlet point");
      return false;
      }
    for (vtkIdType p=0; p<cellPointIds->GetNumberOfIds(); p++)
      {
      vtkIdType id = cellPointIds->GetId(p);
      if (newPointIds[id] < 0)
        {
        vtkIdType newId = points->InsertNextPoint(centerlines->GetPoint(id));
        newPointIds[id] = newId;
        vtkIdType line[2] = {newPointIds[cellPointIds->GetId(p-1)], newId};
        lines->InsertNextCell(2,line);
        radius->SetValue(newId,inputRadius->GetTuple1(id));
        globalNodeId->SetValue(newId,newId);
        }
      centerlineId->SetComponent(newPointIds[id],c,1);
      }
    }

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);
  polyData->SetLines(lines);

  if (polyData->GetNumberOfPoints() != numberOfPoints)
    {
    vtkErrorMacro(<<"Centerlines have points that belong to no centerline");
    return false;
    }

  if (polyData->GetNumberOfPoints() != polyData->GetNumberOfCells() + 1)
    {
    vtkErrorMacro(<<"Centerlines do not form a tree");
    return false;
    }

  // Preliminary branches: a new branch starts after each outlet and each
  // point where the centerlines split
  vtkNew<vtkIntArray> bifurcationIds;
  bifurcationIds->SetName(BifurcationIdTmpArrayName);
  bifurcationIds->SetNumberOfValues(numberOfPoints);
  bifurcationIds->Fill(Undefined);
  vtkNew<vtkIntArray> branchIds;
  branchIds->SetName(BranchIdTmpArrayName);
  branchIds->SetNumberOfValues(numberOfPoints);
  branchIds->Fill(Undefined);

  polyData->GetPointData()->AddArray(bifurcationIds);
  polyData->GetPointData()->AddArray(branchIds);
  polyData->GetPointData()->AddArray(radius);
  polyData->GetPointData()->AddArray(globalNodeId);
  polyData->GetPointData()->AddArray(centerlineId);

  std::vector<std::vector<vtkIdType>> pointCells = GetPointCells(polyData);

  int numberOfBranches = 0;
  for (vtkIdType p=0; p<numberOfPoints; p++)
    {
    branchIds->SetValue(p,numberOfBranches);
    if (pointCells[p].size() == 1 && p != 0)
      {
      numberOfBranches++;
      }
    else if (pointCells[p].size() > 2)
      {
      bifurcationIds->SetValue(p,Bifurcation);
      numberOfBranches++;
      }
    }

  vtkNew<vtkPointLocator> locator;
  BuildPointLocator(polyData,locator);
  vtkNew<vtkIdList> closePoints;
  double point[3];

  // Points of the branches leaving a bifurcation that are within the
  // inscribed sphere of the bifurcation point are part of the bifurcation
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    if (pointCells[i].size() <= 2)
      {
      continue;
      }
    int upstreamBranchId = branchIds->GetValue(i);
    std::set<int> downstreamBranchIds;
    for (vtkIdType cellId : pointCells[i])
      {
      polyData->GetCellPoints(cellId,cellPointIds);
      for (vtkIdType k=0; k<cellPointIds->GetNumberOfIds(); k++)
        {
        int id = branchIds->GetValue(cellPointIds->GetId(k));
        if (id != upstreamBranchId)
          {
          downstreamBranchIds.insert(id);
          }
        }
      }
    polyData->GetPoint(i,point);
    locator->FindPointsWithinRadius(radius->GetValue(i),point,closePoints);
    for (vtkIdType k=0; k<closePoints->GetNumberOfIds(); k++)
      {
      vtkIdType closePointId = closePoints->GetId(k);
      if (downstreamBranchIds.count(branchIds->GetValue(closePointId)))
        {
        bifurcationIds->SetValue(closePointId,BifurcationDownstream);
        }
      }
    }

  // Points within two inscribed sphere radii of a cap are smoothed more
  // strongly, perpendicular to the centerline, to remove the wiggles often
  // seen there
  std::vector<bool> nearCap(numberOfPoints,false);
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    if (pointCells[i].size() != 1)
      {
      continue;
      }
    polyData->GetPoint(i,point);
    locator->FindPointsWithinRadius(2.0 * radius->GetValue(i),point,closePoints);
    for (vtkIdType k=0; k<closePoints->GetNumberOfIds(); k++)
      {
      vtkIdType closePointId = closePoints->GetId(k);
      if (branchIds->GetValue(closePointId) == branchIds->GetValue(i))
        {
        nearCap[closePointId] = true;
        }
      }
    }

  // Moving average along each branch. Points of a branch are numbered
  // consecutively, and only points whose two neighbors are in the same branch
  // are moved.
  const int numberOfIterations = 200;
  const double capRelaxation = 1.0;
  const double relaxation = 0.01;
  double point0[3], point1[3], point2[3], displacement[3], tangent[3];
  for (int iteration=0; iteration<numberOfIterations; iteration++)
    {
    for (vtkIdType k=1; k<numberOfPoints-1; k++)
      {
      int branchId = branchIds->GetValue(k);
      if (branchIds->GetValue(k-1) != branchId || branchIds->GetValue(k+1) != branchId)
        {
        continue;
        }
      points->GetPoint(k-1,point0);
      points->GetPoint(k,point1);
      points->GetPoint(k+1,point2);
      for (int l=0; l<3; l++)
        {
        displacement[l] = 0.5 * (point0[l] + point2[l]) - point1[l];
        }
      if (nearCap[k])
        {
        double distance01 = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        double distance12 = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
        for (int l=0; l<3; l++)
          {
          tangent[l] = (point1[l] - point0[l]) / distance01 + (point2[l] - point1[l]) / distance12;
          }
        vtkMath::Normalize(tangent);
        double tangentialDisplacement = vtkMath::Dot(displacement,tangent);
        for (int l=0; l<3; l++)
          {
          point1[l] += capRelaxation * (displacement[l] - tangentialDisplacement * tangent[l]);
          }
        }
      else
        {
        for (int l=0; l<3; l++)
          {
          point1[l] += relaxation * displacement[l];
          }
        }
      points->SetPoint(k,point1);
      }
    }

  this->Centerlines->DeepCopy(polyData);
  return true;
}

void vtkvmtkPolyDataCenterlineBranchSplitting::ComputeTangents()
{
  const vtkIdType numberOfPoints = this->Centerlines->GetNumberOfPoints();
  vtkNew<vtkDoubleArray> tangents;
  tangents->SetName(this->CenterlineSectionNormalArrayName);
  tangents->SetNumberOfComponents(3);
  tangents->SetNumberOfTuples(numberOfPoints);

  vtkDataArray* surfaceNormals = this->Surface->GetPointData()->GetNormals();
  vtkNew<vtkPointLocator> locator;
  BuildPointLocator(this->Surface,locator);

  std::vector<std::vector<vtkIdType>> pointCells = GetPointCells(this->Centerlines);
  vtkNew<vtkIdList> cellPointIds;
  double point[3], point0[3], point1[3], direction[3];
  for (vtkIdType p=0; p<numberOfPoints; p++)
    {
    double tangent[3] = {0.0, 0.0, 0.0};
    if (pointCells[p].size() == 1)
      {
      // A cap: use the normal of the surface, which points out of it, and
      // flip it at the inlet so that tangents follow the tree
      this->Centerlines->GetPoint(p,point);
      surfaceNormals->GetTuple(locator->FindClosestPoint(point),tangent);
      if (p == 0)
        {
        vtkMath::MultiplyScalar(tangent,-1.0);
        }
      }
    else
      {
      for (vtkIdType cellId : pointCells[p])
        {
        this->Centerlines->GetCellPoints(cellId,cellPointIds);
        this->Centerlines->GetPoint(cellPointIds->GetId(0),point0);
        this->Centerlines->GetPoint(cellPointIds->GetId(1),point1);
        vtkMath::Subtract(point1,point0,direction);
        vtkMath::Normalize(direction);
        vtkMath::Add(tangent,direction,tangent);
        }
      vtkMath::Normalize(tangent);
      }
    tangents->SetTuple(p,tangent);
    }

  this->Centerlines->GetPointData()->AddArray(tangents);
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::RefineCapPoints()
{
  vtkPolyData* centerlines = this->Centerlines;
  const vtkIdType numberOfPoints = centerlines->GetNumberOfPoints();
  std::vector<std::vector<vtkIdType>> pointCells = GetPointCells(centerlines);

  vtkDataArray* radius = centerlines->GetPointData()->GetArray(this->RadiusArrayName);
  vtkDataArray* tangents = centerlines->GetPointData()->GetArray(this->CenterlineSectionNormalArrayName);
  vtkDataArray* bifurcationIds = centerlines->GetPointData()->GetArray(BifurcationIdTmpArrayName);
  vtkDataArray* branchIds = centerlines->GetPointData()->GetArray(BranchIdTmpArrayName);
  vtkDataArray* centerlineId = centerlines->GetPointData()->GetArray(this->CenterlineIdArrayName);

  // New point ids: a cap is preceded by the new point at the outlets and
  // followed by it at the inlet, so that ids keep increasing along the tree
  std::vector<vtkIdType> newPointIds(numberOfPoints,-1);
  std::vector<vtkIdType> newCapNeighborIds(numberOfPoints,-1);
  vtkIdType numberOfNewPoints = 0;
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    if (pointCells[i].size() != 1)
      {
      newPointIds[i] = numberOfNewPoints++;
      }
    else if (i == 0)
      {
      newPointIds[i] = numberOfNewPoints++;
      newCapNeighborIds[i] = numberOfNewPoints++;
      }
    else
      {
      newCapNeighborIds[i] = numberOfNewPoints++;
      newPointIds[i] = numberOfNewPoints++;
      }
    }

  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(numberOfNewPoints);

  vtkNew<vtkDoubleArray> newRadius;
  newRadius->SetName(this->RadiusArrayName);
  newRadius->SetNumberOfValues(numberOfNewPoints);
  vtkNew<vtkDoubleArray> newTangents;
  newTangents->SetName(this->CenterlineSectionNormalArrayName);
  newTangents->SetNumberOfComponents(3);
  newTangents->SetNumberOfTuples(numberOfNewPoints);
  vtkNew<vtkIntArray> newBifurcationIds;
  newBifurcationIds->SetName(BifurcationIdTmpArrayName);
  newBifurcationIds->SetNumberOfValues(numberOfNewPoints);
  vtkNew<vtkIntArray> newBranchIds;
  newBranchIds->SetName(BranchIdTmpArrayName);
  newBranchIds->SetNumberOfValues(numberOfNewPoints);
  vtkNew<vtkIntArray> newGlobalNodeId;
  newGlobalNodeId->SetName(this->GlobalNodeIdArrayName);
  newGlobalNodeId->SetNumberOfValues(numberOfNewPoints);
  vtkNew<vtkIntArray> newCenterlineId;
  newCenterlineId->SetName(this->CenterlineIdArrayName);
  newCenterlineId->SetNumberOfComponents(this->NumberOfCenterlines);
  newCenterlineId->SetNumberOfTuples(numberOfNewPoints);

  vtkNew<vtkIdList> cellPointIds;
  double point[3], neighborPoint[3], tangent[3], neighborTangent[3];
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    centerlines->GetPoint(i,point);
    tangents->GetTuple(i,tangent);

    std::vector<vtkIdType> ids = {newPointIds[i]};
    if (newCapNeighborIds[i] >= 0)
      {
      ids.push_back(newCapNeighborIds[i]);
      }
    for (vtkIdType id : ids)
      {
      newBifurcationIds->SetValue(id,static_cast<int>(bifurcationIds->GetTuple1(i)));
      newBranchIds->SetValue(id,static_cast<int>(branchIds->GetTuple1(i)));
      newCenterlineId->SetTuple(id,i,centerlineId);
      newGlobalNodeId->SetValue(id,static_cast<int>(id));
      }

    points->SetPoint(newPointIds[i],point);
    newRadius->SetValue(newPointIds[i],radius->GetTuple1(i));
    newTangents->SetTuple(newPointIds[i],tangent);

    if (newCapNeighborIds[i] < 0)
      {
      continue;
      }
    // The new point is halfway to the other point of the cap's line
    centerlines->GetCellPoints(pointCells[i][0],cellPointIds);
    vtkIdType neighborId = cellPointIds->GetId(0) == i ? cellPointIds->GetId(1) : cellPointIds->GetId(0);
    centerlines->GetPoint(neighborId,neighborPoint);
    tangents->GetTuple(neighborId,neighborTangent);
    double newPoint[3], newTangent[3];
    for (int j=0; j<3; j++)
      {
      newPoint[j] = 0.5 * (point[j] + neighborPoint[j]);
      newTangent[j] = 0.5 * (tangent[j] + neighborTangent[j]);
      }
    vtkMath::Normalize(newTangent);
    points->SetPoint(newCapNeighborIds[i],newPoint);
    newTangents->SetTuple(newCapNeighborIds[i],newTangent);
    newRadius->SetValue(newCapNeighborIds[i],0.5 * (radius->GetTuple1(i) + radius->GetTuple1(neighborId)));
    }

  // Lines keep the order of the cells, with the line to a new point inserted
  // next to the line of its cap
  vtkNew<vtkCellArray> lines;
  if (newCapNeighborIds[0] >= 0)
    {
    vtkIdType line[2] = {newPointIds[0], newCapNeighborIds[0]};
    lines->InsertNextCell(2,line);
    }
  for (vtkIdType cellId=0; cellId<centerlines->GetNumberOfCells(); cellId++)
    {
    centerlines->GetCellPoints(cellId,cellPointIds);
    vtkIdType pointId0 = cellPointIds->GetId(0);
    vtkIdType pointId1 = cellPointIds->GetId(1);
    if (pointId1 == 0 || (pointId0 != 0 && newCapNeighborIds[pointId0] >= 0))
      {
      vtkErrorMacro(<<"Centerline lines are not oriented from the inlet");
      return false;
      }
    vtkIdType newPointId0 = pointId0 == 0 && newCapNeighborIds[0] >= 0 ? newCapNeighborIds[0] : newPointIds[pointId0];
    if (newCapNeighborIds[pointId1] >= 0)
      {
      vtkIdType line0[2] = {newPointId0, newCapNeighborIds[pointId1]};
      vtkIdType line1[2] = {newCapNeighborIds[pointId1], newPointIds[pointId1]};
      lines->InsertNextCell(2,line0);
      lines->InsertNextCell(2,line1);
      }
    else
      {
      vtkIdType line[2] = {newPointId0, newPointIds[pointId1]};
      lines->InsertNextCell(2,line);
      }
    }

  vtkNew<vtkPolyData> refined;
  refined->SetPoints(points);
  refined->SetLines(lines);
  refined->GetPointData()->AddArray(newBifurcationIds);
  refined->GetPointData()->AddArray(newBranchIds);
  refined->GetPointData()->AddArray(newRadius);
  refined->GetPointData()->AddArray(newCenterlineId);
  refined->GetPointData()->AddArray(newGlobalNodeId);
  refined->GetPointData()->AddArray(newTangents);

  this->Centerlines->DeepCopy(refined);
  return true;
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::LabelSurface(const char* labelArrayName, const char* otherArrayName)
{
  vtkDataArray* centerlineLabels = this->Centerlines->GetPointData()->GetArray(labelArrayName);
  vtkDataArray* centerlineOtherLabels = this->Centerlines->GetPointData()->GetArray(otherArrayName);
  if (!centerlineLabels || !centerlineOtherLabels)
    {
    vtkErrorMacro(<<"Centerlines have no point data array named " << (centerlineLabels ? otherArrayName : labelArrayName));
    return false;
    }

  const vtkIdType numberOfSurfacePoints = this->Surface->GetNumberOfPoints();
  vtkNew<vtkIntArray> surfaceLabels;
  surfaceLabels->SetName(labelArrayName);
  surfaceLabels->SetNumberOfValues(numberOfSurfacePoints);
  surfaceLabels->Fill(-1);
  std::vector<double> labelDistances(numberOfSurfacePoints,-1.0);

  vtkNew<vtkPointLocator> locator;
  BuildPointLocator(this->Surface,locator);

  vtkDataArray* radius = this->Centerlines->GetPointData()->GetArray(this->RadiusArrayName);
  vtkDataArray* surfaceNormals = this->Surface->GetPointData()->GetNormals();

  vtkNew<vtkIdList> surfacePointIds;
  double centerlinePoint[3], surfacePoint[3], normal[3], offset[3];
  for (vtkIdType i=0; i<this->Centerlines->GetNumberOfPoints(); i++)
    {
    if (centerlineOtherLabels->GetTuple1(i) != -1)
      {
      continue;
      }
    int label = static_cast<int>(centerlineLabels->GetTuple1(i));

    this->Centerlines->GetPoint(i,centerlinePoint);
    locator->FindPointsWithinRadius(10.0 * radius->GetTuple1(i),centerlinePoint,surfacePointIds);
    for (vtkIdType j=0; j<surfacePointIds->GetNumberOfIds(); j++)
      {
      vtkIdType surfacePointId = surfacePointIds->GetId(j);
      this->Surface->GetPoint(surfacePointId,surfacePoint);
      surfaceNormals->GetTuple(surfacePointId,normal);
      vtkMath::Subtract(surfacePoint,centerlinePoint,offset);
      double distance = vtkMath::Norm(offset);
      // Only surface points that face the centerline point from the outside
      // TODO: the tolerance is in absolute units
      if (vtkMath::Dot(offset,normal) < -1.0e-2)
        {
        continue;
        }
      if (surfaceLabels->GetValue(surfacePointId) > -1 && distance > labelDistances[surfacePointId])
        {
        continue;
        }
      surfaceLabels->SetValue(surfacePointId,label);
      labelDistances[surfacePointId] = distance;
      }
    }

  this->Surface->GetPointData()->AddArray(surfaceLabels);
  return true;
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::CleanBifurcations()
{
  vtkPolyData* centerlines = this->Centerlines;
  const vtkIdType numberOfPoints = centerlines->GetNumberOfPoints();

  vtkIntArray* bifurcation = vtkIntArray::SafeDownCast(centerlines->GetPointData()->GetArray(this->CenterlineSectionBifurcationArrayName));
  vtkDataArray* bifurcationIds = centerlines->GetPointData()->GetArray(BifurcationIdTmpArrayName);

  vtkNew<vtkIntArray> remove;
  remove->SetName(RemoveArrayName);
  remove->SetNumberOfValues(numberOfPoints);
  remove->Fill(0);
  centerlines->GetPointData()->AddArray(remove);

  // Points where the centerlines split are always in a bifurcation region, so
  // that each bifurcation region is connected
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    if (bifurcationIds->GetTuple1(i) == Bifurcation)
      {
      bifurcation->SetValue(i,Bifurcation);
      }
    }

  vtkNew<vtkPolyData> bifurcations;
  vtkNew<vtkPolyData> branches;
  this->SplitCenterlines(bifurcations,branches);

  // A bifurcation region whose points are all in one preliminary branch is in
  // the middle of a branch: it becomes branch again, and its points other
  // than caps are removed
  if (bifurcations->GetNumberOfCells() > 0)
    {
    vtkNew<vtkConnectivityFilter> connectivity;
    connectivity->SetInputData(bifurcations);
    connectivity->SetExtractionModeToAllRegions();
    connectivity->ColorRegionsOn();
    connectivity->Update();

    vtkNew<vtkThreshold> threshold;
    threshold->SetInputConnection(connectivity->GetOutputPort());
    threshold->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,RegionIdArrayName);
    threshold->SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);

    vtkNew<vtkIdList> cellPointIds;
    for (vtkIdType regionId=0; regionId<connectivity->GetNumberOfExtractedRegions(); regionId++)
      {
      threshold->SetLowerThreshold(regionId);
      threshold->SetUpperThreshold(regionId);
      threshold->Update();
      vtkUnstructuredGrid* region = threshold->GetOutput();
      vtkDataArray* regionBranchIds = region->GetPointData()->GetArray(BranchIdTmpArrayName);
      vtkDataArray* regionGlobalNodeIds = region->GetPointData()->GetArray(this->GlobalNodeIdArrayName);

      std::set<int> branchIdsInRegion;
      for (vtkIdType j=0; j<region->GetNumberOfPoints(); j++)
        {
        branchIdsInRegion.insert(static_cast<int>(regionBranchIds->GetTuple1(j)));
        }
      if (branchIdsInRegion.size() != 1)
        {
        continue;
        }

      std::vector<int> numberOfRegionPointCells(region->GetNumberOfPoints(),0);
      for (vtkIdType cellId=0; cellId<region->GetNumberOfCells(); cellId++)
        {
        region->GetCellPoints(cellId,cellPointIds);
        for (vtkIdType k=0; k<cellPointIds->GetNumberOfIds(); k++)
          {
          numberOfRegionPointCells[cellPointIds->GetId(k)]++;
          }
        }
      for (vtkIdType j=0; j<region->GetNumberOfPoints(); j++)
        {
        vtkIdType pointId = static_cast<vtkIdType>(regionGlobalNodeIds->GetTuple1(j));
        bifurcation->SetValue(pointId,Branch);
        if (numberOfRegionPointCells[j] != 1)
          {
          remove->SetValue(pointId,1);
          }
        }
      }
    }

  // A branch point with no branch neighbor between bifurcations is part of
  // the bifurcation
  std::vector<std::vector<vtkIdType>> pointCells = GetPointCells(centerlines);
  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    if (bifurcation->GetValue(i) != Branch)
      {
      continue;
      }
    bool isolated = true;
    for (vtkIdType cellId : pointCells[i])
      {
      centerlines->GetCellPoints(cellId,cellPointIds);
      for (vtkIdType k=0; k<cellPointIds->GetNumberOfIds(); k++)
        {
        vtkIdType id = cellPointIds->GetId(k);
        if (id != i && bifurcation->GetValue(id) == Branch)
          {
          isolated = false;
          }
        }
      }
    if (isolated)
      {
      bifurcation->SetValue(i,Bifurcation);
      }
    }

  // The two points at each cap are in a branch
  bifurcation->SetValue(0,Branch);
  bifurcation->SetValue(1,Branch);
  for (vtkIdType i=1; i<numberOfPoints; i++)
    {
    if (pointCells[i].size() == 1)
      {
      bifurcation->SetValue(i-1,Branch);
      bifurcation->SetValue(i,Branch);
      }
    }

  // Remove the points marked for removal with the lines that use them
  vtkNew<vtkPointDataToCellData> pointToCellData;
  pointToCellData->SetInputData(centerlines);
  pointToCellData->PassPointDataOn();

  vtkNew<vtkThreshold> threshold;
  threshold->SetInputConnection(pointToCellData->GetOutputPort());
  threshold->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,RemoveArrayName);
  threshold->SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);
  threshold->SetLowerThreshold(0.0);
  threshold->SetUpperThreshold(0.0);

  threshold->Update();
  vtkNew<vtkPolyData> kept;
  LinesToPolyData(threshold->GetOutput(),kept);

  // Points keep their order, so a gap in the old point ids is where points
  // were removed: connect the points on either side of it
  vtkDataArray* oldGlobalNodeIds = kept->GetPointData()->GetArray(this->GlobalNodeIdArrayName);
  vtkNew<vtkCellArray> lines;
  lines->DeepCopy(kept->GetLines());
  vtkNew<vtkIntArray> globalNodeIds;
  globalNodeIds->SetName(this->GlobalNodeIdArrayName);
  globalNodeIds->SetNumberOfValues(kept->GetNumberOfPoints());
  if (kept->GetNumberOfPoints() > 0)
    {
    globalNodeIds->SetValue(0,0);
    }
  for (vtkIdType i=1; i<kept->GetNumberOfPoints(); i++)
    {
    if (oldGlobalNodeIds->GetTuple1(i-1) + 1 != oldGlobalNodeIds->GetTuple1(i))
      {
      vtkIdType line[2] = {i-1, i};
      lines->InsertNextCell(2,line);
      }
    globalNodeIds->SetValue(i,static_cast<int>(i));
    }

  vtkNew<vtkPolyData> cleaned;
  cleaned->SetPoints(kept->GetPoints());
  cleaned->SetLines(lines);
  cleaned->GetPointData()->ShallowCopy(kept->GetPointData());
  cleaned->GetPointData()->RemoveArray(RemoveArrayName);
  cleaned->GetPointData()->AddArray(globalNodeIds);

  if (cleaned->GetNumberOfPoints() != cleaned->GetNumberOfCells() + 1)
    {
    vtkErrorMacro(<<"Centerlines do not form a tree after removing bifurcation regions within branches");
    return false;
    }

  this->Centerlines->DeepCopy(cleaned);
  return true;
}

void vtkvmtkPolyDataCenterlineBranchSplitting::SplitCenterlines(vtkPolyData* bifurcations, vtkPolyData* branches)
{
  // A line is in a bifurcation region if either of its points is
  vtkDataArray* pointBifurcation = this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionBifurcationArrayName);
  vtkNew<vtkIntArray> cellBifurcation;
  cellBifurcation->SetName(this->CenterlineSectionBifurcationArrayName);
  cellBifurcation->SetNumberOfValues(this->Centerlines->GetNumberOfCells());
  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType cellId=0; cellId<this->Centerlines->GetNumberOfCells(); cellId++)
    {
    this->Centerlines->GetCellPoints(cellId,cellPointIds);
    int value = Branch;
    for (vtkIdType k=0; k<cellPointIds->GetNumberOfIds(); k++)
      {
      if (pointBifurcation->GetTuple1(cellPointIds->GetId(k)) == Bifurcation)
        {
        value = Bifurcation;
        }
      }
    cellBifurcation->SetValue(cellId,value);
    }
  this->Centerlines->GetCellData()->AddArray(cellBifurcation);

  vtkNew<vtkThreshold> threshold;
  threshold->SetInputData(this->Centerlines);
  threshold->SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);
  threshold->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,this->CenterlineSectionBifurcationArrayName);

  for (int value : {Branch, Bifurcation})
    {
    threshold->SetLowerThreshold(value);
    threshold->SetUpperThreshold(value);
    threshold->Update();
    LinesToPolyData(threshold->GetOutput(),value == Branch ? branches : bifurcations);
    }
}

void vtkvmtkPolyDataCenterlineBranchSplitting::LabelConnectedRegions(vtkPolyData* lines, const char* labelArrayName, const char* otherArrayName)
{
  vtkNew<vtkConnectivityFilter> connectivity;
  connectivity->SetInputData(lines);
  connectivity->SetExtractionModeToAllRegions();
  connectivity->ColorRegionsOn();
  connectivity->Update();

  vtkNew<vtkPolyData> labeled;
  labeled->DeepCopy(connectivity->GetOutput());
  vtkDataArray* regionIds = labeled->GetPointData()->GetArray(RegionIdArrayName);
  vtkDataArray* globalNodeIds = labeled->GetPointData()->GetArray(this->GlobalNodeIdArrayName);
  const vtkIdType numberOfPoints = labeled->GetNumberOfPoints();

  // The path of each point is the length of the polyline through the points
  // of its region, in the order of their ids, up to the point
  vtkNew<vtkDoubleArray> path;
  path->SetName(this->PathArrayName);
  path->SetNumberOfValues(numberOfPoints);
  path->Fill(-1.0);

  std::vector<std::vector<std::pair<double,vtkIdType>>> regionPoints(connectivity->GetNumberOfExtractedRegions());
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    regionPoints[static_cast<size_t>(regionIds->GetTuple1(i))].push_back({globalNodeIds->GetTuple1(i), i});
    }
  double point[3], previousPoint[3];
  for (std::vector<std::pair<double,vtkIdType>>& points : regionPoints)
    {
    std::sort(points.begin(),points.end());
    double distance = 0.0;
    for (size_t k=0; k<points.size(); k++)
      {
      labeled->GetPoint(points[k].second,point);
      if (k > 0)
        {
        distance += sqrt(vtkMath::Distance2BetweenPoints(point,previousPoint));
        }
      path->SetValue(points[k].second,distance);
      previousPoint[0] = point[0];
      previousPoint[1] = point[1];
      previousPoint[2] = point[2];
      }
    }
  labeled->GetPointData()->AddArray(path);

  RenameRegionIds(labeled->GetPointData(),labelArrayName);
  RenameRegionIds(labeled->GetCellData(),labelArrayName);

  vtkNew<vtkIdTypeArray> otherPointIds;
  otherPointIds->SetName(otherArrayName);
  otherPointIds->SetNumberOfValues(numberOfPoints);
  otherPointIds->Fill(-1);
  labeled->GetPointData()->AddArray(otherPointIds);
  vtkNew<vtkIdTypeArray> otherCellIds;
  otherCellIds->SetName(otherArrayName);
  otherCellIds->SetNumberOfValues(labeled->GetNumberOfCells());
  otherCellIds->Fill(-1);
  labeled->GetCellData()->AddArray(otherCellIds);

  lines->DeepCopy(labeled);
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::GroupCenterlines()
{
  vtkNew<vtkPolyData> bifurcations;
  vtkNew<vtkPolyData> branches;
  this->SplitCenterlines(bifurcations,branches);

  vtkNew<vtkAppendFilter> append;
  append->MergePointsOn();
  if (bifurcations->GetNumberOfCells() > 0)
    {
    this->LabelConnectedRegions(bifurcations,this->BifurcationIdArrayName,this->BranchIdArrayName);
    append->AddInputData(bifurcations);
    }
  if (branches->GetNumberOfCells() > 0)
    {
    this->LabelConnectedRegions(branches,this->BranchIdArrayName,this->BifurcationIdArrayName);
    append->AddInputData(branches);
    }
  if (append->GetNumberOfInputConnections(0) == 0)
    {
    vtkErrorMacro(<<"Centerlines have no lines left");
    return false;
    }

  append->Update();
  vtkNew<vtkPolyData> merged;
  LinesToPolyData(append->GetOutput(),merged);

  if (!IsOnePiece(merged))
    {
    vtkErrorMacro(<<"Centerlines consist of more than one piece after grouping");
    return false;
    }

  // Put the points back in the order of their ids
  const vtkIdType numberOfPoints = merged->GetNumberOfPoints();
  vtkDataArray* globalNodeIds = merged->GetPointData()->GetArray(this->GlobalNodeIdArrayName);
  std::vector<vtkIdType> mergedPointIds(numberOfPoints,-1);
  for (vtkIdType i=0; i<numberOfPoints; i++)
    {
    vtkIdType id = static_cast<vtkIdType>(globalNodeIds->GetTuple1(i));
    if (id < 0 || id >= numberOfPoints || mergedPointIds[id] >= 0)
      {
      vtkErrorMacro(<<"Centerline point ids are not unique after grouping");
      return false;
      }
    mergedPointIds[id] = i;
    }

  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(numberOfPoints);
  vtkNew<vtkPolyData> ordered;
  ordered->GetPointData()->CopyAllocate(merged->GetPointData(),numberOfPoints);
  for (vtkIdType id=0; id<numberOfPoints; id++)
    {
    points->SetPoint(id,merged->GetPoint(mergedPointIds[id]));
    ordered->GetPointData()->CopyData(merged->GetPointData(),mergedPointIds[id],id);
    }

  vtkNew<vtkCellArray> lines;
  vtkNew<vtkIdList> cellPointIds;
  for (vtkIdType cellId=0; cellId<merged->GetNumberOfCells(); cellId++)
    {
    merged->GetCellPoints(cellId,cellPointIds);
    lines->InsertNextCell(cellPointIds->GetNumberOfIds());
    for (vtkIdType k=0; k<cellPointIds->GetNumberOfIds(); k++)
      {
      lines->InsertCellPoint(static_cast<vtkIdType>(globalNodeIds->GetTuple1(cellPointIds->GetId(k))));
      }
    }

  ordered->SetPoints(points);
  ordered->SetLines(lines);

  this->Centerlines->DeepCopy(ordered);
  return true;
}

bool vtkvmtkPolyDataCenterlineBranchSplitting::ComputeSectionPlane(vtkIdType cellId, vtkIdType cellPointIndex, double origin[3], double normal[3])
{
  vtkNew<vtkIdList> cellPointIds;
  this->Centerlines->GetCellPoints(cellId,cellPointIds);
  vtkIdType pointId = cellPointIds->GetId(cellPointIndex);

  this->Centerlines->GetPoint(pointId,origin);
  this->Internals->CenterlineNormal->GetTuple(pointId,normal);
  if (vtkMath::Norm(normal) == 0.0)
    {
    return false;
    }

  // Move the plane slightly into the tree at caps, so that it cuts the vessel
  // wall rather than the cap
  if (this->Internals->NumberOfPointCells[pointId] == 1)
    {
    const double offset = pointId == 0 ? -1.0e-3 : 1.0e-3;
    for (int j=0; j<3; j++)
      {
      origin[j] -= offset * normal[j];
      }
    }
  return true;
}

void vtkvmtkPolyDataCenterlineBranchSplitting::ExtractSection(vtkPolyData* input, double origin[3], double normal[3], vtkPolyData* section, bool & closed)
{
  vtkvmtkPolyDataBranchSections::ExtractCylinderSection(input,origin,normal,section,closed,BranchIdTmpArrayName);
  // Sections of only a few points are slivers of the surface
  if (section->GetNumberOfPoints() < 4)
    {
    section->Initialize();
    }
}

void vtkvmtkPolyDataCenterlineBranchSplitting::ProcessSection(vtkPolyData* vtkNotUsed(input), vtkIdType pointId, vtkIdType vtkNotUsed(sectionId), vtkPolyData* section, double origin[3], double normal[3], bool vtkNotUsed(closed))
{
  // A section is in a single branch if one centerline passes through it and
  // it touches the surface of one branch
  int numberOfCenterlines = vtkvmtkPolyDataBranchSections::CountBranchSectionCenterlines(section,this->Centerlines,origin,normal);
  int numberOfBranches = vtkvmtkPolyDataBranchSections::CountBranchSectionIds(section,BranchIdTmpArrayName);
  int bifurcation = numberOfCenterlines == 1 && numberOfBranches == 1 ? Branch : Bifurcation;

  this->Internals->SectionBifurcation->InsertNextValue(bifurcation);
  this->Internals->SectionGlobalNodeId->InsertNextValue(static_cast<int>(pointId));
  this->Internals->CenterlineBifurcation->SetValue(pointId,bifurcation);
}

void vtkvmtkPolyDataCenterlineBranchSplitting::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "RadiusArrayName: " << (this->RadiusArrayName ? this->RadiusArrayName : "(none)") << "\n";
  os << indent << "BranchIdArrayName: " << (this->BranchIdArrayName ? this->BranchIdArrayName : "(none)") << "\n";
  os << indent << "BifurcationIdArrayName: " << (this->BifurcationIdArrayName ? this->BifurcationIdArrayName : "(none)") << "\n";
  os << indent << "PathArrayName: " << (this->PathArrayName ? this->PathArrayName : "(none)") << "\n";
  os << indent << "CenterlineIdArrayName: " << (this->CenterlineIdArrayName ? this->CenterlineIdArrayName : "(none)") << "\n";
  os << indent << "GlobalNodeIdArrayName: " << (this->GlobalNodeIdArrayName ? this->GlobalNodeIdArrayName : "(none)") << "\n";
  os << indent << "CenterlineSectionNormalArrayName: " << (this->CenterlineSectionNormalArrayName ? this->CenterlineSectionNormalArrayName : "(none)") << "\n";
  os << indent << "CenterlineSectionBifurcationArrayName: " << (this->CenterlineSectionBifurcationArrayName ? this->CenterlineSectionBifurcationArrayName : "(none)") << "\n";
}
