/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCapPolyData.cxx,v $
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

#include "vtkvmtkCapPolyData.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkvmtkBoundaryReferenceSystems.h"
#include "vtkvmtkBoundaryLabels.h"
#include "vtkvmtkConstants.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkPolyLine.h"
#include "vtkLine.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkCapPolyData);

vtkvmtkCapPolyData::vtkvmtkCapPolyData()
{
  this->BoundaryIds = NULL;
  this->Displacement = 1E-1;
  this->InPlaneDisplacement = 1E-1;
  this->CapCenterIds = NULL;
  this->CellEntityIdsArrayName = NULL;
  this->CellEntityIdOffset = 1;
  this->BoundaryLabelsArrayName = NULL;
  this->BoundaryPointOrderArrayName = NULL;
  this->BoundaryCellEntityIds = NULL;
}

vtkvmtkCapPolyData::~vtkvmtkCapPolyData()
{
  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }
  if (this->CapCenterIds)
    {
    this->CapCenterIds->Delete();
    this->CapCenterIds = NULL;
    }
  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
  this->SetBoundaryLabelsArrayName(NULL);
  this->SetBoundaryPointOrderArrayName(NULL);
  if (this->BoundaryCellEntityIds)
    {
    this->BoundaryCellEntityIds->Delete();
    this->BoundaryCellEntityIds = NULL;
    }
}

