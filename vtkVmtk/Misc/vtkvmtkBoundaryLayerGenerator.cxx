/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundaryLayerGenerator.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkBoundaryLayerGenerator.h"
#include "vtkvmtkConstants.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkBoundaryLayerGenerator, "$Revision: 1.7 $");
vtkStandardNewMacro(vtkvmtkBoundaryLayerGenerator);

vtkvmtkBoundaryLayerGenerator::vtkvmtkBoundaryLayerGenerator()
{
  this->WarpVectorsArrayName = NULL;
  this->LayerThicknessArrayName = NULL;

  this->WarpVectorsArray = NULL;
  this->LayerThicknessArray = NULL;

  this->UseWarpVectorMagnitudeAsThickness = 0;
  this->ConstantThickness = 0;

  this->LayerThickness = 1.0;
  this->LayerThicknessRatio = 1.0; // ratio with respect to the LayerThickness (both constant and local)
  this->MaximumLayerThickness = VTK_VMTK_LARGE_DOUBLE;
  this->NumberOfSubLayers = 1;
  this->SubLayerRatio = 1.0; // thickness ratio between successive sublayers (moving from the surface)

  this->IncludeSurfaceCells = 0;
  this->IncludeSidewallCells = 0;
  this->NegateWarpVectors = 0;

  this->CellEntityIdsArrayName = NULL;
  this->InnerSurfaceCellEntityId = 0;
  this->OuterSurfaceCellEntityId = 0;
  this->SidewallCellEntityId = 0;
  this->VolumeCellEntityId = 0;

  this->InnerSurface = NULL;
}

vtkvmtkBoundaryLayerGenerator::~vtkvmtkBoundaryLayerGenerator()
{
  if (this->WarpVectorsArrayName)
    {
    delete[] this->WarpVectorsArrayName;
    this->WarpVectorsArrayName = NULL;
    }

  if (this->LayerThicknessArrayName)
    {
    delete[] this->LayerThicknessArrayName;
    this->LayerThicknessArrayName = NULL;
    }

  if (this->InnerSurface)
    {
    this->InnerSurface->Delete();
    this->InnerSurface = NULL;
    }

  if (this->CellEntityIdsArrayName)
    {
    delete[] this->CellEntityIdsArrayName;
    this->CellEntityIdsArrayName = NULL;
    }
}

