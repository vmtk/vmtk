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
#include "vtkIdList.h"
#include "vtkCellArray.h"
#include "vtkMeshQuality.h"
#include "vtkCellLocator.h"
#include "vtkTriangle.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataSurfaceRemeshing, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkvmtkPolyDataSurfaceRemeshing);

vtkvmtkPolyDataSurfaceRemeshing::vtkvmtkPolyDataSurfaceRemeshing()
{
  this->AspectRatioThreshold = 1.3;
  this->MaxArea = 1.0;
  this->InternalAngleTolerance = 0.0;
  this->NormalAngleTolerance = 0.2;
  this->Relaxation = 0.1;

  this->Mesh = NULL;
  this->Locator = NULL;
}

vtkvmtkPolyDataSurfaceRemeshing::~vtkvmtkPolyDataSurfaceRemeshing()
{
  if (this->Mesh)
    {
    this->Mesh->Delete();
    this->Mesh = NULL;
    }

  if (this->Locator)
    {
    this->Locator->Delete();
    this->Locator = NULL;
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

  if (this->Mesh)
    {
    this->Mesh->Delete();
    this->Mesh = NULL;
    }

  this->Mesh = vtkPolyData::New();
  vtkPoints* workingPoints = vtkPoints::New();
  vtkCellArray* workingCells = vtkCellArray::New();
  workingPoints->DeepCopy(input->GetPoints());
  
  for (int i=0; i<input->GetNumberOfCells(); i++)
    {
    if (input->GetCellType(i) != VTK_TRIANGLE)
      {
      continue;
      }
    workingCells->InsertNextCell(input->GetCell(i));
    }

  this->Mesh->SetPoints(workingPoints);
  this->Mesh->SetPolys(workingCells);

  workingPoints->Delete();
  workingCells->Delete(); 

  this->Mesh->BuildCells();
  this->Mesh->BuildLinks();

  if (this->Locator)
    {
    this->Locator->Delete();
    this->Locator = NULL;
    }

  this->Locator = vtkCellLocator::New();
  this->Locator->SetDataSet(input);
  this->Locator->BuildLocator();

  this->AspectRatioThreshold = 1.3;
  this->MaxArea = 0.4;
  this->InternalAngleTolerance = 0.0;
  this->NormalAngleTolerance = 0.2;
  this->Relaxation = 0.5;

  int numberOfIterations = 10;

  for (int n=0; n<numberOfIterations; n++)
    {
    this->EdgeCollapseIteration();
    this->EdgeFlipIteration();
    this->EdgeSplitIteration();
    this->EdgeFlipIteration();
    this->PointRelocationIteration();
    this->EdgeFlipIteration();

    while(this->EdgeFlipConnectivityOptimizationIteration());
    }
  this->EdgeFlipIteration();
  
  vtkPoints* newPoints = vtkPoints::New();
  vtkCellArray* newCells = vtkCellArray::New();

  newPoints->DeepCopy(this->Mesh->GetPoints());

  int numberOfFinalCells = this->Mesh->GetNumberOfCells();
  for (int i=0; i<numberOfFinalCells; i++)
    {
    if (this->Mesh->GetCellType(i) == VTK_EMPTY_CELL)
      {
      continue;
      }
    newCells->InsertNextCell(this->Mesh->GetCell(i));
    }

  output->SetPoints(newPoints);
  output->SetPolys(newCells);
 
  newPoints->Delete();
  newCells->Delete();

  return 1;
}

int vtkvmtkPolyDataSurfaceRemeshing::EdgeFlipConnectivityOptimizationIteration()
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
      int test = this->TestConnectivityFlipEdge(tripts[j],tripts[(j+1)%3],this->NormalAngleTolerance);
      if (test == DO_NOTHING)
        {
        continue;
        }
      this->FlipEdge(tripts[j],tripts[(j+1)%3]);
      numberOfChanges++;
      break;
      }
    }
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
      int test = this->TestDelaunayFlipEdge(tripts[j],tripts[(j+1)%3],this->InternalAngleTolerance,this->NormalAngleTolerance);
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
    int test = this->TestAspectRatioCollapseEdge(i,this->AspectRatioThreshold,this->MaxArea,pt1,pt2);
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
    int test = this->TestAreaSplitEdge(i,this->MaxArea,pt1,pt2);
    if (test == DO_NOTHING)
      {
      continue;
      }
    this->SplitEdge(pt1,pt2);
    numberOfChanges++;
    }
  return numberOfChanges;
}

void vtkvmtkPolyDataSurfaceRemeshing::PointRelocationIteration()
{
  int numberOfPoints = this->Mesh->GetNumberOfPoints();
  for (int i=0; i<numberOfPoints; i++)
    {
    this->RelocatePoint(i,this->Relaxation);
    } 
}

