/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearizeMeshFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkLinearizeMeshFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkIdTypeArray.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkLinearizeMeshFilter);

vtkvmtkLinearizeMeshFilter::vtkvmtkLinearizeMeshFilter()
{
  this->CleanOutput = 1;
}

vtkvmtkLinearizeMeshFilter::~vtkvmtkLinearizeMeshFilter()
{
}

int vtkvmtkLinearizeMeshFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* outputPoints = vtkPoints::New();

  vtkCellArray* outputCellArray = vtkCellArray::New();
  vtkIdList* outputCellTypes = vtkIdList::New();

  int numberOfPoints = input->GetNumberOfPoints();
  int numberOfCells = input->GetNumberOfCells();

  vtkPointData* inputPointData = input->GetPointData();
  vtkPointData* outputPointData = output->GetPointData();
  outputPointData->CopyAllocate(inputPointData,numberOfPoints);

  vtkCellData* inputCellData = input->GetCellData();
  vtkCellData* outputCellData = output->GetCellData();
  outputCellData->CopyAllocate(inputCellData,numberOfCells);

  vtkIdTypeArray* pointUsedArray = vtkIdTypeArray::New();
  pointUsedArray->SetNumberOfTuples(numberOfPoints);
  pointUsedArray->FillComponent(0,0.0);

  vtkIdList* cellPoints = vtkIdList::New();
  vtkIdList* outputCellPoints = vtkIdList::New();
  int pointId;
  int outputCellId;
  int i,j;
  for (i=0; i<numberOfCells; i++)
    {
    int cellType = input->GetCellType(i);
    cellPoints->Initialize();
    input->GetCellPoints(i,cellPoints);
    outputCellPoints->Initialize();
    switch (cellType)
      {
      case VTK_TRIANGLE:
      case VTK_QUADRATIC_TRIANGLE:
        for (j=0; j<3; j++)
          {
          pointId = cellPoints->GetId(j);
          outputCellPoints->InsertNextId(pointId);
          pointUsedArray->SetValue(pointId,1);
          }
        outputCellId = outputCellTypes->InsertNextId(VTK_TRIANGLE);
        outputCellArray->InsertNextCell(outputCellPoints);
        outputCellData->CopyData(inputCellData,i,outputCellId);
        break;
      case VTK_QUAD:
      case VTK_QUADRATIC_QUAD:
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
      case VTK_QUADRATIC_LINEAR_QUAD:
      case VTK_BIQUADRATIC_QUAD:
#endif
        for (j=0; j<4; j++)
          {
          pointId = cellPoints->GetId(j);
          outputCellPoints->InsertNextId(pointId);
          pointUsedArray->SetValue(pointId,1);
          }
        outputCellId = outputCellTypes->InsertNextId(VTK_QUAD);
        outputCellArray->InsertNextCell(outputCellPoints);
        outputCellData->CopyData(inputCellData,i,outputCellId);
        break;
      case VTK_TETRA:
      case VTK_QUADRATIC_TETRA:
        for (j=0; j<4; j++)
          {
          pointId = cellPoints->GetId(j);
          outputCellPoints->InsertNextId(pointId);
          pointUsedArray->SetValue(pointId,1);
          }
        outputCellId = outputCellTypes->InsertNextId(VTK_TETRA);
        outputCellArray->InsertNextCell(outputCellPoints);
        outputCellData->CopyData(inputCellData,i,outputCellId);
        break;
      case VTK_HEXAHEDRON:
      case VTK_QUADRATIC_HEXAHEDRON:
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
      case VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON:
      case VTK_TRIQUADRATIC_HEXAHEDRON:
#endif
        for (j=0; j<8; j++)
          {
          pointId = cellPoints->GetId(j);
          outputCellPoints->InsertNextId(pointId);
          pointUsedArray->SetValue(pointId,1);
          }
        outputCellId = outputCellTypes->InsertNextId(VTK_HEXAHEDRON);
        outputCellArray->InsertNextCell(outputCellPoints);
        outputCellData->CopyData(inputCellData,i,outputCellId);
        break;
      case VTK_WEDGE:
      case VTK_QUADRATIC_WEDGE:
#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
      case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
#endif
        for (j=0; j<6; j++)
          {
          pointId = cellPoints->GetId(j);
          outputCellPoints->InsertNextId(pointId);
          pointUsedArray->SetValue(pointId,1);
          }
        outputCellId = outputCellTypes->InsertNextId(VTK_WEDGE);
        outputCellArray->InsertNextCell(outputCellPoints);
        outputCellData->CopyData(inputCellData,i,outputCellId);
        break;
      default:
        vtkWarningMacro(<<"Unsupported element to linearize. Skipping element.");
        break;
      }
    }

  vtkIdTypeArray* pointIdMapArray = vtkIdTypeArray::New();
  pointIdMapArray->SetNumberOfTuples(numberOfPoints);
  for (i=0; i<numberOfPoints; i++)
    {
    if (this->CleanOutput)
      {
      if (!pointUsedArray->GetValue(i))
        {
        continue;
        }
      }
    pointId = outputPoints->InsertNextPoint(input->GetPoint(i));
    outputPointData->CopyData(inputPointData,i,pointId);
    pointIdMapArray->SetValue(i,pointId);
    }

  vtkCellArray* remappedOutputCellArray = vtkCellArray::New();
  int numberOfOutputCells = outputCellArray->GetNumberOfCells();
  outputCellArray->InitTraversal();
  vtkIdType npts = 0;
  vtkIdType *pts = NULL;
  for (i=0; i<numberOfOutputCells; i++)
    {
    outputCellArray->GetNextCell(npts,pts);
    outputCellPoints->Initialize();
    for (j=0; j<npts; j++)
      {
      outputCellPoints->InsertNextId(pointIdMapArray->GetValue(pts[j]));
      }  
    remappedOutputCellArray->InsertNextCell(outputCellPoints);
    }

  output->GetPointData()->Squeeze();
  output->GetCellData()->Squeeze();

  output->SetPoints(outputPoints);

  int* outputCellTypesInt = new int[outputCellTypes->GetNumberOfIds()];
  for (i=0; i<outputCellTypes->GetNumberOfIds(); i++)
    {
    outputCellTypesInt[i] = outputCellTypes->GetId(i);
    }

  output->SetCells(outputCellTypesInt,remappedOutputCellArray);

  delete[] outputCellTypesInt;

  outputPoints->Delete();
  outputCellArray->Delete();
  remappedOutputCellArray->Delete();
  outputCellTypes->Delete();
  pointUsedArray->Delete();
  pointIdMapArray->Delete();
  cellPoints->Delete();
  outputCellPoints->Delete();

  return 1;
}

void vtkvmtkLinearizeMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
