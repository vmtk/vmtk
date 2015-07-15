/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataSurfaceRemeshing.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataSurfaceRemeshing.h"
#include "vtkvmtkPolyDataUmbrellaStencil.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkPolyDataNormals.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkMeshQuality.h"
#include "vtkCellLocator.h"
#include "vtkTriangle.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataSurfaceRemeshing);

vtkvmtkPolyDataSurfaceRemeshing::vtkvmtkPolyDataSurfaceRemeshing()
{
  this->AspectRatioThreshold = 1.2;
  this->InternalAngleTolerance = 0.0;
  this->NormalAngleTolerance = 0.2;
  this->CollapseAngleThreshold = 0.5;
  this->Relaxation = 0.5;
  this->TargetArea = 1.0;
  this->TargetAreaFactor = 1.0;
  this->TriangleSplitFactor = 1.0;
  this->MinAreaFactor = 0.5;
  this->MaxArea = VTK_VMTK_LARGE_FLOAT;
  this->MinArea = 0.0;
  this->TargetAreaArrayName = NULL;
  this->TargetAreaArray = NULL;
  this->NumberOfConnectivityOptimizationIterations = 20;
  this->NumberOfIterations = 10;
  this->ElementSizeMode = TARGET_AREA;
  this->PreserveBoundaryEdges = 0;
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdsArray = NULL;

  this->Mesh = NULL;
  this->InputBoundary = NULL;
  this->InputEntityBoundary = NULL;
  this->Locator = NULL;
  this->BoundaryLocator = NULL;
  this->EntityBoundaryLocator = NULL;

  this->ExcludedEntityIds = NULL;
}

vtkvmtkPolyDataSurfaceRemeshing::~vtkvmtkPolyDataSurfaceRemeshing()
{
  if (this->Mesh)
    {
    this->Mesh->Delete();
    this->Mesh = NULL;
    }

  if (this->InputBoundary)
    {
    this->InputBoundary->Delete();
    this->InputBoundary = NULL;
    }

  if (this->InputEntityBoundary)
    {
    this->InputEntityBoundary->Delete();
    this->InputEntityBoundary = NULL;
    }

  if (this->Locator)
    {
    this->Locator->Delete();
    this->Locator = NULL;
    }

  if (this->BoundaryLocator)
    {
    this->BoundaryLocator->Delete();
    this->BoundaryLocator = NULL;
    }

  if (this->EntityBoundaryLocator)
    {
    this->EntityBoundaryLocator->Delete();
    this->EntityBoundaryLocator = NULL;
    }

  if (this->TargetAreaArrayName)
    {
    delete[] this->TargetAreaArrayName;
    this->TargetAreaArrayName = NULL;
    }

  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }

  if (this->CellEntityIdsArray)
    {
    this->CellEntityIdsArray->Delete();
    this->CellEntityIdsArray = NULL;
    } 

  if (this->ExcludedEntityIds)
    {
    this->ExcludedEntityIds->Delete();
    this->ExcludedEntityIds = NULL;
    }
}

int vtkvmtkPolyDataSurfaceRemeshing::RequestData(
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

  if (this->ElementSizeMode == TARGET_AREA_ARRAY)
    {
    if (!this->TargetAreaArrayName)
      {
      vtkErrorMacro(<<"TargetAreaArrayName not specified");
      return 1;
      }
    this->TargetAreaArray = input->GetPointData()->GetArray(this->TargetAreaArrayName);
    if (!this->TargetAreaArray)
      {
      vtkErrorMacro(<<"TargetAreaArray with name specified does not exist");
      return 1;
      }
    }

  if (this->CellEntityIdsArray)
    {
    this->CellEntityIdsArray->Delete();
    this->CellEntityIdsArray = NULL;
    }
    
  this->CellEntityIdsArray = vtkIntArray::New();
  if (this->CellEntityIdsArrayName)
    {
    vtkDataArray* cellEntityIdsArray = input->GetCellData()->GetArray(this->CellEntityIdsArrayName);
    if (!cellEntityIdsArray)
      {
      vtkErrorMacro(<<"CellEntityIdsArray with name specified does not exist");
      return 1;
      }
    this->CellEntityIdsArray->DeepCopy(cellEntityIdsArray);
    }
  else
    {
    this->CellEntityIdsArray->SetNumberOfValues(input->GetNumberOfCells());
    this->CellEntityIdsArray->FillComponent(0,0.0);
    }

  if (this->Mesh)
    {
    this->Mesh->Delete();
    this->Mesh = NULL;
    }

  vtkPolyDataNormals* normals = vtkPolyDataNormals::New();
#if (VTK_MAJOR_VERSION <= 5)
  normals->SetInput(input);
#else
  normals->SetInputData(input);
#endif
  normals->ComputePointNormalsOff();
  normals->ComputeCellNormalsOn();
  normals->AutoOrientNormalsOn();
  normals->ConsistencyOn();
  normals->SplittingOff();
  normals->Update();

  vtkDataArray* cellNormals = normals->GetOutput()->GetCellData()->GetNormals();

  this->Mesh = vtkPolyData::New();
  vtkPoints* workingPoints = vtkPoints::New();
  vtkCellArray* workingCells = vtkCellArray::New();
  workingPoints->DeepCopy(input->GetPoints());
  
  double point1[3], point2[3], point3[3];
  double cellNormal[3], orientedCellNormal[3];
  for (int i=0; i<input->GetNumberOfCells(); i++)
    {
    if (input->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    vtkCell* cell = input->GetCell(i);
    cell->GetPoints()->GetPoint(0,point1);
    cell->GetPoints()->GetPoint(1,point2);
    cell->GetPoints()->GetPoint(2,point3);
    vtkTriangle::ComputeNormal(point1,point2,point3,cellNormal); 
    cellNormals->GetTuple(i,orientedCellNormal);
    workingCells->InsertNextCell(3);
    if (vtkMath::Dot(cellNormal,orientedCellNormal) > 0.0)
      {
      workingCells->InsertCellPoint(cell->GetPointId(0));
      workingCells->InsertCellPoint(cell->GetPointId(1));
      workingCells->InsertCellPoint(cell->GetPointId(2));
      }
    else
      {
      workingCells->InsertCellPoint(cell->GetPointId(1));
      workingCells->InsertCellPoint(cell->GetPointId(0));
      workingCells->InsertCellPoint(cell->GetPointId(2));
      }
    }

  this->Mesh->SetPoints(workingPoints);
  this->Mesh->SetPolys(workingCells);

  workingPoints->Delete();
  workingCells->Delete(); 
  normals->Delete();

  this->Mesh->BuildCells();
  this->Mesh->BuildLinks();

  if (this->Locator)
    {
    this->Locator->Delete();
    this->Locator = NULL;
    }

  this->Locator = vtkCellLocator::New();
  this->Locator->SetDataSet(input);
  this->Locator->SetNumberOfCellsPerBucket(5);
  this->Locator->CacheCellBoundsOn();
  this->Locator->BuildLocator();

  if (this->InputBoundary)
    {
    this->InputBoundary->Delete();
    this->InputBoundary = NULL;
    }

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
#if (VTK_MAJOR_VERSION <= 5)
  boundaryExtractor->SetInput(input);
#else
  boundaryExtractor->SetInputData(input);
#endif
  boundaryExtractor->Update();

  this->InputBoundary = vtkPolyData::New();
  this->InputBoundary->DeepCopy(boundaryExtractor->GetOutput());

  boundaryExtractor->Delete();

  if (this->BoundaryLocator)
    {
    this->BoundaryLocator->Delete();
    this->BoundaryLocator = NULL;
    }

  if (this->InputBoundary->GetNumberOfCells() > 0)
    {
    this->BoundaryLocator = vtkCellLocator::New();
    this->BoundaryLocator->SetDataSet(this->InputBoundary);
    this->BoundaryLocator->BuildLocator();
    }

  if (this->InputEntityBoundary)
    {
    this->InputEntityBoundary->Delete();
    this->InputEntityBoundary = NULL;
    }

  this->InputEntityBoundary = vtkPolyData::New();
  this->BuildEntityBoundary(this->Mesh,this->InputEntityBoundary);

  if (this->EntityBoundaryLocator)
    {
    this->EntityBoundaryLocator->Delete();
    this->EntityBoundaryLocator = NULL;
    }

  if (this->InputEntityBoundary->GetNumberOfCells() > 0)
    {
    this->EntityBoundaryLocator = vtkCellLocator::New();
    this->EntityBoundaryLocator->SetDataSet(this->InputEntityBoundary);
    this->EntityBoundaryLocator->BuildLocator();
    }

  int relocationSuccess = RELOCATE_SUCCESS;
  for (int n=0; n<this->NumberOfIterations; n++)
    {
    cout<<"Iteration "<<n+1<<"/"<<this->NumberOfIterations<<endl;
    this->EdgeCollapseIteration();
    this->EdgeFlipIteration();
    this->EdgeSplitIteration();
    this->EdgeFlipIteration();
    relocationSuccess = this->PointRelocationIteration();
    if (relocationSuccess == RELOCATE_FAILURE)
      {
      break;
      }
    this->EdgeFlipIteration();

    for (int i=0; i<this->NumberOfConnectivityOptimizationIterations; i++)
      {
      this->EdgeFlipConnectivityOptimizationIteration();
      }
    }

  cout<<"Final mesh improvement"<<endl;
  bool projectToSurface = false;
  for (int i=0; i<this->NumberOfIterations; i++)
    {
    if (i == this->NumberOfIterations/2)
      {
      projectToSurface = true;
      }
    relocationSuccess = this->PointRelocationIteration(projectToSurface);
    if (relocationSuccess == RELOCATE_FAILURE)
      {
      break;
      }
    }

  if (relocationSuccess == RELOCATE_FAILURE)
    {
    return 1;
    }
 
  this->TriangleSplitIteration();
 
  vtkPoints* newPoints = vtkPoints::New();
  vtkCellArray* newCells = vtkCellArray::New();
  vtkIntArray* newCellEntityIdsArray = vtkIntArray::New();
  newCellEntityIdsArray->SetName(this->CellEntityIdsArrayName);

  newPoints->DeepCopy(this->Mesh->GetPoints());

  int numberOfFinalCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfFinalCells; i++)
    {
    if (this->Mesh->GetCellType(i) == VTK_EMPTY_CELL)
      {
      continue;
      }
    if (this->GetNumberOfBoundaryEdges(i) > 1)
      {
      continue;
      }
    newCells->InsertNextCell(this->Mesh->GetCell(i));
    newCellEntityIdsArray->InsertNextValue(this->CellEntityIdsArray->GetValue(i));
    }

  output->SetPoints(newPoints);
  output->SetPolys(newCells);
  output->GetCellData()->AddArray(newCellEntityIdsArray);
 
  newPoints->Delete();
  newCells->Delete();
  newCellEntityIdsArray->Delete();

  return 1;
}

