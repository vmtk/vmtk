/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCellDimensionFilter.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/

#include "vtkvmtkCellDimensionFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkThreshold.h"
#include "vtkIntArray.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkCellDimensionFilter);

vtkvmtkCellDimensionFilter::vtkvmtkCellDimensionFilter()
{
  this->Threshold = vtkThreshold::New();
}

vtkvmtkCellDimensionFilter::~vtkvmtkCellDimensionFilter()
{
  if (this->Threshold)
    {
    this->Threshold->Delete();
    }
}

int vtkvmtkCellDimensionFilter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}


void vtkvmtkCellDimensionFilter::ThresholdByLower(int lower)
{
  Threshold->ThresholdByLower(lower);
}

void vtkvmtkCellDimensionFilter::ThresholdByUpper(int upper)
{
  Threshold->ThresholdByUpper(upper);
}

void vtkvmtkCellDimensionFilter::ThresholdBetween(int lower, int upper)
{
  Threshold->ThresholdBetween(lower,upper);
}

int vtkvmtkCellDimensionFilter::GetLowerThreshold()
{
  return Threshold->GetLowerThreshold();
}

int vtkvmtkCellDimensionFilter::GetUpperThreshold()
{
  return Threshold->GetUpperThreshold();
}


int vtkvmtkCellDimensionFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkUnstructuredGrid::DATA_OBJECT()));

  vtkIdType numberOfCells = input->GetNumberOfCells();
  
  vtkIntArray *cellDimensionArray = vtkIntArray::New();
  cellDimensionArray->SetName("CellDimensionArray");
  cellDimensionArray->SetNumberOfComponents(1);
  cellDimensionArray->SetNumberOfTuples(numberOfCells);
  
  for (int i=0; i<numberOfCells; i++)
    {
    cellDimensionArray->SetValue(i,input->GetCell(i)->GetCellDimension());
    
    }
  input->GetCellData()->AddArray(cellDimensionArray);

  Threshold->SetInputData(input);
  Threshold->SetInputArrayToProcess(0,0,0,1,"CellDimensionArray");
  Threshold->Update();
  output->DeepCopy(Threshold->GetOutput());
  
  
  
  input->GetCellData()->RemoveArray("CellDimensionArray");
  cellDimensionArray->Delete();

  return 1;
}

void vtkvmtkCellDimensionFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
