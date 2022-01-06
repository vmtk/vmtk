/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataLineEmbedder.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataLineEmbedder.h"
#include "vtkvmtkMath.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"
#include "vtkPolyLine.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataLineEmbedder);

vtkvmtkPolyDataLineEmbedder::vtkvmtkPolyDataLineEmbedder()
{
  this->Lines = NULL;
  this->EdgeArrayName = NULL;
  this->EdgePCoordArrayName = NULL;
  this->EmbeddedLinePointIds = vtkIdList::New();
  this->EmbeddedLinePointIds->Register(this);
  this->EmbeddedLinePointIds->Delete();
  this->SnapToMeshTolerance = 0.0;
}

vtkvmtkPolyDataLineEmbedder::~vtkvmtkPolyDataLineEmbedder()
{
  this->EmbeddedLinePointIds->UnRegister(this);

  if (this->Lines)
    {
    this->Lines->Delete();
    this->Lines = NULL;
    }
}

vtkIdType vtkvmtkPolyDataLineEmbedder::GetCellId(vtkPolyData* input, vtkIdList* pointIds)
{
  vtkIdType i, cellId;
  vtkIdList *cellIds, *neighborCellIds;

  cellIds = vtkIdList::New();
  neighborCellIds = vtkIdList::New();

  for (i=0; i<pointIds->GetNumberOfIds(); i++)
    {
    input->GetCellEdgeNeighbors(-1,pointIds->GetId(i),pointIds->GetId(i+1),neighborCellIds);
    if (cellIds->GetNumberOfIds()==0)
      {
      cellIds->DeepCopy(neighborCellIds);
      }
    else
      {
      cellIds->IntersectWith(*neighborCellIds);
      }
    }  

  cellId = cellIds->GetId(0);

  cellIds->Delete();
  neighborCellIds->Delete();

  return cellId;
}

void vtkvmtkPolyDataLineEmbedder::GetNeighbors(vtkIdType pointId, vtkIdList* neighborPointIds)
{
  vtkIdType i, j;
  vtkIdType *cells, npts;
  const vtkIdType *pts;
  vtkIdType ncells;

  this->Lines->GetPointCells(pointId,ncells,cells);

  for (i=0; i<ncells; i++)
    {
    this->Lines->GetCellPoints(cells[i],npts,pts);
    for (j=0; j<npts; j++)
      {
      if (pts[j]==pointId)
        {
        if (j>0)
          {
          neighborPointIds->InsertUniqueId(pts[j-1]);
          }
        if (j<npts-1)
          {
          neighborPointIds->InsertUniqueId(pts[j+1]);
          }
        }
      }
    }
}

void vtkvmtkPolyDataLineEmbedder::OrderNeighborhood(vtkIdList* cellPointIds, vtkIdList* neighborIds, vtkIdList* embeddedPointIds, vtkIdList* snapToMeshIds, vtkDataArray* edgeArray, vtkDataArray* edgePCoordArray, vtkIdList* orderedNeighborIds)
{
  vtkIdType j, k, h;
  vtkIdType lastCellPointIdId;
  double pCoord;
  vtkIdType edgePointIds[2];
  vtkDoubleArray* orderedNeighborPCoordArray;

  orderedNeighborPCoordArray = vtkDoubleArray::New();

  for (j=0; j<cellPointIds->GetNumberOfIds(); j++)
    {
    orderedNeighborIds->InsertNextId(cellPointIds->GetId(j));
    orderedNeighborPCoordArray->InsertNextValue(0.0);

    lastCellPointIdId = orderedNeighborIds->GetNumberOfIds()-1;

    for (k=0; k<neighborIds->GetNumberOfIds(); k++)
      {
      if (snapToMeshIds->GetId(neighborIds->GetId(k))!=-1)
        {
        continue;
        }

      edgePointIds[0] = (int)edgeArray->GetComponent(neighborIds->GetId(k),0);
      edgePointIds[1] = (int)edgeArray->GetComponent(neighborIds->GetId(k),1);

      if ((cellPointIds->GetId(j)==edgePointIds[0]) && (cellPointIds->GetId((j+1)%cellPointIds->GetNumberOfIds())==edgePointIds[1]))
        {
        pCoord = edgePCoordArray->GetComponent(neighborIds->GetId(k),0);
        }
      else if ((cellPointIds->GetId(j)==edgePointIds[1]) && (cellPointIds->GetId((j+1)%cellPointIds->GetNumberOfIds())==edgePointIds[0]))
        {
        pCoord = 1.0 - edgePCoordArray->GetComponent(neighborIds->GetId(k),0);
        }
      else 
        {
        continue;
        }

      for (h=orderedNeighborIds->GetNumberOfIds()-1; h>=lastCellPointIdId; h--)
        {
        if (pCoord<orderedNeighborPCoordArray->GetValue(h))
          {
          orderedNeighborIds->InsertId(h+1,orderedNeighborIds->GetId(h));
          orderedNeighborPCoordArray->InsertValue(h+1,orderedNeighborPCoordArray->GetValue(h));
          }
        else
          {
          orderedNeighborIds->InsertId(h+1,embeddedPointIds->GetId(neighborIds->GetId(k)));
          orderedNeighborPCoordArray->InsertValue(h+1,pCoord);
          break;
          }
        }         
      }
    }

  orderedNeighborPCoordArray->Delete();
}

