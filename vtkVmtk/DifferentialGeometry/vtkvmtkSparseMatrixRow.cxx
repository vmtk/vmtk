/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSparseMatrixRow.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:07:30 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

vtkCxxRevisionMacro(vtkvmtkSparseMatrixRow, "$Revision: 1.2 $");
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

void vtkvmtkSparseMatrixRow::DeepCopy(vtkvmtkItem *src)
  {
  this->Superclass::DeepCopy(src);

  vtkvmtkSparseMatrixRow* rowSrc = vtkvmtkSparseMatrixRow::SafeDownCast(src);

  if (rowSrc==NULL)
    {
    vtkErrorMacro(<<"Trying to deep copy a non-row item");
    }

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

  this->NElements = rowSrc->NElements;

  if (rowSrc->NElements>0)
    {
    this->ElementIds = new vtkIdType[rowSrc->NElements];
    memcpy(this->ElementIds, rowSrc->ElementIds, this->NElements * sizeof(vtkIdType));
    this->Elements = new double[rowSrc->NElements];
    memcpy(this->Elements, rowSrc->Elements, this->NElements * sizeof(double));
    }

  this->DiagonalElement = rowSrc->DiagonalElement;
  }

