/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataNetworkExtraction.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataNetworkExtraction.h"
#include "vtkCleanPolyData.h"
#include "vtkStripper.h"
#include "vtkAppendPolyData.h"
#include "vtkFeatureEdges.h"
#include "vtkMath.h"
#include "vtkPolyDataCollection.h"
#include "vtkCollection.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"
#include "vtkLine.h"
#include "vtkCommand.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataNetworkExtraction);

vtkvmtkPolyDataNetworkExtraction::vtkvmtkPolyDataNetworkExtraction()
{
  this->MarksArrayName = new char[256];
  strcpy(this->MarksArrayName,"Marks");
  this->RadiusArrayName = new char[256];
  strcpy(this->RadiusArrayName,"Radius");
  this->TopologyArrayName = new char[256];
  strcpy(this->TopologyArrayName,"Topology");

  this->MinimumStep = 5E-4;
  this->TotalMarkedPoints = 0;

  this->GraphLayout = NULL;
}

vtkvmtkPolyDataNetworkExtraction::~vtkvmtkPolyDataNetworkExtraction()
{
  if (this->MarksArrayName)
    {
    delete[] this->MarksArrayName;
    this->MarksArrayName = NULL;
    }
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }
  if (this->TopologyArrayName)
    {
    delete[] this->TopologyArrayName;
    this->TopologyArrayName = NULL;
    }
  if (this->GraphLayout)
    {
    this->GraphLayout->Delete();
    this->GraphLayout = NULL;
    }
}

void vtkvmtkPolyDataNetworkExtraction::BoundaryExtractor(vtkPolyData* polyInput, vtkPolyData* boundary)
{
  if (polyInput->GetNumberOfCells()==0) 
    {
    return;
    }

  vtkFeatureEdges *boundaryExtractor = vtkFeatureEdges::New();
  boundaryExtractor->BoundaryEdgesOn();
  boundaryExtractor->FeatureEdgesOff();
  boundaryExtractor->NonManifoldEdgesOff();
  boundaryExtractor->ManifoldEdgesOff();
  boundaryExtractor->SetInputData(polyInput);
  boundaryExtractor->Update();

  if (boundaryExtractor->GetOutput()->GetNumberOfCells()==0) 
    {
    boundaryExtractor->Delete();
    return;
    }

  vtkCleanPolyData *boundaryExtractorCleaner = vtkCleanPolyData::New();
  boundaryExtractorCleaner->SetInputConnection(boundaryExtractor->GetOutputPort());

  vtkStripper *boundaryExtractorStripper = vtkStripper::New();
  boundaryExtractorStripper->SetInputConnection(boundaryExtractorCleaner->GetOutputPort());
  boundaryExtractorStripper->Update();

  boundary->DeepCopy(boundaryExtractorStripper->GetOutput());

  boundaryExtractorStripper->Delete();
  boundaryExtractorCleaner->Delete();
  boundaryExtractor->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::BoundarySeparator(vtkPolyData* appendedBoundaries, vtkPolyDataCollection* boundaries)
{
  appendedBoundaries->BuildLinks();

  vtkIdType i;
  for (i=0; i<appendedBoundaries->GetNumberOfCells(); i++)
    {
    vtkPoints* boundaryPoints = vtkPoints::New();
    vtkCellArray* boundaryCellArray = vtkCellArray::New();
    vtkIdType npts;
    const vtkIdType *pts;
    appendedBoundaries->GetCellPoints(i,npts,pts);
    boundaryCellArray->InsertNextCell(npts+1);
    vtkIdType j;
    for (j=0; j<npts; j++)
      {
      boundaryPoints->InsertNextPoint(appendedBoundaries->GetPoint(pts[j]));
      boundaryCellArray->InsertCellPoint(j);
      }
    boundaryCellArray->InsertCellPoint(0);
    vtkPolyData* boundary = vtkPolyData::New();
    boundary->SetPoints(boundaryPoints);
    boundary->SetLines(boundaryCellArray);
    boundaries->AddItem(boundary);
    boundary->Delete();
    boundaryPoints->Delete();
    boundaryCellArray->Delete();
    }
}

void vtkvmtkPolyDataNetworkExtraction::InsertInEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType pointId0, vtkIdType pointId1)
{
  vtkIdType edge[2];
  edge[0] = pointId0;
  edge[1] = pointId1;
  edgeTable->InsertNextTypedTuple(edge);
}

bool vtkvmtkPolyDataNetworkExtraction::InsertUniqueInEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType pointId0, vtkIdType pointId1)
{
  vtkIdType edge[2];
  edge[0] = pointId0;
  edge[1] = pointId1;
  vtkIdType i;
  for (i=0; i<edgeTable->GetNumberOfTuples(); i++)
    {
    vtkIdType currentEdge[2];
    edgeTable->GetTypedTuple(i, currentEdge);
    if (currentEdge[0]==edge[0] && currentEdge[1]==edge[1])
      {
      return false;
      }
    }

  edgeTable->InsertNextTypedTuple(edge);

  return true;
}

void vtkvmtkPolyDataNetworkExtraction::GetFromEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType position, vtkIdType edge[2])
{
  edgeTable->GetTypedTuple(position, edge);
}

void vtkvmtkPolyDataNetworkExtraction::UpdateEdgeTableCollectionReal(vtkPolyData* model,vtkPolyDataCollection* profiles,vtkCollection* edgeTables)
{
  profiles->InitTraversal();
  vtkIdType j;
  for (j=0; j<profiles->GetNumberOfItems(); j++)
    {
    vtkPolyData* profile=profiles->GetNextItem();
    vtkIdTypeArray* edgeTable = vtkIdTypeArray::New();
    edgeTable->SetNumberOfComponents(2);
    vtkIdType i;
    for (i=0; i<profile->GetNumberOfPoints(); i++)
      {
      vtkIdType modelPointId = model->FindPoint(profile->GetPoint(i));
      this->InsertInEdgeTable(edgeTable,modelPointId,modelPointId);
      }
    edgeTables->AddItem(edgeTable);
    edgeTable->Delete();
    }
}

double vtkvmtkPolyDataNetworkExtraction::Distance(double point1[3], double point2[3])
{
  return sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
}

double vtkvmtkPolyDataNetworkExtraction::GetFurthestDistance(vtkPolyDataCollection* polyDataCollection, double fromPoint[3])
{
  vtkIdType i,j;
  double dist, maxDist = 0.0;
  polyDataCollection->InitTraversal();
  for (i=0; i<polyDataCollection->GetNumberOfItems(); i++)
    {
    vtkPolyData* item = polyDataCollection->GetNextItem();
    for (j=0; j<item->GetNumberOfPoints(); j++)
      {
      dist = vtkMath::Distance2BetweenPoints(item->GetPoint(j),fromPoint);
      if (dist > maxDist)
        {
        maxDist = dist;
        }
      }
    }

  return sqrt(maxDist);
}

void vtkvmtkPolyDataNetworkExtraction::Barycenter(vtkPoints* points, double barycenter[3])
{
  vtkIdType numberOfPoints = points->GetNumberOfPoints();
  vtkIdType i,j;
  for (i=0; i<3; i++)
    {
    double sum = 0.0;
    for (j=0; j<numberOfPoints; j++)
      {
      sum = sum + ((points->GetPoint(j))[i]);
      }
    barycenter[i] = sum / numberOfPoints;
    }
}

void vtkvmtkPolyDataNetworkExtraction::ProfileBarycenter(vtkPoints* points, double barycenter[3])
{
  vtkIdType numberOfPoints = points->GetNumberOfPoints();

  if (numberOfPoints == 0)
    {
    barycenter[0] = barycenter[1] = barycenter[2] = 0.0;
    return;
    }

  vtkDoubleArray* lineLengths = vtkDoubleArray::New();
  double totalLength = 0.0;
  vtkIdType i;
  for (i=0; i<numberOfPoints; i++)
    {
    double point0[3], point1[3];
    points->GetPoint(i,point0);
    if (i < numberOfPoints-1)
      {
      points->GetPoint(i+1,point1);
      }
    else
      {
      points->GetPoint(0,point1);
      }
    double lineLength = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
    lineLengths->InsertNextValue(lineLength);
    totalLength += lineLength;
    }

  if (totalLength == 0.0)
    {
    points->GetPoint(0,barycenter);
    lineLengths->Delete();
    return;
    }

  vtkDoubleArray* pointWeights = vtkDoubleArray::New();
  for (i=0; i<numberOfPoints; i++)
    {
    double length0 = 0.0;
    if (i != 0)
      {
      length0 = lineLengths->GetValue(i-1);
      }
    else
      {
      length0 = lineLengths->GetValue(numberOfPoints-1);
      }
    double length1 = lineLengths->GetValue(i);
    double pointWeight = (length0 + length1) / 2.0;
    pointWeights->InsertNextValue(pointWeight);
    }
  
  for (i=0; i<3; i++)
    {
    double sum = 0.0;
    vtkIdType j;
    for (j=0; j<numberOfPoints; j++)
      {
      double pointCoord = points->GetPoint(j)[i];
      double weight = pointWeights->GetValue(j);
      sum += pointCoord * weight;
      }
    barycenter[i] = sum / totalLength;
    }

  lineLengths->Delete();
  pointWeights->Delete();    
}

void vtkvmtkPolyDataNetworkExtraction::DefineVirtualSphere(vtkPolyDataCollection* baseProfiles, double center[3], double &radius, double ratio)
{
  vtkPoints* barycenters = vtkPoints::New();
  baseProfiles->InitTraversal();
  vtkIdType i;
  for (i=0; i<baseProfiles->GetNumberOfItems(); i++)
    {
    vtkPolyData* baseProfile = baseProfiles->GetNextItem();
    double barycenter[3];
    this->ProfileBarycenter(baseProfile->GetPoints(),barycenter);
    barycenters->InsertNextPoint(barycenter);
    }
  this->Barycenter(barycenters,center);
  barycenters->Delete();
  double distance = this->GetFurthestDistance(baseProfiles,center);
  radius = distance * ratio;
}

