/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkNonManifoldFastMarching.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "vtkvmtkNonManifoldFastMarching.h"
#include "vtkvmtkConstants.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkCharArray.h"
#include "vtkMath.h"
#include "vtkPolyLine.h"
#include "vtkTriangle.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkNonManifoldFastMarching);


vtkvmtkNonManifoldFastMarching::vtkvmtkNonManifoldFastMarching()
{
  this->Seeds = NULL;
  this->TScalars = vtkDoubleArray::New();
  this->StatusScalars = vtkCharArray::New();
  this->ConsideredMinHeap = vtkvmtkMinHeap::New();

  this->Regularization = 0.0;
  this->StopTravelTime = VTK_VMTK_LARGE_DOUBLE;
  this->StopNumberOfPoints = VTK_VMTK_LARGE_INTEGER;
  this->UnitSpeed = 0;
  this->InitializeFromScalars = 0;
  this->InitializationArrayName = NULL;
  this->SolutionArrayName = NULL;
  this->CostFunctionArrayName = NULL;

  this->SeedsBoundaryConditions = 0;
  this->PolyDataBoundaryConditions = 0;

  this->BoundaryPolyData = NULL;
  this->IntersectedEdgesArrayName = NULL;

  this->NumberOfAcceptedPoints = 0;

  this->AllowLineUpdate = 1;
  this->UpdateFromConsidered = 1;
  this->StopSeedId = NULL;
}

vtkvmtkNonManifoldFastMarching::~vtkvmtkNonManifoldFastMarching()
{
  if (this->Seeds)
    {
    this->Seeds->Delete();
    this->Seeds = NULL;
    }

  if (this->BoundaryPolyData)
    {
    this->BoundaryPolyData->Delete();
    this->BoundaryPolyData = NULL;
    }

   if (this->SolutionArrayName)
    {
    delete[] this->SolutionArrayName;
    this->SolutionArrayName = NULL;
    }

  if (this->CostFunctionArrayName)
    {
    delete[] this->CostFunctionArrayName;
    this->CostFunctionArrayName = NULL;
    }

  if (this->StopSeedId)
    {
    this->StopSeedId->Delete();
    this->StopSeedId = NULL;
    }
  this->TScalars->Delete();
  this->StatusScalars->Delete();
  this->ConsideredMinHeap->Delete();
}

