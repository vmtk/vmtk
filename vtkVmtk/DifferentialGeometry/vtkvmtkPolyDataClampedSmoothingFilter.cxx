/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataClampedSmoothingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataClampedSmoothingFilter.h"
#include "vtkvmtkPolyDataManifoldNeighborhood.h"
#include "vtkCurvatures.h"
#include "vtkPolyDataNormals.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataClampedSmoothingFilter);

vtkvmtkPolyDataClampedSmoothingFilter::vtkvmtkPolyDataClampedSmoothingFilter()
{
  this->SetSmoothingTypeToLaplacian();
  this->NumberOfIterations = 100;
  this->Clamp = 0;
  this->ClampArrayName = NULL;
  this->ClampThreshold = 0.0;
  this->TimeStepFactor = 1E-1;
}

vtkvmtkPolyDataClampedSmoothingFilter::~vtkvmtkPolyDataClampedSmoothingFilter()
{
  if (this->ClampArrayName)
    {
    delete[] this->ClampArrayName;
    this->ClampArrayName = NULL;
    }
}

double vtkvmtkPolyDataClampedSmoothingFilter::ComputeTimeStep(vtkPolyData* surface)
{
  int numberOfCells = surface->GetNumberOfCells();
  vtkIdType npts, *pts;
  double point0[3], point1[3], point2[3];
  double minTriangleArea = 1E20;
  for (int i=0; i<numberOfCells; i++)
    {
    surface->GetCellPoints(i,npts,pts);
    if (npts < 3)
      {
      continue;
      }
    else if (npts > 3)
      {
      vtkErrorMacro(<<"Not triangle");
      return 0.0;
      }
    surface->GetPoint(pts[0],point0);
    surface->GetPoint(pts[1],point1);
    surface->GetPoint(pts[2],point2);
    double triangleArea = vtkvmtkMath::TriangleArea(point0,point1,point2);
    if (triangleArea < minTriangleArea)
      {
      minTriangleArea = triangleArea;
      }
    }

  return this->TimeStepFactor * minTriangleArea;
}

void vtkvmtkPolyDataClampedSmoothingFilter::LaplacianIteration(vtkPolyData* surface)
{
  vtkDataArray* clampArray = surface->GetPointData()->GetArray(this->ClampArrayName);

  vtkvmtkPolyDataManifoldNeighborhood* neighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
  neighborhood->SetDataSet(surface);

  int numberOfPoints = surface->GetNumberOfPoints();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(surface->GetPoints());

  double deltaT = this->ComputeTimeStep(surface);

  for (int i=0; i<numberOfPoints; i++)
    {
    if (clampArray->GetTuple1(i) > this->ClampThreshold)
      {
      continue;
      }

    neighborhood->SetDataSetPointId(i);
    neighborhood->Build();
    
    if (neighborhood->GetIsBoundary())
      {
      continue;
      }

    double point[3];
    surface->GetPoint(i,point);

    double neighborPoint[3];
    double laplace[3];
    laplace[0] = laplace[1] = laplace[2] = 0.0;
    int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
    double weight = 1.0 / double(numberOfNeighborhoodPoints);
    for (int j=0; j<numberOfNeighborhoodPoints; j++)
      {
      surface->GetPoint(neighborhood->GetPointId(j),neighborPoint);
      laplace[0] += weight * (neighborPoint[0] - point[0]);
      laplace[1] += weight * (neighborPoint[1] - point[1]);
      laplace[2] += weight * (neighborPoint[2] - point[2]);
      }

    double newPoint[3];
    newPoint[0] = point[0];
    newPoint[1] = point[1];
    newPoint[2] = point[2];

    newPoint[0] += deltaT * laplace[0];
    newPoint[1] += deltaT * laplace[1];
    newPoint[2] += deltaT * laplace[2];

    newPoints->SetPoint(i,newPoint);
    }

  surface->GetPoints()->DeepCopy(newPoints);

  neighborhood->Delete();
  newPoints->Delete();
}