void vtkvmtkPolyDataLineEmbedder::Triangulate(vtkIdList* cellPointIds, vtkIdList* orderedNeighborIds, vtkIdList* triangulationIds)
{
  vtkIdType triangleIds[3];
  vtkIdType noMove[3];
  vtkIdType numberOfNeighbors;
  bool done;

  numberOfNeighbors = orderedNeighborIds->GetNumberOfIds();

  if (numberOfNeighbors==3)
    {
    triangulationIds->InsertNextId(0);
    triangulationIds->InsertNextId(1);
    triangulationIds->InsertNextId(2);
    }

  done = false;
  
  triangleIds[0] = 0;
  while (!done)
    {
    triangleIds[1] = (triangleIds[0]+1)%numberOfNeighbors;
    triangleIds[2] = (triangleIds[0]-1+numberOfNeighbors)%numberOfNeighbors;

    if ((cellPointIds->IsId(orderedNeighborIds->GetId(triangleIds[0]))==-1)||
        (cellPointIds->IsId(orderedNeighborIds->GetId(triangleIds[1]))==-1)||
        (cellPointIds->IsId(orderedNeighborIds->GetId(triangleIds[2]))==-1))
      {
      done = true;
      break;
      }

    triangleIds[0] = triangleIds[1];
    }
  

  noMove[0] = 0;
  noMove[1] = 0;
  noMove[2] = 0;

  done = false;
  while (!done)
    {
    triangulationIds->InsertNextId(triangleIds[0]);
    triangulationIds->InsertNextId(triangleIds[1]);
    triangulationIds->InsertNextId(triangleIds[2]);

    if (cellPointIds->IsId(orderedNeighborIds->GetId(triangleIds[1]))!=-1)
      {
      triangleIds[0] = triangleIds[1];
      triangleIds[1] = (triangleIds[1]+1)%numberOfNeighbors;
      noMove[1] = 0;
      ++noMove[2];
      }
    else if (cellPointIds->IsId(orderedNeighborIds->GetId(triangleIds[2]))!=-1)
      {
      triangleIds[0] = triangleIds[2];
      triangleIds[2] = (triangleIds[2]-1+numberOfNeighbors)%numberOfNeighbors;
      ++noMove[1];
      noMove[2] = 0;
      }
    else
      {
      if (noMove[1]>noMove[2])
        {
        triangleIds[0] = triangleIds[1];
        triangleIds[1] = (triangleIds[1]+1)%numberOfNeighbors;
        noMove[1] = 0;
        ++noMove[2];
        }
      else
        {
        triangleIds[0] = triangleIds[2];
        triangleIds[2] = (triangleIds[2]-1+numberOfNeighbors)%numberOfNeighbors;
        ++noMove[1];
        noMove[2] = 0;
        }
      }

    if (triangleIds[1]==triangleIds[2])
      done = true;
    }
}

