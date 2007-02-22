/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFELaplaceAssembler.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
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

#include "vtkvmtkPolyDataFELaplaceAssembler.h"
#include "vtkvmtkGaussQuadrature.h"
#include "vtkvmtkFEShapeFunctions.h"
#include "vtkvmtkSparseMatrixRow.h"
#include "vtkvmtkNeighborhoods.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataFELaplaceAssembler, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkPolyDataFELaplaceAssembler);

vtkvmtkPolyDataFELaplaceAssembler::vtkvmtkPolyDataFELaplaceAssembler()
{
  this->DataSet = NULL;
  this->Matrix = NULL;
  this->QuadratureOrder = 1;
}

vtkvmtkPolyDataFELaplaceAssembler::~vtkvmtkPolyDataFELaplaceAssembler()
{
  if (this->DataSet)
    {
    this->DataSet->Delete();
    this->DataSet = NULL;
    }
  if (this->Matrix)
    {
    this->Matrix->Delete();
    this->Matrix = NULL;
    }
}

void vtkvmtkPolyDataFELaplaceAssembler::Build()
{
  vtkvmtkNeighborhoods* neighborhoods = vtkvmtkNeighborhoods::New();
  neighborhoods->SetNeighborhoodTypeToPolyDataNeighborhood();
  neighborhoods->SetDataSet(this->DataSet);
  neighborhoods->Build();
  this->Matrix->AllocateRowsFromNeighborhoods(neighborhoods);
  neighborhoods->Delete();

  vtkvmtkGaussQuadrature* gaussQuadrature = vtkvmtkGaussQuadrature::New();
  gaussQuadrature->SetOrder(this->QuadratureOrder);

  vtkvmtkFEShapeFunctions* feShapeFunctions = vtkvmtkFEShapeFunctions::New();

  int numberOfCells = this->DataSet->GetNumberOfCells();
  int k;
  for (k=0; k<numberOfCells; k++)
    {
    vtkCell* cell = this->DataSet->GetCell(k);
    if (cell->GetCellDimension() != 2)
      {
      continue;
      } 
    gaussQuadrature->Initialize(cell->GetCellType());
    feShapeFunctions->Initialize(cell,gaussQuadrature->GetQuadraturePoints());
    int numberOfQuadraturePoints = gaussQuadrature->GetNumberOfQuadraturePoints();
    double quadraturePCoords[3], quadraturePoint[3];
    int numberOfCellPoints = cell->GetNumberOfPoints();
    double* weights = new double[numberOfCellPoints];
    double* derivs = new double[2*numberOfCellPoints];
    int i, j;
    int q;
    for (q=0; q<numberOfQuadraturePoints; q++)
      {
      gaussQuadrature->GetQuadraturePoint(q,quadraturePCoords);
      double quadratureWeight = gaussQuadrature->GetQuadratureWeight(q);
      double jacobian = feShapeFunctions->GetJacobian(q);
      double dphii[3], dphij[3];
      for (i=0; i<numberOfCellPoints; i++)
        {
        vtkIdType iId = cell->GetPointId(i);
        vtkvmtkSparseMatrixRow* row = this->Matrix->GetRow(iId);
        feShapeFunctions->GetDPhi(q,i,dphii);
        for (j=0; j<numberOfCellPoints; j++)
          {
          vtkIdType jId = cell->GetPointId(j);
          feShapeFunctions->GetDPhi(q,j,dphij);
          double gradphii_gradphij = vtkMath::Dot(dphii,dphij);
          if ((iId == 55 && jId == 54) || (iId == 54 && jId == 55))
            {
            cout<<iId<<" "<<jId<<" "<<dphii[0]<<" "<<dphii[1]<<" "<<dphii[2]<<" "<<dphij[0]<<" "<<dphij[1]<<" "<<dphij[2]<<" "<<endl;
            }
          double value = jacobian * quadratureWeight * gradphii_gradphij;
          if (iId != jId)
            {
            double currentValue = row->GetElement(row->GetElementIndex(jId));
            row->SetElement(row->GetElementIndex(jId),currentValue+value);
            }
          else
            {
            double currentValue = row->GetDiagonalElement();
            row->SetDiagonalElement(currentValue+value);
            }
          }
        }
      }
    delete[] weights;
    delete[] derivs;
    }

  gaussQuadrature->Delete();
  feShapeFunctions->Delete();
}

void vtkvmtkPolyDataFELaplaceAssembler::DeepCopy(vtkvmtkPolyDataFELaplaceAssembler *src)
{
  this->DataSet->DeepCopy(src->DataSet);
  this->Matrix->DeepCopy(src->Matrix);
  this->QuadratureOrder = src->QuadratureOrder;
}
 
void vtkvmtkPolyDataFELaplaceAssembler::ShallowCopy(vtkvmtkPolyDataFELaplaceAssembler *src)
{
  this->DataSet = src->DataSet;
  this->DataSet->Register(this);
  this->Matrix = src->Matrix;
  this->Matrix->Register(this);
  this->QuadratureOrder = src->QuadratureOrder;
}