void vtkvmtkPolyDataSurfaceRemeshing::RelocatePoint(vtkIdType pointId, double relaxation)
{
  vtkvmtkPolyDataUmbrellaStencil* stencil = vtkvmtkPolyDataUmbrellaStencil::New();
  stencil->SetDataSet(this->Mesh);
  stencil->SetDataSetPointId(pointId);
  stencil->NegateWeightsOff();
  stencil->Build();

  double targetPoint[3];
  targetPoint[0] = targetPoint[1] = targetPoint[2] = 0.0;
  
  for (int i=0; i<stencil->GetNumberOfPoints(); i++)
    {
    double stencilPoint[3];
    this->Mesh->GetPoint((stencil->GetPointId(i)),stencilPoint);
    double stencilWeight = stencil->GetWeight(i);
    targetPoint[0] += stencilWeight * stencilPoint[0];
    targetPoint[1] += stencilWeight * stencilPoint[1];
    targetPoint[2] += stencilWeight * stencilPoint[2];
    }

  double point[3];
  this->Mesh->GetPoint(pointId,point);

  double relocatedPoint[3];
  relocatedPoint[0] = point[0] + relaxation * (targetPoint[0] - point[0]);
  relocatedPoint[1] = point[1] + relaxation * (targetPoint[1] - point[1]);
  relocatedPoint[2] = point[2] + relaxation * (targetPoint[2] - point[2]);

  double projectedRelocatedPoint[3];
  vtkIdType cellId;
  int subId;
  double dist2;
  this->Locator->FindClosestPoint(relocatedPoint,projectedRelocatedPoint,cellId,subId,dist2);

  this->Mesh->GetPoints()->SetPoint(pointId,projectedRelocatedPoint);

  stencil->Delete();
}

int vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(vtkIdType pt1, vtkIdType pt2, vtkIdType& cell1, vtkIdType& cell2, vtkIdType& pt3, vtkIdType& pt4)
{
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

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::SplitTriangle(vtkIdType cellId)
{
  if (this->Mesh->GetCellType(cellId) != VTK_TRIANGLE)
    {
    return NOT_TRIANGLES;
    }

  vtkIdType npts, *pts;
  this->Mesh->GetCellPoints(cellId,npts,pts);
  vtkIdType pt1 = pts[0];
  vtkIdType pt2 = pts[1];
  vtkIdType pt3 = pts[2];

  double point1[3], point2[3], point3[3], newPoint[3];
  this->Mesh->GetPoint(pt1,point1);
  this->Mesh->GetPoint(pt2,point2);
  this->Mesh->GetPoint(pt3,point3);
  vtkTriangle::TriangleCenter(point1,point2,point3,newPoint);

  vtkIdType newpt = this->Mesh->InsertNextLinkedPoint(newPoint,1);

  this->Mesh->ReplaceCellPoint(cellId,pt3,newpt);
  this->Mesh->RemoveReferenceToCell(pt3,cellId);
  this->Mesh->AddReferenceToCell(newpt,cellId);

  vtkIdType tri[3];
  tri[0] = pt2;
  tri[1] = pt3;
  tri[2] = newpt;
  vtkIdType cell1 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);

  tri[0] = pt3;
  tri[1] = pt1;
  tri[2] = newpt;
  vtkIdType cell2 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::CollapseTriangle(vtkIdType cellId)
{
  cout<<"Not implemented!"<<endl;
  return -1;
}

int vtkvmtkPolyDataSurfaceRemeshing::SplitEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
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

  vtkIdType newpt = this->Mesh->InsertNextLinkedPoint(newPoint,2);

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
  tri[0] = pt2;
  tri[1] = newpt;
  tri[2] = pt4;
  vtkIdType cell4 = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::CollapseEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return success;
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
  this->Mesh->DeleteCell(cell2); 

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

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::FlipEdge(vtkIdType pt1, vtkIdType pt2)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return success;
    }

  this->Mesh->RemoveCellReference(cell1);
  this->Mesh->RemoveCellReference(cell2);
  this->Mesh->DeleteCell(cell1); 
  this->Mesh->DeleteCell(cell2); 

  vtkIdType tri[3];
  tri[0] = pt3;
  tri[1] = pt1;
  tri[2] = pt4;
  vtkIdType cell1b = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
  tri[0] = pt2;
  tri[1] = pt3;
  tri[2] = pt4;
  vtkIdType cell2b = this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);

  vtkIdType pt3b, pt4b;
  if (vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt3,pt4,cell1,cell2,pt3b,pt4b) != SUCCESS)
    {
    this->Mesh->RemoveCellReference(cell1b);
    this->Mesh->RemoveCellReference(cell2b);
    this->Mesh->DeleteCell(cell1b);
    this->Mesh->DeleteCell(cell2b);
    tri[0] = pt1;
    tri[1] = pt2;
    tri[2] = pt3;
    this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    tri[0] = pt1;
    tri[1] = pt4;
    tri[2] = pt2;
    this->Mesh->InsertNextLinkedCell(VTK_TRIANGLE,3,tri);
    }

  return SUCCESS;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestFlipEdgeValidity(vtkIdType pt1, vtkIdType pt2, vtkIdType cell1, vtkIdType cell2, vtkIdType pt3, vtkIdType pt4, double normalAngleTolerance)
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

  if (acos(vtkMath::Dot(normal3,normal4)) > acos(vtkMath::Dot(normal1,normal2)) + normalAngleTolerance)
    {
    return DO_NOTHING;
    } 
}

