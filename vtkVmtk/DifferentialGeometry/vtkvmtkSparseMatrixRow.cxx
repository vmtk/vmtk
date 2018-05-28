/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSparseMatrixRow.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:07:30 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkSparseMatrixRow.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkSparseMatrixRow);

vtkvmtkSparseMatrixRow::vtkvmtkSparseMatrixRow()
{
  this->NElements = 0;
  this->ElementIds = NULL;
  this->Elements = NULL;
  this->DiagonalElement = 0.0;
}

vtkvmtkSparseMatrixRow::~vtkvmtkSparseMatrixRow()
{
  if (this->ElementIds!=NULL)
    {
    delete[] this->ElementIds;
    this->ElementIds = NULL;
    }

  if (this->Elements!=NULL)
    {
    delete[] this->Elements;
    this->Elements = NULL;
    }
}

void vtkvmtkSparseMatrixRow::Initialize()
{
  this->NElements = 0;

  if (this->ElementIds!=NULL)
    {
    delete[] this->ElementIds;
    this->ElementIds = NULL;
    }

  if (this->Elements!=NULL)
    {
    delete[] this->Elements;
    this->Elements = NULL;
    }

  this->DiagonalElement = 0.0;
}

void vtkvmtkSparseMatrixRow::SetNumberOfElements(vtkIdType numberOfElements)
{
  vtkIdType i;

  this->NElements = numberOfElements;

  if (this->ElementIds!=NULL)
    {
    delete[] this->ElementIds;
    this->ElementIds = NULL;
    }

  this->ElementIds = new vtkIdType[this->NElements];

  for (i=0; i<this->NElements; i++)
    {
    this->ElementIds[i] = 0;
    }

  if (this->Elements!=NULL)
    {
    delete[] this->Elements;
    this->Elements = NULL;
    }

  this->Elements = new double[this->NElements];

  for (i=0; i<this->NElements; i++)
    {
    this->Elements[i] = 0.0;
    }

  this->DiagonalElement = 0.0;
}

vtkIdType vtkvmtkSparseMatrixRow::GetElementIndex(vtkIdType id)
{
  vtkIdType index = -1;
  int j;
  for (j=0; j<this->NElements; j++)
    {
    if (this->ElementIds[j] == id)
      {
      index = j;
      break;
      }
    }
  if (index == -1)
    {
    vtkErrorMacro("Error: ElementId not in sparse matrix");
    return -1;
    } 
  return index;
}

void vtkvmtkSparseMatrixRow::CopyStencil(vtkvmtkStencil* stencil)
{
  vtkIdType i;

  this->NElements = stencil->GetNumberOfPoints();

  if (this->ElementIds!=NULL)
    {
    delete[] this->ElementIds;
    this->ElementIds = NULL;
    }

  this->ElementIds = new vtkIdType[this->NElements];

  for (i=0; i<this->NElements; i++)
    {
    this->ElementIds[i] = stencil->GetPointId(i);
    }

  if (this->Elements!=NULL)
    {
    delete[] this->Elements;
    this->Elements = NULL;
    }

  this->Elements = new double[this->NElements];

  for (i=0; i<this->NElements; i++)
    {
    this->Elements[i] = stencil->GetWeight(i);
    }

  this->DiagonalElement = stencil->GetCenterWeight();
}

void vtkvmtkSparseMatrixRow::CopyNeighborhood(vtkvmtkNeighborhood* neighborhood)
{
  vtkIdType i;

  int numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();

  this->SetNumberOfElements(numberOfNeighborhoodPoints);

  for (i=0; i<numberOfNeighborhoodPoints; i++)
    {
    this->ElementIds[i] = neighborhood->GetPointId(i);
    }
}

void vtkvmtkSparseMatrixRow::DeepCopy(vtkvmtkSparseMatrixRow *src)
{
  if (this->ElementIds != NULL)
    {
    delete[] this->ElementIds;
    this->ElementIds = NULL;
    }

  if (this->Elements != NULL)
    {
    delete[] this->Elements;
    this->Elements = NULL;
    }

  this->NElements = src->NElements;

  if (src->NElements > 0)
    {
    this->ElementIds = new vtkIdType[src->NElements];
    memcpy(this->ElementIds, src->ElementIds, this->NElements * sizeof(vtkIdType));
    this->Elements = new double[src->NElements];
    memcpy(this->Elements, src->Elements, this->NElements * sizeof(double));
    }

  this->DiagonalElement = src->DiagonalElement;
}

