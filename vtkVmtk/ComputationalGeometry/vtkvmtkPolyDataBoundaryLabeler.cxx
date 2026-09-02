/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataBoundaryLabeler.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"

#include "vtkvmtkBoundaryLabels.h"
#include "vtkvmtkBoundaryReferenceSystems.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"

#include <algorithm>
#include <map>
#include <vector>

namespace
{

// How far the point of the ring nearest to, and furthest from, origin actually is. The furthest
// is what says whether the boundary lies entirely within a distance of the origin; the nearest is
// what ranks one boundary against another.
void DistancesFromPoint(vtkPolyData* surface, const std::vector<vtkIdType>& ring, const double origin[3], double& nearestDistance, double& furthestDistance)
{
  double nearestDistance2 = 0.0;
  double furthestDistance2 = 0.0;
  for (size_t j=0; j<ring.size(); j++)
    {
    double point[3];
    surface->GetPoint(ring[j],point);
    double distance2 = vtkMath::Distance2BetweenPoints(origin,point);
    if (j == 0 || distance2 < nearestDistance2)
      {
      nearestDistance2 = distance2;
      }
    if (distance2 > furthestDistance2)
      {
      furthestDistance2 = distance2;
      }
    }
  nearestDistance = sqrt(nearestDistance2);
  furthestDistance = sqrt(furthestDistance2);
}

}

vtkStandardNewMacro(vtkvmtkPolyDataBoundaryLabeler);

vtkvmtkPolyDataBoundaryLabeler::vtkvmtkPolyDataBoundaryLabeler()
{
  this->BoundaryLabelsArrayName = nullptr;
  this->SetBoundaryLabelsArrayName(vtkvmtkBoundaryLabels::GetDefaultBoundaryLabelsArrayName());
  this->BoundaryPointOrderArrayName = nullptr;
  this->SetBoundaryPointOrderArrayName(vtkvmtkBoundaryLabels::GetDefaultBoundaryPointOrderArrayName());

  this->LabelingMode = BOUNDARY_EXTRACTION_ORDER;
  this->Annular = false;
  this->AnnularOuterBoundaryOffset = 1000;

  this->PlaneOrigins = nullptr;
  this->PlaneNormals = nullptr;
  this->PlaneLabels = nullptr;
  this->MaximumDistanceFromPlane = 0.0;
  this->MaximumDistanceFromPlaneOrigin = 0.0;

  this->NumberOfBoundaries = 0;
  this->BoundaryLabels = vtkIdList::New();
  this->UnmatchedPlaneLabels = vtkIdList::New();
}

vtkvmtkPolyDataBoundaryLabeler::~vtkvmtkPolyDataBoundaryLabeler()
{
  this->SetBoundaryLabelsArrayName(nullptr);
  this->SetBoundaryPointOrderArrayName(nullptr);

  this->SetPlaneOrigins(nullptr);
  this->SetPlaneNormals(nullptr);
  this->SetPlaneLabels(nullptr);

  this->BoundaryLabels->Delete();
  this->BoundaryLabels = nullptr;
  this->UnmatchedPlaneLabels->Delete();
  this->UnmatchedPlaneLabels = nullptr;
}