void vtkvmtkNonManifoldFastMarching::InitPropagation(vtkPolyData* input)
{
  vtkIdType i, j, k, l;
  vtkIdType pointId;
  vtkIdType npts, *cells;
  const vtkIdType *pts;
  vtkIdType ncells;
  vtkIdType intersectedEdge[2];
  vtkDataArray* initializationArray, *costFunctionArray, *intersectedEdgesArray;
  vtkIdList* neighborCells;
  vtkIdList* neighborIds;
  vtkIdList* boundaryPointIds;
  int subId;
  double pcoords[3], *weights;
  double closestPoint[3];
  double distance, distance2, minDistance;
  int allowLineUpdateBackup;
  vtkPolyLine* polyLine;

  boundaryPointIds = vtkIdList::New();
  neighborCells = vtkIdList::New();
  neighborIds = vtkIdList::New();

  allowLineUpdateBackup = this->AllowLineUpdate;
  this->AllowLineUpdate = 1;

  initializationArray = NULL;
  if (this->InitializeFromScalars)
    {
    initializationArray = input->GetPointData()->GetArray(this->InitializationArrayName);
    }

  if (!this->UnitSpeed)
    {
    costFunctionArray = input->GetPointData()->GetArray(this->CostFunctionArrayName);
    }

  input->BuildCells();
  input->BuildLinks();

  this->StatusScalars->SetNumberOfTuples(input->GetNumberOfPoints());
  this->StatusScalars->FillComponent(0,VTK_VMTK_FAR_STATUS);

  this->TScalars->SetNumberOfTuples(input->GetNumberOfPoints());
  this->TScalars->FillComponent(0,VTK_VMTK_LARGE_DOUBLE);

  this->NumberOfAcceptedPoints = 0;

  this->ConsideredMinHeap->SetMinHeapScalars(this->TScalars);
  this->ConsideredMinHeap->Initialize();

  if (this->SeedsBoundaryConditions)
    {
    for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
      {
      pointId = this->Seeds->GetId(i);
      boundaryPointIds->InsertUniqueId(pointId);
      if (this->InitializeFromScalars)
        {
        this->TScalars->SetComponent(pointId,0,initializationArray->GetTuple1(pointId));
        }
      else
        {
        this->TScalars->SetComponent(pointId,0,0.0f);
        }
      }
    }
  
  if (this->PolyDataBoundaryConditions)
    {
    this->BoundaryPolyData->BuildCells();
    this->BoundaryPolyData->BuildLinks();
    intersectedEdgesArray = this->BoundaryPolyData->GetPointData()->GetArray(this->IntersectedEdgesArrayName);
    for (i=0; i<this->BoundaryPolyData->GetNumberOfPoints(); i++)
      {
      intersectedEdge[0] = static_cast<vtkIdType>(intersectedEdgesArray->GetComponent(i,0));
      intersectedEdge[1] = static_cast<vtkIdType>(intersectedEdgesArray->GetComponent(i,1));

      neighborCells->Initialize();

      if (intersectedEdge[0]==intersectedEdge[1])
        {
        input->GetPointCells(intersectedEdge[0],neighborCells);
        boundaryPointIds->InsertUniqueId(intersectedEdge[0]);
        this->TScalars->SetComponent(intersectedEdge[0],0,0.0);
        }
      else
        {
        input->GetCellEdgeNeighbors(-1,intersectedEdge[0],intersectedEdge[1],neighborCells);
        boundaryPointIds->InsertUniqueId(intersectedEdge[0]);
        boundaryPointIds->InsertUniqueId(intersectedEdge[1]);
        }

      for (j=0; j<neighborCells->GetNumberOfIds(); j++)
        {
        input->GetCellPoints(neighborCells->GetId(j),npts,pts);
        for (k=0; k<npts; k++)
          {
          boundaryPointIds->InsertUniqueId(pts[k]);
          minDistance = this->TScalars->GetComponent(pts[k],0);
          this->BoundaryPolyData->GetPointCells(i,ncells,cells);
          for (l=0; l<ncells; l++)
            {
            polyLine = vtkPolyLine::SafeDownCast(this->BoundaryPolyData->GetCell(cells[l]));
            if (!polyLine)
              {
              vtkWarningMacro(<<"BoundaryPolyData is not all made up of PolyLines");
              continue;
              }
            weights = new double[polyLine->GetNumberOfPoints()];
            double point[3];
            input->GetPoint(pts[k],point);
            polyLine->EvaluatePosition(point,closestPoint,subId,pcoords,distance2,weights);
            delete[] weights;
            distance = sqrt(distance2);
            if (distance - minDistance < -VTK_VMTK_DOUBLE_TOL)
              {
              this->TScalars->SetComponent(pts[k],0,distance);
              minDistance = distance;
              }
            }
          }
        }
      }
    }

  vtkIdType numberOfBoundaryPointIds;
  numberOfBoundaryPointIds = boundaryPointIds->GetNumberOfIds();

  for (i=0; i<numberOfBoundaryPointIds; i++)
    {
    this->StatusScalars->SetValue(boundaryPointIds->GetId(i),VTK_VMTK_ACCEPTED_STATUS);
    this->NumberOfAcceptedPoints++;
    }

  for (k=0; k<3; k++)   // get a good initial solution
    {
    for (i=0; i<numberOfBoundaryPointIds; i++)
      {
      this->UpdateNeighborhood(input,boundaryPointIds->GetId(i));
      }
    }

  this->AllowLineUpdate = allowLineUpdateBackup;

  boundaryPointIds->Delete();
  neighborCells->Delete();
  neighborIds->Delete();
}

void vtkvmtkNonManifoldFastMarching::GetNeighbors(vtkPolyData* input, vtkIdType pointId, vtkIdList* neighborIds)
{
  vtkIdType i, j;
  vtkIdType npts, *cells;
  const vtkIdType *pts;
  vtkIdType ncells;

  input->GetPointCells(pointId,ncells,cells);
  for (i=0; i<ncells; i++)
    {
    input->GetCellPoints(cells[i],npts,pts);
    for (j=0; j<npts; j++)
      {
      if (pts[j]!=pointId)
        {
        neighborIds->InsertUniqueId(pts[j]);
        }
      }
    }
}