vtkIdType vtkvmtkPolyDataNetworkExtraction::CurrentPointId(vtkPolyData* model,vtkIdType currentEdge[2])
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));

  vtkIdType currentPointId;
  if (currentEdge[0] != currentEdge[1])
    {
    if (marksArray->GetValue(currentEdge[1]) == NON_VISITED)
      {
      currentPointId = currentEdge[1];
      }
    else if (marksArray->GetValue(currentEdge[0]) == NON_VISITED)
      {
      currentPointId = currentEdge[0];
      }
    else 
      {
      currentPointId = -1;
      }
    }
  else
    {
    currentPointId = currentEdge[0];
    }
  
  return currentPointId;
}

void vtkvmtkPolyDataNetworkExtraction::InsertEdgeForNewProfiles(vtkPolyData* model, vtkIdType* edge, vtkIdTypeArray* edgeTable, vtkIdTypeArray* cellPairs, vtkIdList* pointIds)
{
  if (this->InsertUniqueInEdgeTable(edgeTable,edge[0],edge[1]))
    {
    vtkIdType ncells0;
    vtkIdType *cell0;
    model->GetPointCells(edge[0],ncells0,cell0);
    vtkIdType ncells1;
    vtkIdType *cell1;
    model->GetPointCells(edge[1],ncells1,cell1);

    vtkIdType cellPair[2];
    cellPair[0] = -1;
    cellPair[1] = -1;
    vtkIdType i,j;
    for (i=0; i<ncells0; i++)
      {
      for (j=0; j<ncells1; j++)
        {
        if (cell0[i]==cell1[j])
          {
          if (cellPair[0]==-1)
            {
            cellPair[0]=cell0[i];
            }
          else
            {
            cellPair[1]=cell0[i];
            }
          }
        }
      }

    this->InsertInEdgeTable(cellPairs,cellPair[0],cellPair[1]);  

    pointIds->InsertUniqueId(edge[0]);
    pointIds->InsertUniqueId(edge[1]);
    }
}


bool vtkvmtkPolyDataNetworkExtraction::LookForNeighbors(vtkPolyData* model, vtkIdType pointId, vtkIdList* notVisitedIds, vtkIdTypeArray* edgeTableForIncludedGlobalProfiles)
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));

  bool someNeighborsFound = false;
  // return non visited neighbors, and true if there are any, false otherwise
  vtkIdType ncells;
  vtkIdType *cell;
  model->GetPointCells(pointId,ncells,cell);
  vtkIdType i;
  for (i=0; i<ncells; i++)
    {
    vtkIdType npts;
    const vtkIdType *pts;
    model->GetCellPoints(cell[i],npts,pts);
    vtkIdType j;
    for (j=0; j<npts; j++)
      {
      if (pts[j]!=pointId)
        {
        vtkIdType scalar = marksArray->GetValue(pts[j]);
        if (scalar == NON_VISITED)
          {
          notVisitedIds->InsertUniqueId(pts[j]);
          someNeighborsFound=true;
          }
        else if (scalar == GLOBAL)
          {
          this->InsertInEdgeTable(edgeTableForIncludedGlobalProfiles,pointId,pts[j]);
          }
        }
      }
    }

  return someNeighborsFound;
}

void vtkvmtkPolyDataNetworkExtraction::PropagateFromBaseProfilePoint(vtkPolyData* model, vtkIdList* toVisitPointIds, double center[3], double radius, vtkIdTypeArray* edgeTableForNewProfiles, vtkIdTypeArray* cellPairsForNewProfiles, vtkIdList* pointIdsForNewProfiles, vtkPoints* markedPoints, vtkIdList* markedPointIds, vtkIdTypeArray* edgeTableForIncludedGlobalProfiles)
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));

  while (toVisitPointIds->GetNumberOfIds())
    {
    vtkIdType currentPointId = toVisitPointIds->GetId(0);
    if (marksArray->GetValue(currentPointId) == NON_VISITED)
      {
      marksArray->InsertValue(currentPointId,VISITED);
      if (markedPointIds->IsId(currentPointId)==-1)
        {
        double markedPoint[3];
        model->GetPoint(currentPointId,markedPoint);
        markedPoints->InsertNextPoint(markedPoint);
        markedPointIds->InsertNextId(currentPointId);
        }
      }
   
    // call LookForNeighbors, compare distances and call PropagateFromBaseProfilePovtkIdType again is povtkIdType 
    // is non visited and falls inside the sphere. If it falls outside add adge to edge list
    vtkIdList* notVisitedNeighbors = vtkIdList::New();
    if (LookForNeighbors(model,currentPointId,notVisitedNeighbors,edgeTableForIncludedGlobalProfiles))
      {
      marksArray->InsertValue(currentPointId,VISITED);
      vtkIdType edge[2];
      vtkIdType i;
      for (i=0; i<notVisitedNeighbors->GetNumberOfIds(); i++)
        {
        double point[3];
        model->GetPoint(notVisitedNeighbors->GetId(i),point);
        if (this->Distance(center,point) > radius)
          {
          edge[0]=currentPointId; edge[1]=notVisitedNeighbors->GetId(i);
          this->InsertEdgeForNewProfiles(model,edge,edgeTableForNewProfiles,cellPairsForNewProfiles,pointIdsForNewProfiles);
          }
        else
          {
          toVisitPointIds->InsertUniqueId(notVisitedNeighbors->GetId(i));
          }
        }
      }
    toVisitPointIds->DeleteId(currentPointId);
    notVisitedNeighbors->Delete();
    }
}

void vtkvmtkPolyDataNetworkExtraction::LocateVirtualPoint(vtkIdType edge[2], double center[3], double radius, vtkIdList* pointIdsForNewProfiles, vtkPoints* pointsForNewProfiles, vtkDoubleArray* pointDistancesForNewProfiles, double virtualPoint[3])
{
  virtualPoint[0] = virtualPoint[1] = virtualPoint[2] = 0.0;

  vtkIdType position0 = pointIdsForNewProfiles->IsId(edge[0]);
  vtkIdType position1 = pointIdsForNewProfiles->IsId(edge[1]);

  double point0[3], point1[3];
  pointsForNewProfiles->GetPoint(position0,point0);
  pointsForNewProfiles->GetPoint(position1,point1);

  double distance0 = pointDistancesForNewProfiles->GetValue(position0);
  double distance1 = pointDistancesForNewProfiles->GetValue(position1);

  double distance2 = vtkMath::Distance2BetweenPoints(point0,point1);

  if (radius > 0.0 && distance2 > 1E-12)
    {
    double h2 = vtkLine::DistanceToLine(center,point0,point1);
   
    double distance = sqrt(distance2);
   
    double b = 0.0;
    if (radius*radius - h2 > 0.0)
      {
      b = sqrt(radius*radius - h2);
      }
    
    double a = 0.0;
    if (distance0*distance0 - h2 > 0.0)
      {
      a = sqrt(distance0*distance0 - h2);
      }
    
    double parametricCoordinate = 0.0;
    if (distance2 < (distance1*distance1 - h2))
      {
      parametricCoordinate = b - a;
      }
    else
      {
      parametricCoordinate = b + a;
      }
   
    virtualPoint[0] = point0[0] + parametricCoordinate * (point1[0]-point0[0]) / distance;
    virtualPoint[1] = point0[1] + parametricCoordinate * (point1[1]-point0[1]) / distance;
    virtualPoint[2] = point0[2] + parametricCoordinate * (point1[2]-point0[2]) / distance;
    }
  else
    {
    if (distance0<distance1)
      {
      virtualPoint[0] = point0[0];
      virtualPoint[1] = point0[1];
      virtualPoint[2] = point0[2];
      }
    else
      {
      virtualPoint[0] = point1[0];
      virtualPoint[1] = point1[1];
      virtualPoint[2] = point1[2];
      }
    }
}