int vtkvmtkPolyDataBoundaryLabeler::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // The mesh is untouched; only the two arrays are new, so pass everything through and add them
  // to the output's own point data rather than to the input's.
  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  this->NumberOfBoundaries = 0;
  this->BoundaryLabels->Initialize();
  this->UnmatchedPlaneLabels->Initialize();

  if (!this->BoundaryLabelsArrayName || !this->BoundaryLabelsArrayName[0]
      || !this->BoundaryPointOrderArrayName || !this->BoundaryPointOrderArrayName[0])
    {
    vtkErrorMacro(<<"Both BoundaryLabelsArrayName and BoundaryPointOrderArrayName have to be set.");
    return 1;
    }

  const vtkIdType invalidLabel = vtkvmtkBoundaryLabels::GetInvalidBoundaryLabel();
  vtkIdType numberOfPoints = input->GetNumberOfPoints();

  vtkNew<vtkIntArray> labelsArray;
  labelsArray->SetName(this->BoundaryLabelsArrayName);
  labelsArray->SetNumberOfTuples(numberOfPoints);
  labelsArray->FillComponent(0,static_cast<double>(invalidLabel));

  vtkNew<vtkIntArray> orderArray;
  orderArray->SetName(this->BoundaryPointOrderArrayName);
  orderArray->SetNumberOfTuples(numberOfPoints);
  orderArray->FillComponent(0,static_cast<double>(invalidLabel));

  vtkNew<vtkvmtkPolyDataBoundaryExtractor> boundaryExtractor;
  boundaryExtractor->SetInputData(input);
  boundaryExtractor->Update();
  vtkPolyData* boundaries = boundaryExtractor->GetOutput();
  vtkDataArray* boundaryPointIds = boundaries->GetPointData()->GetScalars();

  // The points of each boundary, as ids into the input, in the order the extractor walked them.
  std::vector<std::vector<vtkIdType> > rings;
  for (vtkIdType boundaryIndex=0; boundaryIndex<boundaries->GetNumberOfCells(); boundaryIndex++)
    {
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryIndex));
    if (!boundary || !boundaryPointIds)
      {
      continue;
      }
    vtkIdType numberOfBoundaryPoints = boundary->GetNumberOfPoints();
    std::vector<vtkIdType> ring;
    ring.reserve(numberOfBoundaryPoints);
    for (vtkIdType j=0; j<numberOfBoundaryPoints; j++)
      {
      ring.push_back(static_cast<vtkIdType>(vtkMath::Round(
        boundaryPointIds->GetComponent(boundary->GetPointId(j),0))));
      }
    rings.push_back(ring);
    }

  vtkIdType numberOfBoundaries = static_cast<vtkIdType>(rings.size());
  std::vector<vtkIdType> labels(rings.size(),invalidLabel);

  // Labels already in use, so that a boundary given a fresh one never collides with a label a
  // end point asked for, or with one that is on the surface already but on no boundary any more.
  vtkIdType highestLabelInUse = invalidLabel;

  vtkIdType numberOfPlanes = 0;
  if ((this->LabelingMode == CLOSEST_TO_PLANE_ORIGIN || this->LabelingMode == ON_PLANE)
      && this->PlaneOrigins)
    {
    numberOfPlanes = this->PlaneOrigins->GetNumberOfPoints();
    if (this->LabelingMode == ON_PLANE
        && this->PlaneNormals && this->PlaneNormals->GetNumberOfTuples() < numberOfPlanes)
      {
      numberOfPlanes = this->PlaneNormals->GetNumberOfTuples();
      }
    }
  std::vector<vtkIdType> planeLabels(numberOfPlanes,invalidLabel);
  std::vector<bool> planeMatched(numberOfPlanes,false);
  for (vtkIdType planeIndex=0; planeIndex<numberOfPlanes; planeIndex++)
    {
    planeLabels[planeIndex] = this->PlaneLabels && planeIndex < this->PlaneLabels->GetNumberOfIds()
      ? this->PlaneLabels->GetId(planeIndex) : planeIndex;
    if (planeLabels[planeIndex] > highestLabelInUse)
      {
      highestLabelInUse = planeLabels[planeIndex];
      }
    }

  switch (this->LabelingMode)
    {
    case CLOSEST_TO_PLANE_ORIGIN:
      {
      for (vtkIdType planeIndex=0; planeIndex<numberOfPlanes; planeIndex++)
        {
        double origin[3];
        this->PlaneOrigins->GetPoint(planeIndex,origin);
        vtkIdType nearestBoundary = -1;
        double nearestDistance = 0.0;
        for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
          {
          double boundaryNearestDistance, boundaryFurthestDistance;
          DistancesFromPoint(input,rings[boundaryIndex],origin,boundaryNearestDistance,boundaryFurthestDistance);
          // Entirely within, or not a candidate at all: one point of the boundary out beyond the
          // limit and it is passed over, however near the rest of it comes.
          if (this->MaximumDistanceFromPlaneOrigin > 0.0
              && boundaryFurthestDistance > this->MaximumDistanceFromPlaneOrigin)
            {
            continue;
            }
          if (nearestBoundary == -1 || boundaryNearestDistance < nearestDistance)
            {
            nearestBoundary = boundaryIndex;
            nearestDistance = boundaryNearestDistance;
            }
          }
        if (nearestBoundary == -1)
          {
          continue;
          }
        // A boundary an earlier plane already took is not handed over: the second plane is
        // reported unmatched instead, rather than silently moved to a boundary it did not choose.
        if (labels[nearestBoundary] != invalidLabel)
          {
          continue;
          }
        labels[nearestBoundary] = planeLabels[planeIndex];
        planeMatched[planeIndex] = true;
        }
      break;
      }
    case ON_PLANE:
      {
      if (this->MaximumDistanceFromPlane <= 0.0)
        {
        vtkErrorMacro(<<"MaximumDistanceFromPlane has to be positive in the OnPlane mode; no boundary was matched to a plane.");
        break;
        }
      if (!this->PlaneNormals)
        {
        vtkErrorMacro(<<"PlaneNormals has to be set in the OnPlane mode; no boundary was matched to a plane.");
        break;
        }
      // A plane at a time, taking the nearest of the boundaries that qualify for it. Doing it
      // the other way round - a boundary at a time, first plane that fits - would let two
      // boundaries lying in the same plane be told apart by nothing but which of them the
      // extractor happened to reach first, which is the dependence on mesh numbering this whole
      // arrangement exists to be rid of.
      for (vtkIdType planeIndex=0; planeIndex<numberOfPlanes; planeIndex++)
        {
        double origin[3], normal[3];
        this->PlaneOrigins->GetPoint(planeIndex,origin);
        this->PlaneNormals->GetTuple(planeIndex,normal);
        if (vtkMath::Normalize(normal) == 0.0)
          {
          continue;
          }

        vtkIdType nearestBoundary = -1;
        double nearestDistance = 0.0;
        vtkIdType numberOfQualifyingBoundaries = 0;
        for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
          {
          // A boundary an earlier plane already took carries that plane's label and is not on
          // offer to this one.
          if (labels[boundaryIndex] != invalidLabel)
            {
            continue;
            }
          // A plane is infinite and a surface is not, so lying in the plane is not on its own
          // enough: the boundary has to be near this origin too, or a boundary on another branch
          // that happens to be coplanar with this cut would be claimed by it.
          double boundaryNearestDistance, boundaryFurthestDistance;
          DistancesFromPoint(input,rings[boundaryIndex],origin,boundaryNearestDistance,boundaryFurthestDistance);
          if (this->MaximumDistanceFromPlaneOrigin > 0.0
              && boundaryFurthestDistance > this->MaximumDistanceFromPlaneOrigin)
            {
            continue;
            }
          // Every point of the boundary has to be in the plane, so that a boundary lying only
          // partly in it - a hole the surface already had, or another cut's - is left alone
          // rather than claimed by the wrong plane.
          bool inPlane = true;
          for (size_t j=0; j<rings[boundaryIndex].size(); j++)
            {
            double boundaryPoint[3];
            input->GetPoint(rings[boundaryIndex][j],boundaryPoint);
            double offset[3];
            for (int k=0; k<3; k++)
              {
              offset[k] = boundaryPoint[k] - origin[k];
              }
            if (fabs(vtkMath::Dot(offset,normal)) >= this->MaximumDistanceFromPlane)
              {
              inPlane = false;
              break;
              }
            }
          if (!inPlane)
            {
            continue;
            }
          numberOfQualifyingBoundaries++;
          if (nearestBoundary == -1 || boundaryNearestDistance < nearestDistance)
            {
            nearestBoundary = boundaryIndex;
            nearestDistance = boundaryNearestDistance;
            }
          }

        if (nearestBoundary == -1)
          {
          continue;
          }
        if (numberOfQualifyingBoundaries > 1)
          {
          // Answerable by geometry, but only just: say so, because it usually means
          // MaximumDistanceFromPlaneOrigin is looser than the surface warrants.
          vtkWarningMacro(<<numberOfQualifyingBoundaries<<" boundaries lie in the plane of the one labelled "
                          <<planeLabels[planeIndex]<<"; the nearest to its origin was taken.");
          }
        labels[nearestBoundary] = planeLabels[planeIndex];
        planeMatched[planeIndex] = true;
        }
      break;
      }
    case MATCH_EXISTING_LABELS:
      {
      vtkDataArray* existingLabels = input->GetPointData()->GetArray(this->BoundaryLabelsArrayName);
      if (existingLabels && existingLabels->GetNumberOfComponents() == 1
          && existingLabels->GetNumberOfTuples() == numberOfPoints)
        {
        for (vtkIdType pointId=0; pointId<numberOfPoints; pointId++)
          {
          vtkIdType label = static_cast<vtkIdType>(vtkMath::Round(existingLabels->GetComponent(pointId,0)));
          if (label > highestLabelInUse)
            {
            highestLabelInUse = label;
            }
          }
        // A boundary keeps the label a strict majority of its points agree on. Anything less is
        // not a boundary that came through a filter intact: a newly cut one is made of new
        // points, which carry no label of their own or, where a filter interpolated the array
        // onto them, one that no majority backs.
        std::map<vtkIdType,std::pair<vtkIdType,vtkIdType> > bestClaimByLabel;
        std::vector<vtkIdType> claimedLabel(rings.size(),invalidLabel);
        std::vector<vtkIdType> claimCount(rings.size(),0);
        for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
          {
          std::map<vtkIdType,vtkIdType> counts;
          for (size_t j=0; j<rings[boundaryIndex].size(); j++)
            {
            vtkIdType label = static_cast<vtkIdType>(vtkMath::Round(
              existingLabels->GetComponent(rings[boundaryIndex][j],0)));
            if (label >= 0)
              {
              counts[label]++;
              }
            }
          vtkIdType ringSize = static_cast<vtkIdType>(rings[boundaryIndex].size());
          for (std::map<vtkIdType,vtkIdType>::iterator count=counts.begin(); count!=counts.end(); ++count)
            {
            if (count->second * 2 > ringSize)
              {
              claimedLabel[boundaryIndex] = count->first;
              claimCount[boundaryIndex] = count->second;
              break;
              }
            }
          if (claimedLabel[boundaryIndex] == invalidLabel)
            {
            continue;
            }
          // A label two boundaries both claim - a cut that split one boundary into two, say -
          // can only stay with one of them.
          std::map<vtkIdType,std::pair<vtkIdType,vtkIdType> >::iterator best =
            bestClaimByLabel.find(claimedLabel[boundaryIndex]);
          if (best == bestClaimByLabel.end())
            {
            bestClaimByLabel[claimedLabel[boundaryIndex]] =
              std::make_pair(claimCount[boundaryIndex],boundaryIndex);
            }
          else
            {
            vtkWarningMacro(<<"More than one boundary carries the label "<<claimedLabel[boundaryIndex]
                            <<"; it stays with the one holding it most widely and the others are relabeled.");
            if (claimCount[boundaryIndex] > best->second.first)
              {
              best->second = std::make_pair(claimCount[boundaryIndex],boundaryIndex);
              }
            }
          }
        for (std::map<vtkIdType,std::pair<vtkIdType,vtkIdType> >::iterator best=bestClaimByLabel.begin();
             best!=bestClaimByLabel.end(); ++best)
          {
          labels[best->second.second] = best->first;
          }
        }
      break;
      }
    case BOUNDARY_EXTRACTION_ORDER:
    default:
      {
      for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
        {
        labels[boundaryIndex] = boundaryIndex;
        if (boundaryIndex > highestLabelInUse)
          {
          highestLabelInUse = boundaryIndex;
          }
        }
      break;
      }
    }

  for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
    {
    if (labels[boundaryIndex] > highestLabelInUse)
      {
      highestLabelInUse = labels[boundaryIndex];
      }
    }
  for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
    {
    if (labels[boundaryIndex] == invalidLabel)
      {
      labels[boundaryIndex] = ++highestLabelInUse;
      }
    }

  if (this->Annular)
    {
    this->LabelAnnularPairs(boundaries,labels);
    }

  for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
    {
    for (size_t j=0; j<rings[boundaryIndex].size(); j++)
      {
      labelsArray->SetValue(rings[boundaryIndex][j],static_cast<int>(labels[boundaryIndex]));
      orderArray->SetValue(rings[boundaryIndex][j],static_cast<int>(j));
      }
    this->BoundaryLabels->InsertNextId(labels[boundaryIndex]);
    }

  for (vtkIdType planeIndex=0; planeIndex<numberOfPlanes; planeIndex++)
    {
    if (!planeMatched[planeIndex])
      {
      this->UnmatchedPlaneLabels->InsertNextId(planeLabels[planeIndex]);
      }
    }

  this->NumberOfBoundaries = numberOfBoundaries;

  output->GetPointData()->AddArray(labelsArray);
  output->GetPointData()->AddArray(orderArray);

  return 1;
}

