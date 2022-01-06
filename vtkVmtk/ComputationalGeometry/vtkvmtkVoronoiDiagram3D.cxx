/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVoronoiDiagram3D.cxx,v $
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

#include "vtkvmtkVoronoiDiagram3D.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTetra.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkVoronoiDiagram3D);

vtkvmtkVoronoiDiagram3D::vtkvmtkVoronoiDiagram3D()
{
  this->BuildLines = 0;
  this->PoleIds = vtkIdList::New();
  this->RadiusArrayName = NULL;
}

vtkvmtkVoronoiDiagram3D::~vtkvmtkVoronoiDiagram3D()
{
  this->PoleIds->Delete();

  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }
}

int vtkvmtkVoronoiDiagram3D::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

void vtkvmtkVoronoiDiagram3D::ExtractUniqueEdges(vtkUnstructuredGrid* input, vtkCellArray* edgeArray)  // could be a class
{
  int isVisited;
  vtkIdType i, j, k;
  vtkIdType edgePts[2];
  vtkIdType npts;
  const vtkIdType *pts;
  vtkIntArray* visited;
  vtkIdList* pointCells;
  vtkIdList* insertedLoopPoints;

  visited = vtkIntArray::New();
  visited->SetNumberOfTuples(input->GetNumberOfPoints());
  visited->FillComponent(0,0);

  pointCells = vtkIdList::New();
  insertedLoopPoints = vtkIdList::New();

  for (i=0; i<input->GetNumberOfPoints(); i++)
    {           
    visited->SetValue(i,1);
    edgePts[0] = i;
    pointCells->Initialize();
    insertedLoopPoints->Initialize();
    input->GetPointCells(i,pointCells);
    for (j=0; j<pointCells->GetNumberOfIds(); j++)
      {
      input->GetCellPoints(pointCells->GetId(j),npts,pts);
      for (k=0; k<npts; k++)
        {
        isVisited = visited->GetValue(pts[k]);
        if (!isVisited)
          if (insertedLoopPoints->IsId(pts[k])==-1)
            {
            edgePts[1] = pts[k];
            insertedLoopPoints->InsertNextId(pts[k]);
            edgeArray->InsertNextCell(2,edgePts);
            }
        }
      }
    }

  visited->Delete();
  pointCells->Delete();
  insertedLoopPoints->Delete();

}

void vtkvmtkVoronoiDiagram3D::BuildVoronoiPolys(vtkUnstructuredGrid* input, vtkCellArray* voronoiPolys)
{
  bool boundaryTetra;
  vtkIdType npts;
  const vtkIdType *pts;
  pts = NULL;
  vtkIdType neighborTetraId;
  vtkIdType i, k, h;
  vtkCellArray* edgeArray;
  vtkIdList* polyIds;
  vtkIdList* neighborCells;
  vtkIdList* neighborTrianglePointIds;
  vtkIdList* neighborNeighborCells;
  vtkIdList* linePointIds;
  vtkTetra* neighborTetra;

  edgeArray = vtkCellArray::New();
  polyIds = vtkIdList::New();
  neighborCells = vtkIdList::New();
  neighborTrianglePointIds = vtkIdList::New();
  neighborNeighborCells = vtkIdList::New();
  linePointIds = vtkIdList::New();

  this->ExtractUniqueEdges(input,edgeArray);

  edgeArray->InitTraversal();
  for (i=0; i<edgeArray->GetNumberOfCells(); i++)
    {
    edgeArray->GetNextCell(npts,pts);
    linePointIds->Initialize();
    linePointIds->InsertNextId(pts[0]);
    linePointIds->InsertNextId(pts[1]);
                
    boundaryTetra = false;
    neighborCells->Initialize();
    neighborTrianglePointIds->Initialize();
    polyIds->Initialize();
    input->GetCellNeighbors(-1,linePointIds,neighborCells);

    neighborTrianglePointIds->InsertNextId(linePointIds->GetId(0));
    neighborTrianglePointIds->InsertNextId(linePointIds->GetId(1));

    neighborTetraId = neighborCells->GetId(0);
    neighborTetra = vtkTetra::SafeDownCast(input->GetCell(neighborTetraId));
    polyIds->InsertNextId(neighborTetraId);
    for (k=0; k<neighborCells->GetNumberOfIds(); k++)
      {
      neighborNeighborCells->Initialize();
      for (h=0; h<neighborTetra->GetNumberOfPoints(); h++)
        {
        if (neighborTrianglePointIds->IsId(neighborTetra->GetPointId(h))==-1)
          {
          neighborTrianglePointIds->InsertId(2,neighborTetra->GetPointId(h));
          input->GetCellNeighbors(neighborTetraId,neighborTrianglePointIds,neighborNeighborCells);
          if (neighborNeighborCells->GetNumberOfIds()==0)
            {
            boundaryTetra = true;
            break;
            }
          else if (neighborNeighborCells->GetNumberOfIds()==1)
            {
            neighborTetraId = neighborNeighborCells->GetId(0);
            neighborTetra = vtkTetra::SafeDownCast(input->GetCell(neighborTetraId));
            polyIds->InsertUniqueId(neighborTetraId);
            break;
            }
          }
        }
      if (boundaryTetra)
        break;
      }
    if (!boundaryTetra)
      voronoiPolys->InsertNextCell(polyIds);
    }

  edgeArray->Delete();
  polyIds->Delete();
  neighborCells->Delete();
  neighborTrianglePointIds->Delete();
  neighborNeighborCells->Delete();
  linePointIds->Delete();
}