int vtkvmtkBoundaryLayerGenerator::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* inputPoints = input->GetPoints();

  if (!this->WarpVectorsArrayName)
    {
    vtkErrorMacro("WarpVectors array name not specified.");
    return 1;
    }

  if (!this->CellEntityIdsArrayName)
    {
    vtkErrorMacro("CellEntityIds array name not specified.");
    return 1;
    }

  if (!input->GetPointData()->GetArray(this->WarpVectorsArrayName))
    {
    vtkErrorMacro(<< "WarpVectors array with name specified does not exist!");
    return 1;
    }

  this->WarpVectorsArray = input->GetPointData()->GetArray(this->WarpVectorsArrayName);
  
  if ((!this->UseWarpVectorMagnitudeAsThickness) && (!this->ConstantThickness))
    {
    if (!this->LayerThicknessArrayName)
      {
      vtkErrorMacro("LayerThickness array name not specified.");
      return 1;
      }

    if (!input->GetPointData()->GetArray(this->LayerThicknessArrayName))
      {
      vtkErrorMacro(<< "LayerThickness array with name specified does not exist!");
      return 1;
      }

    this->LayerThicknessArray = input->GetPointData()->GetArray(this->LayerThicknessArrayName);
    }

  vtkIdType i;

  vtkPoints* outputPoints = vtkPoints::New();
  vtkPoints* warpedPoints = vtkPoints::New();

  vtkCellArray* boundaryLayerCellArray = vtkCellArray::New();
  vtkIdList* boundaryLayerCellTypes = vtkIdList::New();

  vtkIntArray* cellEntityIdsArray = vtkIntArray::New();
  cellEntityIdsArray->SetName(this->CellEntityIdsArrayName);

  vtkIntArray* innerSurfaceCellEntityIdsArray = vtkIntArray::New();
  innerSurfaceCellEntityIdsArray->SetName(this->CellEntityIdsArrayName);

  vtkIdType numberOfInputPoints = inputPoints->GetNumberOfPoints();
  vtkIdType numberOfInputCells = input->GetNumberOfCells();

  int cellType;
  cellType = input->GetCellType(0);  // TODO: check if all elements are consistent
  bool warpQuadratic = false;
  if (cellType == VTK_QUADRATIC_TRIANGLE)
    {
    warpQuadratic = true;
    }

  vtkIdType numberOfLayerPoints = numberOfInputPoints;
  if (warpQuadratic)
    {
    numberOfLayerPoints = 2 * numberOfInputPoints;
    }  

  vtkIdType numberOfOutputPoints = numberOfInputPoints + numberOfLayerPoints * this->NumberOfSubLayers;
  outputPoints->SetNumberOfPoints(numberOfOutputPoints);

  double point[3];
  for (i=0; i<numberOfInputPoints; i++)
    {
    inputPoints->GetPoint(i,point);
    outputPoints->SetPoint(i,point);
    }

  vtkIdType npts, *pts;
  vtkIdType *surfacePts;

  if (this->IncludeSurfaceCells)
    {
    for (i=0; i<numberOfInputCells; i++)
      {
      input->GetCellPoints(i,npts,pts);
      cellType = input->GetCellType(i);
      surfacePts = new vtkIdType[npts];
      switch(cellType)
        {
        case VTK_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_TRIANGLE);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          break;
        case VTK_QUAD:
          boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          surfacePts[3] = pts[3];
          break;
        case VTK_QUADRATIC_TRIANGLE:
          boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_TRIANGLE);
          surfacePts[0] = pts[0];
          surfacePts[1] = pts[1];
          surfacePts[2] = pts[2];
          surfacePts[3] = pts[3];
          surfacePts[4] = pts[4];
          surfacePts[5] = pts[5];
          break;
        default:
          vtkErrorMacro(<<"Unsupported surface element.");
          return 1;
          break;
        }
      boundaryLayerCellArray->InsertNextCell(npts,surfacePts);
      cellEntityIdsArray->InsertNextValue(this->InnerSurfaceCellEntityId);
      delete[] surfacePts;
      }
    }

  if (this->InnerSurface)
    {
    this->InnerSurface->Delete();
    this->InnerSurface = NULL;
    }

  this->InnerSurface = vtkUnstructuredGrid::New();
  this->InnerSurface->DeepCopy(input);
  vtkPoints* innerSurfacePoints = vtkPoints::New();
  this->WarpPoints(inputPoints,innerSurfacePoints,this->NumberOfSubLayers-1,warpQuadratic);
  this->InnerSurface->GetPoints()->DeepCopy(innerSurfacePoints);
  innerSurfaceCellEntityIdsArray->SetNumberOfTuples(this->InnerSurface->GetNumberOfCells());
  innerSurfaceCellEntityIdsArray->FillComponent(0,this->InnerSurfaceCellEntityId);
  this->InnerSurface->GetCellData()->AddArray(innerSurfaceCellEntityIdsArray);

  vtkIdList* edgePointIds = vtkIdList::New();
  vtkIdList* edgeNeighborCellIds = vtkIdList::New();

  int k;
  for (k=0; k<this->NumberOfSubLayers; k++)
    {
    warpedPoints->Initialize();
    this->WarpPoints(inputPoints,warpedPoints,k,warpQuadratic);

    for (i=0; i<numberOfLayerPoints; i++)
      {
      warpedPoints->GetPoint(i,point);
      outputPoints->SetPoint(i + numberOfInputPoints + k*numberOfLayerPoints,point);
      }
   
    vtkIdType prismNPts, *prismPts;
    vtkIdType quadNPts, *quadPts;
    for (i=0; i<numberOfInputCells; i++)
      {
      input->GetCellPoints(i,npts,pts);
      cellType = input->GetCellType(i);
      if (cellType == VTK_TRIANGLE || cellType == VTK_QUAD)
        {
        prismNPts = npts * 2;
        prismPts = new vtkIdType[prismNPts];
        quadNPts = 4;
        quadPts = new vtkIdType[quadNPts];
        int j;
        for (j=0; j<npts; j++)
          {
          prismPts[j] = pts[j] + k*numberOfLayerPoints;
          }
        for (j=0; j<npts; j++)
          {
          prismPts[j+npts] = pts[j] + (k+1)*numberOfLayerPoints;
          }
        boundaryLayerCellArray->InsertNextCell(prismNPts,prismPts);
        cellEntityIdsArray->InsertNextValue(this->VolumeCellEntityId);

        if (cellType == VTK_TRIANGLE)
          {
          boundaryLayerCellTypes->InsertNextId(VTK_WEDGE);
          }
        else if (cellType == VTK_QUAD)
          {
          boundaryLayerCellTypes->InsertNextId(VTK_HEXAHEDRON);
          }

        if (this->IncludeSidewallCells)
          {
          for (j=0; j<npts; j++)
            {
            vtkIdType jnext = (j+1) % npts;
            edgePointIds->Initialize();
            edgePointIds->SetNumberOfIds(2);
            edgePointIds->SetId(0,pts[j]);
            edgePointIds->SetId(1,pts[jnext]);
            input->GetCellNeighbors(i,edgePointIds,edgeNeighborCellIds);

            if (edgeNeighborCellIds->GetNumberOfIds() > 0)
              {
              continue;
              }

            quadPts[0] = prismPts[j];
            quadPts[1] = prismPts[jnext];
            quadPts[2] = prismPts[jnext+npts];
            quadPts[3] = prismPts[j+npts];

            boundaryLayerCellArray->InsertNextCell(quadNPts,quadPts);
            boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
            cellEntityIdsArray->InsertNextValue(this->SidewallCellEntityId);
            }
          }
        
        delete[] prismPts;
        delete[] quadPts;
        }
      else if (cellType == VTK_QUADRATIC_TRIANGLE)
        {
        prismNPts = npts * 3 - 3;
        prismPts = new vtkIdType[prismNPts];
        quadNPts = 8;
        quadPts = new vtkIdType[quadNPts];
 
        boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_WEDGE);
        
        prismPts[0] = pts[0] + k*numberOfLayerPoints;
        prismPts[1] = pts[1] + k*numberOfLayerPoints;
        prismPts[2] = pts[2] + k*numberOfLayerPoints;

        prismPts[3] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints;
        prismPts[4] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints;
        prismPts[5] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints;

        prismPts[6] = pts[3] + k*numberOfLayerPoints;
        prismPts[7] = pts[4] + k*numberOfLayerPoints;
        prismPts[8] = pts[5] + k*numberOfLayerPoints;

        prismPts[9] = pts[3] + k*numberOfLayerPoints + numberOfLayerPoints;
        prismPts[10] = pts[4] + k*numberOfLayerPoints + numberOfLayerPoints;
        prismPts[11] = pts[5] + k*numberOfLayerPoints + numberOfLayerPoints;

        prismPts[12] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints/2;
        prismPts[13] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints/2;
        prismPts[14] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints/2;

        boundaryLayerCellArray->InsertNextCell(prismNPts,prismPts);
        cellEntityIdsArray->InsertNextValue(this->VolumeCellEntityId);

        if (this->IncludeSidewallCells)
          {
          for (int j=0; j<npts/2; j++)
            {
            vtkIdType jnext = (j+1) % npts;
            edgePointIds->Initialize();
            edgePointIds->SetNumberOfIds(2);
            edgePointIds->SetId(0,pts[j]);
            edgePointIds->SetId(1,pts[jnext]);
            input->GetCellNeighbors(i,edgePointIds,edgeNeighborCellIds);

            if (edgeNeighborCellIds->GetNumberOfIds() > 0)
              {
              continue;
              }

            quadPts[0] = prismPts[j];
            quadPts[1] = prismPts[jnext];
            quadPts[2] = prismPts[jnext+npts/2];
            quadPts[3] = prismPts[j+npts/2];

            quadPts[4] = prismPts[j+npts];
            quadPts[5] = prismPts[jnext+2*npts];
            quadPts[6] = prismPts[j+npts+npts/2];
            quadPts[7] = prismPts[j+2*npts];

            boundaryLayerCellArray->InsertNextCell(quadNPts,quadPts);
            boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
            cellEntityIdsArray->InsertNextValue(this->SidewallCellEntityId);
            }
          }

        delete[] prismPts;
        delete[] quadPts;
        }
      else
        {
        vtkErrorMacro(<<"Unsupported surface element.");
        return 1;
        }
      }

    if (this->IncludeSurfaceCells)
      {
      if (k==this->NumberOfSubLayers-1)
        {
        for (i=0; i<numberOfInputCells; i++)
          {
          input->GetCellPoints(i,npts,pts);
          cellType = input->GetCellType(i);
          surfacePts = new vtkIdType[npts];
          switch(cellType)
            {
            case VTK_TRIANGLE:
              boundaryLayerCellTypes->InsertNextId(VTK_TRIANGLE);
              surfacePts[0] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[1] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[2] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints;
              break;
            case VTK_QUAD:
              boundaryLayerCellTypes->InsertNextId(VTK_QUAD);
              surfacePts[0] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[1] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[2] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[3] = pts[3] + k*numberOfLayerPoints + numberOfLayerPoints;
              break;
            case VTK_QUADRATIC_TRIANGLE:
              boundaryLayerCellTypes->InsertNextId(VTK_QUADRATIC_TRIANGLE);
              surfacePts[0] = pts[0] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[1] = pts[1] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[2] = pts[2] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[3] = pts[3] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[4] = pts[4] + k*numberOfLayerPoints + numberOfLayerPoints;
              surfacePts[5] = pts[5] + k*numberOfLayerPoints + numberOfLayerPoints;
              break;
            default:
              vtkErrorMacro(<<"Unsupported surface element.");
              return 1;
              break;
            }
          boundaryLayerCellArray->InsertNextCell(npts,surfacePts);
          cellEntityIdsArray->InsertNextValue(this->OuterSurfaceCellEntityId);
          delete[] surfacePts;
          }
        }
      }
    }

  output->SetPoints(outputPoints);

  int* boundaryLayerCellTypesInt = new int[boundaryLayerCellTypes->GetNumberOfIds()];
  for (i=0; i<boundaryLayerCellTypes->GetNumberOfIds(); i++)
    {
    boundaryLayerCellTypesInt[i] = boundaryLayerCellTypes->GetId(i);
    }

  output->SetCells(boundaryLayerCellTypesInt,boundaryLayerCellArray);

  delete[] boundaryLayerCellTypesInt;
  
  output->GetCellData()->AddArray(cellEntityIdsArray);

  edgePointIds->Delete();
  edgeNeighborCellIds->Delete();
  
  outputPoints->Delete();
  warpedPoints->Delete();
  boundaryLayerCellArray->Delete();
  boundaryLayerCellTypes->Delete();
  innerSurfacePoints->Delete();

  cellEntityIdsArray->Delete();
  innerSurfaceCellEntityIdsArray->Delete();
 
  return 1;
}