void vtkvmtkPolyDataNetworkExtraction::ReconstructNewProfiles(vtkPoints* virtualPoints, vtkIdTypeArray* edgeTable, vtkIdTypeArray* cellPairs, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables)
{
  // reconstruct using adjacent triangles
  vtkIdList* notVisitedIds = vtkIdList::New();
  vtkIdType i;
  for (i=0; i<virtualPoints->GetNumberOfPoints(); i++)
    {
    notVisitedIds->InsertNextId(i);
    }

  while (notVisitedIds->GetNumberOfIds())
    {
    vtkPolyData* newProfile = vtkPolyData::New();
    vtkPoints* newProfilePoints = vtkPoints::New();
    newProfile->SetPoints(newProfilePoints);
    
    vtkIdTypeArray* newProfileEdgeTable = vtkIdTypeArray::New();
    newProfileEdgeTable->SetNumberOfComponents(2);
    vtkIdType cellPair0[2];
    vtkIdType position1 = notVisitedIds->GetId(0);
    this->GetFromEdgeTable(cellPairs,position1,cellPair0);
    vtkIdType initialCellId = cellPair0[0];
    vtkIdType cellIdToSearch = cellPair0[1];
    newProfile->GetPoints()->InsertNextPoint(virtualPoints->GetPoint(position1));
    vtkIdType newProfileEdge[2];
    this->GetFromEdgeTable(edgeTable,position1,newProfileEdge);
    notVisitedIds->DeleteId(position1);
    this->InsertInEdgeTable(newProfileEdgeTable,newProfileEdge[0],newProfileEdge[1]);

    bool closed = false;
    vtkIdType cellPair1[2];
    while (!closed)
      {
      bool foundNext = false;
      vtkIdType j = 0;
      while (!foundNext)
        {
        if (j >= notVisitedIds->GetNumberOfIds())
          {
          vtkErrorMacro(<<"Error: can't reconstruct new profile.");
          return;
          }

        vtkIdType position2 = notVisitedIds->GetId(j);
        this->GetFromEdgeTable(cellPairs,position2,cellPair1);
        if (cellPair1[0] == cellIdToSearch)
          {
          double virtualPoint[3];
          virtualPoints->GetPoint(position2,virtualPoint);
          newProfile->GetPoints()->InsertNextPoint(virtualPoint);
          this->GetFromEdgeTable(edgeTable,position2,newProfileEdge);
          this->InsertInEdgeTable(newProfileEdgeTable,newProfileEdge[0],newProfileEdge[1]);
          foundNext = true;
          notVisitedIds->DeleteId(position2);
          cellIdToSearch = cellPair1[1];
          }
        else if (cellPair1[1] == cellIdToSearch)
          {
          double virtualPoint[3];
          virtualPoints->GetPoint(position2,virtualPoint);
          newProfile->GetPoints()->InsertNextPoint(virtualPoint);
          this->GetFromEdgeTable(edgeTable,position2,newProfileEdge);
          this->InsertInEdgeTable(newProfileEdgeTable,newProfileEdge[0],newProfileEdge[1]);
          foundNext = true;
          notVisitedIds->DeleteId(position2);
          cellIdToSearch = cellPair1[0];
          }
        j++;
        }

      if (cellIdToSearch == initialCellId)
        {
        closed = true;
        }
      }

    vtkCellArray* newProfileLines = vtkCellArray::New();
    newProfileLines->InsertNextCell(newProfile->GetNumberOfPoints() + 1);
    vtkIdType i;
    for (i=0; i<newProfile->GetNumberOfPoints(); i++)
      {
      newProfileLines->InsertCellPoint(i);
      }
    newProfileLines->InsertCellPoint(0);
    newProfile->SetLines(newProfileLines);
    newProfiles->AddItem(newProfile);
    newProfilesEdgeTables->AddItem(newProfileEdgeTable);
    newProfile->Delete();
    newProfilePoints->Delete();
    newProfileLines->Delete();
    newProfileEdgeTable->Delete();
    }
  notVisitedIds->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::GenerateNewProfiles(vtkPolyData* model, double center[3], double radius, vtkIdTypeArray* edgeTableForNewProfiles, vtkIdTypeArray* cellPairsForNewProfiles, vtkIdList* pointIdsForNewProfiles, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables)
{
  if (edgeTableForNewProfiles->GetNumberOfTuples())
    {
    vtkPoints* pointsForNewProfiles = vtkPoints::New();
    vtkDoubleArray* pointDistancesForNewProfiles = vtkDoubleArray::New();

    vtkIdType i;
    for (i=0; i<pointIdsForNewProfiles->GetNumberOfIds(); i++)
      {
      double point[3];
      model->GetPoint(pointIdsForNewProfiles->GetId(i),point);
      pointsForNewProfiles->InsertNextPoint(point);
      pointDistancesForNewProfiles->InsertNextValue(Distance(center,point));
      }

    vtkPoints* virtualPointsForNewProfiles = vtkPoints::New();
    vtkIdType edge[2];
    for (i=0; i<edgeTableForNewProfiles->GetNumberOfTuples(); i++)
      {
      this->GetFromEdgeTable(edgeTableForNewProfiles,i,edge);
      double virtualPoint[3];
      this->LocateVirtualPoint(edge,center,radius,pointIdsForNewProfiles,pointsForNewProfiles,pointDistancesForNewProfiles,virtualPoint);
      virtualPointsForNewProfiles->InsertNextPoint(virtualPoint);
      }

    this->ReconstructNewProfiles(virtualPointsForNewProfiles,edgeTableForNewProfiles,cellPairsForNewProfiles,newProfiles,newProfilesEdgeTables);

    pointsForNewProfiles->Delete();
    pointDistancesForNewProfiles->Delete();
    virtualPointsForNewProfiles->Delete();
    }
}

void vtkvmtkPolyDataNetworkExtraction::UnmarkPoints(vtkPolyData* model, vtkIdList* markedPointIds)
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));
  vtkIdType i;
  for (i=0; i<markedPointIds->GetNumberOfIds(); i++)
    {
    marksArray->InsertValue(markedPointIds->GetId(i),NON_VISITED);
    }
}

double vtkvmtkPolyDataNetworkExtraction::ComputeStepRadius(vtkPoints* points, double point1[3], double point2[3])
{
  vtkIdType i;
  double radius = 0.0;
  for (i=0; i<points->GetNumberOfPoints(); i++)
    {
    double point[3];
    points->GetPoint(i,point);
    radius += sqrt(vtkLine::DistanceToLine(point,point1,point2));
    }
  radius /= points->GetNumberOfPoints();
  return radius;
}

double vtkvmtkPolyDataNetworkExtraction::ComputeMeanRadius(vtkPoints* points, double point1[3])
{
  vtkIdType i;
  double radius = 0.0;
  for (i=0; i<points->GetNumberOfPoints(); i++)
    {
    double point[3];
    points->GetPoint(i,point);
    radius += sqrt(vtkMath::Distance2BetweenPoints(point,point1));
    }
  radius /= points->GetNumberOfPoints();
  return radius;
}


void vtkvmtkPolyDataNetworkExtraction::PointsForRadius(vtkPoints *markedPoints, vtkPolyDataCollection *baseProfiles, vtkPolyDataCollection *newProfiles, vtkPoints *pointsForRadius)
{
  vtkIdType i,j;
  if (markedPoints)
    {
    for (i=0; i<markedPoints->GetNumberOfPoints(); i++)
      {
      pointsForRadius->InsertNextPoint(markedPoints->GetPoint(i));
      }
    }

  if (baseProfiles)
    {
    baseProfiles->InitTraversal();
    for (i=0; i<baseProfiles->GetNumberOfItems(); i++)
      {
      vtkPoints *baseProfilePoints = baseProfiles->GetNextItem()->GetPoints();
      for (j=0; j<baseProfilePoints->GetNumberOfPoints(); j++)
        {
        pointsForRadius->InsertNextPoint(baseProfilePoints->GetPoint(j));
        }
      }
    }

  if (newProfiles)
    {
    newProfiles->InitTraversal();
    for (i=0; i<newProfiles->GetNumberOfItems(); i++)
      {
      vtkPoints *newProfilePoints = newProfiles->GetNextItem()->GetPoints();
      for (j=0; j<newProfilePoints->GetNumberOfPoints(); j++)
        {
        pointsForRadius->InsertNextPoint(newProfilePoints->GetPoint(j));
        }
      }
    }
}

void vtkvmtkPolyDataNetworkExtraction::LookForIntersectingPoint(vtkPoints* segmentPoints, double center[3], double radius, vtkIdType &intersectingPointId)
{
  intersectingPointId = segmentPoints->GetNumberOfPoints()-1;
  vtkIdType i;
  for (i=segmentPoints->GetNumberOfPoints()-2; i>=0; i--)
    {
    double segmentPoint[3];
    segmentPoints->GetPoint(i,segmentPoint);
    if (this->Distance(center,segmentPoint)>radius)
      {
      intersectingPointId = i+1;
      break;
      }
    }
}

