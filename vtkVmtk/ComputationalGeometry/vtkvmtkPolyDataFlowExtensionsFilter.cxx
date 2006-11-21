/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataFlowExtensionsFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/07 10:46:19 $
Version:   $Revision: 1.12 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataFlowExtensionsFilter, "$Revision: 1.12 $");
vtkStandardNewMacro(vtkvmtkPolyDataFlowExtensionsFilter);

vtkvmtkPolyDataFlowExtensionsFilter::vtkvmtkPolyDataFlowExtensionsFilter()
{
  this->Centerlines = NULL;
  this->ExtensionRatio = 1.0;
  this->TransitionRatio = 0.5;
  this->ExtensionLength = 0.0;
  this->CenterlineNormalEstimationDistanceRatio = 1.0;
  this->AdaptiveExtensionLength = 1;
  this->NumberOfBoundaryPoints = 50;
  this->BoundaryIds = NULL;
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

  if (!this->Centerlines)
    {
    vtkErrorMacro(<< "Centerlines not set.");
    return 1;
    }

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputPolys = vtkCellArray::New();

  outputPoints->DeepCopy(input->GetPoints());
  outputPolys->DeepCopy(input->GetPolys());

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
  boundaryExtractor->SetInput(input);
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  vtkPolyData* centerlines = vtkPolyData::New();
  centerlines->DeepCopy(this->Centerlines);

  const char zeroRadiusArrayName[] = "ZeroRadiusArray";

  vtkDoubleArray* zeroRadiusArray = vtkDoubleArray::New();
  zeroRadiusArray->SetName(zeroRadiusArrayName);
  zeroRadiusArray->SetNumberOfTuples(centerlines->GetNumberOfPoints());
  zeroRadiusArray->FillComponent(0,0.0);
  
  centerlines->GetPointData()->AddArray(zeroRadiusArray);

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(centerlines);
  tube->SetPolyBallRadiusArrayName(zeroRadiusArrayName);

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

    vtkIdList* boundaryIds = vtkIdList::New();
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

    int boundaryDirection = 1;
    if (vtkMath::Dot(normal,outwardNormal) > 0.0)
      {
      boundaryDirection = -1;
      }

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

    // TODO: use an approximating spline or smooth centerline points to better catch the trend in computing centerlineNormal

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

    double flowExtensionNormal[3];

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

    double extensionLength;

    if (this->AdaptiveExtensionLength)
      {
      extensionLength = meanRadius * this->ExtensionRatio;
      }
    else
      {
      extensionLength = this->ExtensionLength;
      }

    double point[3], extensionPoint[3];
    double barycenterToPoint[3];
    double distanceToBarycenter, outOfPlaneDistance, projectedDistanceToBarycenter;
    double projectedBarycenterToPoint[3];
    double projectedMeanRadius = 0.0;
    double maxOutOfPlaneDistance = 0.0;
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
        projectedBarycenterToPoint[k] = barycenterToPoint[k] - outOfPlaneDistance*flowExtensionNormal[k];
        }
      projectedMeanRadius += vtkMath::Norm(projectedBarycenterToPoint);
      if (outOfPlaneDistance > maxOutOfPlaneDistance)
        {
        maxOutOfPlaneDistance = outOfPlaneDistance;
        }
      }
    projectedMeanRadius /= numberOfBoundaryPoints;

    vtkIdList* newBoundaryIds = vtkIdList::New();
    vtkIdList* previousBoundaryIds = vtkIdList::New();
    vtkIdType pointId;

    double advancementRatio, factor;

    previousBoundaryIds->DeepCopy(boundaryIds);

    // TODO: use area, not meanRadius

    double targetDistanceBetweenPoints = 2.0 * sin (vtkMath::Pi() / this->NumberOfBoundaryPoints) * projectedMeanRadius;
    double initialDistanceBetweenPoints = 2.0 * sin (vtkMath::Pi() / numberOfBoundaryPoints) * projectedMeanRadius;

    double currentLength = 0.0;

    vtkThinPlateSplineTransform* thinPlateSplineTransform = vtkThinPlateSplineTransform::New();
    thinPlateSplineTransform->SetBasisToR2LogR();
    thinPlateSplineTransform->SetSigma(1.0);
    
    vtkPoints* sourceLandmarks = vtkPoints::New();
    vtkPoints* targetLandmarks = vtkPoints::New();

    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      double firstBoundaryPoint[3], lastBoundaryPoint[3];
      boundary->GetPoints()->GetPoint(j,point);
      for (k=0; k<3; k++)
        {
        barycenterToPoint[k] = point[k] - barycenter[k];
        }
      distanceToBarycenter = vtkMath::Norm(barycenterToPoint);
      outOfPlaneDistance = vtkMath::Dot(barycenterToPoint,flowExtensionNormal);
      for (k=0; k<3; k++)
        {
        projectedBarycenterToPoint[k] = barycenterToPoint[k] - outOfPlaneDistance*flowExtensionNormal[k];
        }
      vtkMath::Normalize(projectedBarycenterToPoint);
      for (k=0; k<3; k++)
        { 
        firstBoundaryPoint[k] = barycenter[k];
        firstBoundaryPoint[k] += projectedBarycenterToPoint[k] * projectedMeanRadius; 
        firstBoundaryPoint[k] += maxOutOfPlaneDistance * flowExtensionNormal[k]; 
        }
      sourceLandmarks->InsertNextPoint(firstBoundaryPoint);
      targetLandmarks->InsertNextPoint(point);
      for (k=0; k<3; k++)
        { 
        lastBoundaryPoint[k] = barycenter[k];
        lastBoundaryPoint[k] += projectedBarycenterToPoint[k] * projectedMeanRadius; 
        lastBoundaryPoint[k] += maxOutOfPlaneDistance * flowExtensionNormal[k]; 
        lastBoundaryPoint[k] += extensionLength * this->TransitionRatio * flowExtensionNormal[k]; 
        }
      sourceLandmarks->InsertNextPoint(lastBoundaryPoint);
      targetLandmarks->InsertNextPoint(lastBoundaryPoint);
      }
    thinPlateSplineTransform->SetSourceLandmarks(sourceLandmarks);
    thinPlateSplineTransform->SetTargetLandmarks(targetLandmarks);

    if (boundaryDirection == -1)
      {
      previousBoundaryIds->Initialize();
      for (j=0; j<numberOfBoundaryPoints; j++)
        {
        previousBoundaryIds->InsertNextId(boundaryIds->GetId(numberOfBoundaryPoints-j-1));
        }
      }
    
    while (true)
      {
      newBoundaryIds->Initialize();

      advancementRatio = currentLength / extensionLength; // TODO based on length

      if (advancementRatio > 1.0)
        {
        break;
        }

      factor = advancementRatio / this->TransitionRatio;
      factor = factor > 1.0 ?  1.0 : factor;

      double layerTargetDistanceBetweenPoints = targetDistanceBetweenPoints * factor + initialDistanceBetweenPoints * (1.0 - factor);

      for (k=0; k<3; k++)
        {
        barycenter[k] += layerTargetDistanceBetweenPoints * flowExtensionNormal[k];
        }

      currentLength += layerTargetDistanceBetweenPoints;

      double currentDistance = 0.0;

      int numberOfPreviousBoundaryPoints = previousBoundaryIds->GetNumberOfIds();

      for (j=0; j<numberOfPreviousBoundaryPoints; j++)
        {
        vtkIdType prevj = (numberOfPreviousBoundaryPoints+j-1) % numberOfPreviousBoundaryPoints;

        outputPoints->GetPoint(previousBoundaryIds->GetId(j),point);
        double previousPoint[3];
        outputPoints->GetPoint(previousBoundaryIds->GetId(prevj),previousPoint);

        currentDistance += sqrt(vtkMath::Distance2BetweenPoints(point,previousPoint));
#if 0
        if ((currentDistance < layerTargetDistanceBetweenPoints) && (j>0))
          {
          vtkIdType pts[3];
          pts[0] = previousBoundaryIds->GetId(j);
          pts[1] = newBoundaryIds->GetId(newBoundaryIds->GetNumberOfIds()-1);
          pts[2] = previousBoundaryIds->GetId(prevj);
          outputPolys->InsertNextCell(3,pts);
          continue;
          }
#endif
        for (k=0; k<3; k++)
          {
          barycenterToPoint[k] = point[k] - barycenter[k];
          }
        distanceToBarycenter = vtkMath::Norm(barycenterToPoint);
        outOfPlaneDistance = vtkMath::Dot(barycenterToPoint,flowExtensionNormal);
        for (k=0; k<3; k++)
          {
          projectedBarycenterToPoint[k] = barycenterToPoint[k] - outOfPlaneDistance*flowExtensionNormal[k];
          }
        projectedDistanceToBarycenter = vtkMath::Norm(projectedBarycenterToPoint);
#if 0        
        if (factor<1.0)
          {
          for (k=0; k<3; k++)
            {            
            extensionPoint[k] = point[k];
            // tend to circular
            extensionPoint[k] += projectedBarycenterToPoint[k] * (projectedMeanRadius/projectedDistanceToBarycenter - 1.0) * pow(factor,factorDegree);
            // tend to flat
            extensionPoint[k] += (maxOutOfPlaneDistance - outOfPlaneDistance) * flowExtensionNormal[k] * pow(factor,factorDegree);
            // move outwards
            extensionPoint[k] += layerTargetDistanceBetweenPoints * flowExtensionNormal[k];            
            }
          }
        else
          {
          vtkMath::Normalize(projectedBarycenterToPoint);
          for (k=0; k<3; k++)
            {
            extensionPoint[k] = barycenter[k];
            extensionPoint[k] += projectedBarycenterToPoint[k] * projectedMeanRadius;
            extensionPoint[k] += maxOutOfPlaneDistance * flowExtensionNormal[k];
            extensionPoint[k] += layerTargetDistanceBetweenPoints * flowExtensionNormal[k];
            }
          }
#endif
       
        vtkMath::Normalize(projectedBarycenterToPoint);
        for (k=0; k<3; k++)
          {
          extensionPoint[k] = barycenter[k];
          extensionPoint[k] += projectedBarycenterToPoint[k] * projectedMeanRadius;
          extensionPoint[k] += maxOutOfPlaneDistance * flowExtensionNormal[k];
          extensionPoint[k] += layerTargetDistanceBetweenPoints * flowExtensionNormal[k];
          }
       
        if (advancementRatio < this->TransitionRatio)
          {
          thinPlateSplineTransform->TransformPoint(extensionPoint,extensionPoint);
          }
          
        pointId = outputPoints->InsertNextPoint(extensionPoint);
        newBoundaryIds->InsertNextId(pointId);

        if (j==0)
          {
          currentDistance = 0.0;
          continue;
          }

        vtkIdType pts[3];

        pts[0] = newBoundaryIds->GetId(newBoundaryIds->GetNumberOfIds()-1);
        pts[1] = newBoundaryIds->GetId(newBoundaryIds->GetNumberOfIds()-2);
        pts[2] = previousBoundaryIds->GetId(prevj);
        outputPolys->InsertNextCell(3,pts);

        pts[0] = previousBoundaryIds->GetId(j);
        pts[1] = newBoundaryIds->GetId(newBoundaryIds->GetNumberOfIds()-1);
        pts[2] = previousBoundaryIds->GetId(prevj);
        outputPolys->InsertNextCell(3,pts);

        currentDistance = 0.0;
        }

      vtkIdType pts[3];

      pts[0] = newBoundaryIds->GetId(0);
      pts[1] = newBoundaryIds->GetId(newBoundaryIds->GetNumberOfIds()-1);
      pts[2] = previousBoundaryIds->GetId(previousBoundaryIds->GetNumberOfIds()-1);
      outputPolys->InsertNextCell(3,pts);

      pts[0] = previousBoundaryIds->GetId(0);
      pts[1] = newBoundaryIds->GetId(0);
      pts[2] = previousBoundaryIds->GetId(previousBoundaryIds->GetNumberOfIds()-1);
      outputPolys->InsertNextCell(3,pts);

      previousBoundaryIds->DeepCopy(newBoundaryIds);

      vtkTransform* transform = vtkTransform::New();
      int numberOfNewBoundaryIds = newBoundaryIds->GetNumberOfIds();
      if (advancementRatio > this->TransitionRatio)
        {
        double angleFactor = advancementRatio - this->TransitionRatio; 
        double rotationCenter[3];
        for (k=0; k<3; k++)
          {
          rotationCenter[k] = barycenter[k];
          rotationCenter[k] += maxOutOfPlaneDistance * flowExtensionNormal[k];
          rotationCenter[k] += layerTargetDistanceBetweenPoints * flowExtensionNormal[k];
          }
        double baseRadialNormal[3];
        outputPoints->GetPoint(newBoundaryIds->GetId(0),point);
        for (k=0; k<3; k++)
          {
          baseRadialNormal[k] = point[k] - rotationCenter[k];
          }
        vtkMath::Normalize(baseRadialNormal);
        for (j=0; j<numberOfNewBoundaryIds; j++)
          {
          vtkIdType id = newBoundaryIds->GetId(j);
          outputPoints->GetPoint(id,point);
          transform->Identity();
          double radialNormal[3];
          for (k=0; k<3; k++)
            {
            radialNormal[k] = point[k] - rotationCenter[k];
            }
          vtkMath::Normalize(radialNormal);
          double cross[3];
          vtkMath::Cross(flowExtensionNormal,baseRadialNormal,cross);
          double dot = vtkMath::Dot(radialNormal,cross);
          double actualAngle = vtkvmtkMath::AngleBetweenNormals(radialNormal,baseRadialNormal) * 180.0 / vtkMath::Pi();
          if (dot < 0.0 && actualAngle > 0.0)
            {
            actualAngle = 360.0 - actualAngle;
            }
          double targetAngle = double(j) / numberOfNewBoundaryIds * 360.0;
          double angle = angleFactor * (targetAngle - actualAngle);
          transform->Translate(rotationCenter);
          transform->RotateWXYZ(angle,flowExtensionNormal);
          transform->Translate(-rotationCenter[0],-rotationCenter[1],-rotationCenter[2]);
          outputPoints->SetPoint(id,transform->TransformPoint(point));
          }
        }
      transform->Delete();
      }

    newBoundaryIds->Delete();
    previousBoundaryIds->Delete();
    boundaryIds->Delete();
    thinPlateSplineTransform->Delete();
    sourceLandmarks->Delete();
    targetLandmarks->Delete();
    }

  output->SetPoints(outputPoints);
  output->SetPolys(outputPolys);

  outputPoints->Delete();
  outputPolys->Delete();

  tube->Delete();
  zeroRadiusArray->Delete();
  boundaryExtractor->Delete();

  return 1;
}

void vtkvmtkPolyDataFlowExtensionsFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
