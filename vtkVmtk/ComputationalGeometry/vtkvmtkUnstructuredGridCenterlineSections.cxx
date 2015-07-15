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
#include "vtkProbeFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridCenterlineSections);

vtkvmtkUnstructuredGridCenterlineSections::vtkvmtkUnstructuredGridCenterlineSections()
{
  this->Centerlines = NULL;
  this->SectionSource = NULL;
  this->SectionPointsPolyData = NULL;
  this->SectionUpNormalsArrayName = NULL;
  this->SectionNormalsArrayName = NULL;
  this->AdditionalNormalsArrayName = NULL;
  this->AdditionalScalarsArrayName = NULL;
  this->SectionIdsArrayName = NULL;
  this->TransformSections = 0;
  this->UseSectionSource = 0;
  this->SourceScaling = 0;
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
  if (this->SectionSource)
    {
    this->SectionSource->Delete();
    this->SectionSource = NULL;
    }
  if (this->SectionPointsPolyData)
    {
    this->SectionPointsPolyData->Delete();
    this->SectionPointsPolyData = NULL;
    }
  if (this->SectionUpNormalsArrayName)
    {
    delete[] this->SectionUpNormalsArrayName;
    this->SectionUpNormalsArrayName = NULL;
    }
  if (this->SectionNormalsArrayName)
    {
    delete[] this->SectionNormalsArrayName;
    this->SectionNormalsArrayName = NULL;
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
  if (this->SectionIdsArrayName)
    {
    delete[] this->SectionIdsArrayName;
    this->SectionIdsArrayName = NULL;
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

void vtkvmtkUnstructuredGridCenterlineSections::CreateTransform(vtkTransform* transform, double currentOrigin[3], double currentNormal[3], double currentUpNormal[3], double targetOrigin[3], double targetNormal[3], double targetUpNormal[3])
{
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

  if (this->UseSectionSource && !this->SectionSource)
    {
    vtkErrorMacro(<<"SectionSource not set");
    return 1;
    }

  vtkDataArray* upNormalsArray = NULL;

  if (this->TransformSections && !this->SectionUpNormalsArrayName)
    {
    vtkErrorMacro(<<"TransformSection is On and no SectionUpNormalsArrayName specified!");
    return 1;
    }

  if (this->SectionUpNormalsArrayName)
    {
    upNormalsArray = this->Centerlines->GetPointData()->GetArray(this->SectionUpNormalsArrayName);
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

  if (!this->SectionNormalsArrayName)
    {
    vtkErrorMacro(<<"SectionNormalsArrayName not specified!");
    return 1;
    }
 
  if (!this->SectionIdsArrayName)
    {
    vtkErrorMacro(<<"SectionIdsArrayName not specified!");
    return 1;
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
    }

  if (this->AdditionalScalarsArrayName)
    {
    additionalScalarsArray = this->Centerlines->GetPointData()->GetArray(this->AdditionalScalarsArrayName);
    if (!additionalScalarsArray)
      {
      vtkErrorMacro(<<"AdditionalScalarsArray with name specified does not exist!");
      }
    }

  if (this->SectionPointsPolyData)
    {
    this->SectionPointsPolyData->Delete();
    this->SectionPointsPolyData = NULL;
    }

  this->SectionPointsPolyData = vtkPolyData::New();
  vtkPoints* sectionPointsPoints = vtkPoints::New();
  vtkCellArray* sectionPointsVerts = vtkCellArray::New();
  vtkDoubleArray* sectionPointsUpNormals = vtkDoubleArray::New();
  sectionPointsUpNormals->SetNumberOfComponents(3);
  sectionPointsUpNormals->SetName(this->SectionUpNormalsArrayName);
  vtkDoubleArray* sectionPointsNormals = vtkDoubleArray::New();
  sectionPointsNormals->SetNumberOfComponents(3);
  sectionPointsNormals->SetName(this->SectionNormalsArrayName);
  vtkDoubleArray* sectionPointsAdditionalNormals = vtkDoubleArray::New();
  sectionPointsAdditionalNormals->SetNumberOfComponents(3);
  sectionPointsAdditionalNormals->SetName(this->AdditionalNormalsArrayName);
  vtkDoubleArray* sectionPointsAdditionalScalars = vtkDoubleArray::New();
  sectionPointsAdditionalScalars->SetNumberOfComponents(1);
  sectionPointsAdditionalScalars->SetName(this->AdditionalScalarsArrayName);
  this->SectionPointsPolyData->SetPoints(sectionPointsPoints);
  this->SectionPointsPolyData->SetVerts(sectionPointsVerts);
  if (upNormalsArray)
    {
    this->SectionPointsPolyData->GetPointData()->AddArray(sectionPointsUpNormals);
    }
  this->SectionPointsPolyData->GetPointData()->AddArray(sectionPointsNormals);
  if (additionalNormalsArray)
    {
    this->SectionPointsPolyData->GetPointData()->AddArray(sectionPointsAdditionalNormals);
    }
  if (additionalScalarsArray)
    { 
    this->SectionPointsPolyData->GetPointData()->AddArray(sectionPointsAdditionalScalars);
    }

  vtkIdType sectionId = 0;
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
 
      vtkMath::Normalize(tangent);
 
      if (upNormalsArray)
        {
        upNormalsArray->GetTuple(centerlineCell->GetPointId(j),upNormal);
        vtkMath::Normalize(upNormal);
        }
 
      vtkPolyData* section = vtkPolyData::New();
 
      if (!this->UseSectionSource)
        {
        vtkPlane* plane = vtkPlane::New();
        plane->SetOrigin(point);
        plane->SetNormal(tangent);
    
        vtkCutter* cutter = vtkCutter::New();
#if (VTK_MAJOR_VERSION <= 5)
        cutter->SetInput(input);
#else
        cutter->SetInputData(input);
#endif
        cutter->SetCutFunction(plane);
        cutter->GenerateCutScalarsOff();
        cutter->SetValue(0,0.0);
        cutter->Update();
    
        vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
#if (VTK_MAJOR_VERSION <= 5)
        cleaner->SetInput(cutter->GetOutput());
#else
        cleaner->SetInputConnection(cutter->GetOutputPort());
#endif
        cleaner->Update();
    
        if (cleaner->GetOutput()->GetNumberOfPoints() == 0)
          {
          plane->Delete();
          cutter->Delete();
          cleaner->Delete();
          continue;
          }
    
        vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
        connectivityFilter->SetInput(cleaner->GetOutput());
#else
        connectivityFilter->SetInputConnection(cleaner->GetOutputPort());
#endif
        connectivityFilter->SetExtractionModeToClosestPointRegion();
        connectivityFilter->SetClosestPoint(point);
        connectivityFilter->Update();

        section->DeepCopy(connectivityFilter->GetOutput());
        section->GetPointData()->SetActiveVectors(this->VectorsArrayName);

        plane->Delete();
        cutter->Delete();
        cleaner->Delete();
        connectivityFilter->Delete();
        }
      else
        {
        double currentOrigin[3];
        currentOrigin[0] = 0.0;
        currentOrigin[1] = 0.0;
        currentOrigin[2] = 0.0;
  
        double currentNormal[3];
        currentNormal[0] = 0.0;
        currentNormal[1] = 0.0;
        currentNormal[2] = 1.0;
  
        double currentUpNormal[3];
        currentUpNormal[0] = 0.0;
        currentUpNormal[1] = 1.0;
        currentUpNormal[2] = 0.0;
 
        double targetOrigin[3];
        targetOrigin[0] = point[0];
        targetOrigin[1] = point[1];
        targetOrigin[2] = point[2];
  
        double targetNormal[3];
        targetNormal[0] = tangent[0];
        targetNormal[1] = tangent[1];
        targetNormal[2] = tangent[2];
  
        double targetUpNormal[3];
        targetUpNormal[0] = upNormal[0];
        targetUpNormal[1] = upNormal[1];
        targetUpNormal[2] = upNormal[2];
 
        vtkTransform* transform = vtkTransform::New();
        this->CreateTransform(transform,currentOrigin,currentNormal,currentUpNormal,targetOrigin,targetNormal,targetUpNormal);

        vtkTransformPolyDataFilter* transformFilter = vtkTransformPolyDataFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
        transformFilter->SetInput(this->SectionSource);
#else
        transformFilter->SetInputData(this->SectionSource);
#endif
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        vtkProbeFilter* probeFilter = vtkProbeFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
        probeFilter->SetInput(transformFilter->GetOutput());
        probeFilter->SetSource(input);
#else
        probeFilter->SetInputConnection(transformFilter->GetOutputPort());
        probeFilter->SetSourceData(input);
#endif
        probeFilter->Update();

        section->DeepCopy(probeFilter->GetOutput());
        section->GetPointData()->SetActiveVectors(this->VectorsArrayName); 

        probeFilter->Delete();
        transformFilter->Delete();
        transform->Delete();
        }

      vtkIdType numberOfSectionPoints = section->GetNumberOfPoints();
      vtkIdTypeArray* sectionIdsArray = vtkIdTypeArray::New();
      sectionIdsArray->SetName(this->SectionIdsArrayName);
      sectionIdsArray->SetNumberOfTuples(numberOfSectionPoints);
      vtkIdType pointId;
      for (pointId=0; pointId<numberOfSectionPoints; pointId++)
        {
        sectionIdsArray->SetValue(pointId,sectionId);
        }
      sectionId += 1;
      section->GetPointData()->AddArray(sectionIdsArray);

      if (!this->TransformSections)
        {
#if (VTK_MAJOR_VERSION <= 5)
        appendFilter->AddInput(section);
#else
        appendFilter->AddInputData(section);
#endif

        vtkIdType pointId = sectionPointsPoints->InsertNextPoint(point);
        sectionPointsVerts->InsertNextCell(1);
        sectionPointsVerts->InsertCellPoint(pointId);
        sectionPointsNormals->InsertNextTuple(tangent);
        if (upNormalsArray)
          {
          sectionPointsUpNormals->InsertNextTuple(upNormal);
          }
        if (additionalNormalsArray)
          {
          double additionalNormal[3];
          additionalNormalsArray->GetTuple(centerlineCell->GetPointId(j),additionalNormal);
          sectionPointsAdditionalNormals->InsertNextTuple(additionalNormal);
          }
        if (additionalScalarsArray)
          {
          double scalar = additionalScalarsArray->GetTuple1(centerlineCell->GetPointId(j));
          sectionPointsAdditionalScalars->InsertNextTuple1(scalar);
          }
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
        this->CreateTransform(transform,currentOrigin,currentNormal,currentUpNormal,targetOrigin,targetNormal,targetUpNormal);

        vtkTransformPolyDataFilter* transformFilter = vtkTransformPolyDataFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
        transformFilter->SetInput(section);
#else
        transformFilter->SetInputData(section);
#endif
        transformFilter->SetTransform(transform);
        transformFilter->Update();

#if (VTK_MAJOR_VERSION <= 5)
        appendFilter->AddInput(transformFilter->GetOutput());
#else
        appendFilter->AddInputConnection(transformFilter->GetOutputPort());
#endif

        vtkIdType pointId = sectionPointsPoints->InsertNextPoint(targetOrigin);
        sectionPointsVerts->InsertNextCell(1);
        sectionPointsVerts->InsertCellPoint(pointId);
        sectionPointsNormals->InsertNextTuple(targetNormal);
        sectionPointsUpNormals->InsertNextTuple(targetUpNormal);
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
          sectionPointsAdditionalNormals->InsertNextTuple(transformedAdditionalNormal);
          }
        if (additionalScalarsArray)
          {
          double scalar = additionalScalarsArray->GetTuple1(centerlineCell->GetPointId(j));
          sectionPointsAdditionalScalars->InsertNextTuple1(scalar);
          }

        transform->Delete();
        transformFilter->Delete();
        }

      section->Delete();
      }  
    }

  appendFilter->Update();

  output->DeepCopy(appendFilter->GetOutput());

  appendFilter->Delete();
  sectionPointsPoints->Delete();
  sectionPointsVerts->Delete();
  sectionPointsAdditionalNormals->Delete();
  sectionPointsAdditionalScalars->Delete();

  return 1;
}

void vtkvmtkUnstructuredGridCenterlineSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
