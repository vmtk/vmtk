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
  this->ItemType = VTK_VMTK_SPARSE_MATRIX_ROW;
  }

vtkvmtkItem* vtkvmtkSparseMatrix::InstantiateNewItem(int itemType)
  {
  vtkvmtkSparseMatrixRow* newRow;

  switch(itemType)
    {
    case VTK_VMTK_SPARSE_MATRIX_ROW:
      newRow = vtkvmtkSparseMatrixRow::New();
      break;
    default:
      vtkErrorMacro(<<"Invalid row type");
      return NULL;
    }

  newRow->Register(this);
  newRow->Delete();

  return newRow;
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
  this->Allocate(numberOfStencils);
  this->SetNumberOfRows(numberOfStencils);

  for (i=0; i<numberOfStencils; i++)
    {
    this->GetRow(i)->CopyStencil(stencils->GetStencil(i));
    }

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
      yValue = 0.0;
    else if (yValue>VTK_VMTK_LARGE_DOUBLE)
      yValue = VTK_VMTK_LARGE_DOUBLE;
    else if (yValue<-VTK_VMTK_LARGE_DOUBLE)
      yValue = -VTK_VMTK_LARGE_DOUBLE;

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
      yValue = 0.0;
    else if (yValue>VTK_VMTK_LARGE_DOUBLE)
      yValue = VTK_VMTK_LARGE_DOUBLE;
    else if (yValue<-VTK_VMTK_LARGE_DOUBLE)
      yValue = -VTK_VMTK_LARGE_DOUBLE;

    y->SetElement(i,yValue);
   }
  }