vtkIdType vtkvmtkPolyDataNetworkExtraction::StepIteration(vtkPolyData* model, vtkPolyDataCollection* baseProfiles, vtkCollection* baseProfilesEdgeTables, vtkPolyDataCollection* globalProfiles, vtkCollection* globalProfilesEdgeTables, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables, vtkPoints* segmentPoints, vtkDoubleArray* segmentRadii, vtkPoints* bifurcationPoints, vtkDoubleArray* bifurcationRadii, double oldCenter[3], double &oldRadius, double advancementRatio)
{ 
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));
  
  double center[3];
  double radius;
  this->DefineVirtualSphere(baseProfiles,center,radius,advancementRatio);

  bool sameCenter = false;
  if (oldRadius > 0.0)
    {
    if (this->Distance(center,oldCenter) < this->MinimumStep && radius <= oldRadius)
      {
      radius = 2.0 * oldRadius;
      sameCenter = true;
      }
    }

  // now propagate and build virtual profile(s)
  vtkIdTypeArray* edgeTableForNewProfiles = vtkIdTypeArray::New();
  edgeTableForNewProfiles->SetNumberOfComponents(2);
  vtkIdTypeArray* cellPairsForNewProfiles = vtkIdTypeArray::New();
  cellPairsForNewProfiles->SetNumberOfComponents(2);
  vtkIdList* pointIdsForNewProfiles = vtkIdList::New();  //for doubleScalars mapping

  vtkIdTypeArray* edgeTableForIncludedGlobalProfiles = vtkIdTypeArray::New();
  edgeTableForIncludedGlobalProfiles->SetNumberOfComponents(2);

  vtkPoints* markedPoints = vtkPoints::New();
  vtkIdList* markedPointIds = vtkIdList::New();

  vtkIdType i;
  baseProfiles->InitTraversal();
  baseProfilesEdgeTables->InitTraversal();
  vtkIdList* old2MarkedPointIds = vtkIdList::New();
  for (i=0; i<baseProfiles->GetNumberOfItems(); i++)
    {
    vtkPolyData* baseProfile = baseProfiles->GetNextItem();
    vtkIdTypeArray* baseProfileEdgeTable = (vtkIdTypeArray*)baseProfilesEdgeTables->GetNextItemAsObject();
    vtkIdType j;
    for (j=0; j<baseProfile->GetNumberOfPoints(); j++)
      {
      vtkIdType currentEdge[2];
      this->GetFromEdgeTable(baseProfileEdgeTable,j,currentEdge);
      if (marksArray->GetValue(currentEdge[0]) == GLOBAL)
        {
        marksArray->InsertValue(currentEdge[0],VISITED);
        old2MarkedPointIds->InsertNextId(currentEdge[0]);
        }
      }
    }

  // propagate from base profiles
  baseProfiles->InitTraversal();
  baseProfilesEdgeTables->InitTraversal();

  vtkIdList* toVisitPointIds = vtkIdList::New();
  for (i=0; i<baseProfiles->GetNumberOfItems(); i++)
    {
    vtkPolyData* baseProfile = baseProfiles->GetNextItem();
    vtkIdTypeArray* baseProfileEdgeTable = (vtkIdTypeArray*)baseProfilesEdgeTables->GetNextItemAsObject();
    
    vtkIdType j;
    for (j=0; j<baseProfile->GetNumberOfPoints(); j++)
      {
      vtkIdType currentEdge[2];
      this->GetFromEdgeTable(baseProfileEdgeTable,j,currentEdge);
      vtkIdType currentPointId = CurrentPointId(model,currentEdge);

      if (marksArray->GetValue(currentEdge[1]) == GLOBAL)
        {
        this->InsertInEdgeTable(edgeTableForIncludedGlobalProfiles,currentEdge[0],currentEdge[1]);
        }
    
      if (currentPointId!=-1)
        {
        if (this->Distance(model->GetPoint(currentPointId),center)>radius)
          {
          this->InsertEdgeForNewProfiles(model,currentEdge,edgeTableForNewProfiles,cellPairsForNewProfiles,pointIdsForNewProfiles);
          }
        else
          {
          toVisitPointIds->InsertNextId(currentPointId);
          }
        }
      }
    }

  this->PropagateFromBaseProfilePoint(model,toVisitPointIds,center,radius,edgeTableForNewProfiles,cellPairsForNewProfiles,pointIdsForNewProfiles,markedPoints,markedPointIds,edgeTableForIncludedGlobalProfiles);
  
  toVisitPointIds->Delete();

  for (i=0; i<old2MarkedPointIds->GetNumberOfIds(); i++)
    {
    marksArray->InsertValue(old2MarkedPointIds->GetId(i),GLOBAL);
    }

  old2MarkedPointIds->Delete();

  if (edgeTableForIncludedGlobalProfiles->GetNumberOfTuples() > 0)
    {
    vtkIdTypeArray* edgeTableForEffectiveIncludedGlobalProfiles = vtkIdTypeArray::New();
    edgeTableForEffectiveIncludedGlobalProfiles->SetNumberOfComponents(2);

    vtkIdType i;
    for (i=0; i<edgeTableForIncludedGlobalProfiles->GetNumberOfTuples(); i++)
      {
      vtkIdType edgeForIncludedGlobalProfile[2];
      this->GetFromEdgeTable(edgeTableForIncludedGlobalProfiles,i,edgeForIncludedGlobalProfile);
      if (this->Distance(model->GetPoint(edgeForIncludedGlobalProfile[1]),center) <= radius)
        {
        this->InsertInEdgeTable(edgeTableForEffectiveIncludedGlobalProfiles,edgeForIncludedGlobalProfile[0],edgeForIncludedGlobalProfile[1]);
        }
      }

    if (edgeTableForEffectiveIncludedGlobalProfiles->GetNumberOfTuples() > 0)
      {
      vtkIdList* includedGlobalProfilesIds = vtkIdList::New();
      for (i=0; i<edgeTableForEffectiveIncludedGlobalProfiles->GetNumberOfTuples(); i++)
        {
        vtkIdType edgeForIncludedGlobalProfile[2];
        this->GetFromEdgeTable(edgeTableForEffectiveIncludedGlobalProfiles,i,edgeForIncludedGlobalProfile);
        globalProfiles->InitTraversal();
        globalProfilesEdgeTables->InitTraversal();
        vtkIdType j;
        for (j=0; j<globalProfiles->GetNumberOfItems(); j++)
          {
//          vtkPolyData* globalProfile = globalProfiles->GetNextItem();
          vtkIdTypeArray* globalProfileEdgeTable = (vtkIdTypeArray*)globalProfilesEdgeTables->GetNextItemAsObject();
          vtkIdType k;
          for (k=0; k<globalProfileEdgeTable->GetNumberOfTuples(); k++)
            {
            vtkIdType globalProfileEdge[2];
            this->GetFromEdgeTable(globalProfileEdgeTable,k,globalProfileEdge);
            if (globalProfileEdge[0]!=globalProfileEdge[1])
              {
              if ((edgeForIncludedGlobalProfile[0] == globalProfileEdge[0] && edgeForIncludedGlobalProfile[1] == globalProfileEdge[1]) || (edgeForIncludedGlobalProfile[0] == globalProfileEdge[1] && edgeForIncludedGlobalProfile[1] == globalProfileEdge[0]))
                {
                includedGlobalProfilesIds->InsertUniqueId(j);
                }
              }
            else if (globalProfileEdge[0] == globalProfileEdge[1])
              {
              if (edgeForIncludedGlobalProfile[0] == globalProfileEdge[0] || edgeForIncludedGlobalProfile[1] == globalProfileEdge[0])
                {
                includedGlobalProfilesIds->InsertUniqueId(j);
                }
              }
            }
          }
        }

      vtkPolyDataCollection* includedGlobalProfiles = vtkPolyDataCollection::New();
      vtkCollection* includedGlobalProfilesEdgeTables = vtkCollection::New();
      globalProfiles->InitTraversal();
      globalProfilesEdgeTables->InitTraversal();
      for (i=0; i<globalProfiles->GetNumberOfItems(); i++)
        {
        vtkPolyData* globalProfile = globalProfiles->GetNextItem();
        vtkIdTypeArray* globalProfileEdgeTable = (vtkIdTypeArray*)globalProfilesEdgeTables->GetNextItemAsObject();
        if (includedGlobalProfilesIds->IsId(i) != -1)
          {
          includedGlobalProfiles->AddItem(globalProfile);
          baseProfiles->AddItem(globalProfile);
          includedGlobalProfilesEdgeTables->AddItem(globalProfileEdgeTable);
          baseProfilesEdgeTables->AddItem(globalProfileEdgeTable);
          }
        }
      
      includedGlobalProfiles->InitTraversal();
      includedGlobalProfilesEdgeTables->InitTraversal();
      for (i=0; i<includedGlobalProfiles->GetNumberOfItems(); i++)
        {
        globalProfiles->RemoveItem(includedGlobalProfiles->GetNextItem());
        globalProfilesEdgeTables->RemoveItem(includedGlobalProfilesEdgeTables->GetNextItemAsObject());
        }

      this->UnmarkPoints(model,markedPointIds);

      edgeTableForNewProfiles->Delete();
      cellPairsForNewProfiles->Delete();
      pointIdsForNewProfiles->Delete();
      edgeTableForIncludedGlobalProfiles->Delete();
      markedPoints->Delete();
      markedPointIds->Delete();
      edgeTableForEffectiveIncludedGlobalProfiles->Delete();

      includedGlobalProfilesIds->Delete();

      includedGlobalProfiles->Delete();
      includedGlobalProfilesEdgeTables->Delete();
      return STEP_ITERATION_REDEFINE;    
      }
    else
      {
      for (i=0; i<edgeTableForIncludedGlobalProfiles->GetNumberOfTuples(); i++)
        {
        vtkIdType currentEdge[2];
        this->GetFromEdgeTable(edgeTableForIncludedGlobalProfiles,i,currentEdge);
        this->InsertEdgeForNewProfiles(model,currentEdge,edgeTableForNewProfiles,cellPairsForNewProfiles,pointIdsForNewProfiles);
        }
      }
    edgeTableForEffectiveIncludedGlobalProfiles->Delete();
    }

  // build new profiles from lists (virtual points and connectivity)
  this->GenerateNewProfiles(model,center,radius,edgeTableForNewProfiles,cellPairsForNewProfiles,pointIdsForNewProfiles,newProfiles,newProfilesEdgeTables);

  this->TotalMarkedPoints += markedPointIds->GetNumberOfIds();

  // classify step counting base and new profiles
  vtkIdType numberOfBaseProfiles = baseProfiles->GetNumberOfItems();
  vtkIdType numberOfNewProfiles = newProfiles->GetNumberOfItems();

  if (numberOfBaseProfiles == 1 && numberOfNewProfiles == 0)
    {
    segmentPoints->InsertNextPoint(center);
    edgeTableForNewProfiles->Delete();
    cellPairsForNewProfiles->Delete();
    pointIdsForNewProfiles->Delete();
    edgeTableForIncludedGlobalProfiles->Delete();
    markedPoints->Delete();
    markedPointIds->Delete();
    
    return STEP_ITERATION_STOP_CLOSED;
    }
  else if (numberOfBaseProfiles == 2 && numberOfNewProfiles == 0)
    {
    baseProfiles->InitTraversal();
    vtkPolyData* profile;
    profile = baseProfiles->GetNextItem();
    double point1[3];
    this->ProfileBarycenter(profile->GetPoints(),point1);
    segmentPoints->InsertNextPoint(point1);
    profile = baseProfiles->GetNextItem();
    double point2[3];
    this->ProfileBarycenter(profile->GetPoints(),point2);
    segmentPoints->InsertNextPoint(point2);

    vtkPoints *pointsForRadius = vtkPoints::New();
    this->PointsForRadius(markedPoints,baseProfiles,NULL,pointsForRadius);
    segmentRadii->InsertNextValue(ComputeStepRadius(pointsForRadius,point1,point2));

    pointsForRadius->Delete();

    edgeTableForNewProfiles->Delete();
    cellPairsForNewProfiles->Delete();
    pointIdsForNewProfiles->Delete();
    edgeTableForIncludedGlobalProfiles->Delete();
    markedPoints->Delete();
    markedPointIds->Delete();
    
    return STEP_ITERATION_STOP_END;
    }
  else if (numberOfBaseProfiles + numberOfNewProfiles > 2)
    {
    double lastCenter[3];
    if (numberOfBaseProfiles==1)
      {
      lastCenter[0] = center[0];
      lastCenter[1] = center[1];
      lastCenter[2] = center[2];
      }
    else
      {
      baseProfiles->InitTraversal();
      vtkPolyData* profile;
      profile = baseProfiles->GetNextItem();
      this->ProfileBarycenter(profile->GetPoints(),lastCenter);
      }

    segmentPoints->InsertNextPoint(lastCenter);

    double firstBranchRadius = 0.0;
    vtkIdType intersectingPointId = -1;
    if (segmentRadii->GetNumberOfTuples() > 0)
      {
      this->LookForIntersectingPoint(segmentPoints,center,radius,intersectingPointId);
  
      vtkIdType i;
      vtkPoints* backupSegmentPoints = vtkPoints::New();
      for (i=0; i<segmentPoints->GetNumberOfPoints(); i++)
        {
        backupSegmentPoints->InsertNextPoint(segmentPoints->GetPoint(i));
        }
      vtkDoubleArray* backupSegmentRadii = vtkDoubleArray::New();
      for (i=0; i<segmentRadii->GetNumberOfTuples(); i++)
        {
        backupSegmentRadii->InsertNextValue(segmentRadii->GetValue(i));
        }
    
      segmentPoints->Initialize();
      for (i=0; i<=intersectingPointId; i++)
        {
        segmentPoints->InsertNextPoint(backupSegmentPoints->GetPoint(i));
        }
      segmentRadii->Initialize();
      for (i=0; i<intersectingPointId; i++)
        {
        segmentRadii->InsertNextValue(backupSegmentRadii->GetValue(i));
        }

      firstBranchRadius = backupSegmentRadii->GetValue(intersectingPointId-1);

      backupSegmentPoints->Delete();
      backupSegmentRadii->Delete();
      }

    double bifurcationBarycenter[3];
    bifurcationBarycenter[0]  = center[0];
    bifurcationBarycenter[1]  = center[1];
    bifurcationBarycenter[2]  = center[2];

    bifurcationPoints->InsertNextPoint(bifurcationBarycenter);
    bifurcationRadii->InsertNextValue(0.0);
    
    vtkPolyData* profile;
    baseProfiles->InitTraversal();
    vtkIdType i;
    double bifurcationPoint[3];
    for (i=0; i<baseProfiles->GetNumberOfItems(); i++)
      {
      profile=baseProfiles->GetNextItem();
      if (i==0 && segmentRadii->GetNumberOfTuples())
        {
        bifurcationPoints->InsertNextPoint(segmentPoints->GetPoint(intersectingPointId));
        bifurcationRadii->InsertNextValue(firstBranchRadius);
        }
      else
        {
        this->ProfileBarycenter(profile->GetPoints(),bifurcationPoint);
        bifurcationPoints->InsertNextPoint(bifurcationPoint);
        bifurcationRadii->InsertNextValue(this->ComputeMeanRadius(profile->GetPoints(),bifurcationPoint));
        }
      }
    newProfiles->InitTraversal();
    for (i=0; i<newProfiles->GetNumberOfItems(); i++)
      {
      profile=newProfiles->GetNextItem();
      this->ProfileBarycenter(profile->GetPoints(),bifurcationPoint);
      bifurcationPoints->InsertNextPoint(bifurcationPoint);
      bifurcationRadii->InsertNextValue(this->ComputeMeanRadius(profile->GetPoints(),bifurcationPoint));
      }

    vtkIdType j;
    bifurcationBarycenter[0] = bifurcationBarycenter[1] = bifurcationBarycenter[2] = 0.0;
    double weightSum = 0.0;
    for (i=1; i<bifurcationPoints->GetNumberOfPoints(); i++)
      {
      double bifurcationPoint[3];
      bifurcationPoints->GetPoint(i,bifurcationPoint);
      double bifurcationPointRadius = bifurcationRadii->GetValue(i);
      weightSum+=bifurcationPointRadius;
      for (j=0; j<3; j++)
        {
        bifurcationBarycenter[j] += bifurcationPoint[j] * bifurcationPointRadius;
        }
      }
    for (j=0; j<3; j++)
      {
      bifurcationBarycenter[j] /= weightSum;
      }
    bifurcationPoints->InsertPoint(0,bifurcationBarycenter);

    edgeTableForNewProfiles->Delete();
    cellPairsForNewProfiles->Delete();
    pointIdsForNewProfiles->Delete();
    edgeTableForIncludedGlobalProfiles->Delete();
    markedPoints->Delete();
    markedPointIds->Delete();

    return STEP_ITERATION_STOP_BIFURCATION;
    }

  vtkPoints *pointsForRadius = vtkPoints::New();
  this->PointsForRadius(markedPoints,baseProfiles,newProfiles,pointsForRadius);

  newProfiles->InitTraversal();
  double profileBarycenter[3];
  this->ProfileBarycenter(newProfiles->GetNextItem()->GetPoints(),profileBarycenter);
  double stepRadius = this->ComputeStepRadius(pointsForRadius,center,profileBarycenter);

  pointsForRadius->Delete();

  if (!sameCenter)
    {
    segmentRadii->InsertNextValue(stepRadius);
    segmentPoints->InsertNextPoint(center);
    }
  else
    {
    segmentRadii->InsertValue(segmentRadii->GetNumberOfTuples()-1,stepRadius);
    segmentPoints->InsertPoint(segmentPoints->GetNumberOfPoints()-1,center);
    }

  oldCenter[0] = center[0];
  oldCenter[1] = center[1];
  oldCenter[2] = center[2];
  oldRadius = radius;

  edgeTableForNewProfiles->Delete();
  cellPairsForNewProfiles->Delete();
  pointIdsForNewProfiles->Delete();
  edgeTableForIncludedGlobalProfiles->Delete();
  markedPoints->Delete();
  markedPointIds->Delete();

  return STEP_ITERATION_PROCEED;
}


