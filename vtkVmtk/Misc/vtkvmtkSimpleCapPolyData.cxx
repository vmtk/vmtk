/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSimpleCapPolyData.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
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

#include "vtkvmtkSimpleCapPolyData.h"
#include "vtkvmtkPolyDataBoundaryExtractor.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkSimpleCapPolyData, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkvmtkSimpleCapPolyData);

vtkvmtkSimpleCapPolyData::vtkvmtkSimpleCapPolyData()
{
  this->CellMarkerArrayName = NULL;
  this->BoundaryIds = NULL;
}

vtkvmtkSimpleCapPolyData::~vtkvmtkSimpleCapPolyData()
{
  if (this->CellMarkerArrayName)
    {
    delete[] this->CellMarkerArrayName;
    this->CellMarkerArrayName = NULL;
    }

  if (this->BoundaryIds)
    {
    this->BoundaryIds->Delete();
    this->BoundaryIds = NULL;
    }
}

int vtkvmtkSimpleCapPolyData::RequestData(
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

  if ( input->GetNumberOfPoints() < 1 )
    {
    return 1;
    }

  bool markCells = true;

  if (!this->CellMarkerArrayName)
    {
    markCells = false;
    }

  if (strcmp(this->CellMarkerArrayName,"") == 0)
    {
    markCells = false;
    }

  input->BuildLinks();

  vtkPoints* newPoints = vtkPoints::New();
  newPoints->DeepCopy(input->GetPoints());

  vtkCellArray* newPolys = vtkCellArray::New();
  newPolys->DeepCopy(input->GetPolys());

  vtkIntArray* cellMarkerArray = NULL;

  if (markCells)
    {
    cellMarkerArray = vtkIntArray::New();
    cellMarkerArray->SetName(this->CellMarkerArrayName);
    cellMarkerArray->SetNumberOfTuples(newPolys->GetNumberOfCells());
    cellMarkerArray->FillComponent(0,0.0);
    }

  vtkvmtkPolyDataBoundaryExtractor* boundaryExtractor = vtkvmtkPolyDataBoundaryExtractor::New();
  boundaryExtractor->SetInput(input);
  boundaryExtractor->Update();

  vtkPolyData* boundaries = boundaryExtractor->GetOutput();

  for (int i=0; i<boundaries->GetNumberOfCells(); i++)
    {
    if (this->BoundaryIds)
      {
      if (this->BoundaryIds->IsId(i) == -1)
        {
        continue;
        }
      }
    vtkPolyLine* boundary = vtkPolyLine::SafeDownCast(boundaries->GetCell(i));
    vtkIdType numberOfBoundaryPoints = boundary->GetNumberOfPoints();
    vtkIdList* boundaryPointIds = vtkIdList::New();
    boundaryPointIds->SetNumberOfIds(numberOfBoundaryPoints);
    for (int j=0; j<numberOfBoundaryPoints; j++)
      {
      boundaryPointIds->SetId(j,static_cast<vtkIdType>(boundaries->GetPointData()->GetScalars()->GetTuple1(boundary->GetPointId(j))));
      }
    newPolys->InsertNextCell(boundaryPointIds);
    if (markCells)
      {
      cellMarkerArray->InsertNextValue(i+1);
      }
    boundaryPointIds->Delete();
    }

  output->SetPoints(newPoints);
  output->SetPolys(newPolys);

  output->GetPointData()->PassData(input->GetPointData());

  if (markCells)
    {
    output->GetCellData()->AddArray(cellMarkerArray);
    cellMarkerArray->Delete();
    }

  newPoints->Delete();
  newPolys->Delete();
  boundaryExtractor->Delete();

  return 1;
}

void vtkvmtkSimpleCapPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