void vtkvmtkPolyDataBoundaryLabeler::LabelAnnularPairs(vtkPolyData* boundaries, std::vector<vtkIdType>& labels)
{
  vtkIdType numberOfBoundaries = boundaries->GetNumberOfCells();
  if (numberOfBoundaries < 2)
    {
    return;
    }

  // Barycenter and mean radius of each boundary: the first pairs them, the second says which of
  // a pair is the inner one.
  std::vector<std::vector<double> > barycenters(numberOfBoundaries,std::vector<double>(3,0.0));
  std::vector<double> meanRadii(numberOfBoundaries,0.0);
  for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
    {
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryIndex));
    if (!boundary)
      {
      continue;
      }
    double barycenter[3];
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    for (int j=0; j<3; j++)
      {
      barycenters[boundaryIndex][j] = barycenter[j];
      }
    meanRadii[boundaryIndex] = vtkvmtkBoundaryReferenceSystems::ComputeBoundaryMeanRadius(boundary->GetPoints(),barycenter);
    }

  // Each boundary still unpaired takes the closest boundary still unpaired, which is how the
  // annular cappers pair them too.
  std::vector<bool> paired(numberOfBoundaries,false);
  for (vtkIdType boundaryIndex=0; boundaryIndex<numberOfBoundaries; boundaryIndex++)
    {
    if (paired[boundaryIndex])
      {
      continue;
      }
    vtkIdType closestIndex = -1;
    double closestDistance2 = 0.0;
    for (vtkIdType candidateIndex=boundaryIndex+1; candidateIndex<numberOfBoundaries; candidateIndex++)
      {
      if (paired[candidateIndex])
        {
        continue;
        }
      double distance2 = vtkMath::Distance2BetweenPoints(&barycenters[boundaryIndex][0],&barycenters[candidateIndex][0]);
      if (closestIndex == -1 || distance2 < closestDistance2)
        {
        closestIndex = candidateIndex;
        closestDistance2 = distance2;
        }
      }
    if (closestIndex == -1)
      {
      // An odd boundary with nothing left to pair with keeps the label it was given.
      vtkWarningMacro(<<"Annular labeling: the boundary labelled "<<labels[boundaryIndex]
                      <<" has no partner, so it keeps its label and no outer label is derived from it.");
      break;
      }

    paired[boundaryIndex] = true;
    paired[closestIndex] = true;

    vtkIdType innerIndex = boundaryIndex;
    vtkIdType outerIndex = closestIndex;
    if (meanRadii[outerIndex] < meanRadii[innerIndex])
      {
      std::swap(innerIndex,outerIndex);
      }

    // The inner boundary keeps the label it was given; the outer one is named after it, so the
    // pair reads as one vessel end and the inner label is the lower of the two.
    labels[outerIndex] = labels[innerIndex] + this->AnnularOuterBoundaryOffset;
    }
}

void vtkvmtkPolyDataBoundaryLabeler::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