int vtkvmtkCapPolyData::RequestData(
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

  // Declare
  vtkIdType barycenterId, trianglePoints[3];
  vtkIdType i, j;
  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor;
  vtkPolyData* boundaries;
  vtkPoints* newPoints;
  vtkCellArray* newPolys;
  vtkPolyLine* boundary;

  // Initialize
  if ( ((input->GetNumberOfPoints()) < 1) )
    {
    //vtkErrorMacro(<< "No input!");
    return 1;
    }
  input->BuildLinks();

  // Allocate
  newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());
  newPolys = vtkCellArray::New();
  newPolys->DeepCopy(input->GetPolys());
  boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();

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

  // Execute
  // The boundaries either come from the labels the input already carries, which are the same
  // boundaries in the same order every other filter reading those labels sees, or they are
  // extracted here as they always were.
  vtkNew<vtkPolyData> labeledBoundaries;
  vtkNew<vtkIdList> boundaryLabels;
  boundaries = NULL;
  bool useBoundaryLabels = false;
  if (this->BoundaryLabelsArrayName && this->BoundaryLabelsArrayName[0]
      && this->BoundaryPointOrderArrayName && this->BoundaryPointOrderArrayName[0])
    {
    useBoundaryLabels = vtkvmtkBoundaryLabels::GetBoundaries(
      input,this->BoundaryLabelsArrayName,this->BoundaryPointOrderArrayName,
      labeledBoundaries,boundaryLabels);
    if (useBoundaryLabels)
      {
      boundaries = labeledBoundaries;
      }
    else
      {
      vtkWarningMacro(<<"The boundary label arrays are missing from the input surface or no longer describe it; "
                      <<"extracting its boundaries instead, and naming the caps by position.");
      }
    }
  if (!boundaries)
    {
    boundaryExtractor->SetInputData(input);
    boundaryExtractor->Update();
    boundaries = boundaryExtractor->GetOutput();
    }

  if (this->BoundaryCellEntityIds)
    {
    // Silence here is how a caller ends up with caps carrying ids it did not choose and no way
    // of telling. Say which boundaries were left out instead.
    for (vtkIdType boundaryIndex=0; boundaryIndex<boundaries->GetNumberOfCells(); boundaryIndex++)
      {
      vtkIdType boundaryId = useBoundaryLabels ? boundaryLabels->GetId(boundaryIndex) : boundaryIndex;
      if (boundaryId >= this->BoundaryCellEntityIds->GetNumberOfTuples())
        {
        vtkWarningMacro(<<"BoundaryCellEntityIds has no entry for the boundary with id "<<boundaryId
                        <<"; its cap takes the id its position gives it.");
        }
      }
    }

  if (this->CapCenterIds)
    {
    this->CapCenterIds->Delete();
    this->CapCenterIds = NULL;
    }

  this->CapCenterIds = vtkIdList::New();
  this->CapCenterIds->SetNumberOfIds(boundaries->GetNumberOfCells());
  for (i=0; i<this->CapCenterIds->GetNumberOfIds(); i++)
    {
    this->CapCenterIds->SetId(i,-1);
    }

  double barycenter[3], normal[3], outwardNormal[3], meanRadius;

  for (i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    if (this->BoundaryIds)
      {
      // A boundary is named by its label when the labels are in use, and by its position in the
      // extraction order otherwise, here as everywhere else in this filter.
      if (this->BoundaryIds->IsId(useBoundaryLabels ? boundaryLabels->GetId(i) : i) == -1)
        {
        continue;
        }
      }
    boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));

    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryNormal(boundary->GetPoints(),barycenter,normal);
    vtkvmtkBoundaryReferenceSystems::OrientBoundaryNormalOutwards(input,boundaries,i,normal,outwardNormal);
    meanRadius = vtkvmtkBoundaryReferenceSystems::ComputeBoundaryMeanRadius(boundary->GetPoints(),barycenter);

    for (j=0; j<3; j++)
      {
      barycenter[j] += meanRadius * this->Displacement * outwardNormal[j];
      }

    double inplane1[3], inplane2[3];
    vtkMath::Perpendiculars(outwardNormal,inplane1,inplane2,0.0);
    for (j=0; j<3; j++)
      {
      barycenter[j] += meanRadius * this->InPlaneDisplacement * inplane1[j];
      }

    barycenterId = newPoints->InsertNextPoint(barycenter);
    this->CapCenterIds->SetId(i,barycenterId);

    vtkIdType numberOfBoundaryPoints = boundary->GetNumberOfPoints();
    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      trianglePoints[0] = static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundary->GetPointId(j)));
      trianglePoints[1] = barycenterId;
      trianglePoints[2] = static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundary->GetPointId((j+1)%numberOfBoundaryPoints)));

      newPolys->InsertNextCell(3,trianglePoints);

      if (markCells)
        {
        // An id the caller chose for this boundary is used as it stands; CellEntityIdOffset is
        // what moves the ids this filter derives itself out of the way of the input's, and has
        // no business shifting one that was picked deliberately.
        // With the labels in use the boundary's label is its name, so it is also the cap's id
        // unless the caller chose another: a label is as deliberate as an entry in
        // BoundaryCellEntityIds, and neither is shifted by CellEntityIdOffset. Without them
        // there is nothing to go on but the boundary's position.
        vtkIdType boundaryId = useBoundaryLabels ? boundaryLabels->GetId(i) : i;
        vtkIdType capCellEntityId = useBoundaryLabels ? boundaryId : i+1+this->CellEntityIdOffset;
        if (this->BoundaryCellEntityIds
            && boundaryId < this->BoundaryCellEntityIds->GetNumberOfTuples()
            && this->BoundaryCellEntityIds->GetValue(boundaryId) >= 0)
          {
          capCellEntityId = this->BoundaryCellEntityIds->GetValue(boundaryId);
          }
        cellEntityIdsArray->InsertNextValue(capCellEntityId);
        }

      }
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    cellEntityIdsArray->Delete();
    }

  if (useBoundaryLabels)
    {
    // The input's points keep the ids they had - they are deep copied first and the cap centers
    // only appended - so the labels copy across as they stand. They describe no open boundary
    // any more, every one of them having just been closed, but they are left in place as a
    // record of which vessel end each ring of points was.
    const vtkIdType invalidLabel = vtkvmtkBoundaryLabels::GetInvalidBoundaryLabel();
    vtkIdType numberOfInputPoints = input->GetNumberOfPoints();
    vtkIdType numberOfOutputPoints = newPoints->GetNumberOfPoints();

    vtkDataArray* inputLabels = input->GetPointData()->GetArray(this->BoundaryLabelsArrayName);
    vtkDataArray* inputOrder = input->GetPointData()->GetArray(this->BoundaryPointOrderArrayName);

    vtkNew<vtkIntArray> outputLabels;
    outputLabels->SetName(this->BoundaryLabelsArrayName);
    outputLabels->SetNumberOfTuples(numberOfOutputPoints);
    outputLabels->FillComponent(0,static_cast<double>(invalidLabel));

    vtkNew<vtkIntArray> outputOrder;
    outputOrder->SetName(this->BoundaryPointOrderArrayName);
    outputOrder->SetNumberOfTuples(numberOfOutputPoints);
    outputOrder->FillComponent(0,static_cast<double>(invalidLabel));

    for (vtkIdType pointId=0; pointId<numberOfInputPoints; pointId++)
      {
      outputLabels->SetValue(pointId,static_cast<int>(vtkMath::Round(inputLabels->GetComponent(pointId,0))));
      outputOrder->SetValue(pointId,static_cast<int>(vtkMath::Round(inputOrder->GetComponent(pointId,0))));
      }

    output->GetPointData()->AddArray(outputLabels);
    output->GetPointData()->AddArray(outputOrder);
    }

  // TODO: the filter throws all the point and cell data
  //output->GetPointData()->PassData(input->GetPointData()); // Like this?

  // Destroy
  newPoints->Delete();
  newPolys->Delete();
  boundaryExtractor->Delete();

  return 1;
}

void vtkvmtkCapPolyData::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
