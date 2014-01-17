/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridFEVorticityAssembler.cxx,v $
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

#include "vtkvmtkUnstructuredGridFEVorticityAssembler.h"
#include "vtkvmtkGaussQuadrature.h"
#include "vtkvmtkFEShapeFunctions.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridFEVorticityAssembler);

vtkvmtkUnstructuredGridFEVorticityAssembler::vtkvmtkUnstructuredGridFEVorticityAssembler()
{
  this->VelocityArrayName = NULL;
  this->Direction = 0;
}

vtkvmtkUnstructuredGridFEVorticityAssembler::~vtkvmtkUnstructuredGridFEVorticityAssembler()
{
  if (this->VelocityArrayName)
    {
    delete[] this->VelocityArrayName;
    this->VelocityArrayName = NULL;
    }
}

void vtkvmtkUnstructuredGridFEVorticityAssembler::Build()
{
  if (!this->VelocityArrayName)
    {
    vtkErrorMacro("VelocityArrayName not specified!");
    return;
    }

  vtkDataArray* velocityArray = this->DataSet->GetPointData()->GetArray(this->VelocityArrayName);

  if (!velocityArray)
    {
    vtkErrorMacro("VelocityArray with name specified does not exist!");
    return;
    }
 
  if (velocityArray->GetNumberOfComponents() != 3)
    {
    vtkErrorMacro("VelocityArray must have 3 components!");
    return;
    }
  
//  int numberOfVariables = 3;
  int numberOfVariables = 1;
  this->Initialize(numberOfVariables);

  vtkvmtkGaussQuadrature* gaussQuadrature = vtkvmtkGaussQuadrature::New();
  gaussQuadrature->SetOrder(this->QuadratureOrder);

  vtkvmtkFEShapeFunctions* feShapeFunctions = vtkvmtkFEShapeFunctions::New();

  int dimension = 3;

  int numberOfCells = this->DataSet->GetNumberOfCells();
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
      double velocityValue[3];
//      double vorticityValue[3];
//      vorticityValue[0] = vorticityValue[1] = vorticityValue[2] = 0.0;
      double vorticityComponent = 0.0;
      for (i=0; i<numberOfCellPoints; i++)
        {
        vtkIdType iId = cell->GetPointId(i);
        feShapeFunctions->GetDPhi(q,i,dphii);
        velocityArray->GetTuple(iId,velocityValue);
//        vorticityValue[0] += velocityValue[2] * dphii[1] - velocityValue[1] * dphii[2];
//        vorticityValue[1] += velocityValue[0] * dphii[2] - velocityValue[2] * dphii[0];
//        vorticityValue[2] += velocityValue[1] * dphii[0] - velocityValue[0] * dphii[1];
        if (this->Direction == 0)
          {
          vorticityComponent += velocityValue[2] * dphii[1] - velocityValue[1] * dphii[2];
          }
        else if (this->Direction == 1)
          {
          vorticityComponent += velocityValue[0] * dphii[2] - velocityValue[2] * dphii[0];
          }
        else if (this->Direction == 2)
          {
          vorticityComponent += velocityValue[1] * dphii[0] - velocityValue[0] * dphii[1];
          }
        }
      for (i=0; i<numberOfCellPoints; i++)
        {
        vtkIdType iId = cell->GetPointId(i);
        phii = feShapeFunctions->GetPhi(q,i);
//        double value0 = jacobian * quadratureWeight * vorticityValue[0] * phii;
//        double value1 = jacobian * quadratureWeight * vorticityValue[1] * phii;
//        double value2 = jacobian * quadratureWeight * vorticityValue[2] * phii;
        double value = jacobian * quadratureWeight * vorticityComponent * phii;
//        this->RHSVector->AddElement(iId,value0);
//        this->RHSVector->AddElement(iId+numberOfPoints,value1);
//        this->RHSVector->AddElement(iId+2*numberOfPoints,value2);
        this->RHSVector->AddElement(iId,value);
        for (j=0; j<numberOfCellPoints; j++)
          {
          vtkIdType jId = cell->GetPointId(j);
          phij = feShapeFunctions->GetPhi(q,j);
          double value = jacobian * quadratureWeight * phii * phij;
//          this->Matrix->AddElement(iId,jId,value);
//          this->Matrix->AddElement(iId+numberOfPoints,jId+numberOfPoints,value);
//          this->Matrix->AddElement(iId+2*numberOfPoints,jId+2*numberOfPoints,value);
          this->Matrix->AddElement(iId,jId,value);
          }
        }
      }
    }

  gaussQuadrature->Delete();
  feShapeFunctions->Delete();
}