void vtkvmtkNonManifoldFastMarching::SolveQuadratic(double a, double b, double c, char &nSol, double &x0, double &x1)
{
  double delta, q;

  delta = b*b - 4*a*c;

  if (delta < -VTK_VMTK_DOUBLE_TOL)
    {
    nSol = -1;
    x0 = VTK_VMTK_LARGE_DOUBLE;
    x1 = VTK_VMTK_LARGE_DOUBLE;
    return;
    }

  if (fabs(a) > VTK_VMTK_DOUBLE_TOL)
    {
    nSol = 2;
    if (delta < VTK_VMTK_DOUBLE_TOL)
      {
      x0 = -b / 2.0*a;
      x1 = -b / 2.0*a;
      return;
      }

    if (b <  - VTK_VMTK_DOUBLE_TOL)
      {
      q = - 0.5 * (b - sqrt(delta));
      }
    else if (b > VTK_VMTK_DOUBLE_TOL)
      {
      q = - 0.5 * (b + sqrt(delta));
      }
    else
      {
      x0 = - sqrt(-c / a);
      x1 = sqrt(-c / a);
      return;
      }

    x0 = q / a;
    x1 = c / q;
    }
  else if (b > VTK_VMTK_DOUBLE_TOL)
    {
    nSol = 1;
    x0 = -c / b;
    x1 = VTK_VMTK_LARGE_DOUBLE;
    }
  else
    {
    nSol = 0;
    x0 = VTK_VMTK_LARGE_DOUBLE;
    x1 = VTK_VMTK_LARGE_DOUBLE;
    }

}

