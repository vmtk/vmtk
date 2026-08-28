/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataFlowExtensionsFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/07 10:46:19 $
Version:   $Revision: 1.12 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataFlowExtensionsFilter.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkvmtkBoundaryReferenceSystems.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkvmtkMath.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkTransform.h"
#include "vtkPolyLine.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include <vector>


vtkStandardNewMacro(vtkvmtkPolyDataFlowExtensionsFilter);

namespace {

// Resamples a closed polygon into numberOfPoints points equally spaced along its perimeter. The
// first sample is placed at offsetRatio times the sampling step past the polygon's first point,
// so that offsetRatio 0.5 gives the set of points staggered halfway between the unshifted ones.
// The polygon is implicitly closed: the segment from its last to its first point is part of the
// outline. If companionPoints is given, it must have as many points as the polygon and is sampled
// at the same segment and parametric coordinate as the polygon itself, which pairs each sample
// with its counterpart on the companion polygon. Returns the perimeter of the polygon.
double ResampleClosedPolygon(vtkPoints* polygonPoints, int numberOfPoints, double offsetRatio, vtkPoints* resampledPoints, vtkPoints* companionPoints = nullptr, vtkPoints* resampledCompanionPoints = nullptr)
{
  vtkIdType numberOfPolygonPoints = polygonPoints->GetNumberOfPoints();

  std::vector<double> arcLength(numberOfPolygonPoints+1);
  arcLength[0] = 0.0;
  double point0[3], point1[3];
  vtkIdType i;
  for (i=0; i<numberOfPolygonPoints; i++)
    {
    polygonPoints->GetPoint(i,point0);
    polygonPoints->GetPoint((i+1)%numberOfPolygonPoints,point1);
    arcLength[i+1] = arcLength[i] + sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
    }
  double perimeter = arcLength[numberOfPolygonPoints];

  double step = perimeter / numberOfPoints;
  vtkIdType segmentId = 0;
  int j, k;
  double resampledPoint[3];
  for (j=0; j<numberOfPoints; j++)
    {
    // stations are increasing, so the segment they fall in can be tracked incrementally
    double station = (j + offsetRatio) * step;
    while (segmentId < numberOfPolygonPoints-1 && arcLength[segmentId+1] < station)
      {
      segmentId++;
      }
    double segmentLength = arcLength[segmentId+1] - arcLength[segmentId];
    double parametricCoordinate = 0.0;
    if (segmentLength > 0.0)
      {
      parametricCoordinate = (station - arcLength[segmentId]) / segmentLength;
      parametricCoordinate = parametricCoordinate < 0.0 ? 0.0 : (parametricCoordinate > 1.0 ? 1.0 : parametricCoordinate);
      }
    polygonPoints->GetPoint(segmentId,point0);
    polygonPoints->GetPoint((segmentId+1)%numberOfPolygonPoints,point1);
    for (k=0; k<3; k++)
      {
      resampledPoint[k] = point0[k] + parametricCoordinate * (point1[k] - point0[k]);
      }
    resampledPoints->InsertNextPoint(resampledPoint);

    if (companionPoints && resampledCompanionPoints)
      {
      companionPoints->GetPoint(segmentId,point0);
      companionPoints->GetPoint((segmentId+1)%numberOfPolygonPoints,point1);
      for (k=0; k<3; k++)
        {
        resampledPoint[k] = point0[k] + parametricCoordinate * (point1[k] - point0[k]);
        }
      resampledCompanionPoints->InsertNextPoint(resampledPoint);
      }
    }

  return perimeter;
}

// Samples a closed polygon along numberOfPoints rays cast from center within the plane orthogonal
// to planeNormal, the j-th ray pointing at (j + offsetRatio) * angleStep degrees from
// startDirection, so that the samples pair up with the points of a ring built at the same angular
// stations. The polygon is sampled where the ray leaves it, that is at the outermost crossing, and
// the sample is taken on companionPoints, which must have as many points as the polygon, at the
// same segment and parametric coordinate. Returns false, leaving sampledPoints incomplete, if any
// ray misses the polygon, which happens when the polygon is not star-shaped about center.
bool SampleClosedPolygonByAngle(vtkPoints* polygonPoints, vtkPoints* companionPoints, double center[3], double planeNormal[3], double startDirection[3], double angleStep, double offsetRatio, int numberOfPoints, vtkPoints* sampledPoints)
{
  vtkIdType numberOfPolygonPoints = polygonPoints->GetNumberOfPoints();

  double firstAxis[3], secondAxis[3];
  int k;
  for (k=0; k<3; k++)
    {
    firstAxis[k] = startDirection[k];
    }
  vtkMath::Cross(planeNormal,firstAxis,secondAxis);

  // in-plane coordinates of the polygon points relative to the center
  std::vector<double> firstCoordinate(numberOfPolygonPoints), secondCoordinate(numberOfPolygonPoints);
  double point[3], centerToPoint[3];
  vtkIdType i;
  for (i=0; i<numberOfPolygonPoints; i++)
    {
    polygonPoints->GetPoint(i,point);
    for (k=0; k<3; k++)
      {
      centerToPoint[k] = point[k] - center[k];
      }
    firstCoordinate[i] = vtkMath::Dot(centerToPoint,firstAxis);
    secondCoordinate[i] = vtkMath::Dot(centerToPoint,secondAxis);
    }

  double point0[3], point1[3], sampledPoint[3];
  int j;
  for (j=0; j<numberOfPoints; j++)
    {
    double angle = vtkMath::RadiansFromDegrees((j + offsetRatio) * angleStep);
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    // the ray is the positive half of the axis the polygon points are projected onto below; a
    // segment crosses it where its distance to the ray's line changes sign
    double bestRadius = 0.0;
    vtkIdType bestSegmentId = -1;
    double bestParametricCoordinate = 0.0;
    for (i=0; i<numberOfPolygonPoints; i++)
      {
      vtkIdType nextId = (i+1)%numberOfPolygonPoints;
      double distance0 = -firstCoordinate[i]*sinAngle + secondCoordinate[i]*cosAngle;
      double distance1 = -firstCoordinate[nextId]*sinAngle + secondCoordinate[nextId]*cosAngle;
      if (!((distance0 <= 0.0 && distance1 > 0.0) || (distance0 > 0.0 && distance1 <= 0.0)))
        {
        continue;
        }
      double parametricCoordinate = distance0 / (distance0 - distance1);
      double radius0 = firstCoordinate[i]*cosAngle + secondCoordinate[i]*sinAngle;
      double radius1 = firstCoordinate[nextId]*cosAngle + secondCoordinate[nextId]*sinAngle;
      double radius = radius0 + parametricCoordinate * (radius1 - radius0);
      if (radius > bestRadius)
        {
        bestRadius = radius;
        bestSegmentId = i;
        bestParametricCoordinate = parametricCoordinate;
        }
      }

    if (bestSegmentId == -1)
      {
      return false;
      }

    companionPoints->GetPoint(bestSegmentId,point0);
    companionPoints->GetPoint((bestSegmentId+1)%numberOfPolygonPoints,point1);
    for (k=0; k<3; k++)
      {
      sampledPoint[k] = point0[k] + bestParametricCoordinate * (point1[k] - point0[k]);
      }
    sampledPoints->InsertNextPoint(sampledPoint);
    }

  return true;
}

}

