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
#include "vtkvmtkGaussQuadrature.h"
#include "vtkvmtkFEShapeFunctions.h"
#include "vtkGeometryFilter.h"
#include "vtkInterpolatingSubdivisionFilter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkButterflySubdivisionFilter.h"
#include "vtkPolyData.h"
#include "vtkCellLocator.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkGenericCell.h"
#include "vtkEdgeTable.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkLinearToQuadraticMeshFilter);

vtkvmtkLinearToQuadraticMeshFilter::vtkvmtkLinearToQuadraticMeshFilter()
{
  this->UseBiquadraticWedge = 1;
  this->NumberOfNodesHexahedra = 27;
  this->ReferenceSurface = NULL;
  this->CellEntityIdsArrayName = NULL;
  this->ProjectedCellEntityId = -1;
  this->QuadratureOrder = 10;
  this->NegativeJacobianTolerance = 0.0;
  this->JacobianRelaxation = 1;
  this->TestFinalJacobians = 0;
}

vtkvmtkLinearToQuadraticMeshFilter::~vtkvmtkLinearToQuadraticMeshFilter()
{
  if (this->ReferenceSurface)
    {
    this->ReferenceSurface->Delete();
    this->ReferenceSurface = NULL;
    }

  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
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

  vtkIntArray* cellEntityIdsArray = vtkIntArray::New();
  if (this->ReferenceSurface)
    {
    if (this->CellEntityIdsArrayName)
      {
      if (input->GetCellData()->GetArray(this->CellEntityIdsArrayName) == NULL)
        {
        vtkErrorMacro(<< "Error: CellEntityIdsArray with name specified does not exist");
        return 1;
        }
      cellEntityIdsArray->DeepCopy(input->GetCellData()->GetArray(this->CellEntityIdsArrayName));
      }
    }

  vtkPoints* outputPoints = vtkPoints::New();

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkEdgeTable* edgeTable = vtkEdgeTable::New();
  edgeTable->InitEdgeInsertion(numberOfInputPoints,1);

  vtkEdgeTable* faceTable = vtkEdgeTable::New();
  faceTable->InitEdgeInsertion(numberOfInputPoints,1);

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

  vtkIdTypeArray* wedgeIds = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_WEDGE,wedgeIds);

  int numberOfInputWedges = wedgeIds->GetNumberOfTuples();

  vtkIdTypeArray* hexahedraIds = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_HEXAHEDRON,hexahedraIds);

  int numberOfInputHexahedra = hexahedraIds->GetNumberOfTuples();

  vtkIdTypeArray* quadIds = vtkIdTypeArray::New();
  input->GetIdsOfCellsOfType(VTK_QUAD,quadIds);

  int numberOfInputQuads = quadIds->GetNumberOfTuples();

  int numberOfInputCells = numberOfInputTetras + numberOfInputTriangles + numberOfInputWedges + numberOfInputHexahedra + numberOfInputQuads;

  output->SetPoints(outputPoints);
  output->Allocate(numberOfInputCells);

  int numberOfCells = input->GetNumberOfCells();
  vtkIdList* inputToOutputCellIds = vtkIdList::New();
  inputToOutputCellIds->SetNumberOfIds(numberOfInputCells);

  for (i=0; i<numberOfCells; i++)
    {
    inputToOutputCellIds->SetId(i,-1);
    }

  vtkIdType cellId;
  double point0[3], point1[3], point2[3], point3[3], point4[3], point5[3], point6[3], point7[3];
  vtkIdType pointId0, pointId1, pointId2, pointId3, pointId4, pointId5, pointId6, pointId7;
  double point[3];
  vtkIdType edgePointId01, edgePointId02, edgePointId03, edgePointId12, edgePointId13, edgePointId23;
  vtkIdType edgePointId14, edgePointId34, edgePointId45, edgePointId35, edgePointId25, edgePointId37;
  vtkIdType edgePointId56, edgePointId67, edgePointId74, edgePointId04, edgePointId15, edgePointId26;
  vtkIdType facePointId0143, facePointId1254, facePointId2035;
  vtkIdType facePointId3047, facePointId1265, facePointId0154, facePointId2376, facePointId0123, facePointId4567;
  vtkIdType midVolumePoint;
  facePointId0143 = facePointId1254 = facePointId2035 = -1;
  facePointId3047 = facePointId1265 = facePointId0154 = facePointId2376 = facePointId0123 = facePointId4567 = -1;
  midVolumePoint = -1;
  vtkIdType neighborCellId;
  vtkIdType pts[27];
  double weights[4];
  double weightsVolume[8];
  weights[0] = weights[1] = weights[2] = weights[3] = 0.25;
  weightsVolume[0] = weightsVolume[1] = weightsVolume[2] = weightsVolume[3] = weightsVolume[4] = weightsVolume[5] = weightsVolume[6] = weightsVolume[7] = 0.125;

  vtkIdList* facePointIds = vtkIdList::New();
  vtkIdList* cellNeighbors = vtkIdList::New();

  for (i=0; i<numberOfInputWedges; i++)
    {
    cellId = wedgeIds->GetValue(i);
    input->GetCell(cellId,cell);

    cell->Points->GetPoint(0,point0);
    cell->Points->GetPoint(1,point1);
    cell->Points->GetPoint(2,point2);
    cell->Points->GetPoint(3,point3);
    cell->Points->GetPoint(4,point4);
    cell->Points->GetPoint(5,point5);

    pointId0 = cell->PointIds->GetId(0);
    pointId1 = cell->PointIds->GetId(1);
    pointId2 = cell->PointIds->GetId(2);
    pointId3 = cell->PointIds->GetId(3);
    pointId4 = cell->PointIds->GetId(4);
    pointId5 = cell->PointIds->GetId(5);

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

    edgePointId34 = edgeTable->IsEdge(pointId3,pointId4);
    if (edgePointId34 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point3[j] + point4[j]);
        }
      edgePointId34 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId34,pointId3,pointId4,0.5);
      edgeTable->InsertEdge(pointId3,pointId4,edgePointId34);
      }

    edgePointId35 = edgeTable->IsEdge(pointId3,pointId5);
    if (edgePointId35 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point3[j] + point5[j]);
        }
      edgePointId35 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId35,pointId3,pointId5,0.5);
      edgeTable->InsertEdge(pointId3,pointId5,edgePointId35);
      }

    edgePointId45 = edgeTable->IsEdge(pointId4,pointId5);
    if (edgePointId45 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point4[j] + point5[j]);
        }
      edgePointId45 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId45,pointId4,pointId5,0.5);
      edgeTable->InsertEdge(pointId4,pointId5,edgePointId45);
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

    edgePointId14 = edgeTable->IsEdge(pointId1,pointId4);
    if (edgePointId14 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point1[j] + point4[j]);
        }
      edgePointId14 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId14,pointId1,pointId4,0.5);
      edgeTable->InsertEdge(pointId1,pointId4,edgePointId14);
      }

    edgePointId25 = edgeTable->IsEdge(pointId2,pointId5);
    if (edgePointId25 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point2[j] + point5[j]);
        }
      edgePointId25 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId25,pointId2,pointId5,0.5);
      edgeTable->InsertEdge(pointId2,pointId5,edgePointId25);
      }

    if (this->UseBiquadraticWedge)
      {
      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId0);
      facePointIds->InsertNextId(pointId1);
      facePointIds->InsertNextId(pointId4);
      facePointIds->InsertNextId(pointId3);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      neighborCellId = -1;
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_WEDGE)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId0143 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId0143 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point0[j] + point1[j] + point4[j] + point3[j]);
            }
          facePointId0143 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId0143,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId0143);
          }
        }
  
      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId1);
      facePointIds->InsertNextId(pointId2);
      facePointIds->InsertNextId(pointId5);
      facePointIds->InsertNextId(pointId4);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      neighborCellId = -1;
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_WEDGE)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId1254 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId1254 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point1[j] + point2[j] + point5[j] + point4[j]);
            }
          facePointId1254 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId1254,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId1254);
          }
        }
  
      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId2);
      facePointIds->InsertNextId(pointId0);
      facePointIds->InsertNextId(pointId3);
      facePointIds->InsertNextId(pointId5);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      neighborCellId = -1;
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_WEDGE)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId2035 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId2035 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point2[j] + point0[j] + point3[j] + point5[j]);
            }
          facePointId2035 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId2035,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId2035);
          }
        }
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = pointId3;
    pts[4] = pointId4;
    pts[5] = pointId5;

    pts[6] = edgePointId01;
    pts[7] = edgePointId12;
    pts[8] = edgePointId02;
    pts[9] = edgePointId34;
    pts[10] = edgePointId45;
    pts[11] = edgePointId35;
    pts[12] = edgePointId03;
    pts[13] = edgePointId14;
    pts[14] = edgePointId25;
    pts[15] = facePointId0143;
    pts[16] = facePointId1254;
    pts[17] = facePointId2035;

    vtkIdType newCellId;