void vtkvmtkPolyDataNetworkExtraction::MarkModelGlobalProfile(vtkPolyData* model, vtkIdTypeArray* newGlobalProfileEdgeTable)
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));
  vtkIdType i;
  for (i=0; i<newGlobalProfileEdgeTable->GetNumberOfTuples(); i++)
    {
    // the first is currentId, the second is not yet visited
    vtkIdType edge[2];
    this->GetFromEdgeTable(newGlobalProfileEdgeTable,i,edge);
    marksArray->InsertValue(edge[0],GLOBAL);
    }
}

void vtkvmtkPolyDataNetworkExtraction::SegmentTopology(vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, double firstSegmentPoint[3], double lastSegmentPoint[3], double firstPoint[3], double &firstRadius, double lastPoint[3], double &lastRadius, vtkIdType segmentTopology[2])
{
  segmentTopology[0] = segmentTopology[1] = 0;

  bool firstPointFound = false;
  bool lastPointFound = false;
  if (!lastSegmentPoint)
    {
    segmentTopology[1] = -1;
    lastPointFound = true;
    }

  vtkIdType i;
  bifurcations->InitTraversal();
  bifurcationsRadii->InitTraversal();
  for (i=0; i<bifurcations->GetNumberOfItems(); i++)
    {
    vtkPoints* bifurcation = (vtkPoints*)bifurcations->GetNextItemAsObject();
    vtkDoubleArray* bifurcationRadii = (vtkDoubleArray*)bifurcationsRadii->GetNextItemAsObject();
    vtkIdType j;
    for (j=0; j<bifurcation->GetNumberOfPoints(); j++)
      {
      if (i == 0 || (i > 0 && j > 0))
        {
        double point[3];
        bifurcation->GetPoint(j,point);
        if (!firstPointFound)
          {
          if (point[0] == firstSegmentPoint[0] && point[1] == firstSegmentPoint[1] && point[2] == firstSegmentPoint[2])
            {
            segmentTopology[0] = i;
            if (j > 0)
              {
              bifurcation->GetPoint(0,firstPoint);
              firstRadius = bifurcationRadii->GetValue(j);
              }
            firstPointFound = true;
            }
          }
        if (lastSegmentPoint && !lastPointFound)
          {
          if (point[0] == lastSegmentPoint[0] && point[1] == lastSegmentPoint[1] && point[2] == lastSegmentPoint[2])
            {
            segmentTopology[1] = i;
            if (j>0)
              {
              bifurcation->GetPoint(0,lastPoint);
              lastRadius = bifurcationRadii->GetValue(j);
              }
            lastPointFound = true;
            }
          }
        }
      if (firstPointFound && lastPointFound)
        {
        break;
        }
      }
    if (firstPointFound && lastPointFound)
      {
      break;
      }
    }
}