void vtkvmtkPolyDataSurfaceRemeshing::BuildEntityBoundary(vtkPolyData* input, vtkPolyData* entityBoundary)
{
  vtkPoints* entityBoundaryPoints = vtkPoints::New();
  vtkCellArray* entityBoundaryCells = vtkCellArray::New();
  vtkIdList* cellIds = vtkIdList::New();
  vtkIdType numberOfCells = input->GetNumberOfCells();
  vtkIdType npts, *pts;
  for (int i=0; i<numberOfCells; i++)
    {
    if (input->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    input->GetCellPoints(i,npts,pts);
    vtkIdType cellEntityId = this->CellEntityIdsArray->GetValue(i);
    for (int j=0; j<npts; j++)
      {
      cellIds->Initialize();
      vtkIdType pt1 = pts[j];
      vtkIdType pt2 = pts[(j+1)%npts];
      input->GetCellEdgeNeighbors(i,pt1,pt2,cellIds);
      bool entityBoundaryEdge = false;
      for (int k=0; k<cellIds->GetNumberOfIds(); k++)
        {
        if (this->CellEntityIdsArray->GetValue(cellIds->GetId(k)) != cellEntityId)
          {
          entityBoundaryEdge = true;
          break;
          }
        }
      if (entityBoundaryEdge)
        {
        vtkIdType pt1Id = entityBoundaryPoints->InsertNextPoint(input->GetPoint(pt1));
        vtkIdType pt2Id = entityBoundaryPoints->InsertNextPoint(input->GetPoint(pt2));
        entityBoundaryCells->InsertNextCell(2);
        entityBoundaryCells->InsertCellPoint(pt1Id);
        entityBoundaryCells->InsertCellPoint(pt2Id);
        }
      }
    }

  entityBoundary->SetPoints(entityBoundaryPoints);
  entityBoundary->SetLines(entityBoundaryCells);

  cellIds->Delete();
  entityBoundaryPoints->Delete();
  entityBoundaryCells->Delete();
}

int vtkvmtkPolyDataSurfaceRemeshing::GetNumberOfBoundaryEdges(vtkIdType cellId)
{
  int numberOfBoundaryEdges = 0;
  vtkIdList* cellEdgeNeighbors = vtkIdList::New();
  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);
  for (int j=0; j<npts; j++)
    {
    if (this->Mesh->IsEdge(pts[j],pts[(j+1)%npts]))
      {
      this->Mesh->GetCellEdgeNeighbors(cellId,pts[j],pts[(j+1)%npts],cellEdgeNeighbors);
      if (cellEdgeNeighbors->GetNumberOfIds() == 0)
        {
        numberOfBoundaryEdges++;
        }
      }
    }
  cellEdgeNeighbors->Delete();
  return numberOfBoundaryEdges;
}

int vtkvmtkPolyDataSurfaceRemeshing::IsPointOnBoundary(vtkIdType pointId)
{
  unsigned short ncells;
  vtkIdType* cells;
  this->Mesh->GetPointCells(pointId,ncells,cells);
  vtkIdList* cellEdgeNeighbors = vtkIdList::New();
  for (int i=0; i<ncells; i++)
    {
    vtkIdType npts, *pts;
    this->Mesh->GetCellPoints(cells[i],npts,pts);
    for (int j=0; j<npts; j++)
      {
      if (pts[j] == pointId)
        {
        continue;
        }
      if (this->Mesh->IsEdge(pts[j],pointId))
        {
        this->Mesh->GetCellEdgeNeighbors(cells[i],pts[j],pointId,cellEdgeNeighbors);
        if (cellEdgeNeighbors->GetNumberOfIds() == 0)
          {
          cellEdgeNeighbors->Delete();
          return 1;
          }
        }
      }
    }
  cellEdgeNeighbors->Delete();
  return 0;
}

