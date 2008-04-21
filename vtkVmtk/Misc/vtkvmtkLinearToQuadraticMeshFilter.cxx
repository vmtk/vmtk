/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearToQuadraticMeshFilter.cxx,v $
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

#include "vtkvmtkLinearToQuadraticMeshFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkInterpolatingSubdivisionFilter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkButterflySubdivisionFilter.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkGenericCell.h"
#include "vtkEdgeTable.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkLinearToQuadraticMeshFilter, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkvmtkLinearToQuadraticMeshFilter);

vtkvmtkLinearToQuadraticMeshFilter::vtkvmtkLinearToQuadraticMeshFilter()
{
}

vtkvmtkLinearToQuadraticMeshFilter::~vtkvmtkLinearToQuadraticMeshFilter()
{
}

int vtkvmtkLinearToQuadraticMeshFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //TODO: finally project mid-edge surface points to position on the original surface (use locator).
 
//   vtkIdTypeArray* triangleIds = vtkIdTypeArray::New();
//   input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleIds);

  vtkPoints* outputPoints = vtkPoints::New();

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkEdgeTable* edgeTable = vtkEdgeTable::New();
  edgeTable->InitEdgeInsertion(numberOfInputPoints,1);

  vtkPointData* inputPointData = input->GetPointData();
  vtkPointData* outputPointData = output->GetPointData();
  outputPointData->InterpolateAllocate(inputPointData,5*numberOfInputPoints);

  vtkCellData* inputCellData = input->GetCellData();
  vtkCellData* outputCellData = output->GetCellData();
  outputCellData->CopyAllocate(inputCellData,numberOfInputPoints);

  int i, j;
  for (i=0; i<numberOfInputPoints; i++)
    {
    outputPoints->InsertNextPoint(input->GetPoint(i));
    outputPointData->CopyData(inputPointData,i,i);
    }

  vtkGenericCell* cell = vtkGenericCell::New();

  vtkIdTypeArray* tetraIds = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TETRA,tetraIds);

  int numberOfInputTetras = tetraIds->GetNumberOfTuples();

  vtkIdTypeArray* triangleIds = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_TRIANGLE,triangleIds);

  int numberOfInputTriangles = triangleIds->GetNumberOfTuples();

  int numberOfInputCells = numberOfInputTetras + numberOfInputTriangles;

  output->SetPoints(outputPoints);
  output->Allocate(numberOfInputCells);

  vtkIdType cellId;
  double point0[3], point1[3], point2[3], point3[3];
  vtkIdType pointId0, pointId1, pointId2, pointId3;
  double point[3];
  vtkIdType edgePointId01, edgePointId02, edgePointId03, edgePointId12, edgePointId13, edgePointId23;
  vtkIdType pts[10];

  for (i=0; i<numberOfInputTetras; i++)
    {
    cellId = tetraIds->GetValue(i);
    input->GetCell(cellId,cell);

    cell->Points->GetPoint(0,point0);
    cell->Points->GetPoint(1,point1);
    cell->Points->GetPoint(2,point2);
    cell->Points->GetPoint(3,point3);

    pointId0 = cell->PointIds->GetId(0);
    pointId1 = cell->PointIds->GetId(1);
    pointId2 = cell->PointIds->GetId(2);
    pointId3 = cell->PointIds->GetId(3);

    edgePointId01 = edgeTable->IsEdge(pointId0,pointId1);
    if (edgePointId01 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point1[j]);
        }
      edgePointId01 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId01,pointId0,pointId1,0.5);
      edgeTable->InsertEdge(pointId0,pointId1,edgePointId01);
      }

    edgePointId02 = edgeTable->IsEdge(pointId0,pointId2);
    if (edgePointId02 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point2[j]);
        }
      edgePointId02 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId02,pointId0,pointId2,0.5);
      edgeTable->InsertEdge(pointId0,pointId2,edgePointId02);
      }

    edgePointId03 = edgeTable->IsEdge(pointId0,pointId3);
    if (edgePointId03 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point3[j]);
        }
      edgePointId03 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId03,pointId0,pointId3,0.5);
      edgeTable->InsertEdge(pointId0,pointId3,edgePointId03);
      }

    edgePointId12 = edgeTable->IsEdge(pointId1,pointId2);
    if (edgePointId12 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point1[j] + point2[j]);
        }
      edgePointId12 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId12,pointId1,pointId2,0.5);
      edgeTable->InsertEdge(pointId1,pointId2,edgePointId12);
      }

    edgePointId13 = edgeTable->IsEdge(pointId1,pointId3);
    if (edgePointId13 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point1[j] + point3[j]);
        }
      edgePointId13 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId13,pointId1,pointId3,0.5);
      edgeTable->InsertEdge(pointId1,pointId3,edgePointId13);
      }

    edgePointId23 = edgeTable->IsEdge(pointId2,pointId3);
    if (edgePointId23 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point2[j] + point3[j]);
        }
      edgePointId23 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId23,pointId2,pointId3,0.5);
      edgeTable->InsertEdge(pointId2,pointId3,edgePointId23);
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = pointId3;
    pts[4] = edgePointId01;
    pts[5] = edgePointId12;
    pts[6] = edgePointId02;
    pts[7] = edgePointId03;
    pts[8] = edgePointId13;
    pts[9] = edgePointId23;

    vtkIdType newCellId = output->InsertNextCell(VTK_QUADRATIC_TETRA, 10, pts);

    outputCellData->CopyData(inputCellData,cellId,newCellId);
    }

  for (i=0; i<numberOfInputTriangles; i++)
    {
    cellId = triangleIds->GetValue(i);
    input->GetCell(cellId,cell);

    cell->Points->GetPoint(0,point0);
    cell->Points->GetPoint(1,point1);
    cell->Points->GetPoint(2,point2);

    pointId0 = cell->PointIds->GetId(0);
    pointId1 = cell->PointIds->GetId(1);
    pointId2 = cell->PointIds->GetId(2);

    edgePointId01 = edgeTable->IsEdge(pointId0,pointId1);
    if (edgePointId01 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point1[j]);
        }
      edgePointId01 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId01,pointId0,pointId1,0.5);
      edgeTable->InsertEdge(pointId0,pointId1,edgePointId01);
      }

    edgePointId02 = edgeTable->IsEdge(pointId0,pointId2);
    if (edgePointId02 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point2[j]);
        }
      edgePointId02 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId02,pointId0,pointId2,0.5);
      edgeTable->InsertEdge(pointId0,pointId2,edgePointId02);
      }

    edgePointId12 = edgeTable->IsEdge(pointId1,pointId2);
    if (edgePointId12 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point1[j] + point2[j]);
        }
      edgePointId12 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId12,pointId1,pointId2,0.5);
      edgeTable->InsertEdge(pointId1,pointId2,edgePointId12);
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = edgePointId01;
    pts[4] = edgePointId12;
    pts[5] = edgePointId02;

    vtkIdType newCellId = output->InsertNextCell(VTK_QUADRATIC_TRIANGLE, 6, pts);

    outputCellData->CopyData(inputCellData,cellId,newCellId);
    }

  outputPoints->Delete();
  edgeTable->Delete();

  triangleIds->Delete();
  tetraIds->Delete();

  cell->Delete();

  output->Squeeze();

  return 1;
}

void vtkvmtkLinearToQuadraticMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
