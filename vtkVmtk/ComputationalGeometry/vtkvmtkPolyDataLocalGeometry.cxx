/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataLocalGeometry.cxx,v $
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

#include "vtkvmtkPolyDataLocalGeometry.h"
#include "vtkTriangleFilter.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataLocalGeometry);

vtkvmtkPolyDataLocalGeometry::vtkvmtkPolyDataLocalGeometry()
{
  this->ComputePoleVectors = 0;
  this->ComputeGeodesicDistance = 0;
  this->ComputeNormalizedTangencyDeviation = 0;
  this->ComputeEuclideanDistance = 0;
  this->ComputeCenterlineVectors = 0;
  this->ComputeCellIds = 0;
  this->ComputePCoords = 0;

  this->AdjustBoundaryValues = 0;

  this->PoleVectorsArrayName = NULL;
  this->GeodesicDistanceArrayName = NULL;
  this->NormalizedTangencyDeviationArrayName = NULL;
  this->EuclideanDistanceArrayName = NULL;
  this->CenterlineVectorsArrayName = NULL;
  this->CellIdsArrayName = NULL;
  this->PCoordsArrayName = NULL;

  this->VoronoiGeodesicDistanceArrayName = NULL;
  this->VoronoiPoleCenterlineVectorsArrayName = NULL;
  this->VoronoiCellIdsArrayName = NULL;
  this->VoronoiPCoordsArrayName = NULL;
  this->VoronoiDiagram = NULL;
  this->PoleIds = NULL;
}

vtkvmtkPolyDataLocalGeometry::~vtkvmtkPolyDataLocalGeometry()
{
  if (this->VoronoiDiagram)
    {
    this->VoronoiDiagram->Delete();
    this->VoronoiDiagram = NULL;
    }

  if (this->PoleIds)
    {
    this->PoleIds->Delete();
    this->PoleIds = NULL;
    }

  if (this->PoleVectorsArrayName)
    {
    delete[] this->PoleVectorsArrayName;
    }

  if (this->GeodesicDistanceArrayName)
    {
    delete[] this->GeodesicDistanceArrayName;
    }

  if (this->NormalizedTangencyDeviationArrayName)
    {
    delete[] this->NormalizedTangencyDeviationArrayName;
    }

  if (this->EuclideanDistanceArrayName)
    {
    delete[] this->EuclideanDistanceArrayName;
    }

  if (this->CenterlineVectorsArrayName)
    {
    delete[] this->CenterlineVectorsArrayName;
    }

  if (this->CellIdsArrayName)
    {
    delete[] this->CellIdsArrayName;
    }

  if (this->PCoordsArrayName)
    {
    delete[] this->PCoordsArrayName;
    }
}

