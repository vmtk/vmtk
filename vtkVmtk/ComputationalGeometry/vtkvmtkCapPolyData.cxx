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
#include "vtkvmtkConstants.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkPolyLine.h"
#include "vtkLine.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
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

  // Copy cell entitiy ids array
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
  boundaryExtractor->SetInputData(input);
  boundaryExtractor->Update();

  boundaries = boundaryExtractor->GetOutput();

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
      if (this->BoundaryIds->IsId(i) == -1)
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
        cellEntityIdsArray->InsertNextValue(i+1+this->CellEntityIdOffset);
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
