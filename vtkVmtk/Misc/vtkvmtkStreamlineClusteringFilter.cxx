/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineClusteringFilter.cxx,v $
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

#include "vtkvmtkStreamlineClusteringFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkConstants.h"

#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkSplineFilter.h"
//#include "vtkPointLocator.h"
#include "vtkCellArray.h"
#include "vtkCell.h"


vtkStandardNewMacro(vtkvmtkStreamlineClusteringFilter);

vtkvmtkStreamlineClusteringFilter::vtkvmtkStreamlineClusteringFilter()
{
  this->ClusterCenters = NULL;
}

vtkvmtkStreamlineClusteringFilter::~vtkvmtkStreamlineClusteringFilter()
{
  if (this->ClusterCenters)
    {
    this->ClusterCenters->Delete();
    this->ClusterCenters = NULL;
    }
}

int vtkvmtkStreamlineClusteringFilter::RequestData(
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

  if (input->GetNumberOfPoints() < 1)
    {
    return 1;
    }

  double resampleLength = 0.01;
  int numberOfChunkPoints = 40;
  int numberOfClusters = 4;

  vtkSplineFilter* splineFilter = vtkSplineFilter::New();
  splineFilter->SetInputData(input);
  splineFilter->SetSubdivideToLength();
  splineFilter->SetLength(resampleLength);
  splineFilter->Update();

  vtkPolyData* resampledInput = splineFilter->GetOutput();

  vtkPolyData* streamlines = vtkPolyData::New();
  vtkPoints* streamlinesPoints = vtkPoints::New();
  vtkCellArray* streamlinesLines = vtkCellArray::New();
  streamlines->SetPoints(streamlinesPoints);
  streamlines->SetLines(streamlinesLines);

  streamlinesPoints->DeepCopy(splineFilter->GetOutput()->GetPoints());
  
  int numberOfCells = resampledInput->GetNumberOfCells();

  vtkIdList* inputCellIds = vtkIdList::New();
  vtkIdList* inputLabelIds = vtkIdList::New();
  int currentLabelId = -1;
  for (int i=0; i<numberOfCells; i++)
    {
    bool isCluster = false;
    if (i%(numberOfCells/numberOfClusters) == 0)
      {
      isCluster = true;
      currentLabelId++;
      }

    vtkCell* line = resampledInput->GetCell(i);
    int numberOfLinePoints = line->GetNumberOfPoints();
    for (int j=0; j<numberOfLinePoints; j++)
      {
      if (j%numberOfChunkPoints == 0)
        {
        int numberOfChunkLinePoints = numberOfChunkPoints;
        if (numberOfLinePoints-j < numberOfChunkPoints)
          {
          numberOfChunkLinePoints = numberOfLinePoints-j;
          }
        int id = streamlinesLines->InsertNextCell(numberOfChunkLinePoints);
        if (isCluster)
          {
          inputCellIds->InsertNextId(id);
          inputLabelIds->InsertNextId(currentLabelId);
          }
        }
      int pointId = line->GetPointId(j);
      streamlinesLines->InsertCellPoint(pointId);
      }
    }

  numberOfCells = streamlines->GetNumberOfCells();
  
  numberOfClusters = inputCellIds->GetNumberOfIds();

  vtkPolyData* clusterCenters = vtkPolyData::New();
  vtkPoints* clusterCenterPoints = vtkPoints::New();
  vtkCellArray* clusterCenterLines = vtkCellArray::New();
  clusterCenters->SetPoints(clusterCenterPoints);
  clusterCenters->SetLines(clusterCenterLines);

  vtkIntArray* centerLabelArray = vtkIntArray::New();
  centerLabelArray->SetName("Label");

  for (int i=0; i<numberOfClusters; i++)
    {
    vtkCell* line = streamlines->GetCell(inputCellIds->GetId(i));
    int numberOfLinePoints = line->GetNumberOfPoints();
    clusterCenterLines->InsertNextCell(numberOfLinePoints);
    centerLabelArray->InsertNextValue(inputLabelIds->GetId(i));
    for (int j=0; j<numberOfLinePoints; j++)
      {
      double point[3];
      streamlines->GetPoint(line->GetPointId(j),point);
      int pointId = clusterCenterPoints->InsertNextPoint(point);
      clusterCenterLines->InsertCellPoint(pointId);
      }
    }

  clusterCenters->GetCellData()->AddArray(centerLabelArray);

//  vtkDoubleArray* covariance = vtkDoubleArray::New();
//  covariance->SetName("PointCovariance");
//  covariance->SetNumberOfComponents(3);
//  covariance->SetNumberOfTuples(clusterCenterPoints->GetNumberOfPoints());

//  clusterCenters->GetPointData()->AddArray(covariance);

//  vtkPointLocator* pointLocator = vtkPointLocator::New();
//  pointLocator->SetDataSet(clusterCenters);
//  pointLocator->BuildLocator();

  vtkIntArray* labelArray = vtkIntArray::New();
  labelArray->SetName("Label");
  labelArray->SetNumberOfValues(numberOfCells);

  vtkDoubleArray* distance = vtkDoubleArray::New();
  distance->SetName("Distance");
  distance->SetNumberOfTuples(numberOfCells);

  for (int i=0; i<numberOfCells; i++)
    {
    double point[3], clusterCenterPoint[3];
    vtkCell* streamline = streamlines->GetCell(i);
    double minClusterCenterDistance = VTK_VMTK_LARGE_DOUBLE; 
    int minClusterCenterDistanceId = -1;
    for (int j=0; j<numberOfClusters; j++)
      {
      vtkCell* clusterCenterLine = clusterCenters->GetCell(j);
      int numberOfClusterCenterLinePoints = clusterCenterLine->GetNumberOfPoints();
      double squaredDistanceSum = 0.0;
      int numberOfRepeatedMatches = 0;
      int numberOfStreamlinePoints = streamline->GetNumberOfPoints();
      vtkIdList* correspondenceIds = vtkIdList::New();
      for (int ip=0; ip<numberOfStreamlinePoints; ip++)
        {
        streamlines->GetPoint(streamline->GetPointId(ip),point);
        double minPointDistanceSquared = VTK_VMTK_LARGE_DOUBLE;
        int minPointDistanceId = -1;
        for (int jp=0; jp<numberOfClusterCenterLinePoints; jp++)
          {
          clusterCenters->GetPoint(clusterCenterLine->GetPointId(jp),clusterCenterPoint);
          double distanceSquared = vtkMath::Distance2BetweenPoints(point,clusterCenterPoint);
          if (distanceSquared < minPointDistanceSquared)
            {
            minPointDistanceSquared = distanceSquared;
            minPointDistanceId = jp;
            }
          }
        squaredDistanceSum += minPointDistanceSquared;
        if (correspondenceIds->IsId(minPointDistanceId) != -1)
          {
          numberOfRepeatedMatches++;
          }
        else
          {
          correspondenceIds->InsertNextId(minPointDistanceId);
          }
        }
      correspondenceIds->Delete();
      double clusterCenterDistance = sqrt(squaredDistanceSum);
      double averageDistance = clusterCenterDistance / numberOfStreamlinePoints;
      double distancePenalty = (numberOfClusterCenterLinePoints - numberOfStreamlinePoints + numberOfRepeatedMatches) * averageDistance;
      double distance = (clusterCenterDistance + distancePenalty) / numberOfStreamlinePoints;
      if (distance < minClusterCenterDistance)
        {
        minClusterCenterDistance = distance;
        minClusterCenterDistanceId = inputLabelIds->GetId(j);
        }
      }
    labelArray->InsertValue(i,minClusterCenterDistanceId);
    distance->InsertValue(i,minClusterCenterDistance);
    }

  output->DeepCopy(streamlines);
  output->GetCellData()->AddArray(labelArray);
  output->GetCellData()->AddArray(distance);

  if (this->ClusterCenters)
    {
    this->ClusterCenters->Delete();
    this->ClusterCenters = NULL;
    }

  this->ClusterCenters = vtkPolyData::New();
  this->ClusterCenters->DeepCopy(clusterCenters);

  splineFilter->Delete(); 
  inputCellIds->Delete();
  clusterCenters->Delete();
  clusterCenterPoints->Delete();
  clusterCenterLines->Delete();
//  covariance->Delete();
//  pointLocator->Delete();
  labelArray->Delete();
  centerLabelArray->Delete();

  return 1;
}

void vtkvmtkStreamlineClusteringFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
