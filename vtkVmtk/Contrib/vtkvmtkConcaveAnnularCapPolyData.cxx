/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkConcaveAnnularCapPolyData.cxx,v $
Language:  C++
Date:      $Date: 2012/09/19 $
Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkVersion.h"

#include <limits>


vtkStandardNewMacro(vtkvmtkConcaveAnnularCapPolyData);

namespace {
void info(const std::string & s)
{
  std::cout << s << std::endl;
}

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
  this->BoundaryIds = NULL;
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
  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }
}

#include <vector>
#include <utility>
using std::vector;
using std::pair;
using std::make_pair;
typedef pair<vtkIdType,vtkIdType> IdPair;

vector<IdPair> build_closest_pairs(vtkSmartPointer<vtkPoints> barycenters, vtkIdList* boundaryIds)
{
  int numberOfBoundaries = barycenters->GetNumberOfPoints();

  vector<IdPair> pairs;

  // Allocate array for marking visited boundaries
  vector<bool> visited(numberOfBoundaries);
  if (boundaryIds)
    {
      // Skip ids not in boundaryIds list
      for (int i=0; i<numberOfBoundaries; i++)
        {
          visited[i] = true;
        }
      for (int i=0; i<boundaryIds->GetNumberOfIds(); i++)
      {
        visited[boundaryIds->GetId(i)] = false;
      }
    }
  else
    {
    // Skip nothing
    for (int i=0; i<numberOfBoundaries; i++)
      {
        visited[i] = false;
      }
    }

  // Find the closest pairs of boundaries by comparing barycenter distances
  for (int i=0; i<numberOfBoundaries; i++)
    {
    if (visited[i])
      {
        continue;
      }
    double barycenter[3];
    barycenters->GetPoint(i, barycenter);

    // Find which boundary j is closest to boundary i
    double minBarycenterDistance2 = std::numeric_limits<double>::max();
    vtkIdType closest = -1;
    for (int j=i+1; j<numberOfBoundaries; j++)
      {
      if (visited[j])
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
        closest = j;
        }
      }

    // Mark as visited and add pair
    //std::cout << "i, closest = " << i << ", " << closest << std::endl;
    visited[i] = true;
    visited[closest] = true;
    pairs.push_back(make_pair(i, closest));
    }
  return pairs;
}

IdPair find_closest_points(vtkPolyData * points, vtkSmartPointer<vtkIdList> pointIds0, vtkSmartPointer<vtkIdList> pointIds1)
{
  vtkIdType i0 = -1;
  vtkIdType i1 = -1;
  double minPointDistance2 = std::numeric_limits<double>::max();
  int n0 = pointIds0->GetNumberOfIds();
  int n1 = pointIds1->GetNumberOfIds();
  for (int j0=0; j0<n0; j0++)
    {
    double point0[3];
    points->GetPoint(pointIds0->GetId(j0), point0);
    for (int j1=0; j1<n1; j1++)
      {
      double point1[3];
      points->GetPoint(pointIds1->GetId(j1), point1);
      double distance2 = vtkMath::Distance2BetweenPoints(point0, point1);
      if (distance2 < minPointDistance2)
        {
        i0 = j0;
        i1 = j1;
        minPointDistance2 = distance2;
        }
      }
    }
  return make_pair(i0, i1);
}

