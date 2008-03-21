/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridCenterlineSections.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridCenterlineSections.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyLine.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkStripper.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkMath.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"

vtkCxxRevisionMacro(vtkvmtkUnstructuredGridCenterlineSections, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkvmtkUnstructuredGridCenterlineSections);

vtkvmtkUnstructuredGridCenterlineSections::vtkvmtkUnstructuredGridCenterlineSections()
{
  this->Centerlines = NULL;
  this->AdditionalNormalsPolyData = NULL;
  this->UpNormalsArrayName = NULL;
  this->AdditionalNormalsArrayName = NULL;
  this->AdditionalScalarsArrayName = NULL;
  this->TransformSections = 0;
  this->OriginOffset[0] = this->OriginOffset[1] = this->OriginOffset[2] = 0.0;
  this->VectorsArrayName = NULL;
}

vtkvmtkUnstructuredGridCenterlineSections::~vtkvmtkUnstructuredGridCenterlineSections()
{
  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }
  if (this->AdditionalNormalsPolyData)
    {
    this->AdditionalNormalsPolyData->Delete();
    this->AdditionalNormalsPolyData = NULL;
    }
  if (this->UpNormalsArrayName)
    {
    delete[] this->UpNormalsArrayName;
    this->UpNormalsArrayName = NULL;
    }
  if (this->AdditionalNormalsArrayName)
    {
    delete[] this->AdditionalNormalsArrayName;
    this->AdditionalNormalsArrayName = NULL;
    }
  if (this->AdditionalScalarsArrayName)
    {
    delete[] this->AdditionalScalarsArrayName;
    this->AdditionalScalarsArrayName = NULL;
    }
  if (this->VectorsArrayName)
    {
    delete[] this->VectorsArrayName;
    this->VectorsArrayName = NULL;
    }
}

int vtkvmtkUnstructuredGridCenterlineSections::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

double vtkvmtkUnstructuredGridCenterlineSections::ComputeAngle(double vector0[3], double vector1[3])
{
  double normal0[3], normal1[3]; 
  normal0[0] = vector0[0];
  normal0[1] = vector0[1];
  normal0[2] = vector0[2];
  normal1[0] = vector1[0];
  normal1[1] = vector1[1];
  normal1[2] = vector1[2];
  vtkMath::Normalize(normal0);
  vtkMath::Normalize(normal1);
  double sum[3], difference[3];
  sum[0] = normal0[0] + normal1[0];
  sum[1] = normal0[1] + normal1[1];
  sum[2] = normal0[2] + normal1[2];
  difference[0] = normal0[0] - normal1[0];
  difference[1] = normal0[1] - normal1[1];
  difference[2] = normal0[2] - normal1[2];
  double sumNorm = vtkMath::Norm(sum);
  double differenceNorm = vtkMath::Norm(difference);
  double angle = 2.0 * atan2(differenceNorm,sumNorm);
  return angle;
}