vtkvmtkPolyDataFlowExtensionsFilter::vtkvmtkPolyDataFlowExtensionsFilter()
{
  this->Centerlines = NULL;
  this->ExtensionRatio = 1.0;
  this->TransitionRatio = 0.5;
  this->ExtensionLength = 0.0;
  this->ExtensionRadius = 1.0;
  this->ExtensionLengthScaleFactors = NULL;
  this->CenterlineNormalEstimationDistanceRatio = 1.0;
  this->AdaptiveExtensionLength = 1;
  this->AdaptiveExtensionRadius = 1;
  this->PreserveCrossSectionShape = 0;
  this->NumberOfBoundaryPoints = 50;
  this->AdaptiveNumberOfBoundaryPoints = 0;
  this->BoundaryIds = NULL;
  this->Sigma = 1.0;
  this->SetExtensionModeToUseCenterlineDirection();
  this->SetInterpolationModeToThinPlateSpline();
}

vtkvmtkPolyDataFlowExtensionsFilter::~vtkvmtkPolyDataFlowExtensionsFilter()
{
  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }

  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }

  if (this->ExtensionLengthScaleFactors)
    {
    this->ExtensionLengthScaleFactors->Delete();
    this->ExtensionLengthScaleFactors = NULL;
    }
}

int vtkvmtkPolyDataFlowExtensionsFilter::RequestData(
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

  if (this->ExtensionMode == USE_CENTERLINE_DIRECTION)
    {
    if (!this->Centerlines)
      {
      vtkErrorMacro(<< "Centerlines not set.");
      return 1;
      }
    }

  vtkNew<vtkPoints> outputPoints;
  vtkNew<vtkCellArray> outputPolys;

  outputPoints->DeepCopy(input->GetPoints());
  outputPolys->DeepCopy(input->GetPolys());

  vtkNew<vtkvmtkPolyDataBoundaryExtractor> boundaryExtractor;
  boundaryExtractor->SetInputData(input);

  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  vtkNew<vtkPolyData> centerlines;
  vtkNew<vtkvmtkPolyBallLine> tube;
  vtkNew<vtkDoubleArray> zeroRadiusArray;

  if (this->ExtensionMode == USE_CENTERLINE_DIRECTION)
    {
    centerlines->DeepCopy(this->Centerlines);

    const char zeroRadiusArrayName[] = "ZeroRadiusArray";

    zeroRadiusArray->SetName(zeroRadiusArrayName);
    zeroRadiusArray->SetNumberOfTuples(centerlines->GetNumberOfPoints());
    zeroRadiusArray->FillComponent(0,0.0);
    
    centerlines->GetPointData()->AddArray(zeroRadiusArray);
  
    tube->SetInput(centerlines);
    tube->SetPolyBallRadiusArrayName(zeroRadiusArrayName);
    }

  input->BuildCells();
  input->BuildLinks();

  int i, k;
  for (i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    if (this->BoundaryIds)
      {
      if (this->BoundaryIds->IsId(i) == -1)
        {
        continue;
        }
      }
    
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));

    if (!boundary)
      {
      vtkErrorMacro(<<"Boundary not a vtkPolyLine");
      continue;
      }

    int numberOfBoundaryPoints = boundary->GetNumberOfPoints();

    vtkNew<vtkIdList> boundaryIds;
    int j;
    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      boundaryIds->InsertNextId(static_cast<vtkIdType>(vtkMath::Round(boundaries->GetPointData()->GetScalars()->GetComponent(boundary->GetPointId(j),0))));
      }
    
    double barycenter[3];
    double normal[3], outwardNormal[3];
    double meanRadius;

    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    meanRadius = vtkvmtkBoundaryReferenceSystems::ComputeBoundaryMeanRadius(boundary->GetPoints(),barycenter);
    vtkvmtkBoundaryReferenceSystems::ComputeBoundaryNormal(boundary->GetPoints(),barycenter,normal);
    vtkvmtkBoundaryReferenceSystems::OrientBoundaryNormalOutwards(input,boundaries,i,normal,outwardNormal);

    double flowExtensionNormal[3];
    flowExtensionNormal[0] = flowExtensionNormal[1] = flowExtensionNormal[2] = 0.0;  
 
    if (this->ExtensionMode == USE_CENTERLINE_DIRECTION)
      {
      tube->EvaluateFunction(barycenter);
  
      double centerlinePoint[3];
      vtkIdType cellId, subId;
      double pcoord;
      tube->GetLastPolyBallCenter(centerlinePoint);
      cellId = tube->GetLastPolyBallCellId();
      subId = tube->GetLastPolyBallCellSubId();
      pcoord = tube->GetLastPolyBallCellPCoord();
  
      vtkCell* centerline = centerlines->GetCell(cellId);
  
      vtkIdType pointId0, pointId1;
      double abscissa;
  
      double point0[3], point1[3];
  
      pointId0 = 0;
      abscissa = sqrt(vtkMath::Distance2BetweenPoints(centerlinePoint,centerline->GetPoints()->GetPoint(subId)));
      for (j=subId-1; j>=0; j--)
        {
        centerline->GetPoints()->GetPoint(j,point0);
        centerline->GetPoints()->GetPoint(j+1,point1);
        abscissa += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        if (abscissa > meanRadius * this->CenterlineNormalEstimationDistanceRatio)
          {
          pointId0 = j;
          break;
          }
        }
  
      pointId1 = centerline->GetNumberOfPoints()-1;
      abscissa = sqrt(vtkMath::Distance2BetweenPoints(centerlinePoint,centerline->GetPoints()->GetPoint(subId+1)));
      for (j=subId+1; j<centerline->GetNumberOfPoints()-2; j++)
        {
        centerline->GetPoints()->GetPoint(j,point0);
        centerline->GetPoints()->GetPoint(j+1,point1);
        abscissa += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        if (abscissa > meanRadius * this->CenterlineNormalEstimationDistanceRatio)
          {
          pointId1 = j+1;
          break;
          }
        }
  
      // use an approximating spline or smooth centerline points to better catch the trend in computing centerlineNormal?
  
      double centerlineNormal[3];
  
      centerline->GetPoints()->GetPoint(pointId0,point0);
      centerline->GetPoints()->GetPoint(pointId1,point1);
  
      double toleranceFactor = 1E-4;
  
      for (k=0; k<3; k++)
        {
        centerlineNormal[k] = 0.0;
        }
      if (sqrt(vtkMath::Distance2BetweenPoints(point1,centerlinePoint)) > toleranceFactor*meanRadius)
        {
        for (k=0; k<3; k++)
          {
          centerlineNormal[k] += point1[k] - centerlinePoint[k];
          }
        } 
      if (sqrt(vtkMath::Distance2BetweenPoints(centerlinePoint,point0)) > toleranceFactor*meanRadius)
        {
        for (k=0; k<3; k++)
          {
          centerlineNormal[k] += centerlinePoint[k] - point0[k];
          }
        }
  
      vtkMath::Normalize(centerlineNormal);
  
      for (k=0; k<3; k++)
        {
        flowExtensionNormal[k] = centerlineNormal[k];
        }
  
      if (vtkMath::Dot(outwardNormal,centerlineNormal) < 0.0)
        {
        for (k=0; k<3; k++)
          {
          flowExtensionNormal[k] *= -1.0;
          }
        }
      }
    else if (this->ExtensionMode == USE_NORMAL_TO_BOUNDARY)
      {
      for (k=0; k<3; k++)
        {
        flowExtensionNormal[k] = outwardNormal[k];
        }
      }
    else
      {
      vtkErrorMacro(<< "Invalid ExtensionMode.");
      return 1;
      }

    double extensionLength;

    if (this->AdaptiveExtensionLength)
      {
      extensionLength = meanRadius * this->ExtensionRatio;
      }
    else
      {
      extensionLength = this->ExtensionLength;
      }

    if (this->ExtensionLengthScaleFactors && i < this->ExtensionLengthScaleFactors->GetNumberOfTuples())
      {
      extensionLength *= this->ExtensionLengthScaleFactors->GetValue(i);
      }

    double point[3], extensionPoint[3];

    // Project the boundary onto the plane through the barycenter orthogonal to the extension
    // direction. This is the outline the extension is swept from when PreserveCrossSectionShape
    // is on, and its mean radius is the extension radius when AdaptiveExtensionRadius is on.
    vtkNew<vtkPoints> projectedBoundaryPoints;
    double meanProjectedRadius = 0.0;

    double barycenterToPoint[3];
    double outOfPlaneDistance;
    double projectedPoint[3];
    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      boundary->GetPoints()->GetPoint(j,point);
      for (k=0; k<3; k++)
        {
        barycenterToPoint[k] = point[k] - barycenter[k];
        }
      outOfPlaneDistance = vtkMath::Dot(barycenterToPoint,flowExtensionNormal);
      for (k=0; k<3; k++)
        {
        barycenterToPoint[k] -= outOfPlaneDistance*flowExtensionNormal[k];
        projectedPoint[k] = barycenter[k] + barycenterToPoint[k];
        }
      projectedBoundaryPoints->InsertNextPoint(projectedPoint);
      meanProjectedRadius += vtkMath::Norm(barycenterToPoint);
      }
    meanProjectedRadius /= numberOfBoundaryPoints;

    double targetRadius = 0.0;

    if (this->AdaptiveExtensionRadius)
      {
      targetRadius = meanProjectedRadius;
      }
    else
      {
      targetRadius = this->ExtensionRadius;
      }

    if (this->PreserveCrossSectionShape && meanProjectedRadius < 1E-4 * meanRadius)
      {
      vtkWarningMacro(<<"Degenerate boundary outline, skipping flow extension for boundary "<<i);
      continue;
      }

    vtkNew<vtkIdList> newBoundaryIds;
    vtkNew<vtkIdList> previousBoundaryIds;
    vtkIdType pointId;

    previousBoundaryIds->DeepCopy(boundaryIds);

    // TODO: use area, not meanRadius as targetRadius

    int targetNumberOfBoundaryPoints = this->NumberOfBoundaryPoints;
    if (this->AdaptiveNumberOfBoundaryPoints)
      {
      targetNumberOfBoundaryPoints = numberOfBoundaryPoints;
      }

    double targetDistanceBetweenPoints = 0.0;

    vtkNew<vtkThinPlateSplineTransform> thinPlateSplineTransform;
    thinPlateSplineTransform->SetSigma(this->Sigma);
    thinPlateSplineTransform->SetBasisToR2LogR();
