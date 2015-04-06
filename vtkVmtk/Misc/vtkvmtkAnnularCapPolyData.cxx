/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAnnularCapPolyData.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkAnnularCapPolyData.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkvmtkBoundaryReferenceSystems.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkAnnularCapPolyData);

vtkvmtkAnnularCapPolyData::vtkvmtkAnnularCapPolyData()
{
  this->BoundaryIds = NULL;
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdOffset = 1;
}

vtkvmtkAnnularCapPolyData::~vtkvmtkAnnularCapPolyData()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }
}

int vtkvmtkAnnularCapPolyData::RequestData(
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

  if ( input->GetNumberOfPoints() < 1 )
    {
    return 1;
    }

  input->BuildLinks();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());

  vtkCellArray* newPolys = vtkCellArray::New();
  newPolys->DeepCopy(input->GetPolys());

  vtkIdTypeArray* cellEntityIdsArray = NULL;
  bool markCells = this->CellEntityIdsArrayName && this->CellEntityIdsArrayName[0];
  if (markCells)
    {
    cellEntityIdsArray = vtkIdTypeArray::New();
    cellEntityIdsArray->SetName(this->CellEntityIdsArrayName);
    if (input->GetCellData()->GetArray(this->CellEntityIdsArrayName))
      {
      cellEntityIdsArray->DeepCopy(input->GetCellData()->GetArray(this->CellEntityIdsArrayName));
      }
    else
      {
      cellEntityIdsArray->SetNumberOfTuples(newPolys->GetNumberOfCells());
      cellEntityIdsArray->FillComponent(0,static_cast<double>(this->CellEntityIdOffset));
      }
    }

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
#if (VTK_MAJOR_VERSION <= 5)
  boundaryExtractor->SetInput(input);
#else
  boundaryExtractor->SetInputData(input);