#if VTK_MAJOR_VERSION > 5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 0)
    if (this->UseBiquadraticWedge)
      {
      newCellId = output->InsertNextCell(VTK_BIQUADRATIC_QUADRATIC_WEDGE, 18, pts);
      }
    else
      {
      newCellId = output->InsertNextCell(VTK_QUADRATIC_WEDGE, 15, pts);
      }
#else
    newCellId = output->InsertNextCell(VTK_QUADRATIC_WEDGE, 15, pts);
#endif

    inputToOutputCellIds->SetId(cellId,newCellId);
    outputCellData->CopyData(inputCellData,cellId,newCellId);
    }

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

    inputToOutputCellIds->SetId(cellId,newCellId);
    outputCellData->CopyData(inputCellData,cellId,newCellId);
    }

  for (i=0; i<numberOfInputHexahedra; i++)
    {
    cellId = hexahedraIds->GetValue(i);
    input->GetCell(cellId,cell);

    cell->Points->GetPoint(0,point0);
    cell->Points->GetPoint(1,point1);
    cell->Points->GetPoint(2,point2);
    cell->Points->GetPoint(3,point3);
    cell->Points->GetPoint(4,point4);
    cell->Points->GetPoint(5,point5);
    cell->Points->GetPoint(6,point6);
    cell->Points->GetPoint(7,point7);

    pointId0 = cell->PointIds->GetId(0);
    pointId1 = cell->PointIds->GetId(1);
    pointId2 = cell->PointIds->GetId(2);
    pointId3 = cell->PointIds->GetId(3);
    pointId4 = cell->PointIds->GetId(4);
    pointId5 = cell->PointIds->GetId(5);
    pointId6 = cell->PointIds->GetId(6);
    pointId7 = cell->PointIds->GetId(7);

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

    edgePointId04 = edgeTable->IsEdge(pointId0,pointId4);
    if (edgePointId04 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point0[j] + point4[j]);
        }
      edgePointId04 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId04,pointId0,pointId4,0.5);
      edgeTable->InsertEdge(pointId0,pointId4,edgePointId04);
      }

    edgePointId15 = edgeTable->IsEdge(pointId1,pointId5);
    if (edgePointId15 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point1[j] + point5[j]);
        }
      edgePointId15 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId15,pointId1,pointId5,0.5);
      edgeTable->InsertEdge(pointId1,pointId5,edgePointId15);
      }

    edgePointId26 = edgeTable->IsEdge(pointId2,pointId6);
    if (edgePointId26 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point2[j] + point6[j]);
        }
      edgePointId26 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId26,pointId2,pointId6,0.5);
      edgeTable->InsertEdge(pointId2,pointId6,edgePointId26);
      }

    edgePointId37 = edgeTable->IsEdge(pointId3,pointId7);
    if (edgePointId37 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point3[j] + point7[j]);
        }
      edgePointId37 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId37,pointId3,pointId7,0.5);
      edgeTable->InsertEdge(pointId3,pointId7,edgePointId37);
      }

    edgePointId45 = edgeTable->IsEdge(pointId4,pointId5);
    if (edgePointId45 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point4[j] + point5[j]);
        }
      edgePointId45 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId45,pointId4,pointId5,0.5);
      edgeTable->InsertEdge(pointId4,pointId5,edgePointId45);
      }

    edgePointId56 = edgeTable->IsEdge(pointId5,pointId6);
    if (edgePointId56 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point5[j] + point6[j]);
        }
      edgePointId56 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId56,pointId5,pointId6,0.5);
      edgeTable->InsertEdge(pointId5,pointId6,edgePointId56);
      }

    edgePointId67 = edgeTable->IsEdge(pointId6,pointId7);
    if (edgePointId67 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point6[j] + point7[j]);
        }
      edgePointId67 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId67,pointId6,pointId7,0.5);
      edgeTable->InsertEdge(pointId6,pointId7,edgePointId67);
      }

    edgePointId74 = edgeTable->IsEdge(pointId4,pointId7);
    if (edgePointId74 == -1)
      {
      for (j=0; j<3; j++)
        {
        point[j] = 0.5 * (point4[j] + point7[j]);
        }
      edgePointId74 = outputPoints->InsertNextPoint(point);
      outputPointData->InterpolateEdge(inputPointData,edgePointId74,pointId4,pointId7,0.5);
      edgeTable->InsertEdge(pointId4,pointId7,edgePointId74);
      }

    if (this->NumberOfNodesHexahedra > 20)
      {

      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId0);
      facePointIds->InsertNextId(pointId1);
      facePointIds->InsertNextId(pointId5);
      facePointIds->InsertNextId(pointId4);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      facePointId0154 = -1;
      if (cellNeighbors->GetNumberOfIds() == 0)
        {
        neighborCellId = numberOfInputCells + 1;
        }
      else
        {
        neighborCellId = -1;
        }
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId0154 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId0154 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point0[j] + point1[j] + point5[j] + point4[j]);
            }
          facePointId0154 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId0154,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId0154);
          }
        }

      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId1);
      facePointIds->InsertNextId(pointId2);
      facePointIds->InsertNextId(pointId6);
      facePointIds->InsertNextId(pointId5);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      facePointId1265 = -1;
      if (cellNeighbors->GetNumberOfIds() == 0)
        {
        neighborCellId = numberOfInputCells + 2;
        }
      else
        {
        neighborCellId = -1;
        }
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId1265 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId1265 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point1[j] + point2[j] + point6[j] + point5[j]);
            }
          facePointId1265 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId1265,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId1265);
          }
        }

      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId2);
      facePointIds->InsertNextId(pointId3);
      facePointIds->InsertNextId(pointId7);
      facePointIds->InsertNextId(pointId6);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      facePointId2376 = -1;
      if (cellNeighbors->GetNumberOfIds() == 0)
        {
        neighborCellId = numberOfInputCells + 3;
        }
      else
        {
        neighborCellId = -1;
        }
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId2376 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId2376 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point2[j] + point3[j] + point7[j] + point6[j]);
            }
          facePointId2376 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId2376,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId2376);
          }
        }

      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId3);
      facePointIds->InsertNextId(pointId0);
      facePointIds->InsertNextId(pointId4);
      facePointIds->InsertNextId(pointId7);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      facePointId3047 = -1;
      if (cellNeighbors->GetNumberOfIds() == 0)
        {
        neighborCellId = numberOfInputCells + 4;
        }
      else
        {
        neighborCellId = -1;
        }
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
          {
          neighborCellId = cellNeighbors->GetId(j);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        facePointId3047 = faceTable->IsEdge(cellId,neighborCellId);
        if (facePointId3047 == -1)
          {
          for (j=0; j<3; j++)
            {
            point[j] = 0.25 * (point3[j] + point0[j] + point4[j] + point7[j]);
            }
          facePointId3047 = outputPoints->InsertNextPoint(point);
          outputPointData->InterpolatePoint(inputPointData,facePointId3047,facePointIds,weights);
          faceTable->InsertEdge(cellId,neighborCellId,facePointId3047);
          }
        }

      if (this->NumberOfNodesHexahedra > 24)
        {

        facePointIds->Initialize();
        facePointIds->InsertNextId(pointId0);
        facePointIds->InsertNextId(pointId1);
        facePointIds->InsertNextId(pointId2);
        facePointIds->InsertNextId(pointId3);
        input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
        facePointId0123 = -1;
        if (cellNeighbors->GetNumberOfIds() == 0)
          {
          neighborCellId = numberOfInputCells + 5;
          }
        else
          {
          neighborCellId = -1;
          }
        for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
          {
          if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
            {
            neighborCellId = cellNeighbors->GetId(j);
            break;
            }
          }
        if (neighborCellId != -1)
          {
          facePointId0123 = faceTable->IsEdge(cellId,neighborCellId);
          if (facePointId0123 == -1)
            {
            for (j=0; j<3; j++)
              {
              point[j] = 0.25 * (point0[j] + point1[j] + point2[j] + point3[j]);
              }
            facePointId0123 = outputPoints->InsertNextPoint(point);
            outputPointData->InterpolatePoint(inputPointData,facePointId0123,facePointIds,weights);
            faceTable->InsertEdge(cellId,neighborCellId,facePointId0123);
            }
          }

        facePointIds->Initialize();
        facePointIds->InsertNextId(pointId4);
        facePointIds->InsertNextId(pointId5);
        facePointIds->InsertNextId(pointId6);
        facePointIds->InsertNextId(pointId7);
        input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
        facePointId4567 = -1;
        if (cellNeighbors->GetNumberOfIds() == 0)
          {
          neighborCellId = numberOfInputCells + 6;
          }
        else
          {
          neighborCellId = -1;
          }
        for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
          {
          if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_QUAD ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_BIQUADRATIC_QUAD)
            {
            neighborCellId = cellNeighbors->GetId(j);
            break;
            }
          }
        if (neighborCellId != -1)
          {
          facePointId4567 = faceTable->IsEdge(cellId,neighborCellId);
          if (facePointId4567 == -1)
            {
            for (j=0; j<3; j++)
              {
              point[j] = 0.25 * (point4[j] + point5[j] + point6[j] + point7[j]);
              }
            facePointId4567 = outputPoints->InsertNextPoint(point);
            outputPointData->InterpolatePoint(inputPointData,facePointId4567,facePointIds,weights);
            faceTable->InsertEdge(cellId,neighborCellId,facePointId4567);
            }
          }

        facePointIds->Initialize();
        facePointIds->InsertNextId(pointId0);
        facePointIds->InsertNextId(pointId1);
        facePointIds->InsertNextId(pointId2);
        facePointIds->InsertNextId(pointId3);
        facePointIds->InsertNextId(pointId4);
        facePointIds->InsertNextId(pointId5);
        facePointIds->InsertNextId(pointId6);
        facePointIds->InsertNextId(pointId7);
        for (j=0; j<3; j++)
          {
          point[j] = 0.125 * (point0[j] + point1[j] + point2[j] + point3[j] + point4[j] + point5[j] + point6[j] + point7[j]);
          }
        midVolumePoint = outputPoints->InsertNextPoint(point);
        outputPointData->InterpolatePoint(inputPointData,midVolumePoint,facePointIds,weightsVolume);
        }
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = pointId3;
    pts[4] = pointId4;
    pts[5] = pointId5;
    pts[6] = pointId6;
    pts[7] = pointId7;
    pts[8] = edgePointId01;
    pts[9] = edgePointId12;
    pts[10] = edgePointId23;
    pts[11] = edgePointId03;
    pts[12] = edgePointId45;
    pts[13] = edgePointId56;
    pts[14] = edgePointId67;
    pts[15] = edgePointId74;
    pts[16] = edgePointId04;
    pts[17] = edgePointId15;
    pts[18] = edgePointId26;
    pts[19] = edgePointId37;

    if (this->NumberOfNodesHexahedra > 20)
      {
      pts[20] = facePointId3047;
      pts[21] = facePointId1265;
      pts[22] = facePointId0154;
      pts[23] = facePointId2376;
      if (this->NumberOfNodesHexahedra > 24)
        {
        pts[24] = facePointId0123;
        pts[25] = facePointId4567;
        pts[26] = midVolumePoint;
        }
      }

    vtkIdType newCellId;

    switch (this->NumberOfNodesHexahedra)
      {
      case 20:
        {
          newCellId = output->InsertNextCell(VTK_QUADRATIC_HEXAHEDRON, 20, pts);
          break;
        }
      case 24:
        {
          newCellId = output->InsertNextCell(VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON, 24, pts);
          break;
        }
      case 27:
        {
          newCellId = output->InsertNextCell(VTK_TRIQUADRATIC_HEXAHEDRON, 27, pts);
          break;
        }
      default:
        {
          newCellId = output->InsertNextCell(VTK_HEXAHEDRON, 8, pts);
          vtkErrorMacro(<<"Invalid number of nodes for a quadratic hexahedral mesh- choose 20, 24, or 27.");
          return 1;
        }
      }

    inputToOutputCellIds->SetId(cellId,newCellId);
    outputCellData->CopyData(inputCellData,cellId,newCellId);
    }

  vtkCellLocator* locator = NULL;
  if (this->ReferenceSurface)
    {
    locator = vtkCellLocator::New();
    locator->SetDataSet(this->ReferenceSurface);
    locator->BuildLocator();
    }

  double projectedPoint[3];
  vtkIdType referenceCellId;
  int subId;
  double dist2;

  vtkEdgeTable* surfaceEdgeTable = vtkEdgeTable::New();
  vtkEdgeTable* surfaceFaceTable = vtkEdgeTable::New();
  surfaceEdgeTable->InitEdgeInsertion(numberOfInputPoints,1);
  surfaceFaceTable->InitEdgeInsertion(numberOfInputPoints,1);

  for (i=0; i<numberOfInputTriangles; i++)
    {
    cellId = triangleIds->GetValue(i);
    input->GetCell(cellId,cell);

    bool project = true;
    if (locator)
      {
      if (this->CellEntityIdsArrayName) 
        {
        if (cellEntityIdsArray->GetValue(cellId) != this->ProjectedCellEntityId)
          {
          project = false;
          }
        }
      }

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

    if (project)
      {
      surfaceEdgeTable->InsertEdge(pointId0,pointId1,edgePointId01);
      surfaceEdgeTable->InsertEdge(pointId0,pointId2,edgePointId02);
      surfaceEdgeTable->InsertEdge(pointId1,pointId2,edgePointId12);
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = edgePointId01;
    pts[4] = edgePointId12;
    pts[5] = edgePointId02;

    vtkIdType newCellId = output->InsertNextCell(VTK_QUADRATIC_TRIANGLE, 6, pts);

    inputToOutputCellIds->SetId(cellId,newCellId);
    outputCellData->CopyData(inputCellData,cellId,newCellId);

    if (locator && project)
      {
      double point01[3], point12[3], point02[3];
      outputPoints->GetPoint(edgePointId01,point01);
      locator->FindClosestPoint(point01,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId01,projectedPoint);
      outputPoints->GetPoint(edgePointId12,point12);
      locator->FindClosestPoint(point12,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId12,projectedPoint);
      outputPoints->GetPoint(edgePointId02,point02);
      locator->FindClosestPoint(point02,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId02,projectedPoint);
      }
    }

  for (i=0; i<numberOfInputQuads; i++)
    {
    cellId = quadIds->GetValue(i);
    input->GetCell(cellId,cell);

    bool project = true;
    if (locator)
      {
      if (this->CellEntityIdsArrayName) 
        {
        if (cellEntityIdsArray->GetValue(cellId) != this->ProjectedCellEntityId)
          {
          project = false;
          }
        }
      }

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

    if (project)
      {
      surfaceEdgeTable->InsertEdge(pointId0,pointId1,edgePointId01);
      surfaceEdgeTable->InsertEdge(pointId1,pointId2,edgePointId12);
      surfaceEdgeTable->InsertEdge(pointId2,pointId3,edgePointId23);
      surfaceEdgeTable->InsertEdge(pointId0,pointId3,edgePointId03);
      }

    pts[0] = pointId0;
    pts[1] = pointId1;
    pts[2] = pointId2;
    pts[3] = pointId3;
    pts[4] = edgePointId01;
    pts[5] = edgePointId12;
    pts[6] = edgePointId23;
    pts[7] = edgePointId03;

    if (this->NumberOfNodesHexahedra > 20)
      {
      facePointIds->Initialize();
      facePointIds->InsertNextId(pointId0);
      facePointIds->InsertNextId(pointId1);
      facePointIds->InsertNextId(pointId2);
      facePointIds->InsertNextId(pointId3);
      input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
      neighborCellId = -1;
      for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
        {
        if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
            input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON)
          {
          neighborCellId = cellNeighbors->GetId(j);
          facePointId0123 = faceTable->IsEdge(cellId,neighborCellId);
          break;
          }
        }
      if (neighborCellId != -1)
        {
        pts[8] = facePointId0123;
        if (project)
          {
          surfaceFaceTable->InsertEdge(cellId,neighborCellId,facePointId0123);
          }
        }
      }

    vtkIdType newCellId;
    switch (this->NumberOfNodesHexahedra)
      {
      case 20:
        {
          newCellId = output->InsertNextCell(VTK_QUADRATIC_QUAD, 8, pts);
          break;
        }
      case 27:
        {
          newCellId = output->InsertNextCell(VTK_BIQUADRATIC_QUAD, 9, pts);
          break;
        }
      default:
        {
          newCellId = output->InsertNextCell(VTK_QUAD, 4, pts);
          vtkErrorMacro(<<"Invalid number of nodes (" << this->NumberOfNodesHexahedra << ") for a hexahedral mesh with quadratic quad surface- choose 20 or 27.\n");
          return 1;
        }
      }

    inputToOutputCellIds->SetId(cellId,newCellId);
    outputCellData->CopyData(inputCellData,cellId,newCellId);

    if (locator && project)
      {
      double point01[3], point12[3], point23[3], point03[3], facePoint[3];
      outputPoints->GetPoint(edgePointId01,point01);
      locator->FindClosestPoint(point01,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId01,projectedPoint);
      outputPoints->GetPoint(edgePointId12,point12);
      locator->FindClosestPoint(point12,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId12,projectedPoint);
      outputPoints->GetPoint(edgePointId23,point23);
      locator->FindClosestPoint(point23,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId23,projectedPoint);
      outputPoints->GetPoint(edgePointId03,point03);
      locator->FindClosestPoint(point03,projectedPoint,referenceCellId,subId,dist2);
      outputPoints->SetPoint(edgePointId03,projectedPoint);
      if (this->NumberOfNodesHexahedra > 20)
        {
        outputPoints->GetPoint(facePointId0123,facePoint);
        locator->FindClosestPoint(facePoint,projectedPoint,referenceCellId,subId,dist2);
        outputPoints->SetPoint(facePointId0123,projectedPoint);
        }
      }
    }