void vtkvmtkPolyDataClampedSmoothingFilter::CurvatureDiffusionIteration(vtkPolyData* surface)
{
  vtkDataArray* clampArray = surface->GetPointData()->GetArray(this->ClampArrayName);

  vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
#if (VTK_MAJOR_VERSION <= 5)
  normalsFilter->SetInput(surface);
#else
  normalsFilter->SetInputData(surface);
#endif
  normalsFilter->FlipNormalsOff();
  normalsFilter->ConsistencyOn();
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->ComputeCellNormalsOn();
  normalsFilter->Update();
  vtkDataArray* normals = normalsFilter->GetOutput()->GetPointData()->GetNormals();

  vtkCurvatures* curvaturesFilter = vtkCurvatures::New();
#if (VTK_MAJOR_VERSION <= 5)
  curvaturesFilter->SetInput(surface);
#else
  curvaturesFilter->SetInputData(surface);
#endif
//  curvaturesFilter->InvertMeanCurvatureOn();
  curvaturesFilter->SetCurvatureTypeToMean();
  curvaturesFilter->Update();
  vtkDataArray* meanCurvatureArray = curvaturesFilter->GetOutput()->GetPointData()->GetScalars();

  vtkvmtkPolyDataManifoldNeighborhood* neighborhood = vtkvmtkPolyDataManifoldNeighborhood::New();
  neighborhood->SetDataSet(surface);

  int numberOfPoints = surface->GetNumberOfPoints();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(surface->GetPoints());

  double deltaT = this->ComputeTimeStep(surface);

  for (int i=0; i<numberOfPoints; i++)
    {
    if (clampArray->GetTuple1(i) > this->ClampThreshold)
      {
      continue;
      }

    neighborhood->SetDataSetPointId(i);
    neighborhood->Build();
    
    if (neighborhood->GetIsBoundary())
      {
      continue;
      }

    double point[3];
    surface->GetPoint(i,point);

    double meanCurvature = meanCurvatureArray->GetTuple1(i);

    double laplaceH = 0.0;
    int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
    double weight = 1.0 / double(numberOfNeighborhoodPoints);
    for (int j=0; j<numberOfNeighborhoodPoints; j++)
      {
      double neighborMeanCurvature = meanCurvatureArray->GetTuple1(neighborhood->GetPointId(j));
      laplaceH += weight * (neighborMeanCurvature - meanCurvature);
      }

    double normal[3];
    normals->GetTuple(i,normal);

    double newPoint[3];
    newPoint[0] = point[0];
    newPoint[1] = point[1];
    newPoint[2] = point[2];

    newPoint[0] += deltaT * laplaceH * normal[0];
    newPoint[1] += deltaT * laplaceH * normal[1];
    newPoint[2] += deltaT * laplaceH * normal[2];

    newPoints->SetPoint(i,newPoint);
    }

  surface->GetPoints()->DeepCopy(newPoints);

  normalsFilter->Delete();
  curvaturesFilter->Delete();
  neighborhood->Delete();
  newPoints->Delete();
}

int vtkvmtkPolyDataClampedSmoothingFilter::RequestData(
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

  vtkDataArray* clampArray = NULL;
  if (this->Clamp)
    {
    if (!this->ClampArrayName)
      {
      vtkErrorMacro(<<"Clamp is on but ClampArrayName has not been specified");
      return 1;
      }
    if (this->ClampArrayName)
      {
      clampArray = input->GetPointData()->GetArray(this->ClampArrayName);
      }
    if (!clampArray)
      {
      vtkErrorMacro(<<"ClampArray with name specified does not exist");
      return 1;
      }
    } 

  vtkPolyData* surface = vtkPolyData::New();
  surface->DeepCopy(input);
  surface->BuildCells();

  for (int n=0; n<this->NumberOfIterations; n++)
    {
    if (this->SmoothingType == LAPLACIAN)
      {
      this->LaplacianIteration(surface);
      }
    else if (this->SmoothingType == CURVATURE_DIFFUSION)
      {
      this->CurvatureDiffusionIteration(surface);
      }
    }

  output->DeepCopy(surface);

  surface->Delete();
 
  return 1;
}

void vtkvmtkPolyDataClampedSmoothingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