int vtkvmtkConcaveAnnularCapPolyData::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // Administrative data type wrestling
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Minimal check that we get at least some data
  if ( input->GetNumberOfPoints() < 6 )
    {
    vtkErrorMacro(<< "Error: too few input points.");
    return 1;
    }
  input->BuildLinks();

  // Allocate output objects
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

  // Extract all boundaries (polylines) from input mesh
  info("Finding boundaries of input surface...");
  vtkSmartPointer<vtkvmtkPolyDataBoundaryExtractor> boundaryExtractor =
      vtkSmartPointer<vtkvmtkPolyDataBoundaryExtractor>::New();
  boundaryExtractor->SetInputData(input);
  boundaryExtractor->Update();
  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  int numberOfBoundaries = boundaries->GetNumberOfCells();
  std::cout << "Found " << numberOfBoundaries << " boundaries." << std::endl;

  // Compute barycenters for all boundaries
  info("Computing boundary barycenters...");
  vtkSmartPointer<vtkPoints> barycenters = vtkSmartPointer<vtkPoints>::New();
  barycenters->SetNumberOfPoints(numberOfBoundaries);
  for (int i=0; i<numberOfBoundaries; i++)
    {
    double barycenter[3];
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundaries->GetCell(i)->GetPoints(), barycenter);
    barycenters->SetPoint(i, barycenter);
    }

  // Build pairs of boundaries from closest barycenter pairs, limited to list of input ids if any
  info("Pairing boundaries...");
  if ( (this->BoundaryIds && this->BoundaryIds->GetNumberOfIds() % 2)
       || (!this->BoundaryIds && numberOfBoundaries % 2) )
    {
    vtkErrorMacro(<< "Error: the number of boundaries must be even.");
    }
  vector<IdPair> boundaryPairs = build_closest_pairs(barycenters, this->BoundaryIds);

  // Loop over all boundary pairings uniquely
  for (size_t pairingCount=0; pairingCount<boundaryPairs.size(); ++pairingCount)
    {
    vtkIdType i0 = boundaryPairs[pairingCount].first;
    vtkIdType i1 = boundaryPairs[pairingCount].second;

    // These are point ids for all boundaries globally, pointing into global vertex array
    info("Finding boundary point ids...");
    vtkIdTypeArray * boundaryPointIdsArray = vtkIdTypeArray::SafeDownCast(boundaries->GetPointData()->GetScalars());

    // Collect (global!) point ids for boundary 0 in current pairing into a contiguous array
    vtkPolyLine* boundary0 = vtkPolyLine::SafeDownCast(boundaries->GetCell(i0));
    vtkIdType numberOfBoundaryPoints0 = boundary0->GetNumberOfPoints();
    if (numberOfBoundaryPoints0 < 8)
      {
      vtkErrorMacro(<< "Error: Assuming at least 8 boundary points!");
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
      vtkErrorMacro(<< "Error: Assuming at least 8 boundary points!");
      }
    vtkSmartPointer<vtkIdList> boundaryPointIds1 = vtkSmartPointer<vtkIdList>::New();
    boundaryPointIds1->SetNumberOfIds(numberOfBoundaryPoints1);
    for (int j=0; j<numberOfBoundaryPoints1; j++)
      {
      // Translation: local1[j] = global[local2global1[j]];
      boundaryPointIds1->SetId(j, boundaryPointIdsArray->GetValue(boundary1->GetPointId(j)));
      }

    // Find the two closest vertices between the boundaries
    info("Finding closest vertices...");
    IdPair starts = find_closest_points(input, boundaryPointIds0, boundaryPointIds1);
    vtkIdType i0start = starts.first;
    vtkIdType i1start = starts.second;

    // Compute direction of paired boundaries
    // TODO: Why was this numBP0/8 in annular case? Numerical robustness or something?
    int i0forward = (i0start + 1) % numberOfBoundaryPoints0;
    int i1forward = (i1start + 1) % numberOfBoundaryPoints1;
    double cross[2][3];
    double barycenter[3];
    double startingPoint[3];
    double pointForward[3];

    barycenters->GetPoint(i0, barycenter);
    input->GetPoint(boundaryPointIds0->GetId(i0start), startingPoint);
    input->GetPoint(boundaryPointIds0->GetId(i0forward), pointForward);
    cross_diff(startingPoint, pointForward, barycenter, cross[0]);

    barycenters->GetPoint(i1, barycenter);
    input->GetPoint(boundaryPointIds1->GetId(i1start), startingPoint);
    input->GetPoint(boundaryPointIds1->GetId(i1forward), pointForward);
    cross_diff(startingPoint, pointForward, barycenter, cross[1]);

    int dir0 = +1; // TODO: Does the absolute direction matter?
    bool backwards = vtkMath::Dot(cross[0], cross[1]) < 0.0;
    int dir1 = backwards ? dir0: -dir0;

    // Compute vectors in directions away from endpoints of polygon
    double nudgeFactor = 0.01;
    i0forward = (i0start + dir0 + numberOfBoundaryPoints0) % numberOfBoundaryPoints0;
    input->GetPoint(boundaryPointIds0->GetId(i0start), startingPoint);
    input->GetPoint(boundaryPointIds0->GetId(i0forward), pointForward);
    double nudgeVector0[3] = { nudgeFactor*(pointForward[0] - startingPoint[0]),
                               nudgeFactor*(pointForward[1] - startingPoint[1]),
                               nudgeFactor*(pointForward[2] - startingPoint[2]) };
    i1forward = (i1start - dir1 + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
    input->GetPoint(boundaryPointIds1->GetId(i1start), startingPoint);
    input->GetPoint(boundaryPointIds1->GetId(i1forward), pointForward);
    double nudgeVector1[3] = { nudgeFactor*(pointForward[0] - startingPoint[0]),
                               nudgeFactor*(pointForward[1] - startingPoint[1]),
                               nudgeFactor*(pointForward[2] - startingPoint[2]) };

    // Use vtkPolygon::Triangulate to mesh between boundary pair
    info("Setting up n-sided polygon...");
    // Initialize a vtkPolygon instance with list of boundary points
    int npts = numberOfBoundaryPoints0 + numberOfBoundaryPoints1 + 2;
    vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
    vtkPoints * polygonPoints = polygon->GetPoints();
    vtkIdList * polygonPointIds = polygon->GetPointIds();
    polygonPoints->SetNumberOfPoints(npts);
    polygonPointIds->SetNumberOfIds(npts);
    vector<vtkIdType> polygonPointIdToGlobalPointId(npts);
    // Add all vertices from the inner polygon in one
    // direction, starting and ending with i0start.
    int kk = 0;
    for (int k=0; k<=numberOfBoundaryPoints0; k++) // NB! Using <= to repeat first point.
      {
      vtkIdType bp0id = (i0start + dir0*k + numberOfBoundaryPoints0) % numberOfBoundaryPoints0;
      vtkIdType pid = boundaryPointIds0->GetId(bp0id);
      double point[3];
      input->GetPoint(pid, point);
      if (k == 0)
        {
        point[0] += nudgeVector0[0];
        point[1] += nudgeVector0[1];
        point[2] += nudgeVector0[2];
        }
      polygonPointIdToGlobalPointId[kk] = pid;
      polygonPointIds->SetId(kk,kk);
      polygonPoints->InsertPoint(kk, point);
      kk++;
      }
    // Continue by adding all vertices from the outer polygon in the
    // opposite direction, starting and ending with i1start.
    for (int k=0; k<=numberOfBoundaryPoints1; k++) // NB! Using <= to repeat first point.
      {
      vtkIdType bp1id = (i1start + dir1*k + numberOfBoundaryPoints1) % numberOfBoundaryPoints1;
      vtkIdType pid = boundaryPointIds1->GetId(bp1id);
      double point[3];
      input->GetPoint(pid, point);
      if (k == numberOfBoundaryPoints1)
        {
        point[0] += nudgeVector1[0];
        point[1] += nudgeVector1[1];
        point[2] += nudgeVector1[2];
        }
      polygonPointIdToGlobalPointId[kk] = pid;
      polygonPointIds->SetId(kk,kk);
      polygonPoints->InsertPoint(kk, point);
      kk++;
      }

    // Call polygon->Triangulate(...) to mesh the interior of the concave polygon
    info("Triangulating...");
    vtkSmartPointer<vtkIdList> outTris = vtkSmartPointer<vtkIdList>::New();
    int triRes = polygon->NonDegenerateTriangulate(outTris);
    if (triRes == 0)
      {
      vtkErrorMacro(<< "Error: Numerical degeneracy when triangulating endcaps surface! Code: " << triRes);
      }

    // Output of triangulation is now in outTris, outPoints, append to newPolys
    info("Appending datasets...");
    size_t nt = outTris->GetNumberOfIds()/3;
    for (size_t it=0; it<nt; ++it)
      {
      // Get mapped point ids for new triangle
      vtkIdType triangleIds[3] = { polygonPointIdToGlobalPointId[outTris->GetId(3*it+0)],
                                   polygonPointIdToGlobalPointId[outTris->GetId(3*it+1)],
                                   polygonPointIdToGlobalPointId[outTris->GetId(3*it+2)] };

      // Insert new triangle in endcaps mesh!
      newPolys->InsertNextCell(3, triangleIds);
      // Mark this new triangle with the pairing number (starting from 1) + given offset
      if (markCells)
        {
        cellEntityIdsArray->InsertNextValue(pairingCount + 1 + this->CellEntityIdOffset);
        }
      }
    } // end for loop over boundary pairs

  // Collect output datastructures
  output->SetPoints(newPoints);
  output->SetPolys(newPolys);
  output->GetPointData()->PassData(input->GetPointData());
  if (markCells)
    {
    output->GetCellData()->AddArray(cellEntityIdsArray);
    }
  info("Done!");

  return 1;
}

void vtkvmtkConcaveAnnularCapPolyData::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