//#define LEGACY_RELAXATION
#ifdef LEGACY_RELAXATION
  int numberOfRelaxationSteps = 10;
  int maxSignChangeIterations = 20;
  int signChangeCounter = 0;
  bool anySignChange = true;
  while (anySignChange)
    {
    if (signChangeCounter >= maxSignChangeIterations)
      {
      break;
      }
    signChangeCounter++;
    anySignChange = false;
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
      edgePointId12 = edgeTable->IsEdge(pointId1,pointId2);
      edgePointId02 = edgeTable->IsEdge(pointId0,pointId2);
   
      if (locator)
        {
        bool project = true;
        if (this->CellEntityIdsArrayName) 
          {
          if (cellEntityIdsArray->GetValue(cellId) != this->ProjectedCellEntityId)
            {
            project = false;
            }
          }

        if (project)
          {
          vtkIdType volumeCellId = -1;
          vtkIdList* volumeCellIds = vtkIdList::New();
          output->GetCellNeighbors(cellId,cell->PointIds,volumeCellIds);
          if (volumeCellIds->GetNumberOfIds() == 0)
            {
            vtkWarningMacro(<<"Warning: surface element "<<cellId<<" does not have a volume element attached.");
            volumeCellIds->Delete();
            continue;
            }
          if (volumeCellIds->GetNumberOfIds() > 1)
            {
            vtkWarningMacro(<<"Warning: surface element "<<cellId<<" has more than one volume element attached.");
            volumeCellIds->Delete();
            continue;
            }
          volumeCellId = volumeCellIds->GetId(0);
          vtkCell* linearVolumeCell = input->GetCell(volumeCellId);
          vtkCell* quadraticVolumeCell = output->GetCell(volumeCellId);
          double point01[3], point12[3], point02[3];
          double projectedPoint01[3], projectedPoint12[3], projectedPoint02[3];
          outputPoints->GetPoint(edgePointId01,projectedPoint01);
          outputPoints->GetPoint(edgePointId12,projectedPoint12);
          outputPoints->GetPoint(edgePointId02,projectedPoint02);
          int s;
          for (s=0; s<numberOfRelaxationSteps; s++)
            {
            if (!this->HasJacobianChangedSign(linearVolumeCell,quadraticVolumeCell))
              {
              break;
              }
            anySignChange = true;
            vtkWarningMacro(<<"Warning: projection causes element "<<volumeCellId<<" to have a negative Jacobian somewhere. Relaxing projection for this element.");
            double relaxation = (double)(s+1)/(double)numberOfRelaxationSteps;
            for (j=0; j<3; j++)
              {
              point01[j] = (1.0 - relaxation) * projectedPoint01[j] + relaxation * (0.5 * (point0[j] + point1[j]));
              point12[j] = (1.0 - relaxation) * projectedPoint12[j] + relaxation * (0.5 * (point1[j] + point2[j]));
              point02[j] = (1.0 - relaxation) * projectedPoint02[j] + relaxation * (0.5 * (point0[j] + point2[j]));
              }
            outputPoints->SetPoint(edgePointId01,point01);
            outputPoints->SetPoint(edgePointId12,point12);
            outputPoints->SetPoint(edgePointId02,point02);
            quadraticVolumeCell = output->GetCell(volumeCellId);
            }
          volumeCellIds->Delete();
          }
        }
      }

    for (i=0; i<numberOfInputQuads; i++)
      {
      cellId = quadIds->GetValue(i);
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
      edgePointId12 = edgeTable->IsEdge(pointId1,pointId2);
      edgePointId23 = edgeTable->IsEdge(pointId2,pointId3);
      edgePointId03 = edgeTable->IsEdge(pointId0,pointId3);

      if (locator)
        {
        bool project = true;
        if (this->CellEntityIdsArrayName) 
          {
          if (cellEntityIdsArray->GetValue(cellId) != this->ProjectedCellEntityId)
            {
            project = false;
            }
          }
        if (project)
          {
          vtkIdType volumeCellId = 0;
          vtkIdList* volumeCellIds = vtkIdList::New();
          output->GetCellNeighbors(cellId,cell->PointIds,volumeCellIds);
          if (volumeCellIds->GetNumberOfIds() == 0)
            {
            vtkWarningMacro(<<"Warning: surface element "<<cellId<<" does not have a volume element attached.");
            volumeCellIds->Delete();
            continue;
            }
          if (volumeCellIds->GetNumberOfIds() > 1)
            {
            vtkWarningMacro(<<"Warning: surface element "<<cellId<<" has more than one volume element attached.");
            volumeCellIds->Delete();
            continue;
            }
          volumeCellId = volumeCellIds->GetId(0);
          vtkCell* linearVolumeCell = input->GetCell(volumeCellId);
          vtkCell* quadraticVolumeCell = output->GetCell(volumeCellId);
          double point01[3], point12[3], point23[3], point03[3];
          double projectedPoint01[3], projectedPoint12[3], projectedPoint23[3], projectedPoint03[3];
          outputPoints->GetPoint(edgePointId01,projectedPoint01);
          outputPoints->GetPoint(edgePointId12,projectedPoint12);
          outputPoints->GetPoint(edgePointId23,projectedPoint23);
          outputPoints->GetPoint(edgePointId03,projectedPoint03);

          int s;
          for (s=0; s<numberOfRelaxationSteps; s++)
            {
            if (!this->HasJacobianChangedSign(linearVolumeCell,quadraticVolumeCell))
              {
              break;
              }
            anySignChange = true;
            vtkWarningMacro(<<"Warning: projection causes element "<<volumeCellId<<" to have a negative Jacobian somewhere. Relaxing projection for this element.");
            double relaxation = (double)(s+1)/(double)numberOfRelaxationSteps;
            for (j=0; j<3; j++)
              {
              point01[j] = (1.0 - relaxation) * projectedPoint01[j] + relaxation * (0.5 * (point0[j] + point1[j]));
              point12[j] = (1.0 - relaxation) * projectedPoint12[j] + relaxation * (0.5 * (point1[j] + point2[j]));
              point23[j] = (1.0 - relaxation) * projectedPoint23[j] + relaxation * (0.5 * (point2[j] + point3[j]));
              point03[j] = (1.0 - relaxation) * projectedPoint03[j] + relaxation * (0.5 * (point0[j] + point3[j]));
              }
            outputPoints->SetPoint(edgePointId01,point01);
            outputPoints->SetPoint(edgePointId12,point12);
            outputPoints->SetPoint(edgePointId23,point23);
            outputPoints->SetPoint(edgePointId03,point03);
            quadraticVolumeCell = output->GetCell(volumeCellId);
            }
          volumeCellIds->Delete();
          }
        }
      }
    }
