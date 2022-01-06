/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataToUnstructuredGridFilter.cxx,v $
Language:  C++
Date:      $Date: 2005/03/31 15:49:05 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkvmtkPolyDataToUnstructuredGridFilter.h"

#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataToUnstructuredGridFilter);

int vtkvmtkPolyDataToUnstructuredGridFilter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

int vtkvmtkPolyDataToUnstructuredGridFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
  
  vtkCellArray* cellArray = vtkCellArray::New();

  int numberOfCells = input->GetNumberOfCells();

  input->BuildCells();

  int* cellTypes = new int[numberOfCells];

  vtkIdType npts;
  const vtkIdType *pts;
  int cellType;
  for (int i=0; i<numberOfCells; i++)
    {
    input->GetCellPoints(i,npts,pts);
    cellType = input->GetCellType(i);
    
    cellArray->InsertNextCell(npts,pts);
    cellTypes[i] = cellType;
    }

  output->SetCells(cellTypes,cellArray);

  cellArray->Delete();
  delete[] cellTypes;

  return 1;
}

void vtkvmtkPolyDataToUnstructuredGridFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