int vtkvmtkPolyDataSurfaceRemeshing::EdgeFlipConnectivityOptimizationIteration()
{
  //TODO: randomize. 
  int numberOfChanges = 0;
  int numberOfCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    if (this->Mesh->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    vtkIdType npts, *pts;
    this->Mesh->GetCellPoints(i,npts,pts);
    vtkIdType tripts[3];
    tripts[0] = pts[0];
    tripts[1] = pts[1];
    tripts[2] = pts[2];
    for (int j=0; j<3; j++)
      {
      int test = this->TestConnectivityFlipEdge(tripts[j],tripts[(j+1)%3]);
      if (test == DO_NOTHING)
        {
        continue;
        }
      this->FlipEdge(tripts[j],tripts[(j+1)%3]);
      numberOfChanges++;
      break;
      }
    }
  return numberOfChanges;
}

int vtkvmtkPolyDataSurfaceRemeshing::EdgeFlipIteration()
{
  int numberOfChanges = 0;
  int numberOfCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    if (this->Mesh->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    vtkIdType npts, *pts;
    this->Mesh->GetCellPoints(i,npts,pts);
    vtkIdType tripts[3];
    tripts[0] = pts[0];
    tripts[1] = pts[1];
    tripts[2] = pts[2];
    for (int j=0; j<3; j++)
      {
      int test = this->TestDelaunayFlipEdge(tripts[j],tripts[(j+1)%3]);
      if (test == DO_NOTHING)
        {
        continue;
        }
      this->FlipEdge(tripts[j],tripts[(j+1)%3]);
      numberOfChanges++;
      break;
      }
    }
  return numberOfChanges;
}

int vtkvmtkPolyDataSurfaceRemeshing::EdgeCollapseIteration()
{
  int numberOfChanges = 0;
  int numberOfCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    if (this->Mesh->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    vtkIdType pt1, pt2;
    int test = this->TestAspectRatioCollapseEdge(i,pt1,pt2);
    if (test == DO_NOTHING)
      {
      continue;
      }
    this->CollapseEdge(pt1,pt2);
    numberOfChanges++;
    }
  return numberOfChanges;
}

int vtkvmtkPolyDataSurfaceRemeshing::EdgeSplitIteration()
{
  int numberOfChanges = 0;
  int numberOfCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    if (this->Mesh->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    vtkIdType pt1, pt2;
    int test = this->TestAreaSplitEdge(i,pt1,pt2);
    if (test == DO_NOTHING)
      {
      continue;
      }
    this->SplitEdge(pt1,pt2);
    numberOfChanges++;
    }
  return numberOfChanges;
}

int vtkvmtkPolyDataSurfaceRemeshing::TriangleSplitIteration()
{
  int numberOfChanges = 0;
  int numberOfCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfCells; i++)
    {
    if (this->Mesh->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    int test = this->TestTriangleSplit(i);
    if (test == DO_NOTHING)
      {
      continue;
      }
    this->SplitTriangle(i);
    numberOfChanges++;
    }
  return numberOfChanges;
}