#else
  int numberOfRelaxationSteps = 10;
  int maxSignChangeIterations = 20;
  int signChangeCounter = 0;
  bool anySignChange = true;
  if (!locator)
    {
    anySignChange = false;
    }
  while (anySignChange)
    {
    if (signChangeCounter >= maxSignChangeIterations)
      {
      break;
      }
    signChangeCounter++;
    anySignChange = false;

    surfaceEdgeTable->InitTraversal();
    vtkIdType edgePointId = surfaceEdgeTable->GetNextEdge(pointId0,pointId1);
    vtkIdList* edgePointIds = vtkIdList::New();
    vtkIdList* cellEdgeNeighbors = vtkIdList::New();
    while (edgePointId >= 0)
      {
      edgePointIds->Initialize();
      edgePointIds->InsertNextId(pointId0);
      edgePointIds->InsertNextId(pointId1);
      input->GetCellNeighbors(-1,edgePointIds,cellEdgeNeighbors);
      int numberOfCellEdgeNeighbors = cellEdgeNeighbors->GetNumberOfIds();
      for (i=0; i<numberOfCellEdgeNeighbors; i++)
        {
        vtkIdType volumeCellId = cellEdgeNeighbors->GetId(i);
        if (input->GetCell(volumeCellId)->GetCellDimension() != 3)
          {
          continue;
          }
        vtkCell* linearVolumeCell = input->GetCell(volumeCellId);
        vtkIdType outputCellId = inputToOutputCellIds->GetId(volumeCellId);
        if (outputCellId == -1)
          {
          continue;
          }
        vtkCell* quadraticVolumeCell = output->GetCell(outputCellId);

        int s;
        for (s=0; s<numberOfRelaxationSteps; s++)
          {
          if (!this->HasJacobianChangedSign(linearVolumeCell,quadraticVolumeCell))
            {
            break;
            }
          anySignChange = true;
          vtkWarningMacro(<<"Warning: projection causes element "<<volumeCellId<<" to have a negative Jacobian somewhere. Relaxing projection for this element.");
          double relaxation = (double)(s+1)/(double)numberOfRelaxationSteps;
          int numberOfEdges = linearVolumeCell->GetNumberOfEdges();
          int e;
          for (e=0; e<numberOfEdges; e++)
            {
            vtkCell* edge = linearVolumeCell->GetEdge(e);
            vtkIdType pointId0 = edge->GetPointId(0);
            vtkIdType pointId1 = edge->GetPointId(1);
            vtkIdType edgePointId = surfaceEdgeTable->IsEdge(pointId0,pointId1);
            if (edgePointId == -1)
              {
              continue;
              }
            double edgePoint[3], relaxedEdgePoint[3];
            input->GetPoint(pointId0,point0);
            input->GetPoint(pointId1,point1);
            output->GetPoint(edgePointId,edgePoint);
            for (j=0; j<3; j++)
              {
              relaxedEdgePoint[j] = (1.0 - relaxation) * edgePoint[j] + relaxation * (0.5 * (point0[j] + point1[j]));
              }
            outputPoints->SetPoint(edgePointId,relaxedEdgePoint);
            }
          quadraticVolumeCell = output->GetCell(outputCellId);
          }
        }
      edgePointId = surfaceEdgeTable->GetNextEdge(pointId0,pointId1);
      }
    edgePointIds->Delete();
    cellEdgeNeighbors->Delete();
    }
