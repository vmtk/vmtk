/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataStretchMappingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.11 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataStretchMappingFilter.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkContourFilter.h"
#include "vtkStripper.h"
// #include "vtkCardinalSpline.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyLine.h"
#include "vtkCleanPolyData.h"
#include "vtkMath.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkvmtkConstants.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataStretchMappingFilter);

vtkvmtkPolyDataStretchMappingFilter::vtkvmtkPolyDataStretchMappingFilter() 
{
  this->StretchedMappingArrayName = NULL;

  this->HarmonicMappingArrayName = NULL;
  this->GroupIdsArrayName = NULL;

  this->MetricArrayName = NULL;
  this->BoundaryMetricArrayName = NULL;

  this->UseBoundaryMetric = 0;

  this->MetricBoundsGapFactor = 2.0;
}

vtkvmtkPolyDataStretchMappingFilter::~vtkvmtkPolyDataStretchMappingFilter()
{
  if (this->StretchedMappingArrayName)
    {
    delete[] this->StretchedMappingArrayName;
    this->StretchedMappingArrayName = NULL;
    }

  if (this->HarmonicMappingArrayName)
    {
    delete[] this->HarmonicMappingArrayName;
    this->HarmonicMappingArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->MetricArrayName)
    {
    delete[] this->MetricArrayName;
    this->MetricArrayName = NULL;
    }

  if (this->BoundaryMetricArrayName)
    {
    delete[] this->BoundaryMetricArrayName;
    this->BoundaryMetricArrayName = NULL;
    }
}