int vtkvmtkPolyDataSurfaceRemeshing::TestConnectivityFlipEdge(vtkIdType pt1, vtkIdType pt2, double normalAngleTolerance)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return DO_NOTHING;
    }

  if (this->TestFlipEdgeValidity(pt1,pt2,cell1,cell2,pt3,pt4,normalAngleTolerance)==DO_NOTHING)
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

  int currentValence = (ncells1-targetValence)*(ncells1-targetValence) + (ncells2-targetValence)*(ncells2-targetValence) + (ncells3-targetValence)*(ncells3-targetValence) + (ncells4-targetValence)*(ncells4-targetValence);
  int flippedValence = (ncells1-1-targetValence)*(ncells1-1-targetValence) + (ncells2-1-targetValence)*(ncells2-1-targetValence) + (ncells3+1-targetValence)*(ncells3+1-targetValence) + (ncells4+1-targetValence)*(ncells4+1-targetValence);

  if (flippedValence >= currentValence)
    {
    return DO_NOTHING;
    }

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestDelaunayFlipEdge(vtkIdType pt1, vtkIdType pt2, double internalAngleTolerance, double normalAngleTolerance)
{
  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return DO_NOTHING;
    }

  if (this->TestFlipEdgeValidity(pt1,pt2,cell1,cell2,pt3,pt4,normalAngleTolerance)==DO_NOTHING)
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

  if (maxAngle01 < maxAngle23 + internalAngleTolerance)
    {
    return DO_NOTHING;
    }

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestAspectRatioCollapseEdge(vtkIdType cellId, double aspectRatioThreshold, double maxArea, vtkIdType& pt1, vtkIdType& pt2)
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

  if (area > maxArea)
    {
    return DO_NOTHING;
    }

  double side1Squared = vtkMath::Distance2BetweenPoints(point1,point2);
  double side2Squared = vtkMath::Distance2BetweenPoints(point2,point3);
  double side3Squared = vtkMath::Distance2BetweenPoints(point3,point1);
  
  double frobeniusAspectRatio = (side1Squared + side2Squared + side3Squared) / (4.0 * sqrt(3.0) * area);

  if (frobeniusAspectRatio < aspectRatioThreshold)
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
  else if (side3Squared < side1Squared && side3Squared < side1Squared)
    {
    pt1 = tri[2];
    pt2 = tri[0];
    }

  vtkIdType cell1, cell2, pt3, pt4;
  int success = vtkvmtkPolyDataSurfaceRemeshing::GetEdgeCellsAndOppositeEdge(pt1,pt2,cell1,cell2,pt3,pt4);

  if (success != SUCCESS)
    {
    return DO_NOTHING;
    }

  unsigned short ncells3, ncells4;
  vtkIdType *cells;
  this->Mesh->GetPointCells(pt3,ncells3,cells);
  this->Mesh->GetPointCells(pt4,ncells4,cells);
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

    if (vtkMath::Dot(normal1,normal2)<0.0)
      {
      return DO_NOTHING;
      }
    }

  return DO_CHANGE;
}

int vtkvmtkPolyDataSurfaceRemeshing::TestAreaSplitEdge(vtkIdType cellId, double maxArea, vtkIdType& pt1, vtkIdType& pt2)
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
  
  if (area < maxArea)
    {
    return DO_NOTHING;
    }

  if (side1Squared > side2Squared && side1Squared > side3Squared)
    {
    pt1 = tri[0];
    pt2 = tri[1];
    }
  else if (side2Squared > side1Squared && side2Squared > side3Squared)
    {
    pt1 = tri[1];
    pt2 = tri[2];
    }
  else if (side3Squared > side1Squared && side3Squared > side1Squared)
    {
    pt1 = tri[2];
    pt2 = tri[0];
    }

  return DO_CHANGE;
}

void vtkvmtkPolyDataSurfaceRemeshing::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