int vtkvmtkUnstructuredGridCenterlineSections::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  vtkDataArray* upNormalsArray = NULL;

  if (this->TransformSections && !this->UpNormalsArrayName)
    {
    vtkErrorMacro(<<"TransformSection is On and no UpNormalsArrayName specified!");
    return 1;
    }

  if (this->TransformSections && this->UpNormalsArrayName)
    {
    upNormalsArray = this->Centerlines->GetPointData()->GetArray(this->UpNormalsArrayName);
    if (!upNormalsArray)
      {
      vtkErrorMacro(<<"UpNormalsArray with name specified does not exist!");
      return 1;
      }
    if (upNormalsArray->GetNumberOfComponents() != 3)
      {
      vtkErrorMacro(<<"UpNormalsArray does not have 3 components!");
      return 1;
      }
    }

  vtkDataArray* additionalNormalsArray = NULL;
  vtkDataArray* additionalScalarsArray = NULL;
  if (this->AdditionalNormalsArrayName)
    {
    additionalNormalsArray = this->Centerlines->GetPointData()->GetArray(this->AdditionalNormalsArrayName);
    if (!additionalNormalsArray)
      {
      vtkErrorMacro(<<"AdditionalNormalsArray with name specified does not exist!");
      }
    if (additionalNormalsArray->GetNumberOfComponents() != 3)
      {
      vtkErrorMacro(<<"AdditionalNormalsArray does not have 3 components!");
      additionalNormalsArray = NULL;
      }
    if (this->AdditionalScalarsArrayName)
      {
      additionalScalarsArray = this->Centerlines->GetPointData()->GetArray(this->AdditionalScalarsArrayName);
      if (!additionalScalarsArray)
        {
        vtkErrorMacro(<<"AdditionalScalarsArray with name specified does not exist!");
        }
      }
    }

  if (this->AdditionalNormalsPolyData)
    {
    this->AdditionalNormalsPolyData->Delete();
    this->AdditionalNormalsPolyData = NULL;
    }

  this->AdditionalNormalsPolyData = vtkPolyData::New();
  vtkPoints* additionalNormalsPoints = vtkPoints::New();
  vtkCellArray* additionalNormalsVerts = vtkCellArray::New();
  vtkDoubleArray* additionalNormalsNormals = vtkDoubleArray::New();
  additionalNormalsNormals->SetNumberOfComponents(3);
  additionalNormalsNormals->SetName(this->AdditionalNormalsArrayName);
  vtkDoubleArray* additionalNormalsScalars = vtkDoubleArray::New();
  additionalNormalsScalars->SetNumberOfComponents(1);
  additionalNormalsScalars->SetName(this->AdditionalScalarsArrayName);
  this->AdditionalNormalsPolyData->SetPoints(additionalNormalsPoints);
  this->AdditionalNormalsPolyData->SetVerts(additionalNormalsVerts);
  this->AdditionalNormalsPolyData->GetPointData()->AddArray(additionalNormalsNormals);
  this->AdditionalNormalsPolyData->GetPointData()->AddArray(additionalNormalsScalars);

  vtkAppendPolyData* appendFilter = vtkAppendPolyData::New();

  int numberOfCenterlineCells = this->Centerlines->GetNumberOfCells();
  int i;
  for (i=0; i<numberOfCenterlineCells; i++)
  {
    vtkCell* centerlineCell = this->Centerlines->GetCell(i);
    vtkPoints* centerlineCellPoints = centerlineCell->GetPoints();
    int numberOfCellPoints = centerlineCellPoints->GetNumberOfPoints();
  
    int j;
    for (j=0; j<numberOfCellPoints; j++)
    {
      double point[3];
      centerlineCellPoints->GetPoint(j,point);
  
      double tangent[3];
      tangent[0] = tangent[1] = tangent[2] = 0.0;
 
      double upNormal[3];
      upNormal[0] = upNormal[1] = upNormal[2] = 0.0;
  
      double weightSum = 0.0;
      if (j>0)
      {
        double point0[3], point1[3];
        centerlineCellPoints->GetPoint(j-1,point0);
        centerlineCellPoints->GetPoint(j,point1);
        double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        tangent[0] += (point1[0] - point0[0]) / distance;
        tangent[1] += (point1[1] - point0[1]) / distance;
        tangent[2] += (point1[2] - point0[2]) / distance;
        weightSum += 1.0;
      }
  
      if (j<numberOfCellPoints-1)
      {
        double point0[3], point1[3];
        centerlineCellPoints->GetPoint(j,point0);
        centerlineCellPoints->GetPoint(j+1,point1);
        double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        tangent[0] += (point1[0] - point0[0]) / distance;
        tangent[1] += (point1[1] - point0[1]) / distance;
        tangent[2] += (point1[2] - point0[2]) / distance;
        weightSum += 1.0;
      }
  
      tangent[0] /= weightSum;
      tangent[1] /= weightSum;
      tangent[2] /= weightSum;
 
      upNormalsArray->GetTuple(centerlineCell->GetPointId(j),upNormal);
 
      vtkMath::Normalize(tangent);
      vtkMath::Normalize(upNormal);
  
      vtkPlane* plane = vtkPlane::New();
      plane->SetOrigin(point);
      plane->SetNormal(tangent);
    
      vtkCutter* cutter = vtkCutter::New();
      cutter->SetInput(input);
      cutter->SetCutFunction(plane);
      cutter->GenerateCutScalarsOff();
      cutter->SetValue(0,0.0);
      cutter->Update();
    
      vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
      cleaner->SetInput(cutter->GetOutput());
      cleaner->Update();
    
      if (cleaner->GetOutput()->GetNumberOfPoints() == 0)
        {
        plane->Delete();
        cutter->Delete();
        cleaner->Delete();
        continue;
        }
    
      vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
      connectivityFilter->SetInput(cleaner->GetOutput());
      connectivityFilter->SetExtractionModeToClosestPointRegion();
      connectivityFilter->SetClosestPoint(point);
      connectivityFilter->Update();

      if (!this->TransformSections)
        {
        appendFilter->AddInput(connectivityFilter->GetOutput());
        }
      else
        {
        double currentOrigin[3];
        currentOrigin[0] = point[0];
        currentOrigin[1] = point[1];
        currentOrigin[2] = point[2];
  
        double currentNormal[3];
        currentNormal[0] = tangent[0];
        currentNormal[1] = tangent[1];
        currentNormal[2] = tangent[2];
  
        double currentUpNormal[3];
        currentUpNormal[0] = upNormal[0];
        currentUpNormal[1] = upNormal[1];
        currentUpNormal[2] = upNormal[2];
  
        double targetOrigin[3];
        targetOrigin[0] = j * this->OriginOffset[0];
        targetOrigin[1] = j * this->OriginOffset[1];
        targetOrigin[2] = j * this->OriginOffset[2];
  
        double targetNormal[3];
        targetNormal[0] = 0.0;
        targetNormal[1] = 0.0;
        targetNormal[2] = 1.0;
  
        double targetUpNormal[3];
        targetUpNormal[0] = 0.0;
        targetUpNormal[1] = 1.0;
        targetUpNormal[2] = 0.0;
  
        vtkTransform* transform = vtkTransform::New();
        transform->PostMultiply();
  
        double translation0[3];
        translation0[0] = 0.0 - currentOrigin[0];
        translation0[1] = 0.0 - currentOrigin[1];
        translation0[2] = 0.0 - currentOrigin[2];
 
        double translation1[3];
        translation1[0] = targetOrigin[0] - 0.0;
        translation1[1] = targetOrigin[1] - 0.0;
        translation1[2] = targetOrigin[2] - 0.0;
  
        transform->Translate(translation0);
  
        double cross0[3];
        vtkMath::Cross(currentNormal,targetNormal,cross0);
        vtkMath::Normalize(cross0);
        double angle0 = this->ComputeAngle(currentNormal,targetNormal);
        angle0 = angle0 / (2.0*vtkMath::Pi()) * 360.0;
        transform->RotateWXYZ(angle0,cross0);
 
        double dot = vtkMath::Dot(currentUpNormal,currentNormal);
        double currentProjectedUpNormal[3];
        currentProjectedUpNormal[0] = currentUpNormal[0] - dot * currentNormal[0];
        currentProjectedUpNormal[1] = currentUpNormal[1] - dot * currentNormal[1];
        currentProjectedUpNormal[2] = currentUpNormal[2] - dot * currentNormal[2];
        vtkMath::Normalize(currentProjectedUpNormal);
 
        double transformedUpNormal[3];
        transform->TransformNormal(currentProjectedUpNormal,transformedUpNormal);

        double cross1[3];
        vtkMath::Cross(transformedUpNormal,targetUpNormal,cross1);
        vtkMath::Normalize(cross1);
        double angle1 = this->ComputeAngle(transformedUpNormal,targetUpNormal);
        angle1 = angle1 / (2.0*vtkMath::Pi()) * 360.0;
        transform->RotateWXYZ(angle1,cross1);
        transform->Translate(translation1);

        connectivityFilter->GetOutput()->GetPointData()->SetActiveVectors(this->VectorsArrayName);

        vtkTransformPolyDataFilter* transformFilter = vtkTransformPolyDataFilter::New();
        transformFilter->SetInput(connectivityFilter->GetOutput());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        appendFilter->AddInput(transformFilter->GetOutput());

        if (additionalNormalsArray)
          {
          double additionalNormal[3], projectedAdditionalNormal[3], transformedAdditionalNormal[3];
          additionalNormalsArray->GetTuple(centerlineCell->GetPointId(j),additionalNormal);
          double dot = vtkMath::Dot(additionalNormal,currentNormal);
          projectedAdditionalNormal[0] = additionalNormal[0] - dot * currentNormal[0];
          projectedAdditionalNormal[1] = additionalNormal[1] - dot * currentNormal[1];
          projectedAdditionalNormal[2] = additionalNormal[2] - dot * currentNormal[2];
          vtkMath::Normalize(projectedAdditionalNormal);
          transform->TransformNormal(projectedAdditionalNormal,transformedAdditionalNormal);
          vtkIdType pointId = additionalNormalsPoints->InsertNextPoint(targetOrigin);
          additionalNormalsVerts->InsertNextCell(1);
          additionalNormalsVerts->InsertCellPoint(pointId);
          additionalNormalsNormals->InsertNextTuple(transformedAdditionalNormal);
          if (additionalScalarsArray)
            {
            double scalar = additionalScalarsArray->GetTuple1(centerlineCell->GetPointId(j));
            additionalNormalsScalars->InsertNextTuple1(scalar);
            }
          }

        transform->Delete();
        transformFilter->Delete();
        }

      plane->Delete();
      cutter->Delete();
      cleaner->Delete();
      connectivityFilter->Delete();
    }  
  }

  appendFilter->Update();
  output->DeepCopy(appendFilter->GetOutput());

  appendFilter->Delete();
  additionalNormalsPoints->Delete();
  additionalNormalsVerts->Delete();
  additionalNormalsNormals->Delete();
  additionalNormalsScalars->Delete();

  return 1;
}

void vtkvmtkUnstructuredGridCenterlineSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