int vtkvmtkPolyDataSurfaceRemeshing::PointRelocationIteration(bool projectToSurface)
{
  int numberOfPoints = this->Mesh->GetNumberOfPoints();
  int success = RELOCATE_SUCCESS;
  for (int i=0; i<numberOfPoints; i++)
    {
    success = this->RelocatePoint(i,projectToSurface);
    if (success == RELOCATE_FAILURE)
      {
      return RELOCATE_FAILURE;
      }
    } 
  return RELOCATE_SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::IsElementExcluded(vtkIdType cellId)
{
  int isElementExcluded = 0;
  if (this->ExcludedEntityIds == NULL)
    {
    return isElementExcluded;
    }

  vtkIdType cellEntityId = this->CellEntityIdsArray->GetValue(cellId);
  if (this->ExcludedEntityIds->IsId(cellEntityId) != -1)
    {
    isElementExcluded = 1;
    }
  return isElementExcluded;
}

int vtkvmtkPolyDataSurfaceRemeshing::IsPointOnEntityBoundary(vtkIdType pointId)
{
  vtkIdList* ptCells = vtkIdList::New();
  this->Mesh->GetPointCells(pointId,ptCells);
  vtkIdType nptCells = ptCells->GetNumberOfIds();
  vtkIdType neighborhoodCellEntityId = -1;
  bool uniformCellEntityIds = true;
  for (int i=0; i<nptCells; i++)
    {
    vtkIdType cellEntityId = this->CellEntityIdsArray->GetValue(ptCells->GetId(i)); 
    if (cellEntityId == -1)
      {
      continue;
      }
    if (neighborhoodCellEntityId == -1)
      {
      neighborhoodCellEntityId = cellEntityId;
      continue;
      }
    if (neighborhoodCellEntityId == cellEntityId)
      {
      continue;
      }
    uniformCellEntityIds = false;
    break;
    }

  if (!uniformCellEntityIds)
    {
    return 1;
    }

  return 0;
}

int vtkvmtkPolyDataSurfaceRemeshing::FindOneRingNeighbors(vtkIdType pointId, vtkIdList* neighborIds)
{
  int pointLocation = INTERNAL_POINT;

  vtkIdList* cellIds = vtkIdList::New();
  this->Mesh->GetPointCells(pointId,cellIds);

  vtkIdType numberOfCells = cellIds->GetNumberOfIds();
  if (numberOfCells < 1)
    {
    cellIds->Delete();
    return NO_NEIGHBORS;
    }

  vtkIdList* ptIds = vtkIdList::New();
  this->Mesh->GetCellPoints(cellIds->GetId(0),ptIds);

  vtkIdType bp1, bp2;
  vtkIdType i, j;

  vtkIdType p1, p2;
  p2 = ptIds->GetId(0);
  for (i=1; i<3; i++)
    {
    p2 = ptIds->GetId(i);
    if (pointId != p2)
      {
      break;
      }
    }
  neighborIds->InsertNextId(p2);

  this->Mesh->GetCellEdgeNeighbors(-1,pointId,p2,cellIds);

  vtkIdType startCell;
  vtkIdType nextCell = cellIds->GetId(0);
  if (cellIds->GetNumberOfIds() == 1)
    {
    startCell = -1;
    }
  else
    {
    startCell = cellIds->GetId(1);
    }

  bp2 = -1;
  bp1 = p2;

  // walk around the neighborhood counter-clockwise and get cells
  vtkCell* cell = NULL;
  for (j=0; j<numberOfCells; j++)
    {
    cell = this->Mesh->GetCell(nextCell);
    p1 = -1;
    for (i=0; i<3; i++)
      {
      p1 = cell->GetPointId(i);
      if (p1 != pointId && p1 != p2)
        {
        break;
        }
      }

    p2 = p1;
    neighborIds->InsertNextId(p2);
    this->Mesh->GetCellEdgeNeighbors(nextCell,pointId,p2,cellIds);
    if (cellIds->GetNumberOfIds() != 1)
      {
      bp2 = p2;
      j++;
      break;
      }
    nextCell = cellIds->GetId(0);
    }

  // now walk around the other way. this will only happen if there
  // is a boundary cell left that we have not visited
  nextCell = startCell;
  p2 = bp1;
  for (; j<numberOfCells && startCell!=-1; j++)
    {
    cell = this->Mesh->GetCell(nextCell);
    p1 = -1;
    for (i=0; i<3; i++)
      {
      p1 = cell->GetPointId(i);
      if (p1 != pointId && p1 != p2)
        {
        break;
        }
      }
    p2 = p1;
    neighborIds->InsertNextId(p2);
    this->Mesh->GetCellEdgeNeighbors(nextCell,pointId,p2,cellIds);
    if (cellIds->GetNumberOfIds() != 1)
      {
      bp1 = p2;
      break;
      }
    nextCell = cellIds->GetId(0);
    }

  if (bp2 != -1) // boundary edge
    {
    pointLocation = POINT_ON_BOUNDARY;
    vtkIdType id = neighborIds->IsId(bp2);
    for (i=0; i<=id/2; i++)
      {
      p1 = neighborIds->GetId(i);
      p2 = neighborIds->GetId(id-i);
      neighborIds->SetId(i,p2);
      neighborIds->SetId(id-i,p1);
      }
    }
  else
    {
    // Remove last id. It's a duplicate of the first
    neighborIds->SetNumberOfIds(neighborIds->GetNumberOfIds()-1);
    }

  cellIds->Delete();
  ptIds->Delete();

  return pointLocation;
}
#define USE_STENCIL
int vtkvmtkPolyDataSurfaceRemeshing::RelocatePoint(vtkIdType pointId, bool projectToSurface)
{
  vtkIdList* cellIds = vtkIdList::New();
  this->Mesh->GetPointCells(pointId,cellIds);
  int pointOnExcludedEntity = 0;
  for (int i=0; i<cellIds->GetNumberOfIds(); i++)
    {
    if (this->IsElementExcluded(cellIds->GetId(i)))
      {
      pointOnExcludedEntity = 1;
      }
    }
  cellIds->Delete();

  if (pointOnExcludedEntity)
    {
    return RELOCATE_SUCCESS;
    }

#ifndef USE_STENCIL
  vtkIdList* neighborIds = vtkIdList::New();
  int pointLocation = this->FindOneRingNeighbors(pointId,neighborIds);
#else
  vtkvmtkPolyDataUmbrellaStencil* stencil = vtkvmtkPolyDataUmbrellaStencil::New();
  stencil->SetDataSet(this->Mesh);
  stencil->SetDataSetPointId(pointId);
  stencil->NegateWeightsOff();
  stencil->Build();
#endif

#ifndef USE_STENCIL
  if (!pointLocation == INTERNAL_POINT)
#else
  if (!stencil->GetIsBoundary())
#endif
    { 
    double targetPoint[3];
    targetPoint[0] = targetPoint[1] = targetPoint[2] = 0.0;
#ifndef USE_STENCIL 
    int numberOfNeighbors = neighborIds->GetNumberOfIds();
    double stencilWeight = 1.0 / numberOfNeighbors;
    for (int i=0; i<numberOfNeighbors; i++)
      {
      double stencilPoint[3];
      this->Mesh->GetPoint(neighborIds->GetId(i),stencilPoint);
      targetPoint[0] += stencilWeight * stencilPoint[0];
      targetPoint[1] += stencilWeight * stencilPoint[1];
      targetPoint[2] += stencilWeight * stencilPoint[2];
      }
#else
    for (int i=0; i<stencil->GetNumberOfPoints(); i++)
      {
      double stencilPoint[3];
      this->Mesh->GetPoint((stencil->GetPointId(i)),stencilPoint);
      double stencilWeight = stencil->GetWeight(i);
      targetPoint[0] += stencilWeight * stencilPoint[0];
      targetPoint[1] += stencilWeight * stencilPoint[1];
      targetPoint[2] += stencilWeight * stencilPoint[2];
      }
#endif
    double point[3];
    this->Mesh->GetPoint(pointId,point);
  
    double relocatedPoint[3];
    relocatedPoint[0] = point[0] + this->Relaxation * (targetPoint[0] - point[0]);
    relocatedPoint[1] = point[1] + this->Relaxation * (targetPoint[1] - point[1]);
    relocatedPoint[2] = point[2] + this->Relaxation * (targetPoint[2] - point[2]);
 
    double projectedRelocatedPoint[3];
    vtkIdType cellId;
    int subId;
    double dist2;
    if (this->IsPointOnEntityBoundary(pointId))
      {
      this->EntityBoundaryLocator->FindClosestPoint(relocatedPoint,projectedRelocatedPoint,cellId,subId,dist2);
      }
    else
      {
      if (projectToSurface)
        {
        this->Locator->FindClosestPoint(relocatedPoint,projectedRelocatedPoint,cellId,subId,dist2);
        }
      else
        {
        projectedRelocatedPoint[0] = relocatedPoint[0];
        projectedRelocatedPoint[1] = relocatedPoint[1];
        projectedRelocatedPoint[2] = relocatedPoint[2];
        }
      }
    this->Mesh->GetPoints()->SetPoint(pointId,projectedRelocatedPoint);
    }
  else
    {
    double targetPoint[3];
    targetPoint[0] = targetPoint[1] = targetPoint[2] = 0.0;

    double stencilPoint[3];
#ifndef USE_STENCIL
    this->Mesh->GetPoint(neighborIds->GetId(0),stencilPoint);
#else
    this->Mesh->GetPoint((stencil->GetPointId(0)),stencilPoint);
#endif
    double stencilWeight = 0.5;
    targetPoint[0] += stencilWeight * stencilPoint[0];
    targetPoint[1] += stencilWeight * stencilPoint[1];
    targetPoint[2] += stencilWeight * stencilPoint[2];
#ifndef USE_STENCIL
    this->Mesh->GetPoint(neighborIds->GetId(neighborIds->GetNumberOfIds()-1),stencilPoint);
#else
    this->Mesh->GetPoint((stencil->GetPointId(stencil->GetNumberOfPoints()-1)),stencilPoint);
#endif
    targetPoint[0] += stencilWeight * stencilPoint[0];
    targetPoint[1] += stencilWeight * stencilPoint[1];
    targetPoint[2] += stencilWeight * stencilPoint[2];

    double point[3];
    this->Mesh->GetPoint(pointId,point);
  
    double relocatedPoint[3];
    relocatedPoint[0] = point[0] + this->Relaxation * (targetPoint[0] - point[0]);
    relocatedPoint[1] = point[1] + this->Relaxation * (targetPoint[1] - point[1]);
    relocatedPoint[2] = point[2] + this->Relaxation * (targetPoint[2] - point[2]);
  
    double projectedRelocatedPoint[3];
    vtkIdType cellId;
    int subId;
    double dist2;
    if (this->BoundaryLocator)
      {
      this->BoundaryLocator->FindClosestPoint(relocatedPoint,projectedRelocatedPoint,cellId,subId,dist2);
      }
    else
      {
//      vtkErrorMacro(<<"Something's wrong: point on boundary but no BoundaryLocator is allocated.");
      projectedRelocatedPoint[0] = relocatedPoint[0];
      projectedRelocatedPoint[1] = relocatedPoint[1];
      projectedRelocatedPoint[2] = relocatedPoint[2];
//      return RELOCATE_FAILURE;
      }
  
    this->Mesh->GetPoints()->SetPoint(pointId,projectedRelocatedPoint);
    }
#ifndef USE_STENCIL
  neighborIds->Delete();
#else
  stencil->Delete();
#endif
  return RELOCATE_SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(vtkIdType pt1, vtkIdType pt2, vtkIdType& cell1, vtkIdType& cell2, vtkIdType& pt3, vtkIdType& pt4)
{
  cell1 = cell2 = -1;
  pt3 = pt4 = -1;

  if (!this->Mesh->IsEdge(pt1,pt2))
    {
    return NOT_EDGE;
    }

  vtkIdList* cellIds = vtkIdList::New();
  this->Mesh->GetCellEdgeNeighbors(-1,pt1,pt2,cellIds);

  int numberOfCellEdgeNeighbors = cellIds->GetNumberOfIds();
  int numberOfNeighborTriangles = 0;

  for (int i=0; i<numberOfCellEdgeNeighbors; i++)
    {
    vtkIdType cellId = cellIds->GetId(i);
    if (this->Mesh->GetCellType(cellId) == VTK_TRIANGLE)
      {
      if (numberOfNeighborTriangles==0)
        {
        cell1 = cellId;
        }
      else if (numberOfNeighborTriangles==1)
        {
        cell2 = cellId;
        }
      numberOfNeighborTriangles++;
      }
    }
 
  cellIds->Delete();

  if (numberOfNeighborTriangles == 0)
    {
    return NOT_TRIANGLES;
    }
  else if (numberOfNeighborTriangles == 1)
    {
    vtkIdType npts, *pts;
    this->Mesh->GetCellPoints(cell1,npts,pts);
    for (int i=0; i<3; i++)
      {
      if ((pts[i]==pt1 && pts[(i+1)%3]==pt2) || (pts[i]==pt2 && pts[(i+1)%3]==pt1))
        {
        pt3 = pts[(i+2)%3];
        break;
        }
      }
    return EDGE_ON_BOUNDARY;
    }
  else if (numberOfNeighborTriangles > 2)
    {
    return NON_MANIFOLD;
    }

  vtkIdType pt3tmp = -1;
  vtkIdType pt4tmp = -1;
  bool reverse = false;
  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cell1,npts,pts);
  for (int i=0; i<3; i++)
    {
    if ((pts[i]==pt1 && pts[(i+1)%3]==pt2) || (pts[i]==pt2 && pts[(i+1)%3]==pt1))
      {
      pt3tmp = pts[(i+2)%3];
      if (pts[i]==pt2 && pts[(i+1)%3]==pt1)
        {
        reverse = true;
        }
      break;
      }
    }

  this->Mesh->GetCellPoints(cell2,npts,pts);
  for (int i=0; i<3; i++)
    {
    if ((pts[i]==pt1 && pts[(i+1)%3]==pt2) || (pts[i]==pt2 && pts[(i+1)%3]==pt1))
      {
      pt4tmp = pts[(i+2)%3];
      break;
      }
    }

  if (pt3tmp==-1 || pt4tmp==-1)
    {
    return DEGENERATE_TRIANGLES;
    }

  pt3 = pt3tmp;
  pt4 = pt4tmp;
    
  if (reverse)
    {
    vtkIdType tmp;
    tmp = pt3;
    pt3 = pt4;
    pt4 = tmp;
    tmp = cell1;
    cell1 = cell2;
    cell2 = tmp;
    }

  if (this->CellEntityIdsArray->GetValue(cell1) != this->CellEntityIdsArray->GetValue(cell2))
    {
    return EDGE_BETWEEN_ENTITIES;
    }

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::SplitTriangle(vtkIdType cellId)
{
  if (this->Mesh->GetCellType(cellId) != VTK_TRIANGLE)
    {
    return NOT_TRIANGLES;
    }

  if (this->IsElementExcluded(cellId))
    {
    return TRIANGLE_LOCKED;
    }

  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);
  vtkIdType pt1 = pts[0];
  vtkIdType pt2 = pts[1];
  vtkIdType pt3 = pts[2];

  vtkIdType cellEntityId = this->CellEntityIdsArray->GetValue(cellId);

  double point1[3], point2[3], point3[3], newPoint[3];
  this->Mesh->GetPoint(pt1,point1);
  this->Mesh->GetPoint(pt2,point2);
  this->Mesh->GetPoint(pt3,point3);
  vtkTriangle::TriangleCenter(point1,point2,point3,newPoint);

  double projectedNewPoint[3];
  vtkIdType closestCellId;
  int subId;
  double dist2;
  this->Locator->FindClosestPoint(newPoint,projectedNewPoint,closestCellId,subId,dist2);
 
  vtkIdType newpt = this->Mesh->InsertNextLinkedPoint(projectedNewPoint,1);

  this->Mesh->ReplaceCellPoint(cellId,pt3,newpt);
  this->Mesh->RemoveReferenceToCell(pt3,cellId);
  this->Mesh->AddReferenceToCell(newpt,cellId);

  vtkIdType tri[3];
  tri[0] = pt2;
  tri[1] = pt3;
  tri[2] = newpt;
  vtkIdType cell1 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
  this->CellEntityIdsArray->InsertValue(cell1,cellEntityId);

  tri[0] = pt3;
  tri[1] = pt1;
  tri[2] = newpt;
  vtkIdType cell2 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
  this->CellEntityIdsArray->InsertValue(cell2,cellEntityId);

  //cout<<cell1<<" "<<cell2<<" "<<numberOfCells<<endl;

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::CollapseTriangle(vtkIdType cellId)
{
  vtkErrorMacro(<<"CollapseTriangle not yet implemented.");
  return -1;
}

int vtkvmtkPolyDataSurfaceRemeshing::SplitEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (this->IsElementExcluded(cell1) || this->IsElementExcluded(cell2))
    {
    return EDGE_LOCKED;
    }

  bool proceed = false;

  if (success == SUCCESS)
    {
    proceed = true;
    }

  if (!this->PreserveBoundaryEdges && success == EDGE_ON_BOUNDARY)
    {
    proceed = true;
    }

  if (success == EDGE_BETWEEN_ENTITIES)
    {
    proceed = true;
    }

  if (!proceed)
    {
    return success;
    }

  double point1[3], point2[3];
  this->Mesh->GetPoint(pt1,point1);
  this->Mesh->GetPoint(pt2,point2);

  double newPoint[3];
  newPoint[0] = 0.5 * (point1[0] + point2[0]);
  newPoint[1] = 0.5 * (point1[1] + point2[1]);
  newPoint[2] = 0.5 * (point1[2] + point2[2]);

  if (success != EDGE_ON_BOUNDARY)
    {
    double projectedNewPoint[3];
    vtkIdType cellId;
    int subId;
    double dist2;

    if (success == EDGE_BETWEEN_ENTITIES)
      {
      this->EntityBoundaryLocator->FindClosestPoint(newPoint,projectedNewPoint,cellId,subId,dist2);
      }
    else
      {
      this->Locator->FindClosestPoint(newPoint,projectedNewPoint,cellId,subId,dist2);
      }
    vtkIdType newpt = this->Mesh->InsertNextLinkedPoint(projectedNewPoint,2);
  
    this->Mesh->ReplaceCellPoint(cell1,pt2,newpt);
    this->Mesh->ReplaceCellPoint(cell2,pt2,newpt);
  
    this->Mesh->RemoveReferenceToCell(pt2,cell1);
    this->Mesh->RemoveReferenceToCell(pt2,cell2);
  
    this->Mesh->AddReferenceToCell(newpt,cell1);
    this->Mesh->AddReferenceToCell(newpt,cell2);
  
    vtkIdType tri[3];
    tri[0] = pt3;
    tri[1] = newpt;
    tri[2] = pt2;
    vtkIdType cell3 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    this->CellEntityIdsArray->InsertValue(cell3,this->CellEntityIdsArray->GetValue(cell1));

    tri[0] = pt2;
    tri[1] = newpt;
    tri[2] = pt4;
    vtkIdType cell4 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    this->CellEntityIdsArray->InsertValue(cell4,this->CellEntityIdsArray->GetValue(cell2));
    }
  else
    {
    double projectedNewPoint[3];
    vtkIdType cellId;
    int subId;
    double dist2;

    if (this->BoundaryLocator)
      {
      this->BoundaryLocator->FindClosestPoint(newPoint,projectedNewPoint,cellId,subId,dist2);
      }
    else
      {
      vtkErrorMacro(<<"Something's wrong: point on boundary but no BoundaryLocator is allocated.");
      projectedNewPoint[0] = newPoint[0];
      projectedNewPoint[1] = newPoint[1];
      projectedNewPoint[2] = newPoint[2];
      }
    vtkIdType newpt = this->Mesh->InsertNextLinkedPoint(projectedNewPoint,2);
  
    this->Mesh->ReplaceCellPoint(cell1,pt2,newpt);
    this->Mesh->RemoveReferenceToCell(pt2,cell1);
    this->Mesh->AddReferenceToCell(newpt,cell1);
  
    vtkIdType tri[3];
    tri[0] = pt3;
    tri[1] = newpt;
    tri[2] = pt2;
    vtkIdType cell3 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    this->CellEntityIdsArray->InsertValue(cell3,this->CellEntityIdsArray->GetValue(cell1));
    }
 
  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::CollapseEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (this->IsElementExcluded(cell1) || this->IsElementExcluded(cell2))
    {
    return EDGE_LOCKED;
    }

  bool proceed = false;

  if (success == SUCCESS)
    {
    proceed = true;
    }

  if (!this->PreserveBoundaryEdges && success == EDGE_ON_BOUNDARY)
    {
    proceed = true;
    }

  if (success == EDGE_BETWEEN_ENTITIES)
    {
    proceed = true;
    }

  if (!proceed)
    {
    return success;
    }

  if (success != EDGE_ON_BOUNDARY)
    {
    bool swappedForBoundary = false;
    int pt1OnBoundary = this->IsPointOnBoundary(pt1);
    int pt2OnBoundary = this->IsPointOnBoundary(pt2);
    if (!pt1OnBoundary && pt2OnBoundary)
      {
      vtkIdType tmp = pt1;
      pt1 = pt2;
      pt2 = tmp;
      swappedForBoundary = true;
      }
 
    int pt1OnEntityBoundary = this->IsPointOnEntityBoundary(pt1);
    int pt2OnEntityBoundary = this->IsPointOnEntityBoundary(pt2);
    if (!pt1OnEntityBoundary && pt2OnEntityBoundary)
      {
      if (swappedForBoundary)
        {
        return EDGE_LOCKED;
        }
      vtkIdType tmp = pt1;
      pt1 = pt2;
      pt2 = tmp;
      }
 
    vtkIdList* pt2Cells = vtkIdList::New();
    this->Mesh->GetPointCells(pt2,pt2Cells);
    vtkIdType npt2Cells = pt2Cells->GetNumberOfIds();
    
    this->Mesh->RemoveReferenceToCell(pt1,cell1);
    this->Mesh->RemoveReferenceToCell(pt1,cell2);
    this->Mesh->RemoveReferenceToCell(pt3,cell1);
    this->Mesh->RemoveReferenceToCell(pt4,cell2);
    this->Mesh->DeletePoint(pt2);
    this->Mesh->DeleteCell(cell1); 
    this->CellEntityIdsArray->InsertValue(cell1,-1);
    this->Mesh->DeleteCell(cell2); 
    this->CellEntityIdsArray->InsertValue(cell2,-1);
  
    this->Mesh->ResizeCellList(pt1,npt2Cells-2);
  
    for (int i=0; i<npt2Cells; i++)
      {
      vtkIdType pt2Cell = pt2Cells->GetId(i);
      if (pt2Cell == cell1 || pt2Cell == cell2)
        {
        continue;
        }
      this->Mesh->AddReferenceToCell(pt1,pt2Cell);
      this->Mesh->ReplaceCellPoint(pt2Cell,pt2,pt1);
      }
  
    pt2Cells->Delete();
    }
  else
    {
    vtkIdList* pt2Cells = vtkIdList::New();
    this->Mesh->GetPointCells(pt2,pt2Cells);
    vtkIdType npt2Cells = pt2Cells->GetNumberOfIds();
    
    this->Mesh->RemoveReferenceToCell(pt1,cell1);
    this->Mesh->RemoveReferenceToCell(pt3,cell1);
    this->Mesh->DeletePoint(pt2);
    this->Mesh->DeleteCell(cell1); 
    this->CellEntityIdsArray->InsertValue(cell1,-1);
  
    this->Mesh->ResizeCellList(pt1,npt2Cells-1);
  
    for (int i=0; i<npt2Cells; i++)
      {
      vtkIdType pt2Cell = pt2Cells->GetId(i);
      if (pt2Cell == cell1)
        {
        continue;
        }
      this->Mesh->AddReferenceToCell(pt1,pt2Cell);
      this->Mesh->ReplaceCellPoint(pt2Cell,pt2,pt1);
      }
  
    pt2Cells->Delete();
    }

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::FlipEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (this->IsElementExcluded(cell1) || this->IsElementExcluded(cell2))
    {
    return EDGE_LOCKED;
    }

  if (success != SUCCESS)
    {
    return success;
    }

  this->Mesh->RemoveCellReference(cell1);
  this->Mesh->RemoveCellReference(cell2);
  this->Mesh->DeleteCell(cell1); 
  vtkIdType cell1EntityId = this->CellEntityIdsArray->GetValue(cell1);
  this->CellEntityIdsArray->InsertValue(cell1,-1);
  this->Mesh->DeleteCell(cell2); 
  vtkIdType cell2EntityId = this->CellEntityIdsArray->GetValue(cell2);
  this->CellEntityIdsArray->InsertValue(cell2,-1);

  vtkIdType tri[3];
  tri[0] = pt3;
  tri[1] = pt1;
  tri[2] = pt4;
  vtkIdType cell1b = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
  this->CellEntityIdsArray->InsertValue(cell1b,cell1EntityId);
  tri[0] = pt2;
  tri[1] = pt3;
  tri[2] = pt4;
  vtkIdType cell2b = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
  this->CellEntityIdsArray->InsertValue(cell2b,cell2EntityId);

  vtkIdType pt3b, pt4b;
  if (vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt3,pt4,cell1,cell2,pt3b,pt4b) != SUCCESS)
    {
    this->Mesh->RemoveCellReference(cell1b);
    this->Mesh->RemoveCellReference(cell2b);
    this->Mesh->DeleteCell(cell1b);
    this->CellEntityIdsArray->InsertValue(cell1b,-1);
    this->Mesh->DeleteCell(cell2b);
    this->CellEntityIdsArray->InsertValue(cell2b,-1);
    tri[0] = pt1;
    tri[1] = pt2;
    tri[2] = pt3;
    vtkIdType cell1c = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    this->CellEntityIdsArray->InsertValue(cell1c,cell1EntityId);
    tri[0] = pt1;
    tri[1] = pt4;
    tri[2] = pt2;
    vtkIdType cell2c = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    this->CellEntityIdsArray->InsertValue(cell2c,cell2EntityId);
    }

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestFlipEdgeValidity(vtkIdType pt1, vtkIdType pt2, vtkIdType cell1, vtkIdType cell2, vtkIdType pt3, vtkIdType pt4)
{
  vtkIdType tri[4][3];
  tri[0][0] = pt1;
  tri[0][1] = pt2;
  tri[0][2] = pt3;

  tri[1][0] = pt1;
  tri[1][1] = pt4;
  tri[1][2] = pt2;

  tri[2][0] = pt3;
  tri[2][1] = pt1;
  tri[2][2] = pt4;

  tri[3][0] = pt2;
  tri[3][1] = pt3;
  tri[3][2] = pt4;

  double point1[3], point2[3], point3[3];

  double normal1[3], normal2[3], normal3[3], normal4[3];
  this->Mesh->GetPoint(tri[0][0],point1);
  this->Mesh->GetPoint(tri[0][1],point2);
  this->Mesh->GetPoint(tri[0][2],point3);
  vtkTriangle::ComputeNormal(point1,point2,point3,normal1); 
  this->Mesh->GetPoint(tri[1][0],point1);
  this->Mesh->GetPoint(tri[1][1],point2);
  this->Mesh->GetPoint(tri[1][2],point3);
  vtkTriangle::ComputeNormal(point1,point2,point3,normal2); 
  this->Mesh->GetPoint(tri[2][0],point1);
  this->Mesh->GetPoint(tri[2][1],point2);
  this->Mesh->GetPoint(tri[2][2],point3);
  vtkTriangle::ComputeNormal(point1,point2,point3,normal3); 
  this->Mesh->GetPoint(tri[3][0],point1);
  this->Mesh->GetPoint(tri[3][1],point2);
  this->Mesh->GetPoint(tri[3][2],point3);
  vtkTriangle::ComputeNormal(point1,point2,point3,normal4); 

  if (vtkMath::Dot(normal3,normal1)<0.0 || vtkMath::Dot(normal4,normal1)<0.0 || vtkMath::Dot(normal3,normal2)<0.0 || vtkMath::Dot(normal4,normal2)<0.0)
    {
    return DO_NOTHING;
    }

  if (fabs(acos(vtkMath::Dot(normal3,normal4)) - acos(vtkMath::Dot(normal1,normal2))) > this->NormalAngleTolerance)
    {
    return DO_NOTHING;
    } 

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestConnectivityFlipEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return DO_NOTHING;
    }

  if (this->TestFlipEdgeValidity(pt1,pt2,cell1,cell2,pt3,pt4) == DO_NOTHING)
    {
    return DO_NOTHING;
    }

  unsigned short ncells1, ncells2, ncells3, ncells4;
  vtkIdType* cells;
  this->Mesh->GetPointCells(pt1,ncells1,cells);
  this->Mesh->GetPointCells(pt2,ncells2,cells);
  this->Mesh->GetPointCells(pt3,ncells3,cells);
  this->Mesh->GetPointCells(pt4,ncells4,cells);

  int targetValence = 6;

  //TODO: if ncells1 < targetValence and ncells2 < targetValence, then the edge should be collapsed.
  //TODO: if ncells1 > targetValence and ncells2 > targetValence, then the edge should be split.
  int currentCost = (ncells1-targetValence)*(ncells1-targetValence) + (ncells2-targetValence)*(ncells2-targetValence) + (ncells3-targetValence)*(ncells3-targetValence) + (ncells4-targetValence)*(ncells4-targetValence);
  int flippedCost = (ncells1-1-targetValence)*(ncells1-1-targetValence) + (ncells2-1-targetValence)*(ncells2-1-targetValence) + (ncells3+1-targetValence)*(ncells3+1-targetValence) + (ncells4+1-targetValence)*(ncells4+1-targetValence);

  if (flippedCost >= currentCost)
    {
    return DO_NOTHING;
    }

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestDelaunayFlipEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return DO_NOTHING;
    }

  if (this->TestFlipEdgeValidity(pt1,pt2,cell1,cell2,pt3,pt4) == DO_NOTHING)
    {
    return DO_NOTHING;
    }

  vtkIdType tri[4][3];
  tri[0][0] = pt1;
  tri[0][1] = pt2;
  tri[0][2] = pt3;

  tri[1][0] = pt1;
  tri[1][1] = pt4;
  tri[1][2] = pt2;

  tri[2][0] = pt3;
  tri[2][1] = pt1;
  tri[2][2] = pt4;

  tri[3][0] = pt2;
  tri[3][1] = pt3;
  tri[3][2] = pt4;

  double point1[3], point2[3], point3[3];
  double direction1[3], direction2[3];

  double maxAngle01 = 0.0;
  double maxAngle23 = 0.0;

  for (int n=0; n<4; n++)
    {
    for (int i=0; i<3; i++)
      {
      this->Mesh->GetPoint(tri[n][i],point1);
      this->Mesh->GetPoint(tri[n][(i+1)%3],point2);
      this->Mesh->GetPoint(tri[n][(i+2)%3],point3);
      direction1[0] = point2[0] - point1[0];
      direction1[1] = point2[1] - point1[1];
      direction1[2] = point2[2] - point1[2];
      vtkMath::Normalize(direction1);
      direction2[0] = point3[0] - point1[0];
      direction2[1] = point3[1] - point1[1];
      direction2[2] = point3[2] - point1[2];
      vtkMath::Normalize(direction2);
      double angle = acos(vtkMath::Dot(direction1,direction2));
      if ((n==0 || n==1) && (angle > maxAngle01))
        {
        maxAngle01 = angle;
        }
      else if ((n==2 || n==3) && (angle > maxAngle23))
        {
        maxAngle23 = angle;
        }
      }
    } 

  if (maxAngle01 < maxAngle23 + this->InternalAngleTolerance)
    {
    return DO_NOTHING;
    }

  return DO_CHANGE;
}

