/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataSizingFunction.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataSizingFunction.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkTriangle.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataSizingFunction);

vtkvmtkPolyDataSizingFunction::vtkvmtkPolyDataSizingFunction()
{
  this->SizingFunctionArrayName = NULL;
  this->ScaleFactor = 1.0;
}

vtkvmtkPolyDataSizingFunction::~vtkvmtkPolyDataSizingFunction()
{
  if (this->SizingFunctionArrayName)
    {
    delete[] this->SizingFunctionArrayName;
    this->SizingFunctionArrayName = NULL;
    }
}

int vtkvmtkPolyDataSizingFunction::RequestData(
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

  if (input->GetNumberOfPoints() < 1)
    {
    return 1;
    }

  if (!this->SizingFunctionArrayName)
    {
    vtkErrorMacro(<<"No SizingFunctionArrayName specified");
    return 1;
    }

  input->BuildCells();
  input->BuildLinks();

  int numberOfPoints = input->GetNumberOfPoints();

  vtkDoubleArray* sizingFunctionArray = vtkDoubleArray::New();
  sizingFunctionArray->SetName(this->SizingFunctionArrayName);
  sizingFunctionArray->SetNumberOfTuples(numberOfPoints);
  sizingFunctionArray->FillComponent(0,0.0);
  
  vtkIdList* pointCells = vtkIdList::New();

  int i, j;
  for (i=0; i<numberOfPoints; i++)
    {
    input->GetPointCells(i,pointCells);
    int numberOfPointCells = pointCells->GetNumberOfIds();
    double averageArea = 0.0;
    if (numberOfPointCells == 0)
      {
      continue;
      }
    for (j=0; j<numberOfPointCells; j++)
      {
      vtkTriangle* triangle = vtkTriangle::SafeDownCast(input->GetCell(pointCells->GetId(j)));
      if (!triangle)
        {
        vtkErrorMacro(<<"Cell not triangle: skipping cell for sizing function computation");
        }
      double point0[3], point1[3], point2[3];
      triangle->GetPoints()->GetPoint(0,point0);
      triangle->GetPoints()->GetPoint(1,point1);
      triangle->GetPoints()->GetPoint(2,point2);
      averageArea += vtkTriangle::TriangleArea(point0,point1,point2);
      }
    averageArea /= numberOfPointCells;
    
    double sizingFunction = sqrt(averageArea) * this->ScaleFactor;
    sizingFunctionArray->SetValue(i,sizingFunction);
    }

  output->DeepCopy(input);
  output->GetPointData()->AddArray(sizingFunctionArray);

  sizingFunctionArray->Delete();
  pointCells->Delete();

  return 1;
}

void vtkvmtkPolyDataSizingFunction::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
