/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkTopologicalSeamFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2015/12/01 12:26:27 $
  Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#include <queue>

#include "vtkvmtkTopologicalSeamFilter.h"

#include "vtkvmtkConstants.h"

#include "vtkCell.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkUnsignedCharArray.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkTopologicalSeamFilter);

vtkvmtkTopologicalSeamFilter::vtkvmtkTopologicalSeamFilter()
{
  this->ClosestPoint[0] = this->ClosestPoint[1] = this->ClosestPoint[2] = 0.0;
  this->SeamScalarsArrayName = NULL;
  this->SeamFunction = NULL;
}

vtkvmtkTopologicalSeamFilter::~vtkvmtkTopologicalSeamFilter()
{
  if (this->SeamScalarsArrayName)
  {
    delete[] this->SeamScalarsArrayName;
    this->SeamScalarsArrayName = NULL;
  }

  if (this->SeamFunction)
  {
    this->SeamFunction->Delete();
    this->SeamFunction = NULL;
  }
}

int vtkvmtkTopologicalSeamFilter::RequestData(vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (this->SeamScalarsArrayName == NULL)
  {
    vtkErrorMacro("Name of seam scalars array not specified.");
    return 1;
  }

  vtkDoubleArray* sourceArray = vtkDoubleArray::New();

  vtkIdType numberOfPoints = input->GetNumberOfPoints();

  if (this->SeamFunction == NULL)
  {
    if (input->GetPointData()->GetArray(this->SeamScalarsArrayName))
    {
      sourceArray->DeepCopy(input->GetPointData()->GetArray(this->SeamScalarsArrayName));
    }
    else
    {
      vtkErrorMacro("Seam scalars with name specified does not exist and seam function not specified.");
      return 1;
    }
  }
  else
  {
    sourceArray->SetNumberOfTuples(input->GetNumberOfPoints());
    double point[3], value;
    for (vtkIdType i=0; i<numberOfPoints; i++)
    {
      input->GetPoint(i,point);
      value = this->SeamFunction->FunctionValue(point);
      sourceArray->SetValue(i,value);
    }
  }

  vtkIdType numberOfCells = input->GetNumberOfCells();

  vtkUnsignedCharArray* visitedArray = vtkUnsignedCharArray::New();
  visitedArray->SetNumberOfTuples(numberOfPoints);
  visitedArray->FillComponent(0,0.0);

  vtkDoubleArray* seamArray = vtkDoubleArray::New();
  seamArray->SetName(this->SeamScalarsArrayName);
  seamArray->SetNumberOfTuples(numberOfPoints);
  seamArray->FillComponent(0,0.0);

  input->BuildCells();
  input->BuildLinks();

  vtkIdType firstSeamPointId = -1;
  double minDist = VTK_VMTK_LARGE_DOUBLE;

  vtkIdType npts, *pts;
  // get the point that is closest to the implicit plane
  for (vtkIdType i=0; i<numberOfCells; i++)
  {
    input->GetCellPoints(i,npts,pts);
    for (vtkIdType j=0; j<npts-1; j++)
    {
      vtkIdType point0Id = pts[j];
      vtkIdType point1Id = pts[j+1];
      double point0Value = sourceArray->GetTuple1(point0Id);
      double point1Value = sourceArray->GetTuple1(point1Id);
      if (point0Value * point1Value < 0.0)
      {
        double point0[3], point1[3];
        input->GetPoint(point0Id,point0);
        input->GetPoint(point1Id,point1);
        double dist0 = vtkMath::Distance2BetweenPoints(point0,this->ClosestPoint);
        double dist1 = vtkMath::Distance2BetweenPoints(point1,this->ClosestPoint);
        if (dist0 < minDist)
        {
          minDist = dist0;
          firstSeamPointId = point0Id;
        }
        if (dist1 < minDist)
        {
          minDist = dist1;
          firstSeamPointId = point1Id;
        }
        continue;
      }
    }
  }

  std::queue<vtkIdType> seamQueue;

  seamQueue.push(firstSeamPointId);

  std::queue<vtkIdType> leftQueue;
  std::queue<vtkIdType> rightQueue;

  vtkIdType *cells;
  unsigned short ncells;

  while (!seamQueue.empty())
  {
    vtkIdType point0Id = seamQueue.front();
    seamQueue.pop();
    double point0Value = sourceArray->GetTuple1(point0Id);
    visitedArray->SetValue(point0Id,1);
    input->GetPointCells(point0Id,ncells,cells);
    for (vtkIdType i=0; i<ncells; i++)
    {
      vtkIdType cellId = cells[i];
      input->GetCellPoints(cellId,npts,pts);
      for (vtkIdType j=0; j<npts; j++)
      {
        vtkIdType point1Id = pts[j];
        if (visitedArray->GetValue(point1Id) == 1)
        {
          continue;
        }
        double point1Value = sourceArray->GetTuple1(point1Id);
        if (point0Value * point1Value > 0.0)
        {
          seamArray->SetValue(point1Id,point1Value);
          if (point1Value > 0.0)
          {
            rightQueue.push(point1Id);
          }
          else
          {
            leftQueue.push(point1Id);
          }
          continue;
        }
        visitedArray->SetValue(point1Id,1);
        seamArray->SetValue(point1Id,point1Value);
        seamQueue.push(point1Id);
      }
    }
  }

  const double leftValue = -1.0;
  const double rightValue = 1.0;

  while(!leftQueue.empty())
  {
    vtkIdType point0Id = leftQueue.front();
    leftQueue.pop();
    visitedArray->SetValue(point0Id,1);
    input->GetPointCells(point0Id,ncells,cells);
    for (vtkIdType i=0; i<ncells; i++)
    {
      vtkIdType cellId = cells[i];
      input->GetCellPoints(cellId,npts,pts);
      for (vtkIdType j=0; j<npts; j++)
      {
        vtkIdType point1Id = pts[j];
        if (visitedArray->GetValue(point1Id) == 1)
        {
          continue;
        }
        visitedArray->SetValue(point1Id,1);
        seamArray->SetValue(point1Id,leftValue);
        leftQueue.push(point1Id);
      }
    }
  }

  while(!rightQueue.empty())
  {
    vtkIdType point0Id = rightQueue.front();
    rightQueue.pop();
    visitedArray->SetValue(point0Id,1);
    input->GetPointCells(point0Id,ncells,cells);
    for (vtkIdType i=0; i<ncells; i++)
    {
      vtkIdType cellId = cells[i];
      input->GetCellPoints(cellId,npts,pts);
      for (vtkIdType j=0; j<npts; j++)
      {
        vtkIdType point1Id = pts[j];
        if (visitedArray->GetValue(point1Id) == 1)
        {
          continue;
        }
        visitedArray->SetValue(point1Id,1);
        seamArray->SetValue(point1Id,rightValue);
        rightQueue.push(point1Id);
      }
    }
  }

  output->DeepCopy(input);
  output->GetPointData()->AddArray(seamArray);
  output->GetPointData()->SetActiveScalars(this->SeamScalarsArrayName);

  visitedArray->Delete();
  sourceArray->Delete();
  seamArray->Delete();

  return 1;
}

void vtkvmtkTopologicalSeamFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
