/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDijkstraDistanceToPoints.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/

#include "vtkvmtkPolyDataDijkstraDistanceToPoints.h"

#include "vtkVersion.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIOStream.h"

#include "vtkvmtkConstants.h"

#include "vtkDijkstraGraphGeodesicPath.h"

vtkStandardNewMacro(vtkvmtkPolyDataDijkstraDistanceToPoints);

vtkvmtkPolyDataDijkstraDistanceToPoints::vtkvmtkPolyDataDijkstraDistanceToPoints() 
{
  this->DijkstraDistanceToPointsArrayName = NULL;

  this->SeedIds = NULL;

  this->DistanceOffset = 0.;
  this->DistanceScale = 1.;
  this->MinDistance = 0.;
  this->MaxDistance = -1.;
}

vtkvmtkPolyDataDijkstraDistanceToPoints::~vtkvmtkPolyDataDijkstraDistanceToPoints()
{
  if (this->DijkstraDistanceToPointsArrayName)
    {
    delete[] this->DijkstraDistanceToPointsArrayName;
    this->DijkstraDistanceToPointsArrayName = NULL;
    }

  if (this->SeedIds)
    {
    this->SeedIds->Delete();
    this->SeedIds = NULL;
    }
}

int vtkvmtkPolyDataDijkstraDistanceToPoints::RequestData(
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

  if (!this->DijkstraDistanceToPointsArrayName)
    {
    vtkErrorMacro(<<"DijkstraDistanceToPointsArrayName not set.");
    return 1;
    }

  if (!this->SeedIds)
    {
    vtkErrorMacro(<<"SeedIds not set");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  
  vtkDataArray* distanceToPointsArray =  output->GetPointData()->GetArray(this->DijkstraDistanceToPointsArrayName);
  
  bool createArray = !distanceToPointsArray;
  
  if (createArray) 
    {
    distanceToPointsArray = vtkDoubleArray::New();
    distanceToPointsArray->SetName(this->DijkstraDistanceToPointsArrayName);
    distanceToPointsArray->SetNumberOfComponents(1);
    distanceToPointsArray->SetNumberOfTuples(numberOfInputPoints);
    distanceToPointsArray->FillComponent(0, VTK_VMTK_LARGE_DOUBLE);
  
    output->GetPointData()->AddArray(distanceToPointsArray);
    }

  int numberOfSeeds = this->SeedIds->GetNumberOfIds();
  
  vtkDijkstraGraphGeodesicPath *dijkstraAlgo = vtkDijkstraGraphGeodesicPath::New();
  dijkstraAlgo->SetInputData(input);
  dijkstraAlgo->StopWhenEndReachedOff();
  dijkstraAlgo->UseScalarWeightsOff();
  
  double maxd = this->MaxDistance > 0 ? this->MaxDistance : VTK_VMTK_LARGE_DOUBLE;
  
  for (int i=0; i<numberOfSeeds; i++)
    {
    dijkstraAlgo->SetStartVertex(SeedIds->GetId(i));
    dijkstraAlgo->Update();
    vtkDoubleArray *seedDistances = vtkDoubleArray::New();
    dijkstraAlgo->GetCumulativeWeights(seedDistances);
    for (int i=0;i<numberOfInputPoints;i++)
      {
      double newDist = this->DistanceOffset + this->DistanceScale*seedDistances->GetValue(i);
      if (newDist<this->MinDistance) newDist = this->MinDistance;
      if (newDist>maxd) newDist = maxd;
      if (newDist<distanceToPointsArray->GetComponent(i,0)) distanceToPointsArray->SetComponent(i,0,newDist);
      }
    seedDistances->Delete();
    }


  if (createArray) distanceToPointsArray->Delete();

  return 1;
}
