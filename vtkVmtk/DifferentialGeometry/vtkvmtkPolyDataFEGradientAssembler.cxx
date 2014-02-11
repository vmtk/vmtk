/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataFEGradientAssembler.cxx,v $
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

#include "vtkvmtkPolyDataFEGradientAssembler.h"
#include "vtkvmtkGaussQuadrature.h"
#include "vtkvmtkFEShapeFunctions.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataFEGradientAssembler);

vtkvmtkPolyDataFEGradientAssembler::vtkvmtkPolyDataFEGradientAssembler()
{
  this->ScalarsArrayName = NULL;
  this->ScalarsComponent = 0;
}

vtkvmtkPolyDataFEGradientAssembler::~vtkvmtkPolyDataFEGradientAssembler()
{
  if (this->ScalarsArrayName)
    {
    delete[] this->ScalarsArrayName;
    this->ScalarsArrayName = NULL;
    }
}

void vtkvmtkPolyDataFEGradientAssembler::Build()
{
  if (!this->ScalarsArrayName)
    {
    vtkErrorMacro("ScalarsArrayName not specified!");
    return;
    }

  vtkDataArray* scalarsArray = this->DataSet->GetPointData()->GetArray(this->ScalarsArrayName);

  if (!scalarsArray)
    {
    vtkErrorMacro("ScalarsArray with name specified does not exist!");
    return;
    }
  
  int numberOfVariables = 3;
  this->Initialize(numberOfVariables);

  vtkvmtkGaussQuadrature* gaussQuadrature = vtkvmtkGaussQuadrature::New();
  gaussQuadrature->SetOrder(this->QuadratureOrder);

  vtkvmtkFEShapeFunctions* feShapeFunctions = vtkvmtkFEShapeFunctions::New();

  int dimension = 2;

  int numberOfCells = this->DataSet->GetNumberOfCells();
  int numberOfPoints = this->DataSet->GetNumberOfPoints();
  int k;
  for (k=0; k<numberOfCells; k++)
    {
    vtkCell* cell = this->DataSet->GetCell(k);
    if (cell->GetCellDimension() != dimension)
      {
      continue;
      } 
    gaussQuadrature->Initialize(cell->GetCellType());
    feShapeFunctions->Initialize(cell,gaussQuadrature->GetQuadraturePoints());
    int numberOfQuadraturePoints = gaussQuadrature->GetNumberOfQuadraturePoints();
    double quadraturePCoords[3];
    int numberOfCellPoints = cell->GetNumberOfPoints();
    int i, j;
    int q;
    for (q=0; q<numberOfQuadraturePoints; q++)
      {
      gaussQuadrature->GetQuadraturePoint(q,quadraturePCoords);
      double quadratureWeight = gaussQuadrature->GetQuadratureWeight(q);
      double jacobian = feShapeFunctions->GetJacobian(q);
      double phii, phij;
      double dphii[3];
      double gradientValue[3];
      gradientValue[0] = gradientValue[1] = gradientValue[2] = 0.0;
      for (i=0; i<numberOfCellPoints; i++)
        {
        vtkIdType iId = cell->GetPointId(i);
        feShapeFunctions->GetDPhi(q,i,dphii);
        double nodalValue = scalarsArray->GetComponent(iId,this->ScalarsComponent);
        gradientValue[0] += nodalValue * dphii[0];
        gradientValue[1] += nodalValue * dphii[1];
        gradientValue[2] += nodalValue * dphii[2];
        }
      for (i=0; i<numberOfCellPoints; i++)
        {
        vtkIdType iId = cell->GetPointId(i);
        phii = feShapeFunctions->GetPhi(q,i);
        double value0 = jacobian * quadratureWeight * gradientValue[0] * phii;
        double value1 = jacobian * quadratureWeight * gradientValue[1] * phii;
        double value2 = jacobian * quadratureWeight * gradientValue[2] * phii;
        this->RHSVector->AddElement(iId,value0);
        this->RHSVector->AddElement(iId+numberOfPoints,value1);
        this->RHSVector->AddElement(iId+2*numberOfPoints,value2);
        for (j=0; j<numberOfCellPoints; j++)
          {
          vtkIdType jId = cell->GetPointId(j);
          phij = feShapeFunctions->GetPhi(q,j);
          double value = jacobian * quadratureWeight * phii * phij;
          this->Matrix->AddElement(iId,jId,value);
          this->Matrix->AddElement(iId+numberOfPoints,jId+numberOfPoints,value);
          this->Matrix->AddElement(iId+2*numberOfPoints,jId+2*numberOfPoints,value);
          }
        }
      }
    }

  gaussQuadrature->Delete();
  feShapeFunctions->Delete();
}