double vtkvmtkNonManifoldFastMarching::ComputeUpdateFromCellNeighbor(vtkPolyData* input, vtkIdType neighborId, vtkIdType* trianglePts)
{
  double fScalar, neighborT;
  vtkIdType i;
  bool canUpdateFromTriangle, canUpdateFromLine;
  vtkIdType pointIdForLineUpdate, pointId;
  vtkIdType edgesPointId[2];
  double edgesLength[2], edgesTScalar[2];
  double edgesVector[2][3];
  double edgesNormal[2][3];
  double cosTheta;
  vtkIdType aEdgeId, bEdgeId;
  char nSol;
  double bEq, aEq, cEq, uEq, FEq, tEq, tCompEq, t0Eq, t1Eq, t0CompEq, tCompEqLower, tCompEqHigher;
  double edgeLength;
  vtkDataArray* costFunctionArray;

  pointIdForLineUpdate = -1;
  tCompEq = 0.0;
  if (!this->UnitSpeed)
    {
    costFunctionArray = input->GetPointData()->GetArray(this->CostFunctionArrayName);
    }

  if (this->UnitSpeed)
    {
    fScalar = 1.0;
    }
  else
    {
    fScalar = costFunctionArray->GetTuple1(neighborId);
    }
        
  neighborT = this->TScalars->GetValue(neighborId);
        
  canUpdateFromTriangle = true;
  canUpdateFromLine = false;
  for (i=1; i<3; i++)
    {
    pointId = trianglePts[i];
    if (pointId!=neighborId)
      {
      if ((this->StatusScalars->GetValue(pointId) != VTK_VMTK_ACCEPTED_STATUS) ||
          ((this->StatusScalars->GetValue(pointId) == VTK_VMTK_FAR_STATUS)&&(this->UpdateFromConsidered)))
        {
        canUpdateFromTriangle   = false;
        }
      else
        {
        canUpdateFromLine = true;
        pointIdForLineUpdate = pointId;
        }
      }
    }

  if (!canUpdateFromTriangle)
    {
    if ((canUpdateFromLine)&&(this->AllowLineUpdate))
      {
      double neighborPoint[3], lineUpdatePoint[3];
      input->GetPoint(neighborId,neighborPoint);
      input->GetPoint(pointIdForLineUpdate,lineUpdatePoint);
      edgeLength = sqrt(vtkMath::Distance2BetweenPoints(neighborPoint,lineUpdatePoint));
      neighborT = this->Min(this->TScalars->GetValue(pointIdForLineUpdate) + edgeLength * fScalar,neighborT);
      return neighborT;
      }
    else
      return VTK_VMTK_LARGE_DOUBLE;
    }

  if (trianglePts[0]==neighborId)
    {
    edgesPointId[0] = trianglePts[1];
    edgesPointId[1] = trianglePts[2];
    }
  else if (trianglePts[1]==neighborId)
    {
    edgesPointId[0] = trianglePts[2];
    edgesPointId[1] = trianglePts[0];
    }
  else
    {
    edgesPointId[0] = trianglePts[0];
    edgesPointId[1] = trianglePts[1];
    }

  double neighborPoint[3];
  input->GetPoint(neighborId,neighborPoint);

  double edgePoint0[3], edgePoint1[3];
  input->GetPoint(edgesPointId[0],edgePoint0);
  input->GetPoint(edgesPointId[1],edgePoint1);

  edgesVector[0][0] = neighborPoint[0] - edgePoint0[0];
  edgesVector[0][1] = neighborPoint[1] - edgePoint0[1];
  edgesVector[0][2] = neighborPoint[2] - edgePoint0[2];

  edgesVector[1][0] = neighborPoint[0] - edgePoint1[0];
  edgesVector[1][1] = neighborPoint[1] - edgePoint1[1];
  edgesVector[1][2] = neighborPoint[2] - edgePoint1[2];

  edgesLength[0] = vtkMath::Norm(edgesVector[0]);
  edgesLength[1] = vtkMath::Norm(edgesVector[1]);

  if (edgesLength[0] > VTK_VMTK_DOUBLE_TOL)
    {
    edgesNormal[0][0] = edgesVector[0][0] / edgesLength[0];
    edgesNormal[0][1] = edgesVector[0][1] / edgesLength[0];
    edgesNormal[0][2] = edgesVector[0][2] / edgesLength[0];
    }
  else
    {
    edgesLength[0] = 0.0;
    edgesNormal[0][0] = 0.0;
    edgesNormal[0][1] = 0.0;
    edgesNormal[0][2] = 0.0;
    }

  if (edgesLength[1] > VTK_VMTK_DOUBLE_TOL)
    {
    edgesNormal[1][0] = edgesVector[1][0] / edgesLength[1];
    edgesNormal[1][1] = edgesVector[1][1] / edgesLength[1];
    edgesNormal[1][2] = edgesVector[1][2] / edgesLength[1];
    }
  else
    {
    edgesLength[1] = 0.0;
    edgesNormal[1][0] = 0.0;
    edgesNormal[1][1] = 0.0;
    edgesNormal[1][2] = 0.0;
    }

  cosTheta = vtkMath::Dot(edgesNormal[0],edgesNormal[1]);

  if (fabs(cosTheta) < VTK_VMTK_DOUBLE_TOL)
    cosTheta = 0.0f;

  edgesTScalar[0] = this->TScalars->GetValue(edgesPointId[0]);
  edgesTScalar[1] = this->TScalars->GetValue(edgesPointId[1]);

  if (edgesTScalar[0]-edgesTScalar[1] > VTK_VMTK_DOUBLE_TOL)
    {
    aEdgeId = 0;
    bEdgeId = 1;
    }
  else
    {
    aEdgeId = 1;
    bEdgeId = 0;
    }


  FEq = fScalar + this->Regularization;

  uEq = edgesTScalar[aEdgeId]-edgesTScalar[bEdgeId];
  aEq = edgesLength[aEdgeId]*edgesLength[aEdgeId] + edgesLength[bEdgeId]*edgesLength[bEdgeId] - 2*edgesLength[aEdgeId]*edgesLength[bEdgeId]*cosTheta;
  bEq = 2 * edgesLength[bEdgeId] * uEq * (edgesLength[aEdgeId] * cosTheta - edgesLength[bEdgeId]);
  cEq = edgesLength[bEdgeId]*edgesLength[bEdgeId] * (uEq*uEq - FEq*FEq*edgesLength[aEdgeId]*edgesLength[aEdgeId]*(1-cosTheta*cosTheta));

  this->SolveQuadratic(aEq,bEq,cEq,nSol,t0Eq,t1Eq);

  if (nSol==2)
    {
    tEq = this->Min(t0Eq,t1Eq);
    }
  else
    {
    tEq = t0Eq;
    }

  if (fabs(tEq) > VTK_VMTK_DOUBLE_TOL)
    {
    tCompEq = edgesLength[bEdgeId] * (tEq - uEq) / tEq;
    }
  else
    {
    t0CompEq = VTK_VMTK_LARGE_DOUBLE;
    }
    
  tCompEqLower = edgesLength[aEdgeId]*cosTheta;

  if (fabs(cosTheta) > VTK_VMTK_DOUBLE_TOL)
    {
    tCompEqHigher = edgesLength[aEdgeId]/cosTheta;
    }
  else
    {
    tCompEqHigher = VTK_VMTK_LARGE_DOUBLE;
    }

  if ((uEq - tEq < -VTK_VMTK_DOUBLE_TOL) && (tCompEq - tCompEqLower > VTK_VMTK_DOUBLE_TOL) && (tCompEq - tCompEqHigher < -VTK_VMTK_DOUBLE_TOL))
    {
    neighborT = this->Min(tEq + edgesTScalar[bEdgeId],neighborT);
    }
  else
    {
    neighborT = this->Min(edgesLength[aEdgeId]*FEq + edgesTScalar[aEdgeId] , edgesLength[bEdgeId]*FEq + edgesTScalar[bEdgeId]);
    }

  return neighborT;     
}

