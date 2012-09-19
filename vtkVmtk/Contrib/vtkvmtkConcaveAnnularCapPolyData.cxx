/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkConcaveAnnularCapPolyData.cxx,v $
Language:  C++
Date:      $Date: 2012/09/19 $
Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by
    Martin Sandve Alnaes
	Simula Research Laboratory
  Based on vtkvmtkAnnularCapPolyData by Tangui Morvan.

=========================================================================*/

#include "vtkvmtkConcaveAnnularCapPolyData.h"
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
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkvmtkConcaveAnnularCapPolyData, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkvmtkConcaveAnnularCapPolyData);

vtkvmtkConcaveAnnularCapPolyData::vtkvmtkConcaveAnnularCapPolyData()
{
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdOffset = 1;
}

vtkvmtkConcaveAnnularCapPolyData::~vtkvmtkConcaveAnnularCapPolyData()
{
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
}

int vtkvmtkConcaveAnnularCapPolyData::RequestData(
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

  // TODO: One point is not sufficient I think?
  if ( input->GetNumberOfPoints() < 1 )
    {
    return 1;
    }

  input->BuildLinks();

  vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
  newPoints->DeepCopy(input->GetPoints());

  vtkSmartPointer<vtkCellArray> newPolys = vtkSmartPointer<vtkCellArray>::New();
  newPolys->DeepCopy(input->GetPolys());

  // Mark cells if we have a nonempty name
  bool markCells = this->CellEntityIdsArrayName && this->CellEntityIdsArrayName[0];
  vtkSmartPointer<vtkIntArray> cellEntityIdsArray;
  if (markCells)
    {
    cellEntityIdsArray = vtkSmartPointer<vtkIntArray>::New();
    cellEntityIdsArray->SetName(this->CellEntityIdsArrayName);
    cellEntityIdsArray->SetNumberOfTuples(newPolys->GetNumberOfCells());
    cellEntityIdsArray->FillComponent(0,static_cast<double>(this->CellEntityIdOffset));
    }

  vtkSmartPointer<vtkvmtkPolyDataBoundaryExtractor> boundaryExtractor =
      vtkSmartPointer<vtkvmtkPolyDataBoundaryExtractor>::New();
  boundaryExtractor->SetInput(input);
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();
  int numberOfBoundaries = boundaries->GetNumberOfCells();

  if (numberOfBoundaries % 2 != 0)
    {
    vtkErrorMacro(<< "Error: the number of boundaries must be even.");
    }

  vtkSmartPointer<vtkPoints> barycenters = vtkSmartPointer<vtkPoints>::New();
  barycenters->SetNumberOfPoints(numberOfBoundaries);
  vtkSmartPointer<vtkIdList> boundaryPairings = vtkSmartPointer<vtkIdList>::New();
  boundaryPairings->SetNumberOfIds(numberOfBoundaries);
  vtkSmartPointer<vtkIdList> visitedBoundaries = vtkSmartPointer<vtkIdList>::New();
  visitedBoundaries->SetNumberOfIds(numberOfBoundaries);

  for (int i=0; i<numberOfBoundaries; i++)
    {
    double barycenter[3];
    vtkCell* boundary = boundaries->GetCell(i);
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    barycenters->SetPoint(i,barycenter);
    boundaryPairings->SetId(i,-1);
    visitedBoundaries->SetId(i,-1);
    }

  double currentBarycenter[3];
  double minDistance2 = 0.0;
  vtkIdType closestBoundaryId;
  for (int i=0; i<numberOfBoundaries-1; i++)
    {
    if (boundaryPairings->GetId(i) != -1 || boundaryPairings->IsId(i) != -1)
      {
      continue;
      }
    double barycenter[3];
    barycenters->GetPoint(i,barycenter);
    closestBoundaryId = -1;
    for (int j=i+1; j<numberOfBoundaries; j++)
      {
      if (boundaryPairings->GetId(j) != -1 || boundaryPairings->IsId(j) != -1)
        {
        continue;
        }
      barycenters->GetPoint(j,currentBarycenter);
      double distance2 = vtkMath::Distance2BetweenPoints(barycenter,currentBarycenter);
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
    if (boundaryPairings->GetId(i) == -1)
      {
      continue;
      }

    if (visitedBoundaries->GetId(i) != -1)
      {
      continue;
      }
    else
      {
      visitedBoundaries->SetId(i,i);
      visitedBoundaries->SetId(boundaryPairings->GetId(i),i);
      }

    vtkSmartPointer<vtkIdTypeArray> boundaryPointIdsArray = vtkSmartPointer<vtkIdTypeArray>::New();
    boundaryPointIdsArray->DeepCopy(boundaries->GetPointData()->GetScalars());

    vtkPolyLine* boundary0 = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));
    vtkIdType numberOfBoundaryPoints0 = boundary0->GetNumberOfPoints();
    vtkSmartPointer<vtkIdList> boundaryPointIds0 = vtkSmartPointer<vtkIdList>::New();
    boundaryPointIds0->SetNumberOfIds(numberOfBoundaryPoints0);
    for (int j=0; j<numberOfBoundaryPoints0; j++)
      {
      boundaryPointIds0->SetId(j,boundaryPointIdsArray->GetValue(boundary0->GetPointId(j)));
      }

    vtkPolyLine* boundary1 = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryPairings->GetId(i)));
    vtkIdType numberOfBoundaryPoints1 = boundary1->GetNumberOfPoints();
    vtkSmartPointer<vtkIdList> boundaryPointIds1 = vtkSmartPointer<vtkIdList>::New();
    boundaryPointIds1->SetNumberOfIds(numberOfBoundaryPoints1);
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      boundaryPointIds1->SetId(j,boundaryPointIdsArray->GetValue(boundary1->GetPointId(j)));
      }

    double startingPoint[3];
    input->GetPoint(boundaryPointIds0->GetId(0),startingPoint);

    vtkIdType offset = -1;
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      double currentPoint[3];
      input->GetPoint(boundaryPointIds1->GetId(j),currentPoint);
      double distance2 = vtkMath::Distance2BetweenPoints(startingPoint,currentPoint);
      if (offset == -1 || distance2 < minDistance2)
        {
        offset = j;
        minDistance2 = distance2;
        }
      }

    double vectorToStart[3], vectorToForward[3], cross0[3], cross1[3];
    double pointForward[3];

    double barycenter[3];
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


    // FIXME: Try vtkPolygon::Triangulate to build surface!
    // - First find the two closest vertices ij,oj in the inner and outer polygons
    // - Initialize a vtkPolygon instance
    // - Add all vertices from the inner polygon in a clockwise direction,
    //   starting and ending with ij.
    // - Continue by adding all vertices from the outer polygon in a
    //   counterclockwise direction, starting and ending with oj.
    // - vtkIdList * outTris = vtkIdList::New();
    // - vtkPoints * outPoints = vtkPoints::New();
    // - Call polygon->Triangulate(0, outTris, outPoints);
    // - Output is outTris, outPoints, use as appropriate in this code
    // - Make sure to clean up afterwards


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
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  output->GetPointData()->PassData(input->GetPointData());

  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    }

  return 1;
}

void vtkvmtkConcaveAnnularCapPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