int vtkvmtkPolyDataLocalGeometry::RequestData(
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

  vtkIdType i, poleId;
  double surfacePoint[3], polePoint[3], poleVector[3], voronoiPoleVector[3], centerlinePoint[3], centerlineVector[3];
  double voronoiRadius, voronoiGeodesicDistance;
  double geodesicDistance, normalizedTangencyDeviation;
  double euclideanDistance;
  vtkDataArray* voronoiGeodesicDistanceArray = NULL;
  vtkDataArray* voronoiPoleVectorsArray = NULL;
  vtkDataArray* voronoiCellIdsArray = NULL;
  vtkDataArray* voronoiPCoordsArray = NULL;
  vtkDoubleArray* poleVectorsArray = NULL;
  vtkDoubleArray* geodesicDistanceArray = NULL;
  vtkDoubleArray* normalizedTangencyDeviationArray = NULL;
  vtkDoubleArray* euclideanDistanceArray = NULL;
  vtkDoubleArray* centerlineVectorsArray = NULL;
  vtkIntArray* cellIdsArray = NULL;
  vtkDoubleArray* pcoordsArray = NULL;

  if (!this->VoronoiDiagram)
    {
    vtkErrorMacro(<< "No Voronoi diagram specified!");
    return 1;
    }

  if (!this->PoleIds)
    {
    vtkErrorMacro(<< "No poleIds specified!");
    return 1;
    }

  if (this->ComputeGeodesicDistance || this->ComputeNormalizedTangencyDeviation)
    {
    if (!this->VoronoiGeodesicDistanceArrayName)
      {
      vtkErrorMacro(<< "No Voronoi geodesic distance array name specified!");
      return 1;
      }
 
    voronoiGeodesicDistanceArray = this->VoronoiDiagram->GetPointData()->GetArray(this->VoronoiGeodesicDistanceArrayName);
    if (!voronoiGeodesicDistanceArray)
      {
      vtkErrorMacro(<< "Voronoi geodesic distance array with name specified does not exist!");
      return 1;
      }
    }

  if (this->ComputeEuclideanDistance || this->ComputeCenterlineVectors)
    {
    if (!this->VoronoiPoleCenterlineVectorsArrayName)
      {
      vtkErrorMacro(<< "No Voronoi pole vectors array name specified!");
      return 1;
      }
 
    voronoiPoleVectorsArray = this->VoronoiDiagram->GetPointData()->GetArray(this->VoronoiPoleCenterlineVectorsArrayName);
    if (!voronoiPoleVectorsArray)
      {
      vtkErrorMacro(<< "Voronoi pole vectors array with name specified does not exist!");
      return 1;
      }
    }   


  if (this->ComputeCellIds)
    {
    if (!this->VoronoiCellIdsArrayName)
      {
      vtkErrorMacro(<< "No Voronoi cellIds array name specified!");
      return 1;
      }
 
    voronoiCellIdsArray = this->VoronoiDiagram->GetPointData()->GetArray(this->VoronoiCellIdsArrayName);
    if (!voronoiCellIdsArray)
      {
      vtkErrorMacro(<< "Voronoi cellIds array with name specified does not exist!");
      return 1;
      }
    }

  if (this->ComputePCoords)
    {
    if (!this->VoronoiPCoordsArrayName)
      {
      vtkErrorMacro(<< "No Voronoi pcoords array name specified!");
      return 1;
      }
 
    voronoiPCoordsArray = this->VoronoiDiagram->GetPointData()->GetArray(this->VoronoiPCoordsArrayName);
    if (!voronoiPCoordsArray)
      {
      vtkErrorMacro(<< "Voronoi pcoords array with name specified does not exist!");
      return 1;
      }
    }


  if (this->ComputeGeodesicDistance)
    {
    if (!this->GeodesicDistanceArrayName)
      {
      vtkErrorMacro(<< "No geodesic distance array name specified!");
      return 1;
      }
    }

  if (this->ComputeNormalizedTangencyDeviation)
    {
    if (!this->NormalizedTangencyDeviationArrayName)
      {
      vtkErrorMacro(<< "No normalized tangency deviation array name specified!");
      return 1;
      }
    }

  if (this->ComputeEuclideanDistance || this->ComputeCenterlineVectors)
    {
    if (!this->EuclideanDistanceArrayName)
      {
      vtkErrorMacro(<< "No Euclidean distance array name specified!");
      return 1;
      }
    }

  if (this->ComputeCellIds)
    {
    if (!this->CellIdsArrayName)
      {
      vtkErrorMacro(<< "No cellIds array name specified!");
      return 1;
      }
    }

  if (this->ComputePCoords)
    {
    if (!this->PCoordsArrayName)
      {
      vtkErrorMacro(<< "No pcoords array name specified!");
      return 1;
      }
    }

  if (this->ComputePoleVectors)
    {
    if (!this->PoleVectorsArrayName)
      {
      vtkErrorMacro(<< "No pole vectors array name specified!");
      return 1;
      }
    }

  if (this->ComputePoleVectors)
    {
    poleVectorsArray = vtkDoubleArray::New();
    poleVectorsArray->SetName(this->PoleVectorsArrayName);
    poleVectorsArray->SetNumberOfComponents(3);
    poleVectorsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    poleVectorsArray->FillComponent(0,0.0);
    poleVectorsArray->FillComponent(1,0.0);
    poleVectorsArray->FillComponent(2,0.0);
    }

  if (this->ComputeGeodesicDistance)
    {
    geodesicDistanceArray = vtkDoubleArray::New();
    geodesicDistanceArray->SetName(this->GeodesicDistanceArrayName);
    geodesicDistanceArray->SetNumberOfTuples(input->GetNumberOfPoints());
    geodesicDistanceArray->FillComponent(0,0.0);
    }

  if (this->ComputeNormalizedTangencyDeviation)
    {
    normalizedTangencyDeviationArray = vtkDoubleArray::New();
    normalizedTangencyDeviationArray->SetName(this->NormalizedTangencyDeviationArrayName);
    normalizedTangencyDeviationArray->SetNumberOfTuples(input->GetNumberOfPoints());
    normalizedTangencyDeviationArray->FillComponent(0,0.0);
    }

  if (this->ComputeEuclideanDistance)
    {
    euclideanDistanceArray = vtkDoubleArray::New();
    euclideanDistanceArray->SetName(this->EuclideanDistanceArrayName);
    euclideanDistanceArray->SetNumberOfTuples(input->GetNumberOfPoints());
    euclideanDistanceArray->FillComponent(0,0.0);
    }

  if (this->ComputeCenterlineVectors)
    {
    centerlineVectorsArray = vtkDoubleArray::New();
    centerlineVectorsArray->SetNumberOfComponents(3);
    centerlineVectorsArray->SetName(this->CenterlineVectorsArrayName);
    centerlineVectorsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    centerlineVectorsArray->FillComponent(0,0.0);
    centerlineVectorsArray->FillComponent(1,0.0);
    centerlineVectorsArray->FillComponent(2,0.0);

    }

  if (this->ComputeCellIds)
    {
    cellIdsArray = vtkIntArray::New();
    cellIdsArray->SetNumberOfComponents(2);
    cellIdsArray->SetName(this->CellIdsArrayName);
    cellIdsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    cellIdsArray->FillComponent(0,0.0);
    cellIdsArray->FillComponent(1,0.0);
    }

  if (this->ComputePCoords)
    {
    pcoordsArray = vtkDoubleArray::New();
    pcoordsArray->SetName(this->PCoordsArrayName);
    pcoordsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    pcoordsArray->FillComponent(0,0.0);
    }

  for (i=0; i<input->GetNumberOfPoints(); i++)
    {
    poleId = this->PoleIds->GetId(i);

    if (poleId == -1)
      {
      vtkWarningMacro(<<"Invalid PoleId found");
      }

    if (this->ComputePoleVectors)
      {
      input->GetPoint(i,surfacePoint);
      this->VoronoiDiagram->GetPoint(poleId,polePoint);
      poleVector[0] = polePoint[0] - surfacePoint[0];
      poleVector[1] = polePoint[1] - surfacePoint[1];
      poleVector[2] = polePoint[2] - surfacePoint[2];
      poleVectorsArray->SetTuple(i,poleVector);
      }

    if (this->ComputeGeodesicDistance || this->ComputeNormalizedTangencyDeviation)
      {
      input->GetPoint(i,surfacePoint);
      this->VoronoiDiagram->GetPoint(poleId,polePoint);

      voronoiRadius = sqrt(vtkMath::Distance2BetweenPoints(surfacePoint,polePoint));
      voronoiGeodesicDistance = voronoiGeodesicDistanceArray->GetTuple1(poleId);

      geodesicDistance = voronoiGeodesicDistance + voronoiRadius;
      if (geodesicDistance > VTK_VMTK_DOUBLE_TOL)
        {
        normalizedTangencyDeviation = voronoiGeodesicDistance / geodesicDistance;
        }
      else
        {
        normalizedTangencyDeviation = 1.0;
        }

      if (this->ComputeGeodesicDistance)
        {
        geodesicDistanceArray->SetComponent(i,0,geodesicDistance);
        }

      if (this->ComputeNormalizedTangencyDeviation)
        {
        normalizedTangencyDeviationArray->SetComponent(i,0,normalizedTangencyDeviation);
        }
      }
    
    if (this->ComputeEuclideanDistance || this->ComputeCenterlineVectors)
      {
      input->GetPoint(i,surfacePoint);
      this->VoronoiDiagram->GetPoint(poleId,polePoint);
      voronoiPoleVectorsArray->GetTuple(poleId,voronoiPoleVector);
      centerlinePoint[0] = polePoint[0] + voronoiPoleVector[0];
      centerlinePoint[1] = polePoint[1] + voronoiPoleVector[1];
      centerlinePoint[2] = polePoint[2] + voronoiPoleVector[2];
      centerlineVector[0] = centerlinePoint[0] - surfacePoint[0];
      centerlineVector[1] = centerlinePoint[1] - surfacePoint[1];
      centerlineVector[2] = centerlinePoint[2] - surfacePoint[2];
      if (this->ComputeEuclideanDistance)
        {
        euclideanDistance = sqrt(vtkMath::Distance2BetweenPoints(surfacePoint,centerlinePoint));
        euclideanDistanceArray->SetComponent(i,0,euclideanDistance);
        }
      if (this->ComputeCenterlineVectors)
        {
        centerlineVectorsArray->SetTuple(i,centerlineVector);
        }
      }

    if (this->ComputeCellIds)
      {
      cellIdsArray->SetTuple(i,voronoiCellIdsArray->GetTuple(poleId));
      }

    if (this->ComputePCoords)
      {
      pcoordsArray->SetTuple(i,voronoiPCoordsArray->GetTuple(poleId));
      }
    }

   output->CopyStructure(input);
   output->GetPointData()->PassData(input->GetPointData());
   output->GetCellData()->PassData(input->GetCellData());

  if (this->ComputePoleVectors)
    {  
    output->GetPointData()->AddArray(poleVectorsArray);
    poleVectorsArray->Delete();
    }

  if (this->ComputeGeodesicDistance)
    {
    output->GetPointData()->AddArray(geodesicDistanceArray);
    geodesicDistanceArray->Delete();
    }

  if (this->ComputeNormalizedTangencyDeviation)
    {
    output->GetPointData()->AddArray(normalizedTangencyDeviationArray);
    normalizedTangencyDeviationArray->Delete();
    }

  if (this->ComputeEuclideanDistance)
    {
    output->GetPointData()->AddArray(euclideanDistanceArray);
    euclideanDistanceArray->Delete();
    }

  if (this->ComputeCenterlineVectors)
    {
    output->GetPointData()->AddArray(centerlineVectorsArray);
    centerlineVectorsArray->Delete();
    }

  if (this->ComputeCellIds)
    {
    output->GetPointData()->AddArray(cellIdsArray);
    cellIdsArray->Delete();
    }

  if (this->ComputePCoords)
    {
    output->GetPointData()->AddArray(pcoordsArray);
    pcoordsArray->Delete();
    }

  if (this->AdjustBoundaryValues)
    {
    this->AdjustBoundaryQuantities(output);
    }

  return 1;
}