//    thinPlateSplineTransform->SetBasisToR();
    
    vtkNew<vtkPoints> sourceLandmarks;
    vtkNew<vtkPoints> targetLandmarks;

    vtkNew<vtkPoints> targetBoundaryPoints;
    vtkNew<vtkPoints> targetStaggeredBoundaryPoints;

    // The ramp interpolation modes need, for each point of the target cross-section, the point of
    // the real boundary it grows from. The pairing is built along with the target cross-section
    // itself, so that it is ordered and one-to-one whatever the shape of the boundary.
    bool useRampInterpolation = (this->InterpolationMode == USE_LINEAR_INTERPOLATION) || (this->InterpolationMode == USE_RAMP_INTERPOLATION);
    vtkNew<vtkPoints> rimBoundaryPoints;
    vtkNew<vtkPoints> rimStaggeredBoundaryPoints;

    int startNumberOfBoundaryPoints = numberOfBoundaryPoints;

    if (this->PreserveCrossSectionShape)
      {
      // The target cross-section is the boundary's own outline, projected onto the plane
      // orthogonal to the extension direction and uniformly resampled along its perimeter. Since
      // the samples follow the order of the boundary points, their winding matches the boundary's.
      // Sampling the boundary itself at the same stations pairs each target point with the point
      // it is the projection of.
      vtkPoints* rimPoints = useRampInterpolation ? rimBoundaryPoints.GetPointer() : nullptr;
      vtkPoints* rimStaggeredPoints = useRampInterpolation ? rimStaggeredBoundaryPoints.GetPointer() : nullptr;
      double perimeter = ResampleClosedPolygon(projectedBoundaryPoints,targetNumberOfBoundaryPoints,0.0,targetBoundaryPoints,boundary->GetPoints(),rimPoints);
      ResampleClosedPolygon(projectedBoundaryPoints,targetNumberOfBoundaryPoints,0.5,targetStaggeredBoundaryPoints,boundary->GetPoints(),rimStaggeredPoints);

      double targetPoint[3];
      double scale = 1.0;

      if (!this->AdaptiveExtensionRadius)
        {
        // scale the outline about the barycenter so that its mean radius is the requested
        // extension radius; the outline is left at its natural size when the radius is adaptive
        double meanResampledRadius = 0.0;
        for (j=0; j<targetNumberOfBoundaryPoints; j++)
          {
          targetBoundaryPoints->GetPoint(j,targetPoint);
          for (k=0; k<3; k++)
            {
            targetPoint[k] -= barycenter[k];
            }
          meanResampledRadius += vtkMath::Norm(targetPoint);
          }
        meanResampledRadius /= targetNumberOfBoundaryPoints;
        scale = targetRadius / meanResampledRadius;

        for (j=0; j<targetNumberOfBoundaryPoints; j++)
          {
          targetBoundaryPoints->GetPoint(j,targetPoint);
          for (k=0; k<3; k++)
            {
            targetPoint[k] = barycenter[k] + scale * (targetPoint[k] - barycenter[k]);
            }
          targetBoundaryPoints->SetPoint(j,targetPoint);
          targetStaggeredBoundaryPoints->GetPoint(j,targetPoint);
          for (k=0; k<3; k++)
            {
            targetPoint[k] = barycenter[k] + scale * (targetPoint[k] - barycenter[k]);
            }
          targetStaggeredBoundaryPoints->SetPoint(j,targetPoint);
          }
        }

      targetDistanceBetweenPoints = scale * perimeter / targetNumberOfBoundaryPoints;
      }
    else
      {
      targetDistanceBetweenPoints = 2.0 * sin (vtkMath::Pi() / targetNumberOfBoundaryPoints) * targetRadius;

      double baseRadialNormal[3];
      input->GetPoint(previousBoundaryIds->GetId(0),point);
      for (k=0; k<3; k++)
        {
        baseRadialNormal[k] = point[k] - barycenter[k];
        }
      double outOfPlaneComponent = vtkMath::Dot(baseRadialNormal,flowExtensionNormal);
      for (k=0; k<3; k++)
        {
        baseRadialNormal[k] -= outOfPlaneComponent * flowExtensionNormal[k];
        }
      vtkMath::Normalize(baseRadialNormal);
      double angle = 360.0 / targetNumberOfBoundaryPoints;
      vtkNew<vtkTransform> transform;
      transform->RotateWXYZ(0.5*angle,flowExtensionNormal);
      double testRadialNormal[3];
      transform->TransformPoint(baseRadialNormal,testRadialNormal);
      double cross[3], testCross[3], point1[3];
      vtkMath::Cross(baseRadialNormal,testRadialNormal,testCross);
      double dist = 0.0;
      int testId = 1;
      int numberOfPreviousBoundaryIds = previousBoundaryIds->GetNumberOfIds();
      while (dist < 1E-8 && testId < numberOfPreviousBoundaryIds)
        {
        input->GetPoint(previousBoundaryIds->GetId(testId),point1);
        dist = sqrt(vtkMath::Distance2BetweenPoints(point,point1));
        testId++;
        }
      double testRadialVector[3];
      for (k=0; k<3; k++)
        {
        testRadialVector[k] = point1[k] - barycenter[k];
        }
      vtkMath::Cross(baseRadialNormal,testRadialVector,cross);
      if (vtkMath::Dot(cross,testCross) < 0.0)
        {
        angle *= -1.0;
        transform->Identity();
        transform->RotateWXYZ(0.5*angle,flowExtensionNormal);
        }
      double radialVector[3];
      for (k=0; k<3; k++)
        {
        radialVector[k] = targetRadius * baseRadialNormal[k];
        }
      double targetPoint[3];
      for (j=0; j<targetNumberOfBoundaryPoints; j++)
        {
        for (k=0; k<3; k++)
          {
          targetPoint[k] = barycenter[k] + radialVector[k];
          }
        targetBoundaryPoints->InsertNextPoint(targetPoint);
        transform->TransformPoint(radialVector,radialVector);
        for (k=0; k<3; k++)
          {
          targetPoint[k] = barycenter[k] + radialVector[k];
          }
        targetStaggeredBoundaryPoints->InsertNextPoint(targetPoint);
        transform->TransformPoint(radialVector,radialVector);
        }

      if (useRampInterpolation)
        {
        // Pair each point of the circle with the point of the boundary that lies on the same ray
        // from the barycenter, so that the transition is a purely radial morph. Matching by
        // proximity instead would collapse whole arcs of the circle onto the same few boundary
        // points on a strongly non-circular boundary, and would pair the two facing sides of a
        // flat one with each other.
        if (!SampleClosedPolygonByAngle(projectedBoundaryPoints,boundary->GetPoints(),barycenter,flowExtensionNormal,baseRadialNormal,angle,0.0,targetNumberOfBoundaryPoints,rimBoundaryPoints)
            || !SampleClosedPolygonByAngle(projectedBoundaryPoints,boundary->GetPoints(),barycenter,flowExtensionNormal,baseRadialNormal,angle,0.5,targetNumberOfBoundaryPoints,rimStaggeredBoundaryPoints))
          {
          vtkWarningMacro(<<"Boundary "<<i<<" is not star-shaped about its barycenter, pairing it with the extension by arc length instead of by angle");
          rimBoundaryPoints->Initialize();
          rimStaggeredBoundaryPoints->Initialize();
          vtkNew<vtkPoints> discardedPoints;
          ResampleClosedPolygon(projectedBoundaryPoints,targetNumberOfBoundaryPoints,0.0,discardedPoints,boundary->GetPoints(),rimBoundaryPoints);
          discardedPoints->Initialize();
          ResampleClosedPolygon(projectedBoundaryPoints,targetNumberOfBoundaryPoints,0.5,discardedPoints,boundary->GetPoints(),rimStaggeredBoundaryPoints);
          }
        }
      }

    // Displacement taking each point of the target cross-section onto the point of the real
    // boundary it is paired with. Fading it out over the transition length is what the ramp
    // interpolation modes do, so the extension starts on the real boundary and reaches the target
    // cross-section exactly at the end of the transition, whatever the shape of the boundary.
    std::vector<double> displacement, staggeredDisplacement;

    if (useRampInterpolation)
      {
      displacement.resize(3*targetNumberOfBoundaryPoints);
      staggeredDisplacement.resize(3*targetNumberOfBoundaryPoints);
      double targetPoint[3], rimPoint[3];
      for (j=0; j<targetNumberOfBoundaryPoints; j++)
        {
        targetBoundaryPoints->GetPoint(j,targetPoint);
        rimBoundaryPoints->GetPoint(j,rimPoint);
        for (k=0; k<3; k++)
          {
          displacement[3*j+k] = rimPoint[k] - targetPoint[k];
          }
        targetStaggeredBoundaryPoints->GetPoint(j,targetPoint);
        rimStaggeredBoundaryPoints->GetPoint(j,rimPoint);
        for (k=0; k<3; k++)
          {
          staggeredDisplacement[3*j+k] = rimPoint[k] - targetPoint[k];
          }
        }
      }

    if (this->InterpolationMode == USE_THIN_PLATE_SPLINE_INTERPOLATION)
      {
      for (j=0; j<targetNumberOfBoundaryPoints; j++)
        {
        double firstBoundaryPoint[3], lastBoundaryPoint[3];
        targetBoundaryPoints->GetPoint(j,firstBoundaryPoint);
        double distance = 1E20;
        double currentPoint[3];
        for (int j2=0; j2<startNumberOfBoundaryPoints; j2++)
          {
          input->GetPoint(previousBoundaryIds->GetId(j2),currentPoint);
          double currentDistance = vtkMath::Distance2BetweenPoints(currentPoint,firstBoundaryPoint);
          if (currentDistance < distance)
            {
            distance = currentDistance;
            for (k=0; k<3; k++)
              {
              point[k] = currentPoint[k];
              }
            }
          }
        sourceLandmarks->InsertNextPoint(firstBoundaryPoint);
        targetLandmarks->InsertNextPoint(point);
        for (k=0; k<3; k++)
          { 
          lastBoundaryPoint[k] = firstBoundaryPoint[k] + extensionLength * this->TransitionRatio * flowExtensionNormal[k]; 
          }
        sourceLandmarks->InsertNextPoint(lastBoundaryPoint);
        targetLandmarks->InsertNextPoint(lastBoundaryPoint);
        }
      thinPlateSplineTransform->SetSourceLandmarks(sourceLandmarks);
      thinPlateSplineTransform->SetTargetLandmarks(targetLandmarks);
      }

    double transitionLength = extensionLength * this->TransitionRatio;
    int numberOfLayers = extensionLength / targetDistanceBetweenPoints;
    int numberOfTransitionLayers = transitionLength / targetDistanceBetweenPoints;
    int l;
    for (l=0; l<numberOfLayers; l++)
      {
      newBoundaryIds->Initialize();
      for (j=0; j<targetNumberOfBoundaryPoints; j++)
        {
        if (l%2 != 0)
          {
          targetBoundaryPoints->GetPoint(j,extensionPoint);
          }
        else
          {
          targetStaggeredBoundaryPoints->GetPoint(j,extensionPoint);
          }
        for (k=0; k<3; k++)
          {
          extensionPoint[k] += (l+1) * targetDistanceBetweenPoints * flowExtensionNormal[k];
          }
        if (l<numberOfTransitionLayers)
          {
          if (this->InterpolationMode == USE_THIN_PLATE_SPLINE_INTERPOLATION)
            {
            thinPlateSplineTransform->TransformPoint(extensionPoint,extensionPoint);
            }
          else
            {
            // fade the displacement onto the real boundary from full at the boundary to none at
            // the end of the transition; since the whole displacement is scaled by the same
            // weight, the transition spans exactly transitionLength however irregular the
            // boundary is
            double parametricHeight = (l+1) * targetDistanceBetweenPoints / transitionLength;
            parametricHeight = parametricHeight > 1.0 ? 1.0 : parametricHeight;
            double weight = 0.0;
            if (this->InterpolationMode == USE_LINEAR_INTERPOLATION)
              {
              weight = 1.0 - parametricHeight;
              }
            else
              {
              // smoothstep, flat at both ends, so that the extension leaves the boundary
              // tangentially and settles into the uniform tube without a crease
              weight = 1.0 - parametricHeight * parametricHeight * (3.0 - 2.0 * parametricHeight);
              }
            const std::vector<double>& layerDisplacement = (l%2 != 0) ? displacement : staggeredDisplacement;
            for (k=0; k<3; k++)
              {
              extensionPoint[k] += weight * layerDisplacement[3*j+k];
              }
            }
          }
        pointId = outputPoints->InsertNextPoint(extensionPoint);
        newBoundaryIds->InsertNextId(pointId);
        }

      if (l==0)
        {
        vtkIdType pts[3];
        int j2 = 0;
        for (j=0; j<targetNumberOfBoundaryPoints; j++)
          {
          double point0[3], point1[3], point2[3], point3[3];
          outputPoints->GetPoint(previousBoundaryIds->GetId(j2%startNumberOfBoundaryPoints),point0);
          outputPoints->GetPoint(previousBoundaryIds->GetId((j2+1)%startNumberOfBoundaryPoints),point1);
          outputPoints->GetPoint(newBoundaryIds->GetId(j),point2);
          outputPoints->GetPoint(newBoundaryIds->GetId((j+1)%targetNumberOfBoundaryPoints),point3);

          bool advance = false;
          if ((j==0) || (j==targetNumberOfBoundaryPoints-1) || (vtkMath::Distance2BetweenPoints(point0,point3) > vtkMath::Distance2BetweenPoints(point1,point2)))
            {
            advance = true;
            }
          if (j2 == startNumberOfBoundaryPoints)
            {
            advance = false;
            }
          while(advance) 
            {
            pts[0] = previousBoundaryIds->GetId(j2%startNumberOfBoundaryPoints);
            pts[1] = newBoundaryIds->GetId(j);
            pts[2] = previousBoundaryIds->GetId((j2+1)%startNumberOfBoundaryPoints);
            outputPolys->InsertNextCell(3,pts);
            j2 += 1;

            outputPoints->GetPoint(previousBoundaryIds->GetId(j2%startNumberOfBoundaryPoints),point0);
            outputPoints->GetPoint(previousBoundaryIds->GetId((j2+1)%startNumberOfBoundaryPoints),point1);
            outputPoints->GetPoint(newBoundaryIds->GetId(j),point2);
            outputPoints->GetPoint(newBoundaryIds->GetId((j+1)%targetNumberOfBoundaryPoints),point3);

            if (j2 == startNumberOfBoundaryPoints || vtkMath::Distance2BetweenPoints(point0,point3) < vtkMath::Distance2BetweenPoints(point1,point2))
              {
              advance = false;
              if (j2 < startNumberOfBoundaryPoints && j == targetNumberOfBoundaryPoints-1)
                {
                advance = true;
                }
              }
            }
          pts[0] = newBoundaryIds->GetId(j);
          pts[1] = newBoundaryIds->GetId((j+1)%targetNumberOfBoundaryPoints);
          pts[2] = previousBoundaryIds->GetId(j2%startNumberOfBoundaryPoints);
          outputPolys->InsertNextCell(3,pts);
          }
        }
      else
        {
        vtkIdType pts[3];
        for (j=0; j<targetNumberOfBoundaryPoints; j++)
          {
          if (l%2 != 0)
            {
            pts[0] = newBoundaryIds->GetId(j);
            pts[1] = newBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            pts[2] = previousBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            outputPolys->InsertNextCell(3,pts);
  
            pts[0] = previousBoundaryIds->GetId(j);
            pts[1] = newBoundaryIds->GetId(j);
            pts[2] = previousBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            outputPolys->InsertNextCell(3,pts);
            }
          else
            {
            pts[0] = newBoundaryIds->GetId(j);
            pts[1] = newBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            pts[2] = previousBoundaryIds->GetId(j);
            outputPolys->InsertNextCell(3,pts);
    
            pts[0] = previousBoundaryIds->GetId(j);
            pts[1] = newBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            pts[2] = previousBoundaryIds->GetId((j-1+targetNumberOfBoundaryPoints)%targetNumberOfBoundaryPoints);
            outputPolys->InsertNextCell(3,pts);
            }
          }
        }

      previousBoundaryIds->DeepCopy(newBoundaryIds);
      }

    }

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);

  return 1;
}

void vtkvmtkPolyDataFlowExtensionsFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