#endif

  // Legacy relaxation for non-edge points on surface faces
  if (numberOfInputQuads > 0 && this->NumberOfNodesHexahedra > 20)
    {
    int numberOfRelaxationSteps = 10;
    int maxSignChangeIterations = 20;
    int signChangeCounter = 0;
    bool anySignChange = true;
    while (anySignChange)
      {
      if (signChangeCounter >= maxSignChangeIterations)
        {
        break;
        }
      signChangeCounter++;
      anySignChange = false;

      for (i=0; i<numberOfInputQuads; i++)
        {
        cellId = quadIds->GetValue(i);
        input->GetCell(cellId,cell);

        cell->Points->GetPoint(0,point0);
        cell->Points->GetPoint(1,point1);
        cell->Points->GetPoint(2,point2);
        cell->Points->GetPoint(3,point3);

        pointId0 = cell->PointIds->GetId(0);
        pointId1 = cell->PointIds->GetId(1);
        pointId2 = cell->PointIds->GetId(2);
        pointId3 = cell->PointIds->GetId(3);

        facePointIds->Initialize();
        facePointIds->InsertNextId(pointId0);
        facePointIds->InsertNextId(pointId1);
        facePointIds->InsertNextId(pointId2);
        facePointIds->InsertNextId(pointId3);
        input->GetCellNeighbors(cellId,facePointIds,cellNeighbors);
        neighborCellId = -1;
        facePointId0123 = -1;
        for (j=0; j<cellNeighbors->GetNumberOfIds(); j++)
          {
          if (input->GetCellType(cellNeighbors->GetId(j)) == VTK_TRIQUADRATIC_HEXAHEDRON ||
              input->GetCellType(cellNeighbors->GetId(j)) == VTK_HEXAHEDRON)
            {
            neighborCellId = cellNeighbors->GetId(j);
            facePointId0123 = faceTable->IsEdge(cellId,neighborCellId);
            break;
            }
          }
        if (locator)
          {
          bool project = true;
          if (this->CellEntityIdsArrayName) 
            {
            if (cellEntityIdsArray->GetValue(cellId) != this->ProjectedCellEntityId)
              {
              project = false;
              }
            }
          if (project)
            {
            vtkIdType volumeCellId = 0;
            vtkIdList* volumeCellIds = vtkIdList::New();
            output->GetCellNeighbors(cellId,cell->PointIds,volumeCellIds);
            if (volumeCellIds->GetNumberOfIds() == 0)
              {
              vtkWarningMacro(<<"Warning: surface element "<<cellId<<" does not have a volume element attached.");
              volumeCellIds->Delete();
              continue;
              }
            if (volumeCellIds->GetNumberOfIds() > 1)
              {
              vtkWarningMacro(<<"Warning: surface element "<<cellId<<" has more than one volume element attached.");
              volumeCellIds->Delete();
              continue;
              }
            volumeCellId = volumeCellIds->GetId(0);
            vtkCell* linearVolumeCell = input->GetCell(volumeCellId);
            vtkCell* quadraticVolumeCell = output->GetCell(volumeCellId);
            double facePoint[3];
            double projectedFacePoint0123[3];

            outputPoints->GetPoint(facePointId0123,projectedFacePoint0123);
            int s;
            for (s=0; s<numberOfRelaxationSteps; s++)
              {
              if (!this->HasJacobianChangedSign(linearVolumeCell,quadraticVolumeCell))
                {
                break;
                }
              anySignChange = true;
              vtkWarningMacro(<<"Warning: projection causes element "<<volumeCellId<<" to have a negative Jacobian somewhere. Relaxing projection for this element.");
              double relaxation = (double)(s+1)/(double)numberOfRelaxationSteps;
              for (j=0; j<3; j++)
                {
                facePoint[j] = (1.0 - relaxation) * projectedFacePoint0123[j] + relaxation * (0.25 * (point0[j] + point1[j] + point2[j] + point3[j]));
                }
              outputPoints->SetPoint(facePointId0123,facePoint);
              quadraticVolumeCell = output->GetCell(volumeCellId);
              }
            volumeCellIds->Delete();
            }
          }
        }
      }
    }

  if (this->TestFinalJacobians)
    {
    for (i=0; i<numberOfCells; i++)
      {
      vtkCell* linearVolumeCell = input->GetCell(i);
      if (linearVolumeCell->GetCellDimension() != 3)
        {
        continue;
        }
      vtkIdType outputCellId = inputToOutputCellIds->GetId(i);
      if (outputCellId == -1)
        {
        continue;
        }
      vtkCell* quadraticVolumeCell = output->GetCell(outputCellId);
  
      if (this->HasJacobianChangedSign(linearVolumeCell,quadraticVolumeCell))
        {
        vtkErrorMacro("Error: negative Jacobian detected in cell "<<outputCellId<<" even after relaxation. Output quadratic mesh will have negative Jacobians.");
        }
      }
    }

  surfaceEdgeTable->Delete();
  surfaceFaceTable->Delete();

  outputPoints->Delete();
  edgeTable->Delete();
  faceTable->Delete();

  triangleIds->Delete();
  tetraIds->Delete();
  wedgeIds->Delete();
  hexahedraIds->Delete();
  quadIds->Delete();
  facePointIds->Delete();
  cellNeighbors->Delete();

  cell->Delete();
  if (locator)
    {
    locator->Delete();
    locator = NULL;
    }
  cellEntityIdsArray->Delete();

  inputToOutputCellIds->Delete();

  output->Squeeze();

  return 1;
}

