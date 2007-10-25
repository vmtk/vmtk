/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSparseMatrix.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkSparseMatrix, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkSparseMatrix);

vtkvmtkSparseMatrix::vtkvmtkSparseMatrix()
{
  this->NumberOfRows = 0;
  this->Array = NULL;
}

vtkvmtkSparseMatrix::~vtkvmtkSparseMatrix()
{
  if (this->Array)
    {
    for (int i=0; i<this->NumberOfRows; i++)
      {
      this->Array[i]->Delete();
      }
    delete[] this->Array;
    this->Array = NULL;
    }
}

void vtkvmtkSparseMatrix::Initialize()
{
  if (this->Array)
    {
    for (int i=0; i<this->NumberOfRows; i++)
      {
      this->Array[i]->Delete();
      }
    delete[] this->Array;
    this->Array = NULL;
    }
  this->NumberOfRows = 0;
}

void vtkvmtkSparseMatrix::SetNumberOfRows(vtkIdType numberOfRows)
{
  //deallocate previous rows, allocate new ones
  if (this->Array)
    {
    for (int i=0; i<this->NumberOfRows; i++)
      {
      this->Array[i]->Delete();
      }
    delete[] this->Array;
    this->Array = NULL;
    }

  this->NumberOfRows = numberOfRows;
  this->Array = new vtkvmtkSparseMatrixRow*[numberOfRows];
  for (int i=0; i<this->NumberOfRows; i++)
    {
    this->Array[i] = vtkvmtkSparseMatrixRow::New();
    }
}

void vtkvmtkSparseMatrix::CopyRowsFromStencils(vtkvmtkStencils *stencils)
{
  vtkIdType i;
  vtkIdType numberOfStencils;
  
  if (stencils==NULL)
    {
    vtkErrorMacro(<<"No stencils provided.");
    return;
    }

  numberOfStencils = stencils->GetNumberOfStencils();

  this->Initialize();
  this->SetNumberOfRows(numberOfStencils);

  for (i=0; i<numberOfStencils; i++)
    {
    this->GetRow(i)->CopyStencil(stencils->GetStencil(i));
    }
}

void vtkvmtkSparseMatrix::AllocateRowsFromNeighborhoods(vtkvmtkNeighborhoods *neighborhoods, int numberOfVariables)
{
  if (neighborhoods==NULL)
    {
    vtkErrorMacro(<<"No neighborhoods provided.");
    return;
    }

  int numberOfNeighborhoods = neighborhoods->GetNumberOfNeighborhoods();

  int numberOfRows = numberOfVariables*numberOfNeighborhoods;

  this->Initialize();
  this->SetNumberOfRows(numberOfRows);

  int i;
  for (i=0; i<numberOfRows; i++)
    {
    vtkvmtkSparseMatrixRow* row = this->GetRow(i);
    vtkIdType pointId = i % numberOfNeighborhoods;
    vtkIdType variableId = i / numberOfNeighborhoods;
    vtkvmtkNeighborhood* neighborhood = neighborhoods->GetNeighborhood(pointId);
    int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
    int numberOfElements = numberOfNeighborhoodPoints + (numberOfVariables-1)*(numberOfNeighborhoodPoints+1);
    row->SetNumberOfElements(numberOfElements);
    int j;
//    for (j=0; j<numberOfNeighborhoodPoints; j++)
//      {
//      row->SetElementId(j,neighborhood->GetPointId(j));
//      row->SetElement(j,0.0);
//      }
    int n;
//    for (n=0; n<numberOfVariables-1; n++)
//      {
//      int offset = numberOfNeighborhoodPoints + n*(numberOfNeighborhoodPoints+1);
//      row->SetElementId(offset,pointId+n*numberOfNeighborhoods);
//      row->SetElement(offset,0.0);
//      for (j=0; j<numberOfNeighborhoodPoints; j++)
//        {
//        row->SetElementId(offset+1+j,neighborhood->GetPointId(j)+n*numberOfNeighborhoods);
//        row->SetElement(offset+1+j,0.0);
//        }
//      }
    int index = 0;
    for (n=0; n<numberOfVariables; n++)
      {
      for (j=0; j<numberOfNeighborhoodPoints; j++)
        {
        row->SetElementId(index,neighborhood->GetPointId(j)+n*numberOfNeighborhoods);
        row->SetElement(index,0.0);
        index++;
        }
      if (n != variableId)
        {
        row->SetElementId(index,pointId+n*numberOfNeighborhoods);
        row->SetElement(index,0.0);
        index++;
        }
      }
    }
}

