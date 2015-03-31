/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundaryReferenceSystems.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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

#include "vtkvmtkBoundaryReferenceSystems.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkvmtkConstants.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkBoundaryReferenceSystems);

vtkvmtkBoundaryReferenceSystems::vtkvmtkBoundaryReferenceSystems()
{
  this->BoundaryRadiusArrayName = NULL;
  this->BoundaryNormalsArrayName = NULL;
  this->Point1ArrayName = NULL;
  this->Point2ArrayName = NULL;
}

vtkvmtkBoundaryReferenceSystems::~vtkvmtkBoundaryReferenceSystems()
{
  if (this->BoundaryRadiusArrayName)
    {
    delete[] this->BoundaryRadiusArrayName;
    this->BoundaryRadiusArrayName = NULL;
    }

  if (this->BoundaryNormalsArrayName)
    {
    delete[] this->BoundaryNormalsArrayName;
    this->BoundaryNormalsArrayName = NULL;
    }

  if (this->Point1ArrayName)
    {
    delete[] this->Point1ArrayName;
    this->Point1ArrayName = NULL;
    }
    
  if (this->Point2ArrayName)
    {
    delete[] this->Point2ArrayName;
    this->Point2ArrayName = NULL;
    }
}

void vtkvmtkBoundaryReferenceSystems::ComputeBoundaryBarycenter(vtkPoints* points, double* barycenter)
{
  int numberOfPoints;
  double weightSum, weight;
  vtkIdType i;

  // accounts for edge length

  numberOfPoints = points->GetNumberOfPoints();

  weightSum = 0.0;

  barycenter[0] = 0.0;
  barycenter[1] = 0.0;
  barycenter[2] = 0.0;

  double point0[3], point1[3];

  for (i=0; i<numberOfPoints; i++)
    {
    points->GetPoint(i,point0);
    points->GetPoint((i+1)%numberOfPoints,point1);
    weight = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
    if (weight < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }
    weightSum += weight;

    barycenter[0] += (point0[0] + point1[0])/2.0 * weight;
    barycenter[1] += (point0[1] + point1[1])/2.0 * weight;
    barycenter[2] += (point0[2] + point1[2])/2.0 * weight;
    }
  
  barycenter[0] /= weightSum;
  barycenter[1] /= weightSum;
  barycenter[2] /= weightSum;
}

double vtkvmtkBoundaryReferenceSystems::ComputeBoundaryMeanRadius(vtkPoints* points, double* barycenter)
{
  int numberOfPoints = points->GetNumberOfPoints();

  // TODO: weight by edge length as for barycenter

  double meanRadius = 0.0;

  for (int i=0; i<numberOfPoints; i++)
    {
    meanRadius += sqrt(vtkMath::Distance2BetweenPoints(points->GetPoint(i),barycenter));
    }

  meanRadius /= numberOfPoints;

  return meanRadius;
}

void vtkvmtkBoundaryReferenceSystems::ComputeBoundaryNormal(vtkPoints* points, double* barycenter, double* normal)
{
  int numberOfPoints;
  double point[3], vector1[3], vector0[3], cross[3], sumCross[3] = {0.0f,0.0f,0.0f};
  vtkIdType i, j;

  // TODO: weight by edge length as for barycenter

  numberOfPoints = points->GetNumberOfPoints();

  points->GetPoint(numberOfPoints-1,point);
  vector0[0] = point[0] - barycenter[0];
  vector0[1] = point[1] - barycenter[1];
  vector0[2] = point[2] - barycenter[2];

  for (i=0; i<numberOfPoints; i++)
    {
    points->GetPoint(i,point);
    vector1[0] = point[0] - barycenter[0];
    vector1[1] = point[1] - barycenter[1];
    vector1[2] = point[2] - barycenter[2];

    vtkMath::Cross(vector1,vector0,cross);
    
    for (j=0; j<3; j++)
      {
      vector0[j] = vector1[j];
      sumCross[j] += cross[j];
      }
    }

  vtkMath::Normalize(sumCross);

  for (j=0; j<3; j++)  
    {
    normal[j] = sumCross[j];
    }
}

void vtkvmtkBoundaryReferenceSystems::OrientBoundaryNormalOutwards(vtkPolyData* surface, vtkPolyData* boundaries, vtkIdType boundaryCellId, double normal[3], double outwardNormal[3])
{
  vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(boundaryCellId));

  int k=0;
  if (!boundary)
    {
    for (k=0; k<3; k++)
      {
      outwardNormal[k] = normal[k];
      }
    return;
    }

  surface->BuildCells();
  surface->BuildLinks();

  unsigned short ncells;
  vtkIdType *cells;
  vtkIdType npts, *pts;

  double boundaryPoint[3], neighborPoint[3];

  int numberOfBoundaryPoints = boundary->GetNumberOfPoints();

  int j;
  vtkIdList* boundaryIds = vtkIdList::New();
  for (j=0; j<numberOfBoundaryPoints; j++)
    {
    boundaryIds->InsertNextId(static_cast<vtkIdType>(vtkMath::Round(boundaries->GetPointData()->GetScalars()->GetComponent(boundary->GetPointId(j),0))));
    }

  double neighborsToBoundaryNormal[3];
  neighborsToBoundaryNormal[0] = neighborsToBoundaryNormal[1] = neighborsToBoundaryNormal[2] = 0.0;
  for (j=0; j<numberOfBoundaryPoints; j++)
    {
    surface->GetPointCells(boundaryIds->GetId(j),ncells,cells);
    for (int c=0; c<ncells; c++)
      {
      surface->GetCellPoints(cells[c],npts,pts);
      for (int p=0; p<npts; p++)
        {
        if (boundaryIds->IsId(pts[p]) == -1)
          {
          surface->GetPoint(boundaryIds->GetId(j),boundaryPoint);
          surface->GetPoint(pts[p],neighborPoint);
          for (k=0; k<3; k++)
            {
            neighborsToBoundaryNormal[k] += boundaryPoint[k] - neighborPoint[k];
            }
          }
        }
      }
    }

  for (k=0; k<3; k++)
    {
    outwardNormal[k] = normal[k];
    }
  
  vtkMath::Normalize(neighborsToBoundaryNormal); 
  
  if (vtkMath::Dot(normal,neighborsToBoundaryNormal) < 0.0)
    {
    for (k=0; k<3; k++)
      {
      outwardNormal[k] *= -1.0;
      }
    }

  boundaryIds->Delete();
}

