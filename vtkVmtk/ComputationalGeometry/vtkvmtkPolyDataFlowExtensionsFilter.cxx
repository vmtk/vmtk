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
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataFlowExtensionsFilter);

vtkvmtkPolyDataFlowExtensionsFilter::vtkvmtkPolyDataFlowExtensionsFilter()
{
  this->Centerlines = NULL;
  this->ExtensionRatio = 1.0;
  this->TransitionRatio = 0.5;
  this->ExtensionLength = 0.0;
  this->ExtensionRadius = 1.0;
  this->CenterlineNormalEstimationDistanceRatio = 1.0;
  this->AdaptiveExtensionLength = 1;
  this->AdaptiveExtensionRadius = 1;
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

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputPolys = vtkCellArray::New();

  outputPoints->DeepCopy(input->GetPoints());
  outputPolys->DeepCopy(input->GetPolys());

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
  boundaryExtractor->SetInputData(input);

  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  vtkPolyData* centerlines = vtkPolyData::New();
  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  vtkDoubleArray* zeroRadiusArray = vtkDoubleArray::New();

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

    double point[3], extensionPoint[3];
    double targetRadius = 0.0;

    if (this->AdaptiveExtensionRadius)
      {
      double barycenterToPoint[3];
      double outOfPlaneDistance;
      double projectedBarycenterToPoint[3];
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
        targetRadius += vtkMath::Norm(projectedBarycenterToPoint);
        }
      targetRadius /= numberOfBoundaryPoints;
      }
    else
      {
      targetRadius = this->ExtensionRadius;
      }

    vtkIdList* newBoundaryIds = vtkIdList::New();
    vtkIdList* previousBoundaryIds = vtkIdList::New();
    vtkIdType pointId;

    previousBoundaryIds->DeepCopy(boundaryIds);

    // TODO: use area, not meanRadius as targetRadius

    int targetNumberOfBoundaryPoints = this->NumberOfBoundaryPoints;
    if (this->AdaptiveNumberOfBoundaryPoints)
      {
      targetNumberOfBoundaryPoints = numberOfBoundaryPoints;
      }

    double targetDistanceBetweenPoints = 2.0 * sin (vtkMath::Pi() / targetNumberOfBoundaryPoints) * targetRadius;

    vtkThinPlateSplineTransform* thinPlateSplineTransform = vtkThinPlateSplineTransform::New();
    thinPlateSplineTransform->SetSigma(this->Sigma);
    thinPlateSplineTransform->SetBasisToR2LogR();
//    thinPlateSplineTransform->SetBasisToR();
    
    vtkPoints* sourceLandmarks = vtkPoints::New();
    vtkPoints* targetLandmarks = vtkPoints::New();

    vtkPoints* targetBoundaryPoints = vtkPoints::New();
    vtkPoints* targetStaggeredBoundaryPoints = vtkPoints::New();

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
    int startNumberOfBoundaryPoints = numberOfBoundaryPoints;
    double angle = 360.0 / targetNumberOfBoundaryPoints;
    vtkTransform* transform = vtkTransform::New();
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
    transform->Delete();
    
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

    int numberOfLayers = extensionLength / targetDistanceBetweenPoints;
    int numberOfTransitionLayers = (extensionLength * this->TransitionRatio) / targetDistanceBetweenPoints;
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
          if (this->InterpolationMode == USE_LINEAR_INTERPOLATION)
            {
            }
          else if (this->InterpolationMode == USE_THIN_PLATE_SPLINE_INTERPOLATION)
            {
            thinPlateSplineTransform->TransformPoint(extensionPoint,extensionPoint);
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

    targetBoundaryPoints->Delete();
    targetStaggeredBoundaryPoints->Delete();
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

void vtkvmtkPolyDataFlowExtensionsFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