double vtkvmtkSparseMatrix::GetElement(vtkIdType i, vtkIdType j)
{
  vtkvmtkSparseMatrixRow* row = this->GetRow(i);
  if (i != j)
  {
    return row->GetElement(row->GetElementIndex(j));
  }
  else
  {
    return row->GetDiagonalElement();
  }
}

void vtkvmtkSparseMatrix::SetElement(vtkIdType i, vtkIdType j, double value)
{
  vtkvmtkSparseMatrixRow* row = this->GetRow(i);
  if (i != j)
  {
    return row->SetElement(row->GetElementIndex(j),value);
  }
  else
  {
    return row->SetDiagonalElement(value);
  }
}

void vtkvmtkSparseMatrix::AddElement(vtkIdType i, vtkIdType j, double value)
{
  double currentValue = this->GetElement(i,j);
  this->SetElement(i,j,currentValue+value);
}

void vtkvmtkSparseMatrix::Multiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y)
{
  vtkIdType i, j, numberOfRows, numberOfRowElements;
  double yValue, xValue;

  numberOfRows = this->GetNumberOfRows();
  for (i=0; i<numberOfRows; i++)
  {
    yValue = 0.0;
    numberOfRowElements = this->GetRow(i)->GetNumberOfElements();
    for (j=0; j<numberOfRowElements; j++)
    {
      xValue = x->GetElement(this->GetRow(i)->GetElementId(j));
      yValue += this->GetRow(i)->GetElement(j) * xValue;
    }
    xValue = x->GetElement(i);
    yValue += this->GetRow(i)->GetDiagonalElement() * xValue;

    if (fabs(yValue)<VTK_VMTK_PIVOTING_TOL)
      {
      yValue = 0.0;
      }
    else if (yValue>VTK_VMTK_LARGE_DOUBLE)
      {
      yValue = VTK_VMTK_LARGE_DOUBLE;
      }
    else if (yValue<-VTK_VMTK_LARGE_DOUBLE)
      {
      yValue = -VTK_VMTK_LARGE_DOUBLE;
      }

    y->SetElement(i,yValue);
  }
}

void vtkvmtkSparseMatrix::TransposeMultiply(vtkvmtkDoubleVector* x, vtkvmtkDoubleVector* y)
{
  vtkIdType i, j, id, numberOfRows, numberOfRowElements;
  double xValue, yValue;

  yValue = 0.0;
  
  y->Fill(0.0);

  numberOfRows = this->GetNumberOfRows();
  for (i=0; i<numberOfRows; i++)
    {
    numberOfRowElements = this->GetRow(i)->GetNumberOfElements();
    xValue = x->GetElement(i);
    for (j=0; j<numberOfRowElements; j++)
      {
      id = this->GetRow(i)->GetElementId(j);
      yValue = y->GetElement(id);
      yValue += this->GetRow(i)->GetElement(j) * xValue;
      y->SetElement(id,yValue);
      }
    xValue = x->GetElement(i);
    yValue += this->GetRow(i)->GetDiagonalElement() * xValue; 

    if (fabs(yValue)<VTK_VMTK_PIVOTING_TOL)
      {
      yValue = 0.0;
      }
    else if (yValue>VTK_VMTK_LARGE_DOUBLE)
      {
      yValue = VTK_VMTK_LARGE_DOUBLE;
      }
    else if (yValue<-VTK_VMTK_LARGE_DOUBLE)
      {
      yValue = -VTK_VMTK_LARGE_DOUBLE;
      }

    y->SetElement(i,yValue);
    }
}

void vtkvmtkSparseMatrix::DeepCopy(vtkvmtkSparseMatrix *src)
{   
  this->SetNumberOfRows(src->NumberOfRows);

  for (int i=0; i<this->NumberOfRows; i++)
    {
    this->Array[i]->DeepCopy(src->GetRow(i));
    }
}