void vtkvmtkBoundaryReferenceSystems::ComputeReferenceSystemPoints(double origin[3], double normal[3], double radius, double point1[3], double point2[3])
{
  point1[0] = point1[1] = point1[2] = 0.0;
  point2[0] = point2[1] = point2[2] = 0.0;

  double x_p[3], y_p[3], x[3];

  x[0] = 1.0;
  x[1] = 0.0;
  x[2] = 0.0;

  vtkMath::Cross(normal,x,x_p);
  vtkMath::Normalize(x_p);

  vtkMath::Cross(normal,x_p,y_p);
  vtkMath::Normalize(y_p);

  for (int i=0; i<3; i++)
    {
    point1[i] = origin[i] + x_p[i] * radius;
    point2[i] = origin[i] + y_p[i] * radius;
    }
}

int vtkvmtkBoundaryReferenceSystems::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->BoundaryRadiusArrayName)
    {
    vtkErrorMacro(<< "BoundaryRadiusArrayName not specified.");
    return 1;
    }
    
  if (!this->BoundaryNormalsArrayName)
    {
    vtkErrorMacro(<< "BoundaryNormalsArrayName not specified.");
    return 1;
    }

  if (!this->Point1ArrayName)
    {
    vtkErrorMacro(<< "Point1ArrayName not specified.");
    return 1;
    }
    
  if (!this->Point2ArrayName)
    {
    vtkErrorMacro(<< "Point2ArrayName not specified.");
    return 1;
    }

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputVerts = vtkCellArray::New();
  vtkDoubleArray* outputRadius = vtkDoubleArray::New();
  outputRadius->SetName(this->BoundaryRadiusArrayName);
  vtkDoubleArray* outputNormals = vtkDoubleArray::New();
  outputNormals->SetName(this->BoundaryNormalsArrayName);
  outputNormals->SetNumberOfComponents(3);
  vtkDoubleArray* point1Array = vtkDoubleArray::New();
  point1Array->SetName(this->Point1ArrayName);
  point1Array->SetNumberOfComponents(3);
  vtkDoubleArray* point2Array = vtkDoubleArray::New();
  point2Array->SetName(this->Point2ArrayName);
  point2Array->SetNumberOfComponents(3);

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
#if (VTK_MAJOR_VERSION <= 5)
  boundaryExtractor->SetInput(input);
#else
  boundaryExtractor->SetInputData(input);
#endif

  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  double barycenter[3], normal[3], outwardNormal[3], meanRadius;

  for (int i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));

    if (!boundary)
      {
      vtkErrorMacro(<<"Boundary not a vtkPolyLine");
      continue;
      }

    this->ComputeBoundaryBarycenter(boundary->GetPoints(),barycenter);
    meanRadius = this->ComputeBoundaryMeanRadius(boundary->GetPoints(),barycenter);
    this->ComputeBoundaryNormal(boundary->GetPoints(),barycenter,normal);
    this->OrientBoundaryNormalOutwards(input,boundaries,i,normal,outwardNormal);
    double point1[3], point2[3];
    this->ComputeReferenceSystemPoints(barycenter,normal,meanRadius,point1,point2);
    
    vtkIdType pointId = outputPoints->InsertNextPoint(barycenter);
    outputVerts->InsertNextCell(1);
    outputVerts->InsertCellPoint(pointId);
    outputNormals->InsertNextTuple(outwardNormal);
    outputRadius->InsertNextValue(meanRadius);
    point1Array->InsertNextTuple(point1);
    point2Array->InsertNextTuple(point2);
    }

  output->SetPoints(outputPoints);
  output->SetVerts(outputVerts);
  output->GetPointData()->AddArray(outputNormals);
  output->GetPointData()->SetActiveNormals(this->BoundaryNormalsArrayName);
  output->GetPointData()->AddArray(outputRadius);
  output->GetPointData()->SetActiveScalars(this->BoundaryRadiusArrayName);
  output->GetPointData()->AddArray(point1Array);
  output->GetPointData()->AddArray(point2Array);

  outputPoints->Delete();
  outputVerts->Delete();
  outputNormals->Delete();
  outputRadius->Delete();
  point1Array->Delete();
  point2Array->Delete();
  boundaryExtractor->Delete();

  return 1;
}

void vtkvmtkBoundaryReferenceSystems::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