void vtkvmtkBoundaryLayerGenerator::WarpPoints(vtkPoints* inputPoints, vtkPoints* warpedPoints, int subLayerId, bool quadratic)
{
  double point[3], warpedPoint[3], warpVector[3];
  double layerThickness, subLayerThicknessRatio, subLayerThickness;
  double totalLayerZeroSubLayerRatio, subLayerOffsetRatio, subLayerOffset;

  vtkIdType numberOfInputPoints = inputPoints->GetNumberOfPoints();

  totalLayerZeroSubLayerRatio = 0.0;
  int i;
  for (i=0; i<this->NumberOfSubLayers; i++)
    {
    totalLayerZeroSubLayerRatio += pow(this->SubLayerRatio,this->NumberOfSubLayers-i-1);
    }

  subLayerOffsetRatio = 0.0;
  for (i=0; i<subLayerId; i++)
    {
    subLayerOffsetRatio += pow(this->SubLayerRatio,this->NumberOfSubLayers-i-1);
    }
  subLayerOffsetRatio /= totalLayerZeroSubLayerRatio;

  subLayerThicknessRatio = pow(this->SubLayerRatio,this->NumberOfSubLayers-subLayerId-1) / totalLayerZeroSubLayerRatio;

  if (!quadratic)
    {
    warpedPoints->SetNumberOfPoints(numberOfInputPoints);
    }
  else
    {
    warpedPoints->SetNumberOfPoints(2*numberOfInputPoints);
    }

  for (i=0; i<numberOfInputPoints; i++)
    {
    inputPoints->GetPoint(i,point);
    this->WarpVectorsArray->GetTuple(i,warpVector);
    if (this->NegateWarpVectors)
      {
      warpVector[0] *= -1.0;
      warpVector[1] *= -1.0;
      warpVector[2] *= -1.0;
      }

    layerThickness = 0.0;
    if (this->ConstantThickness)
      {
      layerThickness = this->LayerThickness;
      }
    else if (this->UseWarpVectorMagnitudeAsThickness)
      {
      layerThickness = vtkMath::Norm(warpVector);
      }
    else
      {
      layerThickness = this->LayerThicknessArray->GetComponent(i,0);
      layerThickness *= this->LayerThicknessRatio;
      }

    if (layerThickness > this->MaximumLayerThickness)
      {
      layerThickness = this->MaximumLayerThickness;
      }

    vtkMath::Normalize(warpVector);

    subLayerOffset = subLayerOffsetRatio * layerThickness;
    subLayerThickness = subLayerThicknessRatio * layerThickness;

    if (quadratic)
      {
      warpedPoint[0] = point[0] + 0.5 * warpVector[0] * (subLayerOffset + subLayerThickness);
      warpedPoint[1] = point[1] + 0.5 * warpVector[1] * (subLayerOffset + subLayerThickness);
      warpedPoint[2] = point[2] + 0.5 * warpVector[2] * (subLayerOffset + subLayerThickness);
      warpedPoints->SetPoint(i,warpedPoint);
      warpedPoint[0] = point[0] + warpVector[0] * (subLayerOffset + subLayerThickness);
      warpedPoint[1] = point[1] + warpVector[1] * (subLayerOffset + subLayerThickness);
      warpedPoint[2] = point[2] + warpVector[2] * (subLayerOffset + subLayerThickness);
      warpedPoints->SetPoint(i+numberOfInputPoints,warpedPoint);
      }
    else
      {
      warpedPoint[0] = point[0] + warpVector[0] * (subLayerOffset + subLayerThickness);
      warpedPoint[1] = point[1] + warpVector[1] * (subLayerOffset + subLayerThickness);
      warpedPoint[2] = point[2] + warpVector[2] * (subLayerOffset + subLayerThickness);
      warpedPoints->SetPoint(i,warpedPoint);
      }
    }
}

void vtkvmtkBoundaryLayerGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