int vtkvmtkPolyDataStretchMappingFilter::RequestData(
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

  if (!this->StretchedMappingArrayName)
    {
    vtkErrorMacro(<<"StretchedMappingArrayName not set.");
    return 1;
    }

  if (!this->HarmonicMappingArrayName)
    {
    vtkErrorMacro(<<"HarmonicMappingArrayName not set.");
    return 1;
    }

  vtkDataArray* harmonicMappingArray = input->GetPointData()->GetArray(this->HarmonicMappingArrayName);

  if (!harmonicMappingArray)
    {
    vtkErrorMacro(<<"HarmonicMappingArrayName with name specified does not exist.");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetPointData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist.");
    return 1;
    }

  if (!this->MetricArrayName)
    {
    vtkErrorMacro(<<"MetricArrayName not set.");
    return 1;
    }

  vtkDataArray* metricArray = input->GetPointData()->GetArray(this->MetricArrayName);

  if (!metricArray)
    {
    vtkErrorMacro(<<"MetricArrayName with name specified does not exist.");
    return 1;
    }

  vtkDataArray* boundaryMetricArray = NULL;
  if (this->UseBoundaryMetric)
    {
    if (!this->BoundaryMetricArrayName)
      {
      vtkErrorMacro(<<"BoundaryMetricArrayName not set.");
      return 1;
      }

    boundaryMetricArray = input->GetPointData()->GetArray(this->BoundaryMetricArrayName);
    
    if (!boundaryMetricArray)
      {
      vtkErrorMacro(<<"BoundaryMetricArrayName with name specified does not exist.");
      return 1;
      }
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  vtkDoubleArray* stretchedMapping = vtkDoubleArray::New();
  stretchedMapping->SetName(this->StretchedMappingArrayName);
  stretchedMapping->SetNumberOfComponents(1);
  stretchedMapping->SetNumberOfTuples(numberOfInputPoints);
  
  output->GetPointData()->AddArray(stretchedMapping);

  vtkIdList* groupIds = vtkIdList::New();
  vtkvmtkPolyDataBranchUtilities::GetGroupsIdList(input,this->GroupIdsArrayName,groupIds);
 
  int i, j, k;
  for (i=0; i<groupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = groupIds->GetId(i);
    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,groupId,true,cylinder);
    cylinder->GetPointData()->SetActiveScalars(this->HarmonicMappingArrayName);

    // before contouring, extract boundaries and look for boundary values there if UseBoundaryValues is 1

    vtkDataArray* cylinderHarmonicMappingArray = cylinder->GetPointData()->GetArray(this->HarmonicMappingArrayName);
    vtkDataArray* cylinderMetricArray = cylinder->GetPointData()->GetArray(this->MetricArrayName);
    vtkDataArray* cylinderBoundaryMetricArray = cylinder->GetPointData()->GetArray(this->BoundaryMetricArrayName);

    double boundaryMappings[2];
    double boundaryMetrics[2];
    double boundaryMetricUsefulBounds[2];

    // extract boundaries and look at values there.
    vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();    
#if (VTK_MAJOR_VERSION <= 5)
    boundaryExtractor->SetInput(cylinder);
#else
    boundaryExtractor->SetInputData(cylinder);
#endif
    boundaryExtractor->Update();
    
    int numberOfBoundaries = boundaryExtractor->GetOutput()->GetNumberOfCells();
    if (numberOfBoundaries != 2)
      {
      vtkErrorMacro(<<"Branch not topologically a cylinder.");
      return 1;
      }

    double contourMetricBounds[2][2];
    contourMetricBounds[0][0] = VTK_DOUBLE_MAX;
    contourMetricBounds[0][1] = VTK_DOUBLE_MIN;
    contourMetricBounds[1][0] = VTK_DOUBLE_MAX;
    contourMetricBounds[1][1] = VTK_DOUBLE_MIN;
    for (j=0; j<numberOfBoundaries; j++)
      {
      vtkCell* boundary = boundaryExtractor->GetOutput()->GetCell(j);
      if (boundary->GetNumberOfPoints() == 0)
        {
        vtkErrorMacro(<<"Degenerate branch found.");
        return 1;
        }
      vtkDataArray* boundaryPointIds = boundaryExtractor->GetOutput()->GetPointData()->GetScalars();
      vtkIdType boundaryPointId = static_cast<int>(boundaryPointIds->GetComponent(boundary->GetPointId(0),0));
      boundaryMappings[j] = cylinderHarmonicMappingArray->GetComponent(boundaryPointId,0);
      if (UseBoundaryMetric)
        {
        boundaryMetrics[j] = cylinderBoundaryMetricArray->GetComponent(boundaryPointId,0);
        }

      int numberOfBoundaryPoints = boundary->GetNumberOfPoints();
      for (k=0; k<numberOfBoundaryPoints; k++)
        {
        boundaryPointId = static_cast<int>(boundaryPointIds->GetComponent(boundary->GetPointId(k),0));
        double metricValue = cylinderMetricArray->GetComponent(boundaryPointId,0);
        if (metricValue < contourMetricBounds[j][0])
          {
          contourMetricBounds[j][0] = metricValue;
          }
        if (metricValue > contourMetricBounds[j][1])
          {
          contourMetricBounds[j][1] = metricValue;
          }
        }
      }

    if (boundaryMetrics[0] > boundaryMetrics[1])
      {
      double temp;
      temp = boundaryMappings[1];
      boundaryMappings[1] = boundaryMappings[0];
      boundaryMappings[0] = temp;
      temp = boundaryMetrics[1];
      boundaryMetrics[1] = boundaryMetrics[0];
      boundaryMetrics[0] = temp;
      temp = contourMetricBounds[1][0];
      contourMetricBounds[1][0] = contourMetricBounds[0][0];
      contourMetricBounds[0][0] = temp;
      temp = contourMetricBounds[1][1];
      contourMetricBounds[1][1] = contourMetricBounds[0][1];
      contourMetricBounds[0][1] = temp;
      }

//     boundaryMetricUsefulBounds[0] = contourMetricBounds[0][1];
//     boundaryMetricUsefulBounds[1] = contourMetricBounds[1][0];

    boundaryMetricUsefulBounds[0] = contourMetricBounds[0][0] + this->MetricBoundsGapFactor * (contourMetricBounds[0][1] - contourMetricBounds[0][0]);
    boundaryMetricUsefulBounds[1] = contourMetricBounds[1][1] - this->MetricBoundsGapFactor * (contourMetricBounds[1][1] - contourMetricBounds[1][0]);

    if (!this->UseBoundaryMetric)
      {
      boundaryMetrics[0] = contourMetricBounds[0][0];
      boundaryMetrics[1] = contourMetricBounds[1][1];
      }

    boundaryExtractor->Delete();

    vtkContourFilter* contourFilter = vtkContourFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
    contourFilter->SetInput(cylinder);
#else
    contourFilter->SetInputData(cylinder);
#endif
    contourFilter->SetValue(0,0.5);
    contourFilter->Update();

    int numberOfContours = static_cast<int>(10.0 * fabs(boundaryMetrics[1] - boundaryMetrics[0]) / (contourFilter->GetOutput()->GetLength()/2.0));

    double interval = 1.0/static_cast<double>(numberOfContours);
    for (j=0; j<=numberOfContours; j++)
      {
      contourFilter->SetValue(j,static_cast<double>(j) * interval);
      }
    contourFilter->Update();

    vtkStripper* contourStripper = vtkStripper::New();
#if (VTK_MAJOR_VERSION <= 5)
    contourStripper->SetInput(contourFilter->GetOutput());  
#else
    contourStripper->SetInputConnection(contourFilter->GetOutputPort());  
#endif
    contourStripper->Update();

    vtkPolyData* contours = contourStripper->GetOutput();

//     vtkCardinalSpline* stretchFunction = vtkCardinalSpline::New();
    vtkPiecewiseFunction* stretchFunction = vtkPiecewiseFunction::New();

    int numberOfComputedContours = contours->GetNumberOfCells();

    vtkDataArray* contourMetricArray = contours->GetPointData()->GetArray(this->MetricArrayName);

    for (j=0; j<numberOfComputedContours; j++)
      {
      vtkPolyLine* contour = vtkPolyLine::SafeDownCast(contours->GetCell(j));
      
      if (!contour)
        {
        continue;
        }

      double harmonicMappingValue = contours->GetPointData()->GetScalars()->GetComponent(contour->GetPointId(0),0);

      int numberOfContourPoints = contour->GetNumberOfPoints();

      double contourLength = 0.0;
      double metricIntegral = 0.0;

      for (k=0; k<numberOfContourPoints; k++)
        {
        int pointId0 = contour->GetPointId(k);
        int pointId1 = contour->GetPointId((k+1)%numberOfContourPoints);

        double point0[3];
        double point1[3];

        contours->GetPoint(pointId0,point0);
        contours->GetPoint(pointId1,point1);

        double length = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));

        if (length < VTK_VMTK_DOUBLE_TOL)
          {
          continue;
          }

        contourLength +=  length;

        double metricValue0 = contourMetricArray->GetComponent(pointId0,0);
        double metricValue1 = contourMetricArray->GetComponent(pointId1,0);

        metricIntegral += length * 0.5 * (metricValue0 + metricValue1);
        }

      if (contourLength < VTK_VMTK_DOUBLE_TOL)
        {
        continue;
        }

      double metricMean = metricIntegral / contourLength;

//       if (metricMean < boundaryMetricUsefulBounds[0] || metricMean > boundaryMetricUsefulBounds[1])
//         {
//         continue;
//         }

      if (metricMean < boundaryMetricUsefulBounds[0] || metricMean > boundaryMetricUsefulBounds[1])
        {
        continue;
        }

      stretchFunction->AddPoint(harmonicMappingValue, metricMean);
      }

    stretchFunction->AddPoint(boundaryMappings[0], boundaryMetrics[0]);
    stretchFunction->AddPoint(boundaryMappings[1], boundaryMetrics[1]);
    