double vtkvmtkPolyDataSurfaceRemeshing::ComputeTriangleTargetArea(vtkIdType cellId)
{
  double targetArea = 0.0;
  if (this->ElementSizeMode == TARGET_AREA)
    {
    targetArea = this->TargetArea;
    }
  else if (this->ElementSizeMode == TARGET_AREA_ARRAY)
    {
    vtkIdType npts, *pts;
    this->Mesh->GetCellPoints(cellId,npts,pts);
    double point1[3], point2[3], point3[3];
    this->Mesh->GetPoint(pts[0],point1);
    this->Mesh->GetPoint(pts[1],point2);
    this->Mesh->GetPoint(pts[2],point3);
    double center[3];
    vtkTriangle::TriangleCenter(point1,point2,point3,center);
    double projectedCenter[3];
    vtkIdType centerCellId;
    int subId;
    double dist2;
    this->Locator->FindClosestPoint(center,projectedCenter,centerCellId,subId,dist2);
    vtkTriangle* centerCell = vtkTriangle::SafeDownCast(vtkPolyData::SafeDownCast(this->Locator->GetDataSet())->GetCell(centerCellId));
    double pcoords[3], weights[3];
    centerCell->EvaluatePosition(projectedCenter,NULL,subId,pcoords,dist2,weights);
    for (int i=0; i<3; i++)
      {
      targetArea += weights[i] * this->TargetAreaArray->GetTuple1(centerCell->GetPointId(i));
      }
    targetArea *= this->TargetAreaFactor;
    }
  else
    {
    vtkErrorMacro(<<"ElementSizeMode specified is unknown");
    return 0.0;
    }
  if (targetArea > this->MaxArea)
    {
    targetArea = this->MaxArea;
    }
  if (targetArea < this->MinArea)
    {
    targetArea = this->MinArea;
    }
  return targetArea;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestTriangleSplit(vtkIdType cellId)
{
  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);

  vtkIdType tri[3];
  tri[0] = pts[0];
  tri[1] = pts[1];
  tri[2] = pts[2];

  double point1[3], point2[3], point3[3];
  this->Mesh->GetPoint(tri[0],point1);
  this->Mesh->GetPoint(tri[1],point2);
  this->Mesh->GetPoint(tri[2],point3);

  double area = vtkTriangle::TriangleArea(point1,point2,point3);

  double targetArea = this->ComputeTriangleTargetArea(cellId);

  // TODO: make this a parameter
  if (area < this->TriangleSplitFactor * targetArea)
    {
    return DO_NOTHING;
    }

  //cout<<cellId<<" "<<area<<" "<<targetArea<<endl;
  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestAspectRatioCollapseEdge(vtkIdType cellId, vtkIdType& pt1, vtkIdType& pt2)
{
  pt1 = -1;
  pt2 = -1;
  
  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);

  vtkIdType tri[3];
  tri[0] = pts[0];
  tri[1] = pts[1];
  tri[2] = pts[2];

  double point1[3], point2[3], point3[3];
  this->Mesh->GetPoint(tri[0],point1);
  this->Mesh->GetPoint(tri[1],point2);
  this->Mesh->GetPoint(tri[2],point3);

  double area = vtkTriangle::TriangleArea(point1,point2,point3);

  double targetArea = this->ComputeTriangleTargetArea(cellId);

  if (area > targetArea)
    {
    return DO_NOTHING;
    }

  double side1Squared = vtkMath::Distance2BetweenPoints(point1,point2);
  double side2Squared = vtkMath::Distance2BetweenPoints(point2,point3);
  double side3Squared = vtkMath::Distance2BetweenPoints(point3,point1);
  
  double frobeniusAspectRatio = (side1Squared + side2Squared + side3Squared) / (4.0 * sqrt(3.0) * area);

  if (frobeniusAspectRatio < this->AspectRatioThreshold && area > targetArea * this->MinAreaFactor)
    {
    return DO_NOTHING;
    }

  if (side1Squared < side2Squared && side1Squared < side3Squared)
    {
    pt1 = tri[0];
    pt2 = tri[1];
    }
  else if (side2Squared < side1Squared && side2Squared < side3Squared)
    {
    pt1 = tri[1];
    pt2 = tri[2];
    }
  else 
    {
    pt1 = tri[2];
    pt2 = tri[0];
    }

  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (pt3 == pt4)
    {
    return DO_NOTHING;
    }

  bool proceed = false;

  if (success == SUCCESS)
    {
    proceed = true;
    }

  if (!this->PreserveBoundaryEdges && success == EDGE_ON_BOUNDARY)
    {
    proceed = true;
    }

  if (success == EDGE_BETWEEN_ENTITIES)
    {
    proceed = true;
    }

  if (!proceed)
    {
    return DO_NOTHING;
    }

  unsigned short ncells3, ncells4;
  ncells3 = ncells4 = 0;
  vtkIdType *cells;
  this->Mesh->GetPointCells(pt3,ncells3,cells);
  if (success != EDGE_ON_BOUNDARY)
    {
    this->Mesh->GetPointCells(pt4,ncells4,cells);
    }
  if (ncells3==3 || ncells4==3)
    {
    return DO_NOTHING;
    }

  unsigned short ncells2;
  this->Mesh->GetPointCells(pt2,ncells2,cells);
  for (int i=0; i<ncells2; i++)
    {
    if (cells[i] == cell1 || cells[i] == cell2)
      {
      continue;
      }
    this->Mesh->GetCellPoints(cells[i],npts,pts);
    
    double normal1[3], normal2[3];
    this->Mesh->GetPoint(pts[0],point1);
    this->Mesh->GetPoint(pts[1],point2);
    this->Mesh->GetPoint(pts[2],point3);
    vtkTriangle::ComputeNormal(point1,point2,point3,normal1);

    for (int j=0; j<3; j++)
      {
      if (pts[j] != pt2)
        {
        tri[j] = pts[j];
        }
      else
        {
        tri[j] = pt1;
        }
      }
    this->Mesh->GetPoint(tri[0],point1);
    this->Mesh->GetPoint(tri[1],point2);
    this->Mesh->GetPoint(tri[2],point3);
    vtkTriangle::ComputeNormal(point1,point2,point3,normal2);

    if (vtkMath::Dot(normal1,normal2) < 0.0)
      {
      return DO_NOTHING;
      }

    if (acos(vtkMath::Dot(normal1,normal2)) > this->CollapseAngleThreshold)
      {
      return DO_NOTHING;
      }
    }

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestAreaSplitEdge(vtkIdType cellId, vtkIdType& pt1, vtkIdType& pt2)
{
  pt1 = -1;
  pt2 = -1;

  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);

  vtkIdType tri[3];
  tri[0] = pts[0];
  tri[1] = pts[1];
  tri[2] = pts[2];

  double point1[3], point2[3], point3[3];
  this->Mesh->GetPoint(tri[0],point1);
  this->Mesh->GetPoint(tri[1],point2);
  this->Mesh->GetPoint(tri[2],point3);

  double area = vtkTriangle::TriangleArea(point1,point2,point3);

  double side1Squared = vtkMath::Distance2BetweenPoints(point1,point2);
  double side2Squared = vtkMath::Distance2BetweenPoints(point2,point3);
  double side3Squared = vtkMath::Distance2BetweenPoints(point3,point1);
  
  double targetArea = this->ComputeTriangleTargetArea(cellId);

  if (area < targetArea)
    {
    return DO_NOTHING;
    }

  if (side1Squared >= side2Squared && side1Squared >= side3Squared)
    {
    pt1 = tri[0];
    pt2 = tri[1];
    }
  else if (side2Squared >= side1Squared && side2Squared >= side3Squared)
    {
    pt1 = tri[1];
    pt2 = tri[2];
    }
  else if (side3Squared >= side1Squared && side3Squared >= side1Squared)
    {
    pt1 = tri[2];
    pt2 = tri[0];
    }

  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  bool proceed = false;

  if (success == SUCCESS)
    {
    proceed = true;
    }

  if (!this->PreserveBoundaryEdges && success == EDGE_ON_BOUNDARY)
    {
    proceed = true;
    }

  if (success == EDGE_BETWEEN_ENTITIES)
    {
    proceed = true;
    }

  if (!proceed)
    {
    return DO_NOTHING;
    }

  return DO_CHANGE;
}

void vtkvmtkPolyDataSurfaceRemeshing::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

