/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDirichletBoundaryConditions.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/04 11:07:29 $
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

#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkDirichletBoundaryConditions);

void vtkvmtkDirichletBoundaryConditions::Apply()
{
  vtkIdType i, j, k;
  vtkIdType systemSize;
  vtkIdType numberOfRowElements;
  vtkIdType boundaryNode, numberOfBoundaryNodes;
  vtkvmtkSparseMatrix* systemMatrix;
  vtkvmtkDoubleVector* rhsVector;
  vtkvmtkSparseMatrixRow* row;
  double boundaryValue;
  double element;
  double vectorElement;

  this->Superclass::Apply();

  systemSize = this->LinearSystem->GetX()->GetNumberOfElements();
  numberOfBoundaryNodes = this->BoundaryNodes->GetNumberOfIds();

/// FIXME: when boundary conditions apply is called, linear system hasn't checked if matrix and vectors exist
  systemMatrix = this->LinearSystem->GetA();
  rhsVector = this->LinearSystem->GetB();

  for (i=0; i<numberOfBoundaryNodes; i++)
    {
    boundaryNode = this->BoundaryNodes->GetId(i);
    boundaryValue = this->BoundaryValues->GetComponent(i,0);

    for (j=0; j<systemSize; j++)
      {
      if (j==boundaryNode)
        {
        systemMatrix->GetRow(j)->Initialize();
        systemMatrix->GetRow(j)->SetDiagonalElement(1.0);
        rhsVector->SetElement(j,boundaryValue);
        //rhsVector->SetLocked(j,true);
        }
      else
        {
        row = systemMatrix->GetRow(j);
        numberOfRowElements = row->GetNumberOfElements();
        for (k=0; k<numberOfRowElements; k++)
          {
          if (row->GetElementId(k) == boundaryNode)
            {
            element = row->GetElement(k);
            row->SetElement(k,0.0);
            vectorElement = rhsVector->GetElement(j);
            vectorElement -= element * boundaryValue;
            rhsVector->SetElement(j,vectorElement);
            }
          }
        }
      }
    }
}