void vtkvmtkPolyDataLocalGeometry::AdjustBoundaryQuantities(vtkPolyData* output)
{
  vtkDataArray* poleVectorsArray = NULL;
  vtkDataArray* geodesicDistanceArray = NULL;
  vtkDataArray* euclideanDistanceArray = NULL;
  vtkDataArray* centerlineVectorsArray = NULL;

  vtkTriangleFilter* triangleFilter = vtkTriangleFilter::New();
  triangleFilter->SetInputData(output);
  triangleFilter->Update();

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
  boundaryExtractor->SetInputConnection(triangleFilter->GetOutputPort());
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  double point[3], barycenter[3];
  int numberOfBoundaryPoints;

  if (this->ComputePoleVectors)
    {
    poleVectorsArray = output->GetPointData()->GetArray(this->PoleVectorsArrayName);
    }

  if (this->ComputeGeodesicDistance)
    {
    geodesicDistanceArray = output->GetPointData()->GetArray(this->GeodesicDistanceArrayName);
    }
  
  if (this->ComputeEuclideanDistance)
    {
    euclideanDistanceArray = output->GetPointData()->GetArray(this->EuclideanDistanceArrayName);
    }
  
  if (this->ComputeCenterlineVectors)
    {
    centerlineVectorsArray = output->GetPointData()->GetArray(this->CenterlineVectorsArrayName);
    }

  double radialVector[3], radialVectorModulus;
  vtkIdType boundaryPointId;

  int i, j;
  for (i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    vtkCell* boundary = boundaries->GetCell(i);
    numberOfBoundaryPoints = boundary->GetPoints()->GetNumberOfPoints();
    barycenter[0] = barycenter[1] = barycenter[2] = 0.0;
    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      boundary->GetPoints()->GetPoint(j,point);
      barycenter[0] += point[0];
      barycenter[1] += point[1];
      barycenter[2] += point[2];
      }
    barycenter[0] /= numberOfBoundaryPoints;
    barycenter[1] /= numberOfBoundaryPoints;
    barycenter[2] /= numberOfBoundaryPoints;

    for (j=0; j<numberOfBoundaryPoints; j++)
      {
      boundaryPointId = static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundary->GetPointId(j)));
      output->GetPoint(boundaryPointId,point);
      radialVector[0] = barycenter[0] - point[0];
      radialVector[1] = barycenter[1] - point[1];
      radialVector[2] = barycenter[2] - point[2];
      radialVectorModulus = vtkMath::Norm(radialVector);

      if (this->ComputePoleVectors)
        {
        poleVectorsArray->SetTuple(boundaryPointId,radialVector);
        }
      
      if (this->ComputeGeodesicDistance)
        {
        geodesicDistanceArray->SetTuple1(boundaryPointId,radialVectorModulus);
        }
      
      if (this->ComputeEuclideanDistance)
        {
        euclideanDistanceArray->SetTuple1(boundaryPointId,radialVectorModulus);
        }
      
      if (this->ComputeCenterlineVectors)
        {
        centerlineVectorsArray->SetTuple(boundaryPointId,radialVector);
        }
      }
    }

  triangleFilter->Delete();
  boundaryExtractor->Delete();
}

void vtkvmtkPolyDataLocalGeometry::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