void vtkvmtkPolyDataNetworkExtraction::BuildSegment(vtkPoints* segmentPoints, vtkDoubleArray* segmentRadii, vtkIdType segmentTopology[2], double firstPoint[3], double firstRadius, double lastPoint[3], double lastRadius, const double* centralPoint, vtkPolyData* segment)
{
  vtkPoints* segmentPolyPoints = vtkPoints::New();
  vtkDoubleArray* segmentPolyScalars = vtkDoubleArray::New();
  segmentPolyScalars->SetName(RadiusArrayName);
  vtkIdTypeArray* segmentTopologyArray = vtkIdTypeArray::New();
  segmentTopologyArray->SetName(TopologyArrayName);
  segmentTopologyArray->SetNumberOfComponents(2);
  segmentTopologyArray->InsertNextTypedTuple(segmentTopology);

  // start
  if (segmentTopology[0] > 0)
    {
    segmentPolyPoints->InsertNextPoint(firstPoint);
    segmentPolyScalars->InsertNextValue(firstRadius);
    segmentPolyScalars->InsertNextValue(firstRadius);
    }
  else
    {
    segmentPolyScalars->InsertNextValue(0.0);
    }

  vtkIdType i;
  // mid
  if (segmentPoints)
    {
    for (i=0; i<segmentPoints->GetNumberOfPoints(); i++)
      {
      segmentPolyPoints->InsertNextPoint(segmentPoints->GetPoint(i));
      }
    for (i=0; i<segmentRadii->GetNumberOfTuples(); i++)
      {
      segmentPolyScalars->InsertNextValue(segmentRadii->GetValue(i));
      }
    }
  else
    {
    segmentPolyPoints->InsertNextPoint(centralPoint);
    }

  // end
  if (segmentTopology[1] > 0)
    {
    segmentPolyScalars->InsertNextValue(lastRadius);
    segmentPolyPoints->InsertNextPoint(lastPoint);
    segmentPolyScalars->InsertNextValue(lastRadius);
    }
  else
    {
    segmentPolyScalars->InsertNextValue(0.0);
    }

  vtkCellArray* segmentPolyLine = vtkCellArray::New();
  segmentPolyLine->InsertNextCell(segmentPolyPoints->GetNumberOfPoints());
  vtkIdType j;
  for (j=0; j<segmentPolyPoints->GetNumberOfPoints(); j++)
    {
    segmentPolyLine->InsertCellPoint(j);
    }
  segment->SetPoints(segmentPolyPoints);
  
  segment->GetPointData()->AddArray(segmentPolyScalars);
  segment->GetCellData()->AddArray(segmentTopologyArray);
  segment->SetLines(segmentPolyLine);

  segmentPolyPoints->Delete();
  segmentPolyScalars->Delete();
  segmentPolyLine->Delete();
  segmentTopologyArray->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::InsertNewBifurcation(vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, vtkPoints* bifurcationPoints, vtkDoubleArray* bifurcationRadii, vtkPolyDataCollection* additionalSegments)
{
  vtkIdType i;
  bifurcations->InitTraversal();
  bifurcationsRadii->InitTraversal();
  for (i=0; i<bifurcations->GetNumberOfItems(); i++)
    {
    vtkPoints* currentBifurcationPoints = (vtkPoints*)bifurcations->GetNextItemAsObject();
    vtkDoubleArray* currentBifurcationRadii = (vtkDoubleArray*)bifurcationsRadii->GetNextItemAsObject();
    vtkIdType j;
    for (j=0; j<currentBifurcationPoints->GetNumberOfPoints(); j++)
      {
      if (i == 0 || (i > 0 && j > 0))
      {
        double currentBifurcationPoint[3];
        currentBifurcationPoints->GetPoint(j,currentBifurcationPoint);
        vtkIdType k;
        for (k=0; k<bifurcationPoints->GetNumberOfPoints(); k++)
          {
          double bifurcationPoint[3];
          bifurcationPoints->GetPoint(k,bifurcationPoint);
          if ((currentBifurcationPoint[0]==bifurcationPoint[0]) && (currentBifurcationPoint[1]==bifurcationPoint[1]) && (currentBifurcationPoint[2]==bifurcationPoint[2]))
            {
            vtkIdType additionalSegmentTopology[2];
            additionalSegmentTopology[0] = i;
            additionalSegmentTopology[1] = bifurcations->GetNumberOfItems();
            double currentBifurcationPoint0[3], bifurcationPoint0[3];
            currentBifurcationPoints->GetPoint(0,currentBifurcationPoint0);
            bifurcationPoints->GetPoint(0,bifurcationPoint0);
            vtkPolyData* additionalSegment = vtkPolyData::New();
            this->BuildSegment(NULL,NULL,additionalSegmentTopology,currentBifurcationPoint0,currentBifurcationRadii->GetValue(j),bifurcationPoint0,bifurcationRadii->GetValue(k),bifurcationPoint,additionalSegment);
            additionalSegments->AddItem(additionalSegment);
            additionalSegment->Delete();
            }
          }
        }
      }
    }

  bifurcations->AddItem(bifurcationPoints);
  bifurcationsRadii->AddItem(bifurcationRadii);
}

void vtkvmtkPolyDataNetworkExtraction::SegmentIteration(vtkPolyData* model, vtkPolyData* initialProfile, vtkIdTypeArray* initialProfileEdgeTable, vtkPolyDataCollection* globalProfiles, vtkCollection* globalProfilesEdgeTables, vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, vtkPolyDataCollection* segments, double advancementRatio)
{
  vtkPolyDataCollection* baseProfiles = vtkPolyDataCollection::New();
  baseProfiles->AddItem(initialProfile);
  vtkCollection* baseProfilesEdgeTables = vtkCollection::New();
  baseProfilesEdgeTables->AddItem(initialProfileEdgeTable);

  vtkPolyDataCollection* newProfiles = vtkPolyDataCollection::New();
  vtkCollection* newProfilesEdgeTables = vtkCollection::New();

  vtkPoints* segmentPoints = vtkPoints::New();
  vtkDoubleArray* segmentRadii = vtkDoubleArray::New();
  vtkPoints* bifurcationPoints = vtkPoints::New();
  vtkDoubleArray* bifurcationRadii = vtkDoubleArray::New();

  double oldCenter[3];
  double oldRadius = 0.0;
  vtkIdType stepIterationState = STEP_ITERATION_PROCEED;
  
  while (stepIterationState == STEP_ITERATION_PROCEED || stepIterationState == STEP_ITERATION_REDEFINE)
    {
    stepIterationState = STEP_ITERATION_PROCEED;
    stepIterationState = this->StepIteration(model,baseProfiles,baseProfilesEdgeTables,globalProfiles,globalProfilesEdgeTables,newProfiles,newProfilesEdgeTables,segmentPoints,segmentRadii,bifurcationPoints,bifurcationRadii,oldCenter,oldRadius,advancementRatio);

    // avoid zero steps
    if (stepIterationState == STEP_ITERATION_PROCEED && oldRadius == 0.0)
      {
      stepIterationState = STEP_ITERATION_STOP_CLOSED;
      }

    if (stepIterationState == STEP_ITERATION_PROCEED)
      {
      baseProfiles->RemoveAllItems();
      baseProfilesEdgeTables->RemoveAllItems();
      newProfiles->InitTraversal();
      newProfilesEdgeTables->InitTraversal();
      baseProfiles->AddItem(newProfiles->GetNextItem());
      baseProfilesEdgeTables->AddItem(newProfilesEdgeTables->GetNextItemAsObject());
      newProfiles->RemoveAllItems();
      newProfilesEdgeTables->RemoveAllItems();
      }
    }

  vtkPolyData* segment = vtkPolyData::New();
  vtkPolyDataCollection* additionalSegments = vtkPolyDataCollection::New();
  vtkIdType segmentTopology[2];
  double firstPoint[3], lastPoint[3];
  double firstRadius, lastRadius;
  if (stepIterationState == STEP_ITERATION_STOP_BIFURCATION)
    {
    vtkIdType i;
    newProfiles->InitTraversal();
    newProfilesEdgeTables->InitTraversal();
    for (i=0; i<newProfiles->GetNumberOfItems(); i++)
      {
      globalProfiles->AddItem(newProfiles->GetNextItem());
      vtkIdTypeArray* newGlobalProfileEdgeTable = (vtkIdTypeArray*)newProfilesEdgeTables->GetNextItemAsObject();
      globalProfilesEdgeTables->AddItem(newGlobalProfileEdgeTable);
      this->MarkModelGlobalProfile(model,newGlobalProfileEdgeTable);
      }

    // adjacent check
    this->InsertNewBifurcation(bifurcations,bifurcationsRadii,bifurcationPoints,bifurcationRadii,additionalSegments);

    if (segmentPoints->GetNumberOfPoints()>=2)
      {
      double segmentPoint0[3], segmentPoint1[3];
      segmentPoints->GetPoint(0,segmentPoint0);
      segmentPoints->GetPoint(segmentPoints->GetNumberOfPoints()-1,segmentPoint1);
      this->SegmentTopology(bifurcations,bifurcationsRadii,segmentPoint0,segmentPoint1,firstPoint,firstRadius,lastPoint,lastRadius,segmentTopology);
      this->BuildSegment(segmentPoints,segmentRadii,segmentTopology,firstPoint,firstRadius,lastPoint,lastRadius,NULL,segment);
      segments->AddItem(segment);
      }
    }
  else if (stepIterationState == STEP_ITERATION_STOP_END)
    {
    if (segmentPoints->GetNumberOfPoints() >= 2)
      {
      double segmentPoint0[3], segmentPoint1[3];
      segmentPoints->GetPoint(0,segmentPoint0);
      segmentPoints->GetPoint(segmentPoints->GetNumberOfPoints()-1,segmentPoint1);
      this->SegmentTopology(bifurcations,bifurcationsRadii,segmentPoint0,segmentPoint1,firstPoint,firstRadius,lastPoint,lastRadius,segmentTopology);
      this->BuildSegment(segmentPoints,segmentRadii,segmentTopology,firstPoint,firstRadius,lastPoint,lastRadius,NULL,segment);
      segments->AddItem(segment);
      }
    }
  else if (stepIterationState == STEP_ITERATION_STOP_CLOSED)
    {
    if (segmentPoints->GetNumberOfPoints() >= 2)
      {
      double segmentPoint0[3];
      segmentPoints->GetPoint(0,segmentPoint0);
      this->SegmentTopology(bifurcations,bifurcationsRadii,segmentPoint0,NULL,firstPoint,firstRadius,lastPoint,lastRadius,segmentTopology);
      this->BuildSegment(segmentPoints,segmentRadii,segmentTopology,firstPoint,firstRadius,lastPoint,lastRadius,NULL,segment);
      segments->AddItem(segment);
      }
    }
  segment->Delete();
  
  additionalSegments->InitTraversal();
  vtkIdType k;
  for (k=0; k<additionalSegments->GetNumberOfItems(); k++)
    {
    segments->AddItem(additionalSegments->GetNextItem());
    }

  additionalSegments->Delete();

  baseProfiles->Delete();
  baseProfilesEdgeTables->Delete();
  newProfiles->Delete();
  newProfilesEdgeTables->Delete();
  segmentPoints->Delete();
  segmentRadii->Delete();
  bifurcationPoints->Delete();
  bifurcationRadii->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::JoinSegments (vtkPolyData* segment0, vtkPolyData* segment1, bool first0, bool first1, vtkPolyData* segment)
{
  vtkPoints* segmentPoints = vtkPoints::New();
  vtkDoubleArray* radiusArray = vtkDoubleArray::New();
  radiusArray->SetName(RadiusArrayName);
  vtkIdTypeArray* topologyArray = vtkIdTypeArray::New();
  topologyArray->SetNumberOfComponents(2);
  topologyArray->SetName(TopologyArrayName);

  vtkDoubleArray* segment0RadiusArray = vtkDoubleArray::SafeDownCast(segment0->GetPointData()->GetArray(RadiusArrayName));
  vtkDoubleArray* segment1RadiusArray = vtkDoubleArray::SafeDownCast(segment1->GetPointData()->GetArray(RadiusArrayName));

  vtkIdTypeArray* segment0TopologyArray = vtkIdTypeArray::SafeDownCast(segment0->GetCellData()->GetArray(TopologyArrayName));
  vtkIdTypeArray* segment1TopologyArray = vtkIdTypeArray::SafeDownCast(segment1->GetCellData()->GetArray(TopologyArrayName));

  vtkIdType segmentTopology0[2];
  vtkIdType segmentTopology1[2];

  segment0TopologyArray->GetTypedTuple(0, segmentTopology0);
  segment1TopologyArray->GetTypedTuple(0, segmentTopology1);

  vtkIdType segmentTopology[2];
    
  vtkIdType i;
  if (first0)
    {
    segmentTopology[0] = segmentTopology0[1];
    for (i=segment0->GetNumberOfPoints()-1; i>=1; i--)
      {
      segmentPoints->InsertNextPoint(segment0->GetPoint(i));
      radiusArray->InsertNextValue(segment0RadiusArray->GetValue(i));
      }
    }
  else
    {
    segmentTopology[0] = segmentTopology0[0];
    for (i=0; i<segment0->GetNumberOfPoints()-1; i++)
      {
      segmentPoints->InsertNextPoint(segment0->GetPoint(i));
      radiusArray->InsertNextValue(segment0RadiusArray->GetValue(i));
      }
    }

  if (first1)
    {
    segmentTopology[1] = segmentTopology1[1];
    for (i=1; i<segment1->GetNumberOfPoints(); i++)
      {
      segmentPoints->InsertNextPoint(segment1->GetPoint(i));
      radiusArray->InsertNextValue(segment1RadiusArray->GetValue(i));
      }
    }
  else
    {
    segmentTopology[1] = segmentTopology1[1];
    for (i=segment1->GetNumberOfPoints()-2; i>=0; i--)
      {
      segmentPoints->InsertNextPoint(segment1->GetPoint(i));
      radiusArray->InsertNextValue(segment1RadiusArray->GetValue(i));
      }
    }

  topologyArray->InsertNextTypedTuple(segmentTopology);

  vtkCellArray* segmentCell = vtkCellArray::New();
  segmentCell->InsertNextCell(segmentPoints->GetNumberOfPoints());
  for (i=0; i<segmentPoints->GetNumberOfPoints(); i++)
    {
    segmentCell->InsertCellPoint(i);
    }

  segment->SetPoints(segmentPoints);
  segment->SetLines(segmentCell);
  segment->GetPointData()->AddArray(radiusArray);
  segment->GetCellData()->AddArray(topologyArray);

  radiusArray->Delete();
  topologyArray->Delete();
  segmentPoints->Delete();
  segmentCell->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::RemoveDegenerateBifurcations(vtkPolyDataCollection* segments,vtkCollection* bifurcations)
{
  vtkIdList* realBifurcations = vtkIdList::New();
  realBifurcations->InsertNextId(0);
  vtkIdList* degenerateBifurcations = vtkIdList::New();
  vtkIdTypeArray* bifurcationFrequency = vtkIdTypeArray::New();
  bifurcations->InitTraversal();
  vtkIdType i;
  for (i=0; i<bifurcations->GetNumberOfItems(); i++)
    {
    bifurcationFrequency->InsertNextValue(0);
    }
  segments->InitTraversal();
  for (i=0; i<segments->GetNumberOfItems(); i++)
    {
    vtkPolyData* segment = segments->GetNextItem();
//    vtkDoubleArray* radiusArray = vtkDoubleArray::SafeDownCast(segment->GetPointData()->GetArray(RadiusArrayName));
    vtkIdTypeArray* topologyArray = vtkIdTypeArray::SafeDownCast(segment->GetCellData()->GetArray(TopologyArrayName));
    vtkIdType topology[2];
    topologyArray->GetTypedTuple(0, topology);
    vtkIdType bifurcation0 = topology[0];
    if (bifurcation0 > 0)
      {
      vtkIdType frequency0 = bifurcationFrequency->GetValue(bifurcation0);
      bifurcationFrequency->InsertValue(bifurcation0,frequency0+1);
      }
    
    vtkIdType bifurcation1 = topology[1];
    if (bifurcation1 > 0)
      {
      vtkIdType frequency1 = bifurcationFrequency->GetValue(bifurcation1);
      bifurcationFrequency->InsertValue(bifurcation1,frequency1+1);
      }
    }
  for (i=0; i<bifurcationFrequency->GetNumberOfTuples(); i++)
    {
    if (i > 0)
      {
      if (bifurcationFrequency->GetValue(i) < 3)
        {
        degenerateBifurcations->InsertNextId(i);
        }
      else
        {
        realBifurcations->InsertNextId(i);
        }
      }
    }
  
//  vtkIdType n = degenerateBifurcations->GetNumberOfIds();
  while (degenerateBifurcations->GetNumberOfIds() > 0)
  {
    segments->InitTraversal();
    vtkPolyData* segment0 = NULL,* segment1 = NULL,* currentSegment;
    vtkIdType degenerateId = -1;
    bool first0, first1;
    first0 = first1 = false;
    for (i=0; i<segments->GetNumberOfItems(); i++)
      {
      currentSegment = segments->GetNextItem();
//      vtkDoubleArray* radiusArray = vtkDoubleArray::SafeDownCast(currentSegment->GetPointData()->GetArray(RadiusArrayName));
      vtkIdTypeArray* topologyArray = vtkIdTypeArray::SafeDownCast(currentSegment->GetCellData()->GetArray(TopologyArrayName));
      vtkIdType topology[2];
      topologyArray->GetTypedTuple(0,topology);
      if (!segment0)
        {
        degenerateId = topology[0];
        if (degenerateBifurcations->IsId(degenerateId) != -1)
          {
          segment0 = currentSegment;
          first0 = true;
          degenerateBifurcations->DeleteId(degenerateId);
          }
        if (!segment0)
          {
          degenerateId = topology[1];
          if (degenerateBifurcations->IsId(degenerateId) != -1)
            {
            segment0 = currentSegment;
            first0 = false;
            degenerateBifurcations->DeleteId(degenerateId);
            }
          }
        }
      else if (!segment1)
        {
        if (topology[0] == degenerateId)
          {
          segment1 = currentSegment;
          first1 = true;
          }
        if (topology[1] == degenerateId)
          {
          segment1 = currentSegment;
          first1 = false;
          }
        }
      }
    if (segment0 && segment1)
      {
      segment0->Register((vtkObjectBase*)NULL);
      segment1->Register((vtkObjectBase*)NULL);
      segments->RemoveItem(segment0);
      segments->RemoveItem(segment1);
      vtkPolyData* joinedSegment = vtkPolyData::New();
      this->JoinSegments(segment0,segment1,first0,first1,joinedSegment);
      segments->AddItem(joinedSegment);
      joinedSegment->Delete();
      segment0->UnRegister((vtkObjectBase*)NULL);
      segment1->UnRegister((vtkObjectBase*)NULL);
      }
    }

  // rename bifurcations
  segments->InitTraversal();
  vtkIdType scalar0, scalar1, position0, position1;
  for (i=0; i<segments->GetNumberOfItems(); i++)
    {
    vtkPolyData* currentSegment = segments->GetNextItem();
//    vtkDoubleArray* radiusArray = vtkDoubleArray::SafeDownCast(currentSegment->GetPointData()->GetArray(RadiusArrayName));
    vtkIdTypeArray* topologyArray = vtkIdTypeArray::SafeDownCast(currentSegment->GetCellData()->GetArray(TopologyArrayName));
    vtkIdType topology[2];
    topologyArray->GetTypedTuple(0, topology);
    scalar0 = topology[0];
    scalar1 = topology[1];
    if (scalar0 > 0)
      {
      position0 = realBifurcations->InsertUniqueId(scalar0);
      topology[0] = position0;
      }
    if (scalar1>0)
      {
      position1=realBifurcations->InsertUniqueId(scalar1);
      topology[1] = position1;
      }
    topologyArray->InsertTypedTuple(0,topology);
    }

  realBifurcations->Delete();
  degenerateBifurcations->Delete();
  bifurcationFrequency->Delete();
}


void vtkvmtkPolyDataNetworkExtraction::GlobalIteration(vtkPolyData* model, vtkPolyDataCollection* globalProfiles, vtkPolyData* network, double advancementRatio)
{
  vtkCollection* globalProfilesEdgeTables = vtkCollection::New();
  this->UpdateEdgeTableCollectionReal(model,globalProfiles,globalProfilesEdgeTables);

  vtkCollection* bifurcations = vtkCollection::New();
  vtkCollection* bifurcationsRadii = vtkCollection::New();
  vtkPolyDataCollection* segments = vtkPolyDataCollection::New();

  vtkPoints* globalBoundaryBarycenters = vtkPoints::New();
  vtkDoubleArray* globalBoundaryRadii = vtkDoubleArray::New();
  vtkPolyData* profile;
  vtkIdType i;
  globalProfiles->InitTraversal();
  for (i=0; i<globalProfiles->GetNumberOfItems(); i++)
    {
    profile=globalProfiles->GetNextItem();
    double barycenter[3];
    this->ProfileBarycenter(profile->GetPoints(),barycenter);
    globalBoundaryBarycenters->InsertNextPoint(barycenter);
    globalBoundaryRadii->InsertNextValue(ComputeMeanRadius(profile->GetPoints(),barycenter));
    }
  
  bifurcations->AddItem(globalBoundaryBarycenters);
  bifurcationsRadii->AddItem(globalBoundaryRadii);

  while (globalProfiles->GetNumberOfItems())
    {
    globalProfiles->InitTraversal();
    vtkPolyData* initialProfile = globalProfiles->GetNextItem();
    initialProfile->Register((vtkObjectBase*)NULL);
    globalProfiles->RemoveItem(initialProfile);

    globalProfilesEdgeTables->InitTraversal();
    vtkIdTypeArray* initialProfileEdgeTable = (vtkIdTypeArray*)globalProfilesEdgeTables->GetNextItemAsObject();
    initialProfileEdgeTable->Register((vtkObjectBase*)NULL);
    globalProfilesEdgeTables->RemoveItem(initialProfileEdgeTable);

    this->SegmentIteration(model,initialProfile,initialProfileEdgeTable,globalProfiles,globalProfilesEdgeTables,bifurcations,bifurcationsRadii,segments,advancementRatio);
    
    double progress = (double)this->TotalMarkedPoints/(double)model->GetNumberOfPoints();
    this->InvokeEvent(vtkCommand::ProgressEvent,&progress);

    initialProfile->UnRegister((vtkObjectBase*)NULL);
    initialProfileEdgeTable->UnRegister((vtkObjectBase*)NULL);
    }

  this->RemoveDegenerateBifurcations(segments,bifurcations);

  vtkIdTypeArray* topologyArray = vtkIdTypeArray::New();
  topologyArray->SetName(TopologyArrayName);
  topologyArray->SetNumberOfComponents(2);

  segments->InitTraversal();
  for (i=0; i<segments->GetNumberOfItems(); i++)
    {
    vtkIdType topology[2];
    vtkIdTypeArray* segmentTopologyArray = vtkIdTypeArray::SafeDownCast(segments->GetNextItem()->GetCellData()->GetArray(TopologyArrayName));
    segmentTopologyArray->GetTypedTuple(0, topology);
    topologyArray->InsertNextTypedTuple(topology);
    }

  vtkAppendPolyData* segmentsAppended = vtkAppendPolyData::New();
  segments->InitTraversal();
  for (i=0; i<segments->GetNumberOfItems(); i++)
    {
    segmentsAppended->AddInputData(segments->GetNextItem());
    }
  segmentsAppended->Update();

  network->DeepCopy(segmentsAppended->GetOutput());
  network->GetCellData()->AddArray(topologyArray);

  globalProfilesEdgeTables->Delete();
  bifurcations->Delete();
  bifurcationsRadii->Delete();
  segments->Delete();
  globalBoundaryBarycenters->Delete();
  globalBoundaryRadii->Delete();
  segmentsAppended->Delete();
  topologyArray->Delete();
}

void vtkvmtkPolyDataNetworkExtraction::MarkModelRealBoundary(vtkPolyData* model, vtkPolyData* modelBoundary)
{
  vtkIdTypeArray* marksArray = vtkIdTypeArray::SafeDownCast(model->GetPointData()->GetArray(MarksArrayName));
  vtkIdType i;
  for (i=0; i<model->GetNumberOfPoints(); i++)
    {
    marksArray->InsertValue(i,NON_VISITED);
    }
  for (i=0; i<modelBoundary->GetNumberOfPoints(); i++)
    {
    marksArray->InsertValue(model->FindPoint(modelBoundary->GetPoint(i)),GLOBAL);
    }
}

void vtkvmtkPolyDataNetworkExtraction::Graph(vtkPolyData* network, vtkPolyData* graphLayout)
{
  vtkPoints* graphPoints = vtkPoints::New();
  vtkCellArray* graphLines = vtkCellArray::New();
  vtkDoubleArray* graphRadiusArray = vtkDoubleArray::New();
  graphRadiusArray->SetName(this->RadiusArrayName);

  graphLayout->SetPoints(graphPoints);
  graphLayout->SetLines(graphLines);
  graphLayout->GetCellData()->AddArray(graphRadiusArray);
  
  vtkIdType numberOfPoints = network->GetNumberOfPoints();
  vtkIdType numberOfCells = network->GetNumberOfCells();

  vtkIdTypeArray* isPointInserted = vtkIdTypeArray::New();
  isPointInserted->SetNumberOfTuples(numberOfPoints);
  isPointInserted->FillComponent(0,-1);

  vtkDoubleArray* radiusArray = vtkDoubleArray::SafeDownCast(network->GetPointData()->GetArray(this->RadiusArrayName));
  vtkIdTypeArray* topologyArray = vtkIdTypeArray::SafeDownCast(network->GetCellData()->GetArray(this->TopologyArrayName));

  for (int i=0; i<numberOfCells; i++)
    {
    vtkCell* cell = network->GetCell(i);
    vtkIdType topology[2];
    topologyArray->GetTypedTuple(i, topology);

    vtkIdType numberOfCellPoints = cell->GetNumberOfPoints();
    double meanRadius = 0.0;
    for (int j=0; j<numberOfCellPoints; j++)
      {
      double radius = radiusArray->GetValue(cell->GetPointId(j));
      meanRadius += radius;
      }
    meanRadius /= numberOfCellPoints;
    vtkIdType pointId0 = cell->GetPointId(0);
    vtkIdType pointId1 = cell->GetPointId(numberOfCellPoints-1);
    vtkIdType newPointId0 = -1;
    vtkIdType newPointId1 = -1;
    if (isPointInserted->GetValue(pointId0) == -1)
      {
      newPointId0 = graphPoints->InsertNextPoint(network->GetPoint(pointId0));
      isPointInserted->SetValue(pointId0,newPointId0);
      }
    else
      {
      newPointId0 = isPointInserted->GetValue(pointId0);
      }
    if (isPointInserted->GetValue(pointId1) == -1)
      {
      newPointId1 = graphPoints->InsertNextPoint(network->GetPoint(pointId1));
      isPointInserted->SetValue(pointId1,newPointId1);
      }
    else
      {
      newPointId1 = isPointInserted->GetValue(pointId1);
      }
    graphLines->InsertNextCell(2);
    graphLines->InsertCellPoint(newPointId0);
    graphLines->InsertCellPoint(newPointId1);
    graphRadiusArray->InsertNextValue(meanRadius);
    }

  graphPoints->Delete();
  graphLines->Delete();
  graphRadiusArray->Delete();
  isPointInserted->Delete();
}

int vtkvmtkPolyDataNetworkExtraction::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  this->TotalMarkedPoints = 0;

  vtkPolyData* model = vtkPolyData::New();
  model->DeepCopy(input);

  vtkIdTypeArray* marksArray = vtkIdTypeArray::New();
  marksArray->SetName(this->MarksArrayName);
  marksArray->SetNumberOfTuples(model->GetNumberOfPoints());
  model->GetPointData()->AddArray(marksArray);
  
  vtkPolyData* modelBoundary = vtkPolyData::New();
  this->BoundaryExtractor(model,modelBoundary);

  vtkPolyData* network = vtkPolyData::New();

  if (modelBoundary->GetNumberOfPoints() > 0)
    {
    this->MarkModelRealBoundary(model,modelBoundary);

    model->BuildCells();
    model->BuildLinks();

    vtkPolyDataCollection* globalProfiles = vtkPolyDataCollection::New();
    this->BoundarySeparator(modelBoundary,globalProfiles);

    this->GlobalIteration(model,globalProfiles,network,this->AdvancementRatio);
    globalProfiles->Delete();
    }

  marksArray->Delete();
  modelBoundary->Delete();
 
  double progress = 1.0; 
  this->InvokeEvent(vtkCommand::ProgressEvent,&progress);

  output->DeepCopy(network);

  if (this->GraphLayout)
    {
    this->GraphLayout->Delete();
    this->GraphLayout = NULL;
    }

  this->GraphLayout = vtkPolyData::New();
  this->Graph(network,this->GraphLayout);

  network->Delete();

  return 1;
}

void vtkvmtkPolyDataNetworkExtraction::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
