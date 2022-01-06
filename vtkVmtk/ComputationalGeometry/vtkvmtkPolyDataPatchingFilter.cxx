/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataPatchingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/10 14:30:17 $
  Version:   $Revision: 1.11 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataPatchingFilter.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkTriangle.h"
#include "vtkTriangleFilter.h"
#include "vtkCellArray.h"
#include "vtkCleanPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataPatchingFilter);

vtkvmtkPolyDataPatchingFilter::vtkvmtkPolyDataPatchingFilter() 
{
  this->LongitudinalMappingArrayName = NULL;
  this->CircularMappingArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->LongitudinalPatchNumberArrayName = NULL;
  this->CircularPatchNumberArrayName = NULL;
  this->PatchAreaArrayName = NULL;
  this->PatchSize[0] = this->PatchSize[1] = 0.0;
  this->PatchOffsets[0] = this->PatchOffsets[1] = 0.0;
  this->LongitudinalPatchBounds[0] = this->LongitudinalPatchBounds[1] = 0.0;
  this->CircularPatchBounds[0] = this->CircularPatchBounds[1] = 0.0; 
  this->PatchedData = NULL;
  this->CircularPatching = 1;
  this->UseConnectivity = 1;
}

vtkvmtkPolyDataPatchingFilter::~vtkvmtkPolyDataPatchingFilter()
{
  if (this->LongitudinalMappingArrayName)
    {
    delete[] this->LongitudinalMappingArrayName;
    this->LongitudinalMappingArrayName = NULL;
    }

  if (this->CircularMappingArrayName)
    {
    delete[] this->CircularMappingArrayName;
    this->CircularMappingArrayName = NULL;
    }

  if (this->LongitudinalPatchNumberArrayName)
    {
    delete[] this->LongitudinalPatchNumberArrayName;
    this->LongitudinalPatchNumberArrayName = NULL;
    }

  if (this->CircularPatchNumberArrayName)
    {
    delete[] this->CircularPatchNumberArrayName;
    this->CircularPatchNumberArrayName = NULL;
    }

  if (this->PatchAreaArrayName)
    {
    delete[] this->PatchAreaArrayName;
    this->PatchAreaArrayName = NULL;
    }

  if (this->PatchedData)
    {
    this->PatchedData->Delete();
    this->PatchedData = NULL;
    }
}