int vtkvmtkVoronoiDiagram3D::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid* input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Declare
  double p0[3], p1[3], p2[3], p3[3];
  double tetraCenter[3], tetraRadius;
  double currentRadius;
  vtkIdType i, j, id, poleId;
  vtkPoints* newPoints;
  vtkCellArray* newPolys;
  vtkCellArray* newLines;
  vtkDoubleArray* newScalars;
  vtkDoubleArray* thicknessScalars;
  vtkIdList* cellIds;
  vtkTetra* tetra;

  poleId = -1;

  // Allocate
  newPoints = vtkPoints::New();
  newPoints->SetNumberOfPoints(input->GetNumberOfCells());
  newScalars = vtkDoubleArray::New();
  newScalars->SetNumberOfTuples(input->GetNumberOfCells());
  newPolys = vtkCellArray::New();
  newLines = vtkCellArray::New();
  thicknessScalars = vtkDoubleArray::New();
  thicknessScalars->SetNumberOfTuples(input->GetNumberOfPoints());
  thicknessScalars->FillComponent(0,0.0);

  cellIds = vtkIdList::New();

  this->PoleIds->SetNumberOfIds(input->GetNumberOfPoints());

  // Execute

  for (i=0; i<input->GetNumberOfPoints(); i++)
    {
    this->PoleIds->SetId(i,-1);
    }

  for (i=0; i<input->GetNumberOfCells(); i++)
    {
    tetra = (vtkTetra*) input->GetCell(i);
    tetra->GetPoints()->GetPoint(0,p0);
    tetra->GetPoints()->GetPoint(1,p1);
    tetra->GetPoints()->GetPoint(2,p2);
    tetra->GetPoints()->GetPoint(3,p3);
    tetraRadius = sqrt(vtkTetra::Circumsphere(p0,p1,p2,p3,tetraCenter));

    newPoints->SetPoint(i,tetraCenter);
    newScalars->SetValue(i,(double)tetraRadius);
    }

  // compute poles
  input->BuildLinks();
  for (i=0; i<input->GetNumberOfPoints(); i++)
    {
    cellIds->Initialize();
    input->GetPointCells(i,cellIds);
    currentRadius = thicknessScalars->GetValue(i);
    for (j=0; j<cellIds->GetNumberOfIds(); j++)
      {
      id = cellIds->GetId(j);
      tetraRadius = newScalars->GetValue(id);
      if (tetraRadius - currentRadius > VTK_VMTK_DOUBLE_TOL)
        {
        poleId = id;
        currentRadius = tetraRadius;
        }
      }
    this->PoleIds->SetId(i,poleId);
    thicknessScalars->SetValue(i,currentRadius);
    }  

  this->BuildVoronoiPolys(input,newPolys);

  if (this->BuildLines)
    {
    this->BuildVoronoiLines();
    }

  if (this->RadiusArrayName)
    {
    newScalars->SetName(this->RadiusArrayName);
    }
  else
    {
    newScalars->SetName("VoronoiRadius");
    }

  output->SetPoints(newPoints);
  output->GetPointData()->AddArray(newScalars);
  output->GetPointData()->SetActiveScalars(newScalars->GetName());
  output->SetPolys(newPolys);

  if (this->BuildLines)
    {
    output->SetLines(newLines);
    }

  // Destroy
  newPoints->Delete();
  newPolys->Delete();
  newLines->Delete();
  newScalars->Delete();
  thicknessScalars->Delete();
  cellIds->Delete();

  return 1;
}

void vtkvmtkVoronoiDiagram3D::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