void vtkvmtkNonManifoldFastMarching::UpdateNeighbor(vtkPolyData* input, vtkIdType neighborId)
{
  vtkIdType i, j, k;
  vtkIdType npts;
  const vtkIdType *pts;
  vtkIdType trianglePts[3];
  double tMin, tScalar;
  vtkIdList* neighborCellNeighborIds;

  if ((neighborId<0)||(neighborId>=this->TScalars->GetNumberOfTuples()))
    {
    vtkErrorMacro("Requested id exceeds TScalars dimension.");
    return;
    }

  neighborCellNeighborIds = vtkIdList::New();

  input->GetPointCells(neighborId,neighborCellNeighborIds);

  tMin = this->TScalars->GetValue(neighborId);
  trianglePts[0] = neighborId;
  for (i=0; i<neighborCellNeighborIds->GetNumberOfIds(); i++)
    {
    // virtual triangulation
    input->GetCellPoints(neighborCellNeighborIds->GetId(i),npts,pts);
    for (j=0; j<npts; j++)
      {
      if (pts[j]!=neighborId)
        {
        trianglePts[1] = pts[j];
        for (k=j+1; k<npts; k++)
          {
          if (pts[k]!=neighborId)
            {
            trianglePts[2] = pts[k];
            tScalar = this->ComputeUpdateFromCellNeighbor(input,neighborId,trianglePts);
            tMin = this->Min(tScalar,tMin);
            }
          }
        }
      }         
    }

  this->TScalars->SetValue(neighborId,tMin);

  neighborCellNeighborIds->Delete();
}

void vtkvmtkNonManifoldFastMarching::UpdateNeighborhood(vtkPolyData* input, vtkIdType pointId)
{
  vtkIdList* neighborIds;
  vtkIdType i, neighborId;

  neighborIds = vtkIdList::New();

  this->GetNeighbors(input,pointId,neighborIds);
  for (i=0; i<neighborIds->GetNumberOfIds(); i++)
    {
    neighborId = neighborIds->GetId(i);
    if (this->StatusScalars->GetValue(neighborId)!=VTK_VMTK_ACCEPTED_STATUS)
      {
      this->UpdateNeighbor(input,neighborId);
      if (this->StatusScalars->GetValue(neighborId) == VTK_VMTK_FAR_STATUS)
        {
        this->StatusScalars->SetValue(neighborId,VTK_VMTK_CONSIDERED_STATUS);
        this->ConsideredMinHeap->InsertNextId(neighborId);
        }
      else
        {
        this->ConsideredMinHeap->UpdateId(neighborId);
        }
      }
    }

  neighborIds->Delete();
}

void vtkvmtkNonManifoldFastMarching::Propagate(vtkPolyData* input)
{
  double currentTravelTime;
  vtkIdType trialId;

  while (this->ConsideredMinHeap->GetSize()>0)
    {
    trialId = this->ConsideredMinHeap->RemoveMin();
    this->StatusScalars->SetValue(trialId,VTK_VMTK_ACCEPTED_STATUS);
    this->NumberOfAcceptedPoints++;

    this->UpdateNeighborhood(input,trialId);

    currentTravelTime = this->TScalars->GetValue(trialId);
    
    // This will stop execution once the front has propogated from the source point to the
    // Stop point (in the case where StopSeedId is set. Default is NULL)
    if (this->StopSeedId)
    {
      if (trialId == this->StopSeedId->GetId(0))
      {
        break;
      }
    }
    if ((this->StopNumberOfPoints)||(this->StopTravelTime))
      {
      if ((this->NumberOfAcceptedPoints >= this->StopNumberOfPoints)||(currentTravelTime - this->StopTravelTime > VTK_VMTK_DOUBLE_TOL))
        {
        break;
        }
      }
    }   
}