int vtkvmtkPolyDataPatchingFilter::RequestData(
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

  if (!this->LongitudinalPatchNumberArrayName)
    {
    vtkErrorMacro(<<"LongitudinalPatchNumberArrayName not set.");
    return 1;
    }

  if (!this->CircularPatchNumberArrayName)
    {
    vtkErrorMacro(<<"CircularPatchNumberArrayName not set.");
    return 1;
    }

  if (!this->PatchAreaArrayName)
    {
    vtkErrorMacro(<<"PatchAreaArrayName not set.");
    return 1;
    }

  if (!this->LongitudinalMappingArrayName)
    {
    vtkErrorMacro(<<"LongitudinalMappingArrayName not set.");
    return 1;
    }

  vtkDataArray* longitudinalMappingArray = input->GetPointData()->GetArray(this->LongitudinalMappingArrayName);

  if (!longitudinalMappingArray)
    {
    vtkErrorMacro(<<"LongitudinalMappingArray with name specified does not exist.");
    return 1;
    }

  vtkDataArray* circularMappingArray = NULL;
  if (this->CircularPatching)
    {
  
    if (!this->CircularMappingArrayName)
      {
      vtkErrorMacro(<<"CircularMappingArrayName not set.");
      return 1;
      }

    circularMappingArray = input->GetPointData()->GetArray(this->CircularMappingArrayName);

    if (!circularMappingArray)
      {
      vtkErrorMacro(<<"CircularMappingArray with name specified does not exist.");
      return 1;
      }
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

  int numberOfInputPoints = input->GetNumberOfPoints();

  double circumferentialActualPatchSize = this->PatchSize[1] * 2.0 * vtkMath::Pi();
 
  char shiftedCircularMapping90ArrayName[] = "ShiftedCircularMapping90";
  char shiftedCircularMapping180ArrayName[] = "ShiftedCircularMapping180";
  char shiftedCircularMapping270ArrayName[] = "ShiftedCircularMapping270";

  vtkDoubleArray* shiftedCircularMapping90Array = vtkDoubleArray::New();
  shiftedCircularMapping90Array->SetName(shiftedCircularMapping90ArrayName);
  shiftedCircularMapping90Array->SetNumberOfComponents(1);
  shiftedCircularMapping90Array->SetNumberOfTuples(numberOfInputPoints);
  vtkDoubleArray* shiftedCircularMapping180Array = vtkDoubleArray::New();
  shiftedCircularMapping180Array->SetName(shiftedCircularMapping180ArrayName);
  shiftedCircularMapping180Array->SetNumberOfComponents(1);
  shiftedCircularMapping180Array->SetNumberOfTuples(numberOfInputPoints);
  vtkDoubleArray* shiftedCircularMapping270Array = vtkDoubleArray::New();
  shiftedCircularMapping270Array->SetName(shiftedCircularMapping270ArrayName);
  shiftedCircularMapping270Array->SetNumberOfComponents(1);
  shiftedCircularMapping270Array->SetNumberOfTuples(numberOfInputPoints);

  int i;
  if (this->CircularPatching)
    {
    for (i=0; i<numberOfInputPoints; i++)
      {
      double value = circularMappingArray->GetComponent(i,0);
      double shiftedValue = value + 0.5 * vtkMath::Pi();
      if (shiftedValue > vtkMath::Pi())
        {
        shiftedValue -= 2.0 * vtkMath::Pi();
        }
      shiftedCircularMapping90Array->SetValue(i,shiftedValue);
      shiftedValue = value + vtkMath::Pi();
      if (shiftedValue > vtkMath::Pi())
        {
        shiftedValue -= 2.0 * vtkMath::Pi();
        }
      shiftedCircularMapping180Array->SetValue(i,shiftedValue);
      shiftedValue = value + 1.5 * vtkMath::Pi();
      if (shiftedValue > vtkMath::Pi())
        {
        shiftedValue -= 2.0 * vtkMath::Pi();
        }
      shiftedCircularMapping270Array->SetValue(i,shiftedValue);
      }
    }
  else
    {
    shiftedCircularMapping90Array->FillComponent(0,0.0);
    shiftedCircularMapping180Array->FillComponent(0,0.0);
    shiftedCircularMapping270Array->FillComponent(0,0.0);
    }
  
  input->GetPointData()->AddArray(shiftedCircularMapping90Array);
  input->GetPointData()->AddArray(shiftedCircularMapping180Array);
  input->GetPointData()->AddArray(shiftedCircularMapping270Array);
  
  if (this->PatchedData)
    {
    this->PatchedData->Delete();
    this->PatchedData = NULL;
    }

  this->PatchedData = vtkImageData::New();

  vtkPointData* patchedDataPointData = this->PatchedData->GetPointData();
  patchedDataPointData->CopyAllocate(input->GetPointData(),0);

  vtkIntArray* patchedDataLongitudinalPatchNumberArray = vtkIntArray::New();
  patchedDataLongitudinalPatchNumberArray->SetName(this->LongitudinalPatchNumberArrayName);
  patchedDataLongitudinalPatchNumberArray->SetNumberOfComponents(1);

  vtkIntArray* patchedDataCircularPatchNumberArray = vtkIntArray::New();
  patchedDataCircularPatchNumberArray->SetName(this->CircularPatchNumberArrayName);
  patchedDataCircularPatchNumberArray->SetNumberOfComponents(1);

  vtkDoubleArray* patchedDataPatchAreaArray = vtkDoubleArray::New();
  patchedDataPatchAreaArray->SetName(this->PatchAreaArrayName);
  patchedDataPatchAreaArray->SetNumberOfComponents(1);

  vtkAppendPolyData* patchAppendFilter = vtkAppendPolyData::New();

  if (this->CircularPatchBounds[0] == 0.0 && this->CircularPatchBounds[1] == 0.0)
    {
    this->CircularPatchBounds[0] = - vtkMath::Pi();
    this->CircularPatchBounds[1] = vtkMath::Pi();
    }
 
  int circularPatchStartIndex = 0; 
  int circularPatchEndIndex = 0;
 
  if (this->CircularPatching)
    {
    circularPatchStartIndex = vtkMath::Floor((this->CircularPatchBounds[0] - this->PatchOffsets[1]) / circumferentialActualPatchSize);
    if (circularPatchStartIndex*circumferentialActualPatchSize - this->PatchOffsets[1] < - vtkMath::Pi())
      {
      circularPatchStartIndex += 1;
      }
    circularPatchEndIndex = vtkMath::Floor((this->CircularPatchBounds[1] - this->PatchOffsets[1] - 1E-3 * circumferentialActualPatchSize) / circumferentialActualPatchSize);
    }

  int numberOfPreviousPatchDataLines = 0;
  vtkIdList* groupIds = vtkIdList::New();
  vtkvmtkPolyDataBranchUtilities::GetGroupsIdList(input,this->GroupIdsArrayName,groupIds);
  for (i=0; i<groupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = groupIds->GetId(i);
    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,groupId,true,cylinder);
  
    longitudinalMappingArray = cylinder->GetPointData()->GetArray(this->LongitudinalMappingArrayName);
    circularMappingArray = cylinder->GetPointData()->GetArray(this->CircularMappingArrayName);

    cylinder->GetPointData()->SetActiveScalars(this->LongitudinalMappingArrayName);

    vtkClipPolyData* longitudinalClipper0 = vtkClipPolyData::New();
    longitudinalClipper0->SetInputData(cylinder);
    longitudinalClipper0->GenerateClipScalarsOff();
    longitudinalClipper0->InsideOutOff();

    vtkClipPolyData* longitudinalClipper1 = vtkClipPolyData::New();
    longitudinalClipper1->SetInputConnection(longitudinalClipper0->GetOutputPort());
    longitudinalClipper1->GenerateClipScalarsOff();
    longitudinalClipper1->InsideOutOn();

    vtkClipPolyData* circularClipper0 = vtkClipPolyData::New();
    circularClipper0->SetInputConnection(longitudinalClipper1->GetOutputPort());
    circularClipper0->GenerateClipScalarsOff();
    circularClipper0->InsideOutOff();

    vtkClipPolyData* circularClipper1 = vtkClipPolyData::New();
    circularClipper1->SetInputConnection(circularClipper0->GetOutputPort());
    circularClipper1->GenerateClipScalarsOff();
    circularClipper1->InsideOutOn();

    vtkPolyDataConnectivityFilter* patchConnectivityFilter = vtkPolyDataConnectivityFilter::New();
    patchConnectivityFilter->SetInputConnection(circularClipper1->GetOutputPort());
    patchConnectivityFilter->SetExtractionModeToLargestRegion();

    double longitudinalMappingRange[2];

    longitudinalMappingArray->GetRange(longitudinalMappingRange,0);

    double longitudinalCylinderPatchBounds[2];
   
    if (this->LongitudinalPatchBounds[0] == 0.0 && this->LongitudinalPatchBounds[1] == 0.0)
      {
      longitudinalCylinderPatchBounds[0] = longitudinalMappingRange[0];
      longitudinalCylinderPatchBounds[1] = longitudinalMappingRange[1];
      }
    else
      {
      longitudinalCylinderPatchBounds[0] = this->LongitudinalPatchBounds[0];
      longitudinalCylinderPatchBounds[1] = this->LongitudinalPatchBounds[1];
      }

    int longitudinalPatchStartIndex = vtkMath::Floor((longitudinalCylinderPatchBounds[0] - this->PatchOffsets[0]) / this->PatchSize[0]);
    int longitudinalPatchEndIndex = vtkMath::Floor((longitudinalCylinderPatchBounds[1] - this->PatchOffsets[0] - 1E-3 * this->PatchSize[0]) / this->PatchSize[0]);
  
    int j, k;
    for (j=longitudinalPatchStartIndex; j<=longitudinalPatchEndIndex; j++)
      {
      double longitudinalPatchStart = this->PatchOffsets[0] + j * this->PatchSize[0];
      double longitudinalPatchEnd = this->PatchOffsets[0] + (j+1) * this->PatchSize[0];

      longitudinalClipper0->SetValue(longitudinalPatchStart);
      longitudinalClipper1->SetValue(longitudinalPatchEnd);
      longitudinalClipper1->Update();

      if (longitudinalClipper1->GetOutput()->GetNumberOfPoints() == 0)
        {
        continue;
        }

      for (k=circularPatchStartIndex; k<=circularPatchEndIndex; k++)
        {
        int patchId = k - circularPatchStartIndex + (j - longitudinalPatchStartIndex) * (circularPatchEndIndex-circularPatchStartIndex+1) + numberOfPreviousPatchDataLines * (circularPatchEndIndex-circularPatchStartIndex+1);
        double circularPatchCenter = 0.0;
        double shiftedCircularPatchCenter = 0.0;
        char* patchShiftedCircularMappingArrayName = NULL;

        if (this->CircularPatching)
          {
          double circularPatchStart = this->PatchOffsets[1] + k * circumferentialActualPatchSize;
          double circularPatchEnd = this->PatchOffsets[1] + (k+1) * circumferentialActualPatchSize;

          double pi = vtkMath::Pi();

          if (circularPatchStart < -pi)
            {
            circularPatchStart += 2.0 * pi;
            circularPatchEnd += 2.0 * pi;
            }
          
          circularPatchCenter = 0.5 * (circularPatchStart + circularPatchEnd);
         
//          if (circularPatchStart < pi && circularPatchEnd > pi)
          if (circularPatchCenter <= - 0.75 * pi || circularPatchCenter >= 0.75 * pi)
            {
            patchShiftedCircularMappingArrayName = shiftedCircularMapping180ArrayName;
            if (circularPatchCenter <= - 0.75 * pi)
              {
              circularPatchStart += pi;
              circularPatchEnd += pi;
              }
            else if (circularPatchCenter >= 0.75 * pi)
              {
              circularPatchStart -= pi;
              circularPatchEnd -= pi;
              }
            }
//          else if (circularPatchStart < 0.5 * pi && circularPatchEnd > 0.5 * pi)
          else if (circularPatchCenter >= 0.25 * pi && circularPatchCenter < 0.75 * pi)
            {
            patchShiftedCircularMappingArrayName = shiftedCircularMapping270ArrayName;
            circularPatchStart -= 0.5 * pi;
            circularPatchEnd -= 0.5 * pi;
            }
//          else if (circularPatchStart < -0.5 * pi && circularPatchEnd > -0.5 * pi)
          else if (circularPatchCenter > -0.75 * pi && circularPatchCenter <= -0.25 * pi)
            {
            patchShiftedCircularMappingArrayName = shiftedCircularMapping90ArrayName;
            circularPatchStart += 0.5 * pi;
            circularPatchEnd += 0.5 * pi;
            }
          else
            {
            patchShiftedCircularMappingArrayName = this->CircularMappingArrayName;
            }

          longitudinalClipper1->GetOutput()->GetPointData()->SetActiveScalars(patchShiftedCircularMappingArrayName);

          shiftedCircularPatchCenter = 0.5 * (circularPatchStart + circularPatchEnd);

          circularClipper0->Modified();
          circularClipper0->SetValue(circularPatchStart);
          circularClipper1->SetValue(circularPatchEnd);
          circularClipper1->Update();

          if (circularClipper1->GetOutput()->GetNumberOfPoints() == 0)
            {
            continue;
            }
          }

        vtkTriangleFilter* patchTriangleFilter = vtkTriangleFilter::New();

        if (this->UseConnectivity)
          {
          if (this->CircularPatching)
            {
            patchConnectivityFilter->SetInputConnection(circularClipper1->GetOutputPort());
            }
          else
            {
            patchConnectivityFilter->SetInputConnection(longitudinalClipper1->GetOutputPort());
            }
          patchConnectivityFilter->Update();
          patchTriangleFilter->SetInputConnection(patchConnectivityFilter->GetOutputPort());
          }
        else
          {
          if (this->CircularPatching)
            {
            patchTriangleFilter->SetInputConnection(circularClipper1->GetOutputPort());
            }
          else
            {
            patchTriangleFilter->SetInputConnection(longitudinalClipper1->GetOutputPort());
            }
          }

        patchTriangleFilter->Update();

        vtkPolyData* patch = vtkPolyData::New();
        patch->DeepCopy(patchTriangleFilter->GetOutput());

        patch->BuildCells();
        int patchNumberOfCells = patch->GetNumberOfCells();

        if (this->CircularPatching)
          {
          vtkDataArray* patchCircularMappingArray = patch->GetPointData()->GetArray(this->CircularMappingArrayName);
          vtkDataArray* patchShiftedCircularMappingArray = patch->GetPointData()->GetArray(patchShiftedCircularMappingArrayName);

          double angularOffset = circularPatchCenter - shiftedCircularPatchCenter;

          vtkIdType patchNumberOfPoints = patch->GetNumberOfPoints();
          vtkIdType pointId;
          for (pointId=0; pointId<patchNumberOfPoints; pointId++)
            {
            double circularMappingValue = patchShiftedCircularMappingArray->GetTuple1(pointId);
            circularMappingValue += angularOffset;
            patchCircularMappingArray->SetTuple1(pointId,circularMappingValue);
            }
          }

        vtkIntArray* longitudinalPatchNumberArray = vtkIntArray::New();
        longitudinalPatchNumberArray->SetName(this->LongitudinalPatchNumberArrayName);
        longitudinalPatchNumberArray->SetNumberOfComponents(1);
        longitudinalPatchNumberArray->SetNumberOfTuples(patchNumberOfCells);

        vtkIntArray* circularPatchNumberArray = vtkIntArray::New();
        circularPatchNumberArray->SetName(this->CircularPatchNumberArrayName);
        circularPatchNumberArray->SetNumberOfComponents(1);
        circularPatchNumberArray->SetNumberOfTuples(patchNumberOfCells);

        vtkDoubleArray* patchAreaArray = vtkDoubleArray::New();
        patchAreaArray->SetName(this->PatchAreaArrayName);
        patchAreaArray->SetNumberOfComponents(1);
        patchAreaArray->SetNumberOfTuples(patchNumberOfCells);

        double patchArea = 0.0;

        vtkPointData* patchPointData = patch->GetPointData();

        vtkCellData* patchCellData = vtkCellData::New();
        patchCellData->CopyAllocate(patchPointData,patchNumberOfCells);

        vtkCellData* patchedPatchCellData = patch->GetCellData();
        patchedPatchCellData->CopyAllocate(patchCellData,patchNumberOfCells);

        int cellId;
        for (cellId=0; cellId < patchNumberOfCells; cellId++)
          {
          vtkTriangle* triangle = vtkTriangle::SafeDownCast(patch->GetCell(cellId));
          if (!triangle)
            {
            continue;
            }

          double point0[3], point1[3], point2[3];
          triangle->GetPoints()->GetPoint(0,point0);
          triangle->GetPoints()->GetPoint(1,point1);
          triangle->GetPoints()->GetPoint(2,point2);
          patchArea += vtkTriangle::TriangleArea(point0,point1,point2);

          double weights[3];
          weights[1] = 1.0/3.0;
          weights[2] = 1.0/3.0;
          weights[0] = (1.0 - weights[1] - weights[2]);

          patchCellData->InterpolatePoint(patchPointData,cellId,triangle->GetPointIds(),weights);
          }

        for (cellId = 0; cellId < patchNumberOfCells; cellId++)
          {
          longitudinalPatchNumberArray->SetValue(cellId,j);
          circularPatchNumberArray->SetValue(cellId,k);
          patchAreaArray->SetValue(cellId,patchArea);
          }

        patchedDataLongitudinalPatchNumberArray->InsertValue(patchId,j);
        patchedDataCircularPatchNumberArray->InsertValue(patchId,k);
        patchedDataPatchAreaArray->InsertValue(patchId,patchArea);

        int numberOfArrays = patchCellData->GetNumberOfArrays();
        int arrayId;
        for (arrayId = 0; arrayId<numberOfArrays; arrayId++)
          {
          vtkDataArray* patchArray = patchCellData->GetArray(arrayId);
          vtkDataArray* patchedArray = patchedPatchCellData->GetArray(patchArray->GetName());
          vtkDataArray* patchedDataArray = patchedDataPointData->GetArray(arrayId);
          if (!patchedArray)
            {
            continue;
            }

          int dataType = patchArray->GetDataType();
          if (dataType != VTK_FLOAT && dataType != VTK_DOUBLE)
            {
            for (cellId = 0; cellId < patchNumberOfCells; cellId++)
              {
              patchedArray->InsertTuple(cellId,patchArray->GetTuple(cellId));
              }
            if (patchNumberOfCells > 0)
              {
              patchedDataArray->InsertTuple(patchId,patchArray->GetTuple(0));
              }
            continue;
            }

          double* integralTuple = new double[patchArray->GetNumberOfComponents()];
          int componentId;
          for (componentId = 0; componentId < patchArray->GetNumberOfComponents(); componentId++)
            {
            integralTuple[componentId] = 0.0;
            }
          for (cellId = 0; cellId < patchNumberOfCells; cellId++)
            {
            vtkTriangle* triangle = vtkTriangle::SafeDownCast(patch->GetCell(cellId));
            if (!triangle)
              {
              continue;
              }
            double point0[3], point1[3], point2[3];
            triangle->GetPoints()->GetPoint(0,point0);
            triangle->GetPoints()->GetPoint(1,point1);
            triangle->GetPoints()->GetPoint(2,point2);
            double area = vtkTriangle::TriangleArea(point0,point1,point2);

            for (componentId = 0; componentId < patchArray->GetNumberOfComponents(); componentId++)
              {
              integralTuple[componentId] += area * patchArray->GetComponent(cellId,componentId);
              }
            }
          for (componentId = 0; componentId < patchArray->GetNumberOfComponents(); componentId++)
            {
            integralTuple[componentId] /= patchArea;
            }
          for (cellId = 0; cellId < patchNumberOfCells; cellId++)
            {
            patchedArray->InsertTuple(cellId,integralTuple);
            }

          patchedDataArray->InsertTuple(patchId,integralTuple);

          delete[] integralTuple;
          }

        patch->GetCellData()->AddArray(longitudinalPatchNumberArray);
        patch->GetCellData()->AddArray(circularPatchNumberArray);
        patch->GetCellData()->AddArray(patchAreaArray);

        patchAppendFilter->AddInputData(patch);

        patch->Delete();
        patchTriangleFilter->Delete();
        patchCellData->Delete();
        longitudinalPatchNumberArray->Delete();
        circularPatchNumberArray->Delete();
        patchAreaArray->Delete();
        }
      }

    cylinder->Delete();

    longitudinalClipper0->Delete();
    longitudinalClipper1->Delete();
    circularClipper0->Delete();
    circularClipper1->Delete();
    patchConnectivityFilter->Delete();

    numberOfPreviousPatchDataLines += longitudinalPatchEndIndex - longitudinalPatchStartIndex + 1;
    }
  groupIds->Delete();
  
  this->PatchedData->SetOrigin(0.0,0.0,0.0);
  this->PatchedData->SetSpacing(circumferentialActualPatchSize,this->PatchSize[0],1.0);
  this->PatchedData->SetExtent(0,circularPatchEndIndex-circularPatchStartIndex,0,numberOfPreviousPatchDataLines-1,0,0);

  this->PatchedData->GetPointData()->AddArray(patchedDataLongitudinalPatchNumberArray);
  this->PatchedData->GetPointData()->AddArray(patchedDataCircularPatchNumberArray);
  this->PatchedData->GetPointData()->AddArray(patchedDataPatchAreaArray);

  this->PatchedData->GetPointData()->RemoveArray(shiftedCircularMapping90ArrayName);
  this->PatchedData->GetPointData()->RemoveArray(shiftedCircularMapping180ArrayName);
  this->PatchedData->GetPointData()->RemoveArray(shiftedCircularMapping270ArrayName);

  patchAppendFilter->Update();
  output->DeepCopy(patchAppendFilter->GetOutput());

  output->GetPointData()->RemoveArray(shiftedCircularMapping90ArrayName);
  output->GetPointData()->RemoveArray(shiftedCircularMapping180ArrayName);
  output->GetPointData()->RemoveArray(shiftedCircularMapping270ArrayName);
  output->GetCellData()->RemoveArray(shiftedCircularMapping90ArrayName);
  output->GetCellData()->RemoveArray(shiftedCircularMapping180ArrayName);
  output->GetCellData()->RemoveArray(shiftedCircularMapping270ArrayName);

  input->GetPointData()->RemoveArray(shiftedCircularMapping90ArrayName);
  input->GetPointData()->RemoveArray(shiftedCircularMapping180ArrayName);
  input->GetPointData()->RemoveArray(shiftedCircularMapping270ArrayName);

  patchedDataLongitudinalPatchNumberArray->Delete();
  patchedDataCircularPatchNumberArray->Delete();
  patchedDataPatchAreaArray->Delete();

  patchAppendFilter->Delete();

  shiftedCircularMapping90Array->Delete();
  shiftedCircularMapping180Array->Delete();
  shiftedCircularMapping270Array->Delete();

  return 1;
}