#endif
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();
  int numberOfBoundaries = boundaries->GetNumberOfCells();

  if ( (this->BoundaryIds && this->BoundaryIds->GetNumberOfIds() % 2)
       || (!this->BoundaryIds && numberOfBoundaries % 2) )
    {
    vtkErrorMacro(<< "Error: the number of boundaries must be even.");
    newPoints->Delete();
    newPolys->Delete();
    if (markCells)
      {
      cellEntityIdsArray->Delete();
      }
    boundaryExtractor->Delete();
    }

  vtkPoints* barycenters = vtkPoints::New();
  barycenters->SetNumberOfPoints(numberOfBoundaries);

  vtkIdList* boundaryPairings = vtkIdList::New();
  boundaryPairings->SetNumberOfIds(numberOfBoundaries);
  vtkIdList* visitedBoundaries = vtkIdList::New();
  visitedBoundaries->SetNumberOfIds(numberOfBoundaries);

  double barycenter[3];
  for (int i=0; i<numberOfBoundaries; i++)
    {
    vtkCell* boundary = boundaries->GetCell(i);
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    barycenters->SetPoint(i,barycenter);
    boundaryPairings->SetId(i,-1);
    visitedBoundaries->SetId(i,-1);
    }

  double currentBarycenter[3];
  double distance2, minDistance2 = 0.0;
  vtkIdType closestBoundaryId;
  for (int i=0; i<numberOfBoundaries; i++)
    {
    if (this->BoundaryIds && this->BoundaryIds->IsId(i) == -1)
      {
      continue;
      }
    if (boundaryPairings->GetId(i) != -1 || boundaryPairings->IsId(i) != -1)
      {
      continue;
      }
    barycenters->GetPoint(i,barycenter);
    closestBoundaryId = -1;
    for (int j=i+1; j<numberOfBoundaries; j++)
      {
      if (this->BoundaryIds && this->BoundaryIds->IsId(j) == -1)
        {
        continue;
        }
      if (boundaryPairings->GetId(j) != -1 || boundaryPairings->IsId(j) != -1)
        {
        continue;
        }
      barycenters->GetPoint(j,currentBarycenter);
      distance2 = vtkMath::Distance2BetweenPoints(barycenter,currentBarycenter);
      if (closestBoundaryId == -1 || distance2 < minDistance2)
        {
        minDistance2 = distance2;
        closestBoundaryId = j;
        }
      }
    boundaryPairings->SetId(i,closestBoundaryId);
    boundaryPairings->SetId(closestBoundaryId,i);
    }

  for (int i=0; i<numberOfBoundaries; i++)
    {
    if (this->BoundaryIds && this->BoundaryIds->IsId(i) == -1)
      {
      continue;
      }

    if (boundaryPairings->GetId(i) == -1)
      {
      continue;
      }

    if (visitedBoundaries->GetId(i) != -1)
      {
      continue;
      }

    visitedBoundaries->SetId(i,i);
    visitedBoundaries->SetId(boundaryPairings->GetId(i),i);

    vtkIdTypeArray* boundaryPointIdsArray = vtkIdTypeArray::New();
    boundaryPointIdsArray->DeepCopy(boundaries->GetPointData()->GetScalars());

    vtkPolyLine* boundary0 = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));
    vtkIdType numberOfBoundaryPoints0 = boundary0->GetNumberOfPoints();
    vtkIdList* boundaryPointIds0 = vtkIdList::New();
    boundaryPointIds0->SetNumberOfIds(numberOfBoundaryPoints0);
    for (int j=0; j<numberOfBoundaryPoints0; j++)
      {
      boundaryPointIds0->SetId(j,boundaryPointIdsArray->GetValue(boundary0->GetPointId(j)));
      }

    vtkPolyLine* boundary1 = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryPairings->GetId(i)));
    vtkIdType numberOfBoundaryPoints1 = boundary1->GetNumberOfPoints();
    vtkIdList* boundaryPointIds1 = vtkIdList::New();
    boundaryPointIds1->SetNumberOfIds(numberOfBoundaryPoints1);
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      boundaryPointIds1->SetId(j,boundaryPointIdsArray->GetValue(boundary1->GetPointId(j)));
      }

    double startingPoint[3], currentPoint[3];
    input->GetPoint(boundaryPointIds0->GetId(0),startingPoint);

    vtkIdType offset = -1;
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      input->GetPoint(boundaryPointIds1->GetId(j),currentPoint);
      distance2 = vtkMath::Distance2BetweenPoints(startingPoint,currentPoint);
      if (offset == -1 || distance2 < minDistance2)
        {
        offset = j;
        minDistance2 = distance2;
        }
      }

    double vectorToStart[3], vectorToForward[3], cross0[3], cross1[3];
    double pointForward[3];

    barycenters->GetPoint(i,barycenter);
    input->GetPoint(boundaryPointIds0->GetId(0),startingPoint);
    input->GetPoint(boundaryPointIds0->GetId(numberOfBoundaryPoints0/8),pointForward);

    vectorToStart[0] = startingPoint[0] - barycenter[0];
    vectorToStart[1] = startingPoint[1] - barycenter[1];
    vectorToStart[2] = startingPoint[2] - barycenter[2];

    vectorToForward[0] = pointForward[0] - barycenter[0];
    vectorToForward[1] = pointForward[1] - barycenter[1];
    vectorToForward[2] = pointForward[2] - barycenter[2];

    vtkMath::Cross(vectorToStart,vectorToForward,cross0);

    barycenters->GetPoint(boundaryPairings->GetId(i),barycenter);
    input->GetPoint(boundaryPointIds1->GetId(0),startingPoint);
    input->GetPoint(boundaryPointIds1->GetId(numberOfBoundaryPoints1/8),pointForward);

    vectorToStart[0] = startingPoint[0] - barycenter[0];
    vectorToStart[1] = startingPoint[1] - barycenter[1];
    vectorToStart[2] = startingPoint[2] - barycenter[2];

    vectorToForward[0] = pointForward[0] - barycenter[0];
    vectorToForward[1] = pointForward[1] - barycenter[1];
    vectorToForward[2] = pointForward[2] - barycenter[2];

    vtkMath::Cross(vectorToStart,vectorToForward,cross1);

    bool backward = false;
    if (vtkMath::Dot(cross0,cross1) < 0.0)
      {
      backward = true;
      }

    double point0[3], nextPoint0[3];
    double point1[3], nextPoint1[3];
    vtkIdType pointId0, nextPointId0;
    vtkIdType pointId1, nextPointId1;
    int j = 0;
    int k = 0;
    while (true)
      {
      if (j == numberOfBoundaryPoints0 && k == numberOfBoundaryPoints1)
        {
        break;
        }

      pointId0 = j % numberOfBoundaryPoints0;
      nextPointId0 = (pointId0 + 1) % numberOfBoundaryPoints0;
      pointId1 = (k + offset) % numberOfBoundaryPoints1;
      nextPointId1 = (pointId1 + 1) % numberOfBoundaryPoints1;
      if (backward)
        {
        pointId1 = (numberOfBoundaryPoints1 - k + offset + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
        nextPointId1 = (pointId1 - 1 + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
        }

      input->GetPoint(boundaryPointIds0->GetId(pointId0),point0);
      input->GetPoint(boundaryPointIds0->GetId(nextPointId0),nextPoint0);
      input->GetPoint(boundaryPointIds1->GetId(pointId1),point1);
      input->GetPoint(boundaryPointIds1->GetId(nextPointId1),nextPoint1);

      newPolys->InsertNextCell(3);
      newPolys->InsertCellPoint(boundaryPointIds0->GetId(pointId0));
      newPolys->InsertCellPoint(boundaryPointIds1->GetId(pointId1));

      bool next1 = false;
      if (vtkMath::Distance2BetweenPoints(point0,nextPoint1) < vtkMath::Distance2BetweenPoints(point1,nextPoint0))
        {
        next1 = true;
        }
      if (k == numberOfBoundaryPoints1)
        {
        next1 = false;
        }
      else if (j == numberOfBoundaryPoints0)
        {
        next1 = true;
        }

      if (next1)
        {
        newPolys->InsertCellPoint(boundaryPointIds1->GetId(nextPointId1));
        k++;
        }
      else
        {
        newPolys->InsertCellPoint(boundaryPointIds0->GetId(nextPointId0));
        j++;
        }

      if (markCells)
        {
        cellEntityIdsArray->InsertNextValue(i+1+this->CellEntityIdOffset);
        }
      }

    boundaryPointIdsArray->Delete();
    boundaryPointIds0->Delete();
    boundaryPointIds1->Delete();
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  output->GetPointData()->PassData(input->GetPointData());

  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    cellEntityIdsArray->Delete();
    }

  newPoints->Delete();
  newPolys->Delete();
  boundaryExtractor->Delete();
  barycenters->Delete();
  visitedBoundaries->Delete();

  return 1;
}

void vtkvmtkAnnularCapPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
