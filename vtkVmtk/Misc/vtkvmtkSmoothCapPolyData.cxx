/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSmoothCapPolyData.cxx,v $
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

#include "vtkvmtkSmoothCapPolyData.h"
#include "vtkvmtkBoundaryLabels.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"
#include "vtkTriangle.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkSmoothCapPolyData);

vtkvmtkSmoothCapPolyData::vtkvmtkSmoothCapPolyData()
{
  this->BoundaryIds = NULL;
  this->ConstraintFactor = 1.0;
  this->NumberOfRings = 8;
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdOffset = 1;
  this->BoundaryLabelsArrayName = NULL;
  this->BoundaryPointOrderArrayName = NULL;
  this->BoundaryCellEntityIds = NULL;
}

vtkvmtkSmoothCapPolyData::~vtkvmtkSmoothCapPolyData()
{
  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }

  this->SetBoundaryLabelsArrayName(NULL);
  this->SetBoundaryPointOrderArrayName(NULL);

  if (this->BoundaryCellEntityIds)
    {
    this->BoundaryCellEntityIds->Delete();
    this->BoundaryCellEntityIds = NULL;
    }
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
}

int vtkvmtkSmoothCapPolyData::RequestData(
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

  input->BuildCells();
  input->BuildLinks();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());

  vtkCellArray* newPolys = vtkCellArray::New();
  newPolys->DeepCopy(input->GetPolys());

  // Copy cell entity ids array
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

  // The boundaries either come from the labels the input already carries, which are the same
  // boundaries in the same order every other filter reading those labels sees, or they are
  // extracted here as they always were.
  vtkNew<vtkPolyData> boundaries;
  vtkNew<vtkIdList> boundaryLabels;
  bool useBoundaryLabels = vtkvmtkBoundaryLabels::GetOrExtractBoundaries(
    input,this->BoundaryLabelsArrayName,this->BoundaryPointOrderArrayName,boundaries,boundaryLabels,this);

  int boundaryId;
  for (boundaryId=0; boundaryId<boundaries->GetNumberOfCells(); boundaryId++)
    {
    // boundaryId is the boundary's position here; capBoundaryId is what names it, which is its
    // label when the labels are in use and its position otherwise.
    vtkIdType capBoundaryId = useBoundaryLabels ? boundaryLabels->GetId(boundaryId) : boundaryId;
    if (this->BoundaryIds)
      {
      if (this->BoundaryIds->IsId(capBoundaryId) == -1)
        {
        continue;
        }
      }

    vtkPolyLine* boundaryCell = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryId));

    int numberOfBoundaryPoints = boundaryCell->GetNumberOfPoints();
    double boundaryDiagonal = sqrt(boundaryCell->GetLength2());

    vtkIdList* boundaryPointIds = vtkIdList::New();
    int i;
    for (i=0; i<numberOfBoundaryPoints; i++)
      {
      vtkIdType boundaryPointId = static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetComponent(boundaryCell->GetPointId(i),0));
      boundaryPointIds->InsertNextId(boundaryPointId);
      }

    vtkPoints* outerRingPoints = vtkPoints::New();
    vtkIdList* pointCells = vtkIdList::New();
    vtkIdList* cellPoints = vtkIdList::New();
    for (i=0; i<numberOfBoundaryPoints; i++)
      {
      vtkIdType boundaryPointId = boundaryPointIds->GetId(i);
      pointCells->Initialize();
      input->GetPointCells(boundaryPointId,pointCells);
      double outerRingPoint[3];
      outerRingPoint[0] = outerRingPoint[1] = outerRingPoint[2] = 0.0;
      double areaSum = 0.0;
      int j;
      for (j=0; j<pointCells->GetNumberOfIds(); j++)
        {
        vtkIdType cellId = pointCells->GetId(j);
        input->GetCellPoints(cellId,cellPoints);
        //TODO: assert cell is vtkTriangle
        double point0[3], point1[3], point2[3];
        input->GetPoint(cellPoints->GetId(0),point0);
        input->GetPoint(cellPoints->GetId(1),point1);
        input->GetPoint(cellPoints->GetId(2),point2);
        double triangleCenter[3];
        vtkTriangle::TriangleCenter(point0,point1,point2,triangleCenter);
        double triangleArea = vtkTriangle::TriangleArea(point0,point1,point2);
        outerRingPoint[0] += triangleArea * triangleCenter[0];
        outerRingPoint[1] += triangleArea * triangleCenter[1];
        outerRingPoint[2] += triangleArea * triangleCenter[2];
        areaSum += triangleArea;
        }
      outerRingPoint[0] /= areaSum;
      outerRingPoint[1] /= areaSum;
      outerRingPoint[2] /= areaSum;
      double boundaryPoint[3];
      input->GetPoint(boundaryPointId,boundaryPoint);
      double boundaryVector[3];
      boundaryVector[0] = outerRingPoint[0] - boundaryPoint[0];
      boundaryVector[1] = outerRingPoint[1] - boundaryPoint[1];
      boundaryVector[2] = outerRingPoint[2] - boundaryPoint[2];
      vtkMath::Normalize(boundaryVector);
      double distance = this->ConstraintFactor * 0.125 * boundaryDiagonal;
      outerRingPoint[0] = distance * boundaryVector[0] + boundaryPoint[0];
      outerRingPoint[1] = distance * boundaryVector[1] + boundaryPoint[1];
      outerRingPoint[2] = distance * boundaryVector[2] + boundaryPoint[2];
      outerRingPoints->InsertNextPoint(outerRingPoint);
      }

    pointCells->Delete();
    cellPoints->Delete();

    vtkThinPlateSplineTransform* thinPlateSplineTransform = vtkThinPlateSplineTransform::New();
    thinPlateSplineTransform->SetBasisToR2LogR();
    thinPlateSplineTransform->SetSigma(1.0);
    vtkPoints* sourceLandmarks = vtkPoints::New();
    vtkPoints* targetLandmarks = vtkPoints::New();

    for (i=0; i<numberOfBoundaryPoints; i++)
      {
      double angle = (double)i / (double)numberOfBoundaryPoints * 2.0*vtkMath::Pi();
      double sourcePoint[3];
      sourcePoint[0] = cos(angle);
      sourcePoint[1] = sin(angle);
      sourcePoint[2] = 0.0;
      sourceLandmarks->InsertNextPoint(sourcePoint);
      targetLandmarks->InsertNextPoint(boundaryCell->GetPoints()->GetPoint(i));
      double outerRingRadius = 1.25;
      sourcePoint[0] = outerRingRadius * cos(angle);
      sourcePoint[1] = outerRingRadius * sin(angle);
      sourcePoint[2] = 0.0;
      sourceLandmarks->InsertNextPoint(sourcePoint);
      targetLandmarks->InsertNextPoint(outerRingPoints->GetPoint(i));
      }

    thinPlateSplineTransform->SetSourceLandmarks(sourceLandmarks);
    thinPlateSplineTransform->SetTargetLandmarks(targetLandmarks);

    outerRingPoints->Delete();

    vtkIdList* circlePointIds = vtkIdList::New();
    vtkIdList* previousCirclePointIds = vtkIdList::New();

    circlePointIds->DeepCopy(boundaryPointIds);

    int k;
    for (k=1; k<this->NumberOfRings; k++)
      {
      previousCirclePointIds->DeepCopy(circlePointIds);
      circlePointIds->Initialize();
      for (i=0; i<numberOfBoundaryPoints; i++)
        {
        double angle = (double)i / (double)numberOfBoundaryPoints * 2.0*vtkMath::Pi();
        double radius = 1.0 - (double)k/(double)this->NumberOfRings;
        double sourcePoint[3];
        sourcePoint[0] = radius * cos(angle);
        sourcePoint[1] = radius * sin(angle);
        sourcePoint[2] = 0.0;
        double transformedPoint[3];
        thinPlateSplineTransform->TransformPoint(sourcePoint,transformedPoint);
        vtkIdType circlePointId = newPoints->InsertNextPoint(transformedPoint);
        circlePointIds->InsertNextId(circlePointId);
        }
      for (i=0; i<numberOfBoundaryPoints; i++)
        {
        vtkIdList* newPolyIds = vtkIdList::New();
        newPolyIds->InsertNextId(previousCirclePointIds->GetId(i));
        newPolyIds->InsertNextId(previousCirclePointIds->GetId((i+1)%numberOfBoundaryPoints));
        newPolyIds->InsertNextId(circlePointIds->GetId((i+1)%numberOfBoundaryPoints));
        newPolyIds->InsertNextId(circlePointIds->GetId(i));
        newPolys->InsertNextCell(newPolyIds);
        newPolyIds->Delete();
        if (markCells)
          {
          cellEntityIdsArray->InsertNextValue(this->CapCellEntityId(boundaryId,capBoundaryId,useBoundaryLabels));
          }
        }
      }

    newPolys->InsertNextCell(circlePointIds);

    if (markCells)
      {
      cellEntityIdsArray->InsertNextValue(this->CapCellEntityId(boundaryId,capBoundaryId,useBoundaryLabels));
      }

    boundaryPointIds->Delete();

    circlePointIds->Delete();
    previousCirclePointIds->Delete();

    thinPlateSplineTransform->Delete();
    sourceLandmarks->Delete();
    targetLandmarks->Delete();
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    cellEntityIdsArray->Delete();
    }

  newPoints->Delete();
  newPolys->Delete();

  return 1;
}

vtkIdType vtkvmtkSmoothCapPolyData::CapCellEntityId(vtkIdType boundaryIndex, vtkIdType boundaryId, bool useBoundaryLabels)
{
  // An id the caller chose for this boundary is used as it stands; CellEntityIdOffset is what
  // moves the ids this filter derives itself out of the way of the input's, and has no business
  // shifting one that was picked deliberately.
  if (this->BoundaryCellEntityIds
      && boundaryId < this->BoundaryCellEntityIds->GetNumberOfTuples()
      && this->BoundaryCellEntityIds->GetValue(boundaryId) >= 0)
    {
    return this->BoundaryCellEntityIds->GetValue(boundaryId);
    }
  if (useBoundaryLabels)
    {
    // The boundary's label is its name, and so the cap's id; neither it nor a chosen id is
    // shifted by CellEntityIdOffset.
    return boundaryId;
    }
  return boundaryIndex+1+this->CellEntityIdOffset;
}

void vtkvmtkSmoothCapPolyData::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
