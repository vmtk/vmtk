/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataNormalPlaneEstimator.cxx,v $
Language:  C++
Date:      $Date: 2015/12/01 12:26:27 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataNormalPlaneEstimator.h"

#include "vtkPolyDataConnectivityFilter.h"
#include "vtkFloatArray.h"
#include "vtkIdTypeArray.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkCell.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataNormalPlaneEstimator);

vtkvmtkPolyDataNormalPlaneEstimator::vtkvmtkPolyDataNormalPlaneEstimator()
{
  this->UseConnectivity = 1;
  this->MinimumNeighborhoodSize = 10;
  this->OriginPointId = -1;
  this->Origin[0] = this->Origin[1] = this->Origin[2] = 0.0;
  this->Normal[0] = this->Normal[1] = this->Normal[2] = 0.0;
}

vtkvmtkPolyDataNormalPlaneEstimator::~vtkvmtkPolyDataNormalPlaneEstimator()
{
}

int vtkvmtkPolyDataNormalPlaneEstimator::RequestData(
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

  if (this->OriginPointId == -1)
    {
    vtkErrorMacro("Origin point id not specified");
    return 1;
    }

  input->BuildCells();
  input->BuildLinks();

  vtkFloatArray* normals = vtkFloatArray::New();

  if (input->GetPointData()->GetNormals())
    {
      normals->DeepCopy(input->GetPointData()->GetNormals());
    }
  else
    {
      vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
      normalsFilter->SetInputData(input);
      normalsFilter->AutoOrientNormalsOn();
      normalsFilter->FlipNormalsOn();
      normalsFilter->ConsistencyOn();
      normalsFilter->ComputeCellNormalsOff();
      normalsFilter->SplittingOff();
      normalsFilter->Update();

      normals->DeepCopy(normalsFilter->GetOutput()->GetPointData()->GetNormals());

      normalsFilter->Delete();
    }

  vtkPointLocator* locator = vtkPointLocator::New();
  locator->SetDataSet(input);
  locator->BuildLocator();

  vtkIdTypeArray* connectivityArray = vtkIdTypeArray::New();
  if (this->UseConnectivity)
  {
    vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
    connectivityFilter->SetInputData(input);
    connectivityFilter->SetExtractionModeToAllRegions();
    connectivityFilter->ColorRegionsOn();
    connectivityFilter->Update();

    connectivityArray->DeepCopy(connectivityFilter->GetOutput()->GetPointData()->GetScalars());
    connectivityFilter->Delete();
  }

  vtkIdType npts, *cells;
  vtkIdType ncells;

  input->GetPointCells(this->OriginPointId,ncells,cells);

  double averageLength = 0.0;

  for (vtkIdType i=0; i<ncells; i++)
    {
      averageLength += sqrt(input->GetCell(cells[i])->GetLength2());
    }
  averageLength /= ncells;

  double radiusStep = averageLength;

  double direction[3];
  double normal0[3], normal1[3];

  input->GetPoint(this->OriginPointId,this->Origin);
  normals->GetTuple(this->OriginPointId,normal1);

  vtkIdType regionId = -1;
  if (this->UseConnectivity)
  {
    regionId = connectivityArray->GetValue(this->OriginPointId);
  }

  vtkIdList* pointIdsWithinRadius = vtkIdList::New();

  bool doneOnce = false;

  // Future improvement: only consider topologically connected vertices

  for (int k=0; k<100; k++) 
  {
    pointIdsWithinRadius->Initialize();

    double radius = k * radiusStep;
    locator->FindPointsWithinRadius(radius,this->Origin,pointIdsWithinRadius);

    vtkIdType numberOfIds = pointIdsWithinRadius->GetNumberOfIds();

    vtkIdType neighborhoodSize = numberOfIds;

    if (this->UseConnectivity)
    {
      for (vtkIdType i=0; i<numberOfIds; i++)
      {
        vtkIdType id = pointIdsWithinRadius->GetId(i);
        if (connectivityArray->GetValue(id) == regionId)
        {
          neighborhoodSize += 1;
        }
      }
    }

    if (neighborhoodSize < this->MinimumNeighborhoodSize)
    {
      continue;
    }

    double kdirection[3];
    kdirection[0] = kdirection[1] = kdirection[2] = 0.0;

    bool done = false;

    for (vtkIdType i=0; i<numberOfIds; i++) 
    {
      vtkIdType id = pointIdsWithinRadius->GetId(i);

      if (this->UseConnectivity && connectivityArray->GetValue(id) != regionId)
      {
        continue;
      }

      normals->GetTuple(id,normal0);

      double dot = vtkMath::Dot(normal0,normal1);

      if (fabs(dot) > 0.6) {
        continue;
      }

      if (dot < -0.5 && doneOnce) {
        done = true;
        break;
      }

      double cross[3];
      vtkMath::Cross(normal0,normal1,cross);

      if (vtkMath::Dot(kdirection,cross) < 0.0) {
        cross[0] *= -1.0;
        cross[1] *= -1.0;
        cross[2] *= -1.0;
      }

      kdirection[0] += cross[0];
      kdirection[1] += cross[1];
      kdirection[2] += cross[2];
      doneOnce = true;
    }

    if (!doneOnce)
    {
      continue;
    }

    vtkMath::Normalize(kdirection);

    if (done)
    {
      break;
    }

    this->Normal[0] = kdirection[0];
    this->Normal[1] = kdirection[1];
    this->Normal[2] = kdirection[2];
  }

  vtkMath::Normalize(this->Normal);

  normals->Delete();
  locator->Delete();
  pointIdsWithinRadius->Delete();
  connectivityArray->Delete();

  return 1;
}

void vtkvmtkPolyDataNormalPlaneEstimator::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