//     derivative = (stretchFunction->Evaluate(blendingSkip) - stretchFunction->Evaluate(0.0)) / blendingSkip;
//     stretchFunction->AddPoint(-blendingSkip, stretchFunction->Evaluate(0.0) - derivative * blendingSkip);

//     derivative = (stretchFunction->Evaluate(1.0) - stretchFunction->Evaluate(1.0-interval)) / interval;
//     stretchFunction->AddPoint(1.0+blendingSkip, stretchFunction->Evaluate(1.0) + derivative * blendingSkip);

    for (j=0; j<numberOfInputPoints; j++)
      {
      vtkIdType currentGroupId = static_cast<int>(groupIdsArray->GetComponent(j,0));

      if (currentGroupId != groupId)
        {
        continue;
        }

      double harmonicMappingValue = harmonicMappingArray->GetComponent(j,0);

//       double stretchedMappingValue = stretchFunction->Evaluate(harmonicMappingValue);
      double stretchedMappingValue = stretchFunction->GetValue(harmonicMappingValue);

      stretchedMapping->SetComponent(j,0,stretchedMappingValue);
      }

    stretchFunction->Delete();
    contourFilter->Delete();
    contourStripper->Delete();
    cylinder->Delete();
    }

  groupIds->Delete();
  stretchedMapping->Delete();

  return 1;
}
