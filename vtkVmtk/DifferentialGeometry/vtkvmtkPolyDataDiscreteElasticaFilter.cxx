/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDiscreteElasticaFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
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

#include "vtkvmtkPolyDataDiscreteElasticaFilter.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"
#include "vtkPolyDataNormals.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataDiscreteElasticaFilter);

vtkvmtkPolyDataDiscreteElasticaFilter::vtkvmtkPolyDataDiscreteElasticaFilter()
{
}

vtkvmtkPolyDataDiscreteElasticaFilter::~vtkvmtkPolyDataDiscreteElasticaFilter()
{
}
#define CURVATURES
#ifdef CURVATURES
#include "vtkCurvatures.h"
#if 0
int vtkvmtkPolyDataDiscreteElasticaFilter::RequestData(
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

  vtkPolyData* surface = vtkPolyData::New();
  surface->DeepCopy(input);

  vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
  normalsFilter->SetInput(surface);
  normalsFilter->FlipNormalsOff();
  normalsFilter->ConsistencyOn();
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->ComputeCellNormalsOn();

  vtkCurvatures* curvaturesFilter = vtkCurvatures::New();
  curvaturesFilter->SetInput(surface);
//  curvaturesFilter->InvertMeanCurvatureOn();

  vtkvmtkPolyDataManifoldNeighborhood* neighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
  neighborhood->SetDataSet(surface);

  int numberOfPoints = surface->GetNumberOfPoints();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->SetNumberOfPoints(numberOfPoints);

  vtkDoubleArray* meanCurvatureArray = vtkDoubleArray::New();
  vtkDoubleArray* gaussianCurvatureArray = vtkDoubleArray::New();
 
  double deltaT = 1.0;

  int numberOfIterations = 1000;

  for (int n=0; n<numberOfIterations; n++)
    {
    surface->Modified();
    curvaturesFilter->SetCurvatureTypeToMean();
    curvaturesFilter->Update();
    meanCurvatureArray->DeepCopy(curvaturesFilter->GetOutput()->GetPointData()->GetScalars());
    curvaturesFilter->SetCurvatureTypeToGaussian();
    curvaturesFilter->Update();
    gaussianCurvatureArray->DeepCopy(curvaturesFilter->GetOutput()->GetPointData()->GetScalars());
    normalsFilter->Update();
    vtkDataArray* normals = normalsFilter->GetOutput()->GetPointData()->GetNormals();

    newPoints->DeepCopy(surface->GetPoints());

    double minTriangleArea = 1E8;

    for (int i=0; i<numberOfPoints; i++)
      {
      neighborhood->SetDataSetPointId(i);
      neighborhood->Build();

      if (neighborhood->GetIsBoundary())
        {
        continue;
        }

      double point[3];
      surface->GetPoint(i,point);

      if (surface->GetPointData()->GetArray("Clamp")->GetTuple1(i) > 0.5)
        {
        newPoints->SetPoint(i,point);
        continue;
        }

      double meanCurvature = meanCurvatureArray->GetTuple1(i);
      double gaussianCurvature = gaussianCurvatureArray->GetTuple1(i);

      double laplaceBeltramiH = 0.0;

      int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
      double weight = 1.0 / double(numberOfNeighborhoodPoints);
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        double neighborMeanCurvature = meanCurvatureArray->GetTuple1(neighborhood->GetPointId(j));
        laplaceBeltramiH += weight * (neighborMeanCurvature - meanCurvature);
        }

      double point1[3], point2[3];
      double area = 0.0;
      double triangleArea;
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        triangleArea = vtkvmtkMath::TriangleArea(point,point1,point2);
        area += triangleArea;
       if (triangleArea < minTriangleArea)
          {
          minTriangleArea = triangleArea;
          }
        }

      double normal[3];
      normals->GetTuple(i,normal);

      double newPoint[3];
      newPoint[0] = point[0];
      newPoint[1] = point[1];
      newPoint[2] = point[2];

      double f = laplaceBeltramiH - 2.0 * meanCurvature * (meanCurvature*meanCurvature - gaussianCurvature);
      
      if (meanCurvature*meanCurvature - gaussianCurvature < 0.0)
        {
        f = laplaceBeltramiH;
        }

//      f = laplaceBeltramiH;
//      cout<<laplaceBeltramiH<<" "<<meanCurvature<<" "<<gaussianCurvature<<" "<<f<<endl;
//      f = - meanCurvature;

      deltaT = minTriangleArea*minTriangleArea / 1.0;
//      deltaT = minTriangleArea / 10.0;

      newPoint[0] += deltaT * f * normal[0];
      newPoint[1] += deltaT * f * normal[1];
      newPoint[2] += deltaT * f * normal[2];

      newPoints->SetPoint(i,newPoint);
      }

    cout<<deltaT<<endl;      

    surface->GetPoints()->DeepCopy(newPoints);
    }

  output->DeepCopy(surface);

  curvaturesFilter->Delete();
  normalsFilter->Delete();
  neighborhood->Delete();
  meanCurvatureArray->Delete();
  gaussianCurvatureArray->Delete();
  surface->Delete();
  newPoints->Delete();
 
  return 1;
}
#endif
int vtkvmtkPolyDataDiscreteElasticaFilter::RequestData(
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

  vtkPolyData* surface = vtkPolyData::New();
  surface->DeepCopy(input);

  vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
  normalsFilter->SetInputData(surface);
  normalsFilter->FlipNormalsOff();
  normalsFilter->ConsistencyOn();
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->ComputeCellNormalsOn();

  vtkCurvatures* curvaturesFilter = vtkCurvatures::New();
  curvaturesFilter->SetInputData(surface);
//  curvaturesFilter->InvertMeanCurvatureOn();

  vtkvmtkPolyDataManifoldNeighborhood* neighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
  neighborhood->SetDataSet(surface);

  int numberOfPoints = surface->GetNumberOfPoints();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->SetNumberOfPoints(numberOfPoints);

  vtkDoubleArray* meanCurvatureArray = vtkDoubleArray::New();
 
  double deltaT = 1.0;

  int numberOfIterations = 100;

  for (int n=0; n<numberOfIterations; n++)
    {
    surface->Modified();
    curvaturesFilter->SetCurvatureTypeToMean();
    curvaturesFilter->Update();
    meanCurvatureArray->DeepCopy(curvaturesFilter->GetOutput()->GetPointData()->GetScalars());
    normalsFilter->Update();
    vtkDataArray* normals = normalsFilter->GetOutput()->GetPointData()->GetNormals();

    newPoints->DeepCopy(surface->GetPoints());

    double minTriangleArea = 1E8;

    for (int i=0; i<numberOfPoints; i++)
      {
      neighborhood->SetDataSetPointId(i);
      neighborhood->Build();

      if (neighborhood->GetIsBoundary())
        {
        continue;
        }

      double point[3];
      surface->GetPoint(i,point);

      if (surface->GetPointData()->GetArray("Clamp")->GetTuple1(i) > 0.5)
        {
        newPoints->SetPoint(i,point);
        continue;
        }

      double meanCurvature = meanCurvatureArray->GetTuple1(i);

      double laplaceH = 0.0;

      double neighborPoint[3];
      double laplace[3];
      laplace[0] = laplace[1] = laplace[2] = 0.0;
      int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
      double weight = 1.0 / double(numberOfNeighborhoodPoints);
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        double neighborMeanCurvature = meanCurvatureArray->GetTuple1(neighborhood->GetPointId(j));
        laplaceH += weight * (neighborMeanCurvature - meanCurvature);
        surface->GetPoint(neighborhood->GetPointId(j),neighborPoint);
        laplace[0] += weight * (neighborPoint[0] - point[0]);
        laplace[1] += weight * (neighborPoint[1] - point[1]);
        laplace[2] += weight * (neighborPoint[2] - point[2]);
        }

      double point1[3], point2[3];
      double area = 0.0;
      double triangleArea;
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        triangleArea = vtkvmtkMath::TriangleArea(point,point1,point2);
        area += triangleArea;
       if (triangleArea < minTriangleArea)
          {
          minTriangleArea = triangleArea;
          }
        }

      double normal[3];
      normals->GetTuple(i,normal);

      double newPoint[3];
      newPoint[0] = point[0];
      newPoint[1] = point[1];
      newPoint[2] = point[2];

      deltaT = minTriangleArea / 10.0;

//      newPoint[0] += deltaT * laplaceH * normal[0];
//      newPoint[1] += deltaT * laplaceH * normal[1];
//      newPoint[2] += deltaT * laplaceH * normal[2];

//      deltaT = 1E0;
      double alpha = 1.0;
      double beta = 0.0;

      newPoint[0] += deltaT * (alpha * laplaceH * normal[0] + beta * laplace[0]);
      newPoint[1] += deltaT * (alpha * laplaceH * normal[1] + beta * laplace[1]);
      newPoint[2] += deltaT * (alpha * laplaceH * normal[2] + beta * laplace[2]);

      newPoints->SetPoint(i,newPoint);
      }

    cout<<deltaT<<endl;      

    surface->GetPoints()->DeepCopy(newPoints);
    }

  output->DeepCopy(surface);

  curvaturesFilter->Delete();
  normalsFilter->Delete();
  neighborhood->Delete();
  meanCurvatureArray->Delete();
  surface->Delete();
  newPoints->Delete();
 
  return 1;
}
#else
int vtkvmtkPolyDataDiscreteElasticaFilter::RequestData(
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

  vtkPolyData* surface = vtkPolyData::New();
  surface->DeepCopy(input);

  vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
  normalsFilter->SetInput(surface);
  normalsFilter->FlipNormalsOff();
  normalsFilter->ConsistencyOn();
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->ComputeCellNormalsOn();

  vtkvmtkPolyDataManifoldNeighborhood* neighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
  neighborhood->SetDataSet(surface);

  // Explicit filtering (not Gauss-Seidel)
  // For each iteration
  // 1. Compute normals
  // 2. Compute mean curvature (using laplace beltrami) and store it in an array
  // 3. Compute F by computing laplace beltrami of mean curvature, mean curvature and Gauss curvature
  // P_i^{k+1} = P_i^k + \tau^k * F_i^k * N_i^k
  // F = - \Delta_B H - 2 H (H^2 - K)
  // H = 0.5 * N * \Delta_B P
  // K = 3 / A * (2\pi - \sum_i \phi_i)
  // \tau_k = A_k^2 / 150
  // A_k: minimal triangle area

  int numberOfPoints = surface->GetNumberOfPoints();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->SetNumberOfPoints(numberOfPoints);

  vtkDoubleArray* meanCurvatureArray = vtkDoubleArray::New();
  meanCurvatureArray->SetNumberOfTuples(numberOfPoints);
 
  double deltaT = 1.0;

  int numberOfIterations = 300;

  for (int n=0; n<numberOfIterations; n++)
    {
    surface->Modified();
    normalsFilter->Update();
    vtkDataArray* normals = normalsFilter->GetOutput()->GetPointData()->GetNormals();

    newPoints->DeepCopy(surface->GetPoints());

    double minTriangleArea = 1E8;

    for (int i=0; i<numberOfPoints; i++)
      {
      neighborhood->SetDataSetPointId(i);
      neighborhood->Build();

      if (neighborhood->GetIsBoundary())
        {
        continue;
        }

      double normal[3];
      normals->GetTuple(i,normal);

      double point[3];
      surface->GetPoint(i,point);

      double laplaceBeltrami[3];
      laplaceBeltrami[0] = 0.0;
      laplaceBeltrami[1] = 0.0;
      laplaceBeltrami[2] = 0.0;

      double point0[3], point1[3], point2[3];
      int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId((j-1+numberOfNeighborhoodPoints)%numberOfNeighborhoodPoints),point0);
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        double weight = vtkvmtkMath::Cotangent(point,point0,point1) + vtkvmtkMath::Cotangent(point,point2,point1);
        laplaceBeltrami[0] += weight * (point1[0] - point[0]);
        laplaceBeltrami[1] += weight * (point1[1] - point[1]);
        laplaceBeltrami[2] += weight * (point1[2] - point[2]);
        }

      double area = 0.0;
      double triangleArea;
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        triangleArea = vtkvmtkMath::TriangleArea(point,point1,point2);
        area += triangleArea;
        if (triangleArea < minTriangleArea)
          {
          minTriangleArea = triangleArea;
          }
        }

      laplaceBeltrami[0] *= 3.0 / area;
      laplaceBeltrami[1] *= 3.0 / area;
      laplaceBeltrami[2] *= 3.0 / area;

      double meanCurvature = 0.5 * vtkMath::Dot(normal,laplaceBeltrami);

      meanCurvatureArray->SetTuple1(i,meanCurvature);
      }

    for (int i=0; i<numberOfPoints; i++)
      {
      neighborhood->SetDataSetPointId(i);
      neighborhood->Build();

      if (neighborhood->GetIsBoundary())
        {
        continue;
        }

      double normal[3];
      normals->GetTuple(i,normal);

      double point[3];
      surface->GetPoint(i,point);

      double meanCurvature = meanCurvatureArray->GetTuple1(i);

      double laplaceBeltramiH = 0.0;

      double point0[3], point1[3], point2[3];
      int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId((j-1+numberOfNeighborhoodPoints)%numberOfNeighborhoodPoints),point0);
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        double weight = vtkvmtkMath::Cotangent(point,point0,point1) + vtkvmtkMath::Cotangent(point,point2,point1);
        double neighborMeanCurvature = meanCurvatureArray->GetTuple1(neighborhood->GetPointId(j));
        laplaceBeltramiH += weight * (neighborMeanCurvature - meanCurvature);
        }

      double area = 0.0;
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        area += vtkvmtkMath::TriangleArea(point,point1,point2);
        }

      laplaceBeltramiH *= 3.0 / area;

      double newPoint[3];
      newPoint[0] = point[0];
      newPoint[1] = point[1];
      newPoint[2] = point[2];

      double normal1[3], normal2[3];
      double angleSum = 0.0;
      for (int j=0; j<numberOfNeighborhoodPoints; j++)
        {
        surface->GetPoint(neighborhood->GetPointId(j),point1);
        surface->GetPoint(neighborhood->GetPointId((j+1)%numberOfNeighborhoodPoints),point2);
        normal1[0] = point1[0] - point[0];
        normal1[1] = point1[1] - point[1];
        normal1[2] = point1[2] - point[2];
        vtkMath::Normalize(normal1);
        normal2[0] = point2[0] - point[0];
        normal2[1] = point2[1] - point[1];
        normal2[2] = point2[2] - point[2];
        vtkMath::Normalize(normal2);
        angleSum += vtkvmtkMath::AngleBetweenNormals(normal1,normal2);
        }
      double gaussianCurvature = 3.0 / area * (2.0 * vtkMath::Pi() - angleSum);

      double f = - laplaceBeltramiH - 2.0 * meanCurvature * (meanCurvature*meanCurvature - gaussianCurvature);
      
      if (meanCurvature*meanCurvature - gaussianCurvature < 0.0)
        {
        f = - laplaceBeltramiH;
        }

//      f = - meanCurvature;
//      cout<<laplaceBeltramiH<<" "<<meanCurvature<<" "<<gaussianCurvature<<" "<<f<<endl;

      deltaT = minTriangleArea*minTriangleArea / 10.0;

      newPoint[0] += deltaT * f * normal[0];
      newPoint[1] += deltaT * f * normal[1];
      newPoint[2] += deltaT * f * normal[2];

      newPoints->SetPoint(i,newPoint);
      }

    cout<<deltaT<<endl;      

    surface->GetPoints()->DeepCopy(newPoints);
    }

  output->DeepCopy(surface);

  normalsFilter->Delete();
  neighborhood->Delete();
  meanCurvatureArray->Delete();
  surface->Delete();
  newPoints->Delete();
 
  return 1;
}
#endif
void vtkvmtkPolyDataDiscreteElasticaFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

