/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineAttributesFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
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

#include "vtkvmtkCenterlineAttributesFilter.h"
#include "vtkvmtkConstants.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineAttributesFilter);

vtkvmtkCenterlineAttributesFilter::vtkvmtkCenterlineAttributesFilter()
{
  this->AbscissasArrayName = NULL;
  this->ParallelTransportNormalsArrayName = NULL;
}

vtkvmtkCenterlineAttributesFilter::~vtkvmtkCenterlineAttributesFilter()
{
  if (this->AbscissasArrayName)
    {
    delete[] this->AbscissasArrayName;
    this->AbscissasArrayName = NULL;
    }

  if (this->ParallelTransportNormalsArrayName)
    {
    delete[] this->ParallelTransportNormalsArrayName;
    this->ParallelTransportNormalsArrayName = NULL;
    }
}

int vtkvmtkCenterlineAttributesFilter::RequestData(
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

  if (!this->AbscissasArrayName)
    {
    vtkErrorMacro(<<"AbscissasArrayName not specified");
    return 1;
    }

  if (!this->ParallelTransportNormalsArrayName)
    {
    vtkErrorMacro(<<"ParallelTransportNormalsArrayName not specified");
    return 1;
    }

  output->DeepCopy(input);

  vtkDoubleArray* abscissasArray = vtkDoubleArray::New();
  abscissasArray->SetNumberOfComponents(1);
  abscissasArray->SetName(this->AbscissasArrayName);

  this->ComputeAbscissas(input,abscissasArray);
  output->GetPointData()->AddArray(abscissasArray);

  vtkDoubleArray* parallelTransportNormalsArray = vtkDoubleArray::New();
  parallelTransportNormalsArray->SetNumberOfComponents(3);
  parallelTransportNormalsArray->SetName(this->ParallelTransportNormalsArrayName);

  this->ComputeParallelTransportNormals(input,parallelTransportNormalsArray);
  output->GetPointData()->AddArray(parallelTransportNormalsArray);

  abscissasArray->Delete();
  parallelTransportNormalsArray->Delete();

  return 1;
}

void vtkvmtkCenterlineAttributesFilter::ComputeAbscissas(vtkPolyData* input, vtkDoubleArray* abscissasArray)
{
  int numberOfPoints = input->GetNumberOfPoints();

  abscissasArray->SetNumberOfTuples(numberOfPoints);
  abscissasArray->FillComponent(0,0.0);

  double point0[3], point1[3];
  double abscissa;
  vtkIdType id0, id1;

  int numberOfCells = input->GetNumberOfCells();
  int numberOfPointsInCell;

  for (int k=0; k<numberOfCells; k++)
    {
    vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(input->GetCell(k));
    
    if (!polyLine)
      {
      continue;
      }

    numberOfPointsInCell = polyLine->GetNumberOfPoints();

    abscissa = 0.0;

    id0 = polyLine->GetPointId(0);
    abscissasArray->SetValue(id0,abscissa);  

    for (int i=1; i<numberOfPointsInCell; i++)
      {
      id0 = polyLine->GetPointId(i-1);
      id1 = polyLine->GetPointId(i);
      input->GetPoint(id0,point0);
      input->GetPoint(id1,point1);
      abscissa += sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
      abscissasArray->SetValue(id1,abscissa);
      }    
    }
}

void vtkvmtkCenterlineAttributesFilter::ComputeParallelTransportNormals(vtkPolyData* input, vtkDoubleArray* parallelTransportNormalsArray)
{
  int numberOfPoints = input->GetNumberOfPoints();

  parallelTransportNormalsArray->SetNumberOfTuples(numberOfPoints);
  parallelTransportNormalsArray->FillComponent(0,0.0);

  int numberOfCells = input->GetNumberOfCells();
  int numberOfPointsInCell;

  vtkTransform* transform = vtkTransform::New();

  vtkIdType id0, id1, id2;
  double p0[3], p1[3], p2[3];

  double t0[3], t1[3], v[3];
  double dot, theta;
  double n0[3], n1[3];

  id2 = -1;
  
  for (int k=0; k<numberOfCells; k++)
    {
    vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(input->GetCell(k));
    
    if (!polyLine)
      {
      continue;
      }

    numberOfPointsInCell = polyLine->GetNumberOfPoints();

    if (numberOfPointsInCell < 2)
      {
      continue;
      }

    t0[0] = t0[1] = t0[2] = 0.0;

    id0 = polyLine->GetPointId(0);
    input->GetPoint(id0,p0);

    vtkIdType nextId = 1;
    while (vtkMath::Norm(t0) < VTK_VMTK_DOUBLE_TOL)
      {
      id1 = polyLine->GetPointId(nextId);
      input->GetPoint(id1,p1);

      t0[0] = p1[0] - p0[0];
      t0[1] = p1[1] - p0[1];
      t0[2] = p1[2] - p0[2];

      ++nextId;
      }

    vtkMath::Normalize(t0);

    double dummy[3];
    vtkMath::Perpendiculars(t0,n0,dummy,0.0);
    
    parallelTransportNormalsArray->SetTuple(id0,n0);

    for (int i=1; i<numberOfPointsInCell-1; i++)
      {
      id0 = polyLine->GetPointId(i-1);
      id1 = polyLine->GetPointId(i);
      id2 = polyLine->GetPointId(i+1);
      input->GetPoint(id0,p0);
      input->GetPoint(id1,p1);
      input->GetPoint(id2,p2);

      t0[0] = p1[0] - p0[0];
      t0[1] = p1[1] - p0[1];
      t0[2] = p1[2] - p0[2];

      t1[0] = p2[0] - p1[0];
      t1[1] = p2[1] - p1[1];
      t1[2] = p2[2] - p1[2];

      vtkMath::Normalize(t0);
      vtkMath::Normalize(t1);

      dot = vtkMath::Dot(t0,t1);
      
      if (1-dot < VTK_VMTK_DOUBLE_TOL)
        {
        theta = 0.0;
        }
      else
        {
        theta = acos(dot) * 180.0 / vtkMath::Pi();
        }

      vtkMath::Cross(t0,t1,v);

      transform->Identity();
      transform->RotateWXYZ(theta,v);
      transform->TransformPoint(n0,n1);

      dot = vtkMath::Dot(t1,n1);
      n1[0] -= dot * t1[0];
      n1[1] -= dot * t1[1];
      n1[2] -= dot * t1[2];

      vtkMath::Normalize(n1);
      
      parallelTransportNormalsArray->SetTuple(id1,n1);

      n0[0] = n1[0];
      n0[1] = n1[1];
      n0[2] = n1[2];
      }

    parallelTransportNormalsArray->SetTuple(id2,n1);
    }

  transform->Delete();
}

void vtkvmtkCenterlineAttributesFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