int vtkvmtkPolyDataLineEmbedder::RequestData(
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

  vtkIdType i, j, k;
  vtkIdType id, lineNumberOfPoints, lineNumberOfCells, cellId;
  vtkIdType inputNumberOfPoints;
  vtkIdType npts;
  const vtkIdType *pts;
  vtkIdType edgePointIds0[2], edgePointIds1[2];
  double pCoord;
  vtkPoints* newPoints;
  vtkCellArray* newTriangles;
  vtkIdList* embeddedPointIds;
  vtkCellArray* addedTriangles;
  vtkIdList* cellPointIds;
  vtkIdList* newCellPointIds;
  vtkIdList* cellsToRemove;
  vtkPolygon* polygon;
  vtkCell* polyLine;
  vtkIdList* triangulationIds;
  vtkTriangle* triangle;
  vtkIdList* neighborIds;
  vtkIdList* orderedNeighborIds;
  vtkIdList* lineConnectedNeighbors;
  vtkIdList* snapToMeshIds;
  vtkDataArray* edgeArray;
  vtkDataArray* edgePCoordArray;
  double triangleArea, weights[3];

  vtkPointData *inputPD = input->GetPointData();
  vtkPointData *outputPD = output->GetPointData();

  if (this->Lines->GetNumberOfPoints()==0)
    {
    vtkWarningMacro(<<"Line to embed has no points.");
    return 1;
    }

  if (this->EdgeArrayName==NULL)
    {
    vtkErrorMacro(<<"Edge array name not specified.");
    return 1;
    }

  if (this->EdgePCoordArrayName==NULL)
    {
    vtkErrorMacro(<<"EdgePCoord array name not specified.");
    return 1;
    }

  edgeArray = this->Lines->GetPointData()->GetArray(this->EdgeArrayName);

  if (edgeArray==NULL)
    {
    vtkErrorMacro(<<"Edge array with name specified does not exist.");
    return 1;
    }

  edgePCoordArray = this->Lines->GetPointData()->GetArray(this->EdgePCoordArrayName);

  if (edgePCoordArray==NULL)
    {
    vtkErrorMacro(<<"EdgePCoord array with name specified does not exist.");
    return 1;
    }

  newPoints = vtkPoints::New();
  newTriangles = vtkCellArray::New();
  embeddedPointIds = vtkIdList::New();
  addedTriangles = vtkCellArray::New();
  cellPointIds = vtkIdList::New();
  newCellPointIds = vtkIdList::New();
  cellsToRemove = vtkIdList::New();
  polygon = vtkPolygon::New();
  triangulationIds = vtkIdList::New();
  neighborIds = vtkIdList::New();
  orderedNeighborIds = vtkIdList::New();
  lineConnectedNeighbors = vtkIdList::New();
  snapToMeshIds = vtkIdList::New();

  newPoints->DeepCopy(input->GetPoints());

  input->BuildCells();
  input->BuildLinks();

  this->Lines->BuildCells();
  this->Lines->BuildLinks();
  
  inputNumberOfPoints = input->GetNumberOfPoints();
    
  lineNumberOfPoints = this->Lines->GetNumberOfPoints();
  lineNumberOfCells = this->Lines->GetNumberOfCells();

  outputPD->InterpolateAllocate(inputPD,inputNumberOfPoints+lineNumberOfPoints);

  for (i=0; i<inputNumberOfPoints; i++)
    {
    outputPD->CopyData(inputPD,i,i);
    }

  snapToMeshIds->SetNumberOfIds(lineNumberOfPoints);
  embeddedPointIds->SetNumberOfIds(lineNumberOfPoints);

  for (i=0; i<lineNumberOfPoints; i++)
    {
    edgePointIds0[0] = static_cast<vtkIdType>(edgeArray->GetComponent(i,0));
    edgePointIds0[1] = static_cast<vtkIdType>(edgeArray->GetComponent(i,1));
    pCoord = edgePCoordArray->GetComponent(i,0);

    if (this->SnapToMeshTolerance>0.0)
      {
      if (edgePointIds0[0] == -1)
        {
        snapToMeshIds->SetId(i,-1);
        }
      else
        {
        if (pCoord < this->SnapToMeshTolerance)
          {
          snapToMeshIds->SetId(i,edgePointIds0[0]);
          }
        else if (pCoord > 1.0-this->SnapToMeshTolerance)
          {
          snapToMeshIds->SetId(i,edgePointIds0[1]);
          }
        else
          {
          snapToMeshIds->SetId(i,-1);
          }
        }
      }
    else
      {
      snapToMeshIds->SetId(i,-1);
      }

    if (snapToMeshIds->GetId(i)==-1)
      {
      id = newPoints->InsertNextPoint(this->Lines->GetPoint(i));
      if (edgePointIds0[0] != -1)
        {
        outputPD->InterpolateEdge(inputPD,id,edgePointIds0[0],edgePointIds0[1],pCoord);
        }
      }
    else
      {
      id = snapToMeshIds->GetId(i);
      }
    
    embeddedPointIds->SetId(i,id);
    }

  for (k=0; k<lineNumberOfCells; k++)
    {
    polyLine = this->Lines->GetCell(k);
    if (polyLine->GetCellType() != VTK_LINE && polyLine->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    for (i=1; i<polyLine->GetNumberOfPoints(); i++)
      {
      edgePointIds0[0] = static_cast<vtkIdType>(edgeArray->GetComponent(polyLine->GetPointId(i-1),0));
      edgePointIds0[1] = static_cast<vtkIdType>(edgeArray->GetComponent(polyLine->GetPointId(i-1),1));
      edgePointIds1[0] = static_cast<vtkIdType>(edgeArray->GetComponent(polyLine->GetPointId(i),0));
      edgePointIds1[1] = static_cast<vtkIdType>(edgeArray->GetComponent(polyLine->GetPointId(i),1));

      if ((snapToMeshIds->GetId(polyLine->GetPointId(i-1))!=-1)&&(snapToMeshIds->GetId(polyLine->GetPointId(i))!=-1))
        {
        continue;
        }

      if ((edgePointIds0[0]==-1)||(edgePointIds1[0]==-1))
        {
        continue;
        }

      cellPointIds->Initialize();
      cellPointIds->InsertNextId(edgePointIds0[0]);
      cellPointIds->InsertUniqueId(edgePointIds0[1]);
      cellPointIds->InsertUniqueId(edgePointIds1[0]);
      cellPointIds->InsertUniqueId(edgePointIds1[1]);

      cellId = this->GetCellId(input,cellPointIds);

      cellsToRemove->InsertUniqueId(cellId);
      }
    }

  for (i=0; i<cellsToRemove->GetNumberOfIds(); i++)
    {
    cellId = cellsToRemove->GetId(i);
    cellPointIds->Initialize();
    input->GetCellPoints(cellId,cellPointIds);

    neighborIds->Initialize();

    for (k=0; k<lineNumberOfPoints; k++)
      {
      edgePointIds0[0] = static_cast<vtkIdType>(edgeArray->GetComponent(k,0));
      edgePointIds0[1] = static_cast<vtkIdType>(edgeArray->GetComponent(k,1));
        
      for (j=0; j<cellPointIds->GetNumberOfIds(); j++)
        {
        if (((cellPointIds->GetId(j)==edgePointIds0[0])&&(cellPointIds->GetId((j+1)%cellPointIds->GetNumberOfIds())==edgePointIds0[1]))||
            ((cellPointIds->GetId(j)==edgePointIds0[1])&&(cellPointIds->GetId((j+1)%cellPointIds->GetNumberOfIds())==edgePointIds0[0])))
          {
          neighborIds->InsertNextId(k);
          break;
          }
        }
      }

    orderedNeighborIds->Initialize();
    this->OrderNeighborhood(cellPointIds,neighborIds,embeddedPointIds,snapToMeshIds,edgeArray,edgePCoordArray,orderedNeighborIds);

    triangulationIds->Initialize();
    this->Triangulate(cellPointIds,orderedNeighborIds,triangulationIds);

    for (j=0; j<triangulationIds->GetNumberOfIds(); j+=3)
      {
      addedTriangles->InsertNextCell(3);
      addedTriangles->InsertCellPoint(orderedNeighborIds->GetId(triangulationIds->GetId(j)));
      addedTriangles->InsertCellPoint(orderedNeighborIds->GetId(triangulationIds->GetId(j+1)));
      addedTriangles->InsertCellPoint(orderedNeighborIds->GetId(triangulationIds->GetId(j+2)));
      }
    }

  for (i=0; i<lineNumberOfPoints; i++)
    {
    edgePointIds0[0] = static_cast<vtkIdType>(edgeArray->GetComponent(i,0));

    if (edgePointIds0[0]!=-1)
      {
      continue;
      }

    neighborIds->Initialize();
    this->GetNeighbors(i,neighborIds);

    cellPointIds->Initialize();
    for (j=0; j<neighborIds->GetNumberOfIds(); j++)
      {
      edgePointIds1[0] = static_cast<vtkIdType>(edgeArray->GetComponent(neighborIds->GetId(j),0));
      edgePointIds1[1] = static_cast<vtkIdType>(edgeArray->GetComponent(neighborIds->GetId(j),1));
      cellPointIds->InsertUniqueId(edgePointIds1[0]);
      cellPointIds->InsertUniqueId(edgePointIds1[1]);
      }

    cellId = this->GetCellId(input,cellPointIds);

    cellsToRemove->InsertUniqueId(cellId);

    cellPointIds->Initialize();
    input->GetCellPoints(cellId,cellPointIds);

    orderedNeighborIds->Initialize();
    this->OrderNeighborhood(cellPointIds,neighborIds,embeddedPointIds,snapToMeshIds,edgeArray,edgePCoordArray,orderedNeighborIds);

    for (j=0; j<orderedNeighborIds->GetNumberOfIds(); j++)
      {
      addedTriangles->InsertNextCell(3);
      addedTriangles->InsertCellPoint(embeddedPointIds->GetId(i));
      addedTriangles->InsertCellPoint(orderedNeighborIds->GetId(j));
      addedTriangles->InsertCellPoint(orderedNeighborIds->GetId((j+1)%orderedNeighborIds->GetNumberOfIds()));
      }

    double newPoint0[3], newPoint1[3], newPoint2[3], embeddedPoint[3];
    newPoints->GetPoint(cellPointIds->GetId(0),newPoint0);
    newPoints->GetPoint(cellPointIds->GetId(1),newPoint1);
    newPoints->GetPoint(cellPointIds->GetId(2),newPoint2);
    newPoints->GetPoint(embeddedPointIds->GetId(i),embeddedPoint);

    triangleArea = vtkvmtkMath::TriangleArea(newPoint0,newPoint1,newPoint1);
    weights[0] = vtkvmtkMath::TriangleArea(embeddedPoint,newPoint1,newPoint2) / triangleArea;
    weights[1] = vtkvmtkMath::TriangleArea(newPoint0,embeddedPoint,newPoint2) / triangleArea;
    weights[2] = vtkvmtkMath::TriangleArea(newPoint0,newPoint1,embeddedPoint) / triangleArea;

    outputPD->InterpolatePoint(inputPD,embeddedPointIds->GetId(i),cellPointIds,weights);

    }

  for (addedTriangles->InitTraversal(); addedTriangles->GetNextCell(npts,pts); )
    {
    newTriangles->InsertNextCell(npts,pts);
    }

  for (i=0; i<input->GetNumberOfCells(); i++)
    {
    triangle = vtkTriangle::SafeDownCast(input->GetCell(i));
    if (triangle==NULL)
      {
      continue;
      }
    if (cellsToRemove->IsId(i)==-1)
      {
      newTriangles->InsertNextCell(triangle);
      }
    }
  
  output->SetPoints(newPoints);
  output->SetPolys(newTriangles);

  this->EmbeddedLinePointIds->DeepCopy(embeddedPointIds);

  newPoints->Delete();
  newTriangles->Delete();
  embeddedPointIds->Delete();
  addedTriangles->Delete();
  cellPointIds->Delete();
  newCellPointIds->Delete();
  cellsToRemove->Delete();
  polygon->Delete();
  triangulationIds->Delete();
  neighborIds->Delete();
  orderedNeighborIds->Delete();
  lineConnectedNeighbors->Delete();
  snapToMeshIds->Delete();

  return 1;
}

void vtkvmtkPolyDataLineEmbedder::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