double vtkvmtkLinearToQuadraticMeshFilter::ComputeJacobian(vtkCell* cell, double pcoords[3])
{
  double jacobian = 0.0;

  int numberOfCellPoints = cell->GetNumberOfPoints();
  double* derivs = new double[3*numberOfCellPoints];
  
  vtkvmtkFEShapeFunctions::GetInterpolationDerivs(cell,pcoords,derivs);
  
  int i, j;
  
  double jacobianMatrix[3][3];
  for (i=0; i<3; i++)
    {
    jacobianMatrix[0][i] = jacobianMatrix[1][i] = jacobianMatrix[2][i] = 0.0;
    }
  
  double x[3];
  for (j=0; j<numberOfCellPoints; j++)
    {
    cell->GetPoints()->GetPoint(j,x);
    for (i=0; i<3; i++)
      {
      jacobianMatrix[0][i] += x[i] * derivs[j];
      jacobianMatrix[1][i] += x[i] * derivs[numberOfCellPoints+j];
      jacobianMatrix[2][i] += x[i] * derivs[2*numberOfCellPoints+j];
      }
    }
  delete[] derivs;

  jacobian = vtkMath::Determinant3x3(jacobianMatrix);

  return jacobian;
}

bool vtkvmtkLinearToQuadraticMeshFilter::HasJacobianChangedSign(vtkCell* linearVolumeCell, vtkCell* quadraticVolumeCell)
{
  vtkvmtkGaussQuadrature* gaussQuadrature = vtkvmtkGaussQuadrature::New();
  gaussQuadrature->SetOrder(this->QuadratureOrder);
  gaussQuadrature->Initialize(quadraticVolumeCell->GetCellType());
  bool signChanged = false;
  int numberOfQuadraturePoints = gaussQuadrature->GetNumberOfQuadraturePoints();
  double quadraturePCoords[3];
  int q;
  for (q=0; q<numberOfQuadraturePoints; q++)
    {
    gaussQuadrature->GetQuadraturePoint(q,quadraturePCoords);
    double linearJacobian = this->ComputeJacobian(linearVolumeCell,quadraturePCoords);
    double quadraticJacobian = this->ComputeJacobian(quadraticVolumeCell,quadraturePCoords);
    if (linearJacobian*quadraticJacobian < 0.0)
      {
      signChanged = true;
      break;
      }
    }
  gaussQuadrature->Delete();
  if (signChanged)
    {
    return signChanged;
    }

  double* parametricCoords = quadraticVolumeCell->GetParametricCoords();
  int numberOfCellPoints = quadraticVolumeCell->GetNumberOfPoints();
  for (q=0; q<numberOfCellPoints; q++)
    {
    quadraturePCoords[0] = parametricCoords[q*numberOfCellPoints + 0];
    quadraturePCoords[1] = parametricCoords[q*numberOfCellPoints + 1];
    quadraturePCoords[2] = parametricCoords[q*numberOfCellPoints + 2];
    double linearJacobian = this->ComputeJacobian(linearVolumeCell,quadraturePCoords);
    double quadraticJacobian = this->ComputeJacobian(quadraticVolumeCell,quadraturePCoords);
    if (linearJacobian*quadraticJacobian < this->NegativeJacobianTolerance)
      {
      signChanged = true;
      break;
      }
    }

  return signChanged;
}

void vtkvmtkLinearToQuadraticMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