int vtkvmtkNonManifoldFastMarching::RequestData(
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

  char eikonalSolutionArrayName[512];
  vtkIdType i;

  if (this->InitializeFromScalars)
    {
    if (!this->InitializationArrayName)
      {
      vtkErrorMacro(<<"No initialization array name defined!");
      return 1;
      }
    if (!input->GetPointData()->GetArray(this->InitializationArrayName))
      {
      vtkErrorMacro(<< "Initialization array with name specified does not exist!");
      return 1;
      }
    }

  if (!this->UnitSpeed)
    {
    if (!this->CostFunctionArrayName)
      {
      vtkErrorMacro(<<"No cost function array name defined!");
      return 1;
      }
    if (!input->GetPointData()->GetArray(this->CostFunctionArrayName))
      {
      vtkErrorMacro(<< "Cost function array with name specified does not exist!");
      return 1;
      }
    }

  if (!this->SeedsBoundaryConditions && !this->PolyDataBoundaryConditions)
    {
    vtkWarningMacro(<<"No boundary conditions activated!");
    return 1;
    }

  if (this->SeedsBoundaryConditions)
    {
    if (!this->Seeds)
      {
      vtkErrorMacro(<<"Seeds not specified!");
      return 1;
      }
    for (i=0; i<this->Seeds->GetNumberOfIds(); i++)
      {
      if ((this->Seeds->GetId(i)<0) || (this->Seeds->GetId(i)>input->GetNumberOfPoints()))
        {
        vtkErrorMacro(<<"Seed id exceeds input number of points!");
        return 1;
        }
      }
    }
  
  if (this->PolyDataBoundaryConditions)
    {
    if (!this->BoundaryPolyData)
      {
      vtkErrorMacro(<<"Boundary poly data not specified!");
      return 1;
      }
    if (!this->IntersectedEdgesArrayName)
      {
      vtkErrorMacro(<<"Intersected edges array name not specified!");
      return 1;
      }
    if (!this->BoundaryPolyData->GetPointData()->GetArray(this->IntersectedEdgesArrayName))
      {
      vtkErrorMacro(<<"Intersected edges array with name specified does not exist!");
      return 1;
      }
    if (!this->UnitSpeed)
      {
      vtkWarningMacro(<<"Initialization can be inaccurate if unit speed is not used for poly data boundary conditions.");
      }
    }

  this->InitPropagation(input);

  this->Propagate(input);

  int naccepted = 0, nconsidered = 0, nfar = 0;
  for (i=0; i<input->GetNumberOfPoints(); i++)
    {
    if (this->TScalars->GetValue(i)>=VTK_VMTK_LARGE_DOUBLE)
      {
      if (this->StatusScalars->GetValue(i) == VTK_VMTK_ACCEPTED_STATUS)
        {
        this->TScalars->SetValue(i,0.0);
        naccepted++;
        }
      if (this->StatusScalars->GetValue(i) == VTK_VMTK_CONSIDERED_STATUS)
        {
        this->TScalars->SetValue(i,0.0);
        nconsidered++;
        }
      if (this->StatusScalars->GetValue(i) == VTK_VMTK_FAR_STATUS)
        {
        this->TScalars->SetValue(i,0.0);
        nfar++;
        }
      }
    }

  if (naccepted||nconsidered||nfar)
    {
    //     vtkWarningMacro(<<"Unvisited points found: "<<naccepted<<" accepted, "<<nconsidered<<" considered, "<<nfar<<" far.");
    }

  if (this->SolutionArrayName)
    {
    strcpy(eikonalSolutionArrayName,this->SolutionArrayName);
    }
  else
    {
    strcpy(eikonalSolutionArrayName,"EikonalSolution");
    }

  this->TScalars->SetName(eikonalSolutionArrayName);

  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
  output->GetPointData()->AddArray(this->TScalars);
  output->GetPointData()->SetActiveScalars(eikonalSolutionArrayName);

  return 1;
}

void vtkvmtkNonManifoldFastMarching::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
