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
#include "vtkPolygon.h"
#include "vtkPolyLine.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include <limits>

vtkCxxRevisionMacro(vtkvmtkConcaveAnnularCapPolyData, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkvmtkConcaveAnnularCapPolyData);

namespace {
void cross_diff(const double start[3], const double forward[3], const double center[3], double cross[3])
{
  double vectorToStart[3] = { start[0] - center[0],
                              start[1] - center[1],
                              start[2] - center[2] };
  double vectorToForward[3] = { forward[0] - center[0],
                                forward[1] - center[1],
                                forward[2] - center[2] };
  vtkMath::Cross(vectorToStart, vectorToForward, cross);
}
}

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

  // Find all boundaries (polylines) in input mesh
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

  // Compute barycenters for all boundaries
  vtkSmartPointer<vtkPoints> barycenters = vtkSmartPointer<vtkPoints>::New();
  barycenters->SetNumberOfPoints(numberOfBoundaries);
  for (int i=0; i<numberOfBoundaries; i++)
    {
    double barycenter[3];
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundaries->GetCell(i)->GetPoints(), barycenter);
    barycenters->SetPoint(i, barycenter);
    }

  // Allocate boundary pairing array
  vtkSmartPointer<vtkIdList> boundaryPairings = vtkSmartPointer<vtkIdList>::New();
  boundaryPairings->SetNumberOfIds(numberOfBoundaries);
  for (int i=0; i<numberOfBoundaries; i++)
    {
    boundaryPairings->SetId(i, -1);
    }

  // Find the closest pairs of boundaries by comparing barycenter distances
  double minBarycenterDistance2 = std::numeric_limits<double>::max();
  for (int i=0; i<numberOfBoundaries-1; i++)
    {
    // Skip boundary i if already visited
    if (boundaryPairings->GetId(i) != -1 || boundaryPairings->IsId(i) != -1)
      {
      continue;
      }

    // Fetch barycenter i
    double barycenter[3];
    barycenters->GetPoint(i, barycenter);

    // Find which boundary j is closest to boundary i
    vtkIdType closestBoundaryId = -1;
    for (int j=i+1; j<numberOfBoundaries; j++)
      {
      // Skip boundary j if already visited
      if (boundaryPairings->GetId(j) != -1 || boundaryPairings->IsId(j) != -1)
        {
        continue;
        }

      // Fetch barycenter j
      double currentBarycenter[3];
      barycenters->GetPoint(j, currentBarycenter);

      // Pick boundary j if the distance to boundary i is the smallest so far
      double distance2 = vtkMath::Distance2BetweenPoints(barycenter,currentBarycenter);
      if (distance2 < minBarycenterDistance2)
        {
        minBarycenterDistance2 = distance2;
        closestBoundaryId = j;
        }
      }
    // Store bidirectional boundary pairing
    boundaryPairings->SetId(i, closestBoundaryId);
    boundaryPairings->SetId(closestBoundaryId, i);
    }

  // Allocate array for marking visited boundaries // TODO: What's wrong with just a vector<bool>?
  vtkSmartPointer<vtkIdList> visitedBoundaries = vtkSmartPointer<vtkIdList>::New();
  visitedBoundaries->SetNumberOfIds(numberOfBoundaries);
  for (int i=0; i<numberOfBoundaries; i++)
    {
    visitedBoundaries->SetId(i, -1);
    }
  // Loop over all boundary pairings uniquely
  int pairingCount = 0;
  for (int i0=0; i0<numberOfBoundaries; i0++)
    {
    int i1 = boundaryPairings->GetId(i0);
    // Skip if we have no no pairing for i0
    if (i1 == -1)
      {
      continue;
      }
    // Skip if already visited as part of earlier pairing
    if (visitedBoundaries->GetId(i0) != -1)
      {
      continue;
      }
    else
      {
      visitedBoundaries->SetId(i0,i0);
      visitedBoundaries->SetId(i1,i0);
      pairingCount++;
      }

    // These are point ids for all boundaries globally, pointing into global vertex array
    vtkIdTypeArray * boundaryPointIdsArray = vtkIdTypeArray::SafeDownCast(boundaries->GetPointData()->GetScalars());

    // Collect (global!) point ids for boundary 0 in current pairing into a contiguous array
    vtkPolyLine* boundary0 = vtkPolyLine::SafeDownCast(boundaries->GetCell(i0));
    vtkIdType numberOfBoundaryPoints0 = boundary0->GetNumberOfPoints();
    if (numberOfBoundaryPoints0 < 8)
      {
      vtkErrorMacro(<< "Assuming at least 8 boundary points!");
      }
    vtkSmartPointer<vtkIdList> boundaryPointIds0 = vtkSmartPointer<vtkIdList>::New();
    boundaryPointIds0->SetNumberOfIds(numberOfBoundaryPoints0);
    for (int j=0; j<numberOfBoundaryPoints0; j++)
      {
      // Translation: local0[j] = global[local2global0[j]];
      boundaryPointIds0->SetId(j, boundaryPointIdsArray->GetValue(boundary0->GetPointId(j)));
      }

    // Collect (global!) point ids for boundary 1 in current pairing into a contiguous array
    vtkPolyLine* boundary1 = vtkPolyLine::SafeDownCast(boundaries->GetCell(i1));
    vtkIdType numberOfBoundaryPoints1 = boundary1->GetNumberOfPoints();
    if (numberOfBoundaryPoints1 < 8)
      {
      vtkErrorMacro(<< "Assuming at least 8 boundary points!");
      }
    vtkSmartPointer<vtkIdList> boundaryPointIds1 = vtkSmartPointer<vtkIdList>::New();
    boundaryPointIds1->SetNumberOfIds(numberOfBoundaryPoints1);
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      // Translation: local1[j] = global[local2global1[j]];
      boundaryPointIds1->SetId(j, boundaryPointIdsArray->GetValue(boundary1->GetPointId(j)));
      }

    // FIXME: Need arbitrary point to be chosen on the inner boundary, or a
    //        double loop finding the minimal distance between _all_ points
    // Pick arbitrary point in boundary 0
    double startingPoint[3];
    input->GetPoint(boundaryPointIds0->GetId(0), startingPoint);
    // Find nearest point in boundary 1
    vtkIdType offset = -1;
    double minPointDistance2 = std::numeric_limits<double>::max();
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      double currentPoint[3];
      input->GetPoint(boundaryPointIds1->GetId(j),currentPoint);
      double distance2 = vtkMath::Distance2BetweenPoints(startingPoint, currentPoint);
      if (distance2 < minPointDistance2)
        {
        offset = j;
        minPointDistance2 = distance2;
        }
      }

    // Compute direction of paired boundaries
    // FIXME: This logic assumes convexity in both boundaries, right?
    double cross[2][3];
    double barycenter[3];
    //double startingPoint[3]; // Reuse from above
    double pointForward[3];

    int i0start = 0;
    int i0forward = numberOfBoundaryPoints0 / 8; // FIXME: Why /8?
    int i1start = 0;
    int i1forward = numberOfBoundaryPoints1 / 8; // FIXME: Why /8?

    barycenters->GetPoint(i0, barycenter);
    input->GetPoint(boundaryPointIds0->GetId(i0start), startingPoint);
    input->GetPoint(boundaryPointIds0->GetId(i0forward), pointForward);
    cross_diff(startingPoint, pointForward, barycenter, cross[0]);

    barycenters->GetPoint(i1, barycenter);
    input->GetPoint(boundaryPointIds1->GetId(i1start), startingPoint);
    input->GetPoint(boundaryPointIds1->GetId(i1forward), pointForward);
    cross_diff(startingPoint, pointForward, barycenter, cross[1]);

    bool backward = false;
    if (vtkMath::Dot(cross[0], cross[1]) < 0.0)
      {
      backward = true;
      }

    // Use vtkPolygon::Triangulate to mesh between boundary pair
    // - First find the two closest vertices j0,j1 in the inner and outer polygons
    int j0 = 0; // FIXME this is assumed 0 above, not sufficient
    int j1 = 0; // FIXME this is computed as 'offset' above, not sufficient
    int dir0 = 1; // FIXME direction?
    int dir1 = 1; // FIXME direction?
    // - Initialize a list of points to make up polygon
    int npts = numberOfBoundaryPoints0 + numberOfBoundaryPoints1 + 2;
    vtkIdType * polygonIds = new vtkIdType[npts];
    // - Add all vertices from the inner polygon in a clockwise direction,
    //   starting and ending with j0.
    int kk = 0;
    for (int k=0; k<numberOfBoundaryPoints0; k++)
      {
      polygonIds[kk++] = (j0 + dir0*k + numberOfBoundaryPoints0) % numberOfBoundaryPoints0;
      }
    polygonIds[kk++] = (j0 + 0) % numberOfBoundaryPoints0; // repeat first point
    // - Continue by adding all vertices from the outer polygon in a
    //   counterclockwise direction, starting and ending with j1.
    for (int k=0; k<numberOfBoundaryPoints0; k++)
      {
      polygonIds[kk++] = (j1 + dir1*k + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
      }
    polygonIds[kk++] = (j1 + 0) % numberOfBoundaryPoints1; // repeat first point
    // - Initialize a vtkPolygon instance
    vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
    polygon->Initialize(npts, polygonIds, newPoints);
    // - Call polygon->Triangulate(...)
    vtkSmartPointer<vtkIdList> outTris = vtkSmartPointer<vtkIdList>::New();
    vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
    polygon->Triangulate(0, outTris, outPoints);
    // - Output is outTris, outPoints, use as appropriate in this code
    // FIXME: Need to add outTris/outPoints to newPolys/newPoints? Is this straightforward?

    // Loop through all points on both boundaries,
    // and add triangles to endcaps mesh as we go
    int k0 = 0;
    int k1 = 0;
    while (k0 < numberOfBoundaryPoints0 || k1 < numberOfBoundaryPoints1)
      {
      // Advance point on boundary 0
      vtkIdType pointId0 = k0 % numberOfBoundaryPoints0;
      vtkIdType nextPointId0 = (pointId0 + 1) % numberOfBoundaryPoints0;

      // Advance point on boundary 1, possibly backwards
      vtkIdType pointId1, nextPointId1;
      if (backward)
        {
        pointId1 = (numberOfBoundaryPoints1 - k1 + offset + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
        nextPointId1 = (pointId1 - 1 + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
        }
      else
        {
        pointId1 = (k1 + offset) % numberOfBoundaryPoints1;
        nextPointId1 = (pointId1 + 1) % numberOfBoundaryPoints1;
        }

      // Get coordinates for current and next points on each boundary
      double point0[3], nextPoint0[3];
      double point1[3], nextPoint1[3];
      input->GetPoint(boundaryPointIds0->GetId(pointId0),point0);
      input->GetPoint(boundaryPointIds0->GetId(nextPointId0),nextPoint0);
      input->GetPoint(boundaryPointIds1->GetId(pointId1),point1);
      input->GetPoint(boundaryPointIds1->GetId(nextPointId1),nextPoint1);

      // Figure out which boundary point to advance,
      // minimizing length of new edge between boundaries
      bool next1 = false;
      if (vtkMath::Distance2BetweenPoints(point0,nextPoint1) < vtkMath::Distance2BetweenPoints(point1,nextPoint0))
        {
        next1 = true;
        }
      if (k1 == numberOfBoundaryPoints1)
        {
        next1 = false;
        }
      else if (k0 == numberOfBoundaryPoints0)
        {
        next1 = true;
        }

      // Figure out point ids for new triangle
      vtkIdType triangleIds[3] = {
          boundaryPointIds0->GetId(pointId0),
          boundaryPointIds1->GetId(pointId1),
          -2 };
      if (next1)
        {
        triangleIds[2] = boundaryPointIds1->GetId(nextPointId1);
        k1++;
        }
      else
        {
        triangleIds[2] = boundaryPointIds0->GetId(nextPointId0);
        k0++;
        }

      // Insert new triangle in endcaps mesh!
      newPolys->InsertNextCell(3, triangleIds);

      // Mark this new triangle with the pairing number (starting from 1) + given offset
      if (markCells)
        {
        cellEntityIdsArray->InsertNextValue(pairingCount + this->CellEntityIdOffset);
        }
      }
    }

  // Collect output datastructures
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
