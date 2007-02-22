/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkFEShapeFunctions.cxx,v $
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

#include "vtkvmtkFEShapeFunctions.h"
#include "vtkObjectFactory.h"
#include "vtkCellType.h"
#include "vtkLine.h"
#include "vtkQuadraticEdge.h"
#include "vtkQuad.h"
#include "vtkQuadraticQuad.h"
#include "vtkTriangle.h"
#include "vtkQuadraticTriangle.h"
#include "vtkHexahedron.h"
#include "vtkQuadraticHexahedron.h"
#include "vtkWedge.h"
#include "vtkQuadraticWedge.h"
#include "vtkTetra.h"
#include "vtkQuadraticTetra.h"
#include "vtkMath.h"

vtkStandardNewMacro(vtkvmtkFEShapeFunctions);
vtkCxxRevisionMacro(vtkvmtkFEShapeFunctions, "$Revision: 1.2 $");

vtkvmtkFEShapeFunctions::vtkvmtkFEShapeFunctions()
{
  this->Phi = vtkDoubleArray::New();
  this->DPhi = vtkDoubleArray::New();
  this->Jacobians = vtkDoubleArray::New();
}

vtkvmtkFEShapeFunctions::~vtkvmtkFEShapeFunctions()
{
  this->Phi->Delete();
  this->DPhi->Delete();
  this->Jacobians->Delete();
}

void vtkvmtkFEShapeFunctions::Initialize(vtkCell* cell, vtkDoubleArray* pcoords)
{
  vtkIdType cellType = cell->GetCellType();

  vtkIdType numberOfPCoords = pcoords->GetNumberOfTuples();

  this->Phi->Initialize();
  this->Phi->SetNumberOfTuples(numberOfPCoords);

  this->DPhi->Initialize();
  this->DPhi->SetNumberOfComponents(3);
  this->DPhi->SetNumberOfTuples(numberOfPCoords);

  this->Jacobians->Initialize();
  this->Jacobians->SetNumberOfTuples(numberOfPCoords);

  int i;
  for (i=0; i<numberOfPCoords; i++)
  {
    double jacobian = this->ComputeJacobian(cell,pcoords->GetTuple(i));
    this->Jacobians->SetValue(i,jacobian);
  }

  // now take care of Phi, DPhi

  switch(cellType)
  {
//    case VTK_LINE:
//    case VTK_QUADRATIC_EDGE:
//    {
//      this->QuadraturePoints->SetNumberOfComponents(1);
//      this->Initialize1DGauss();
//      break;
//    }
    case VTK_QUAD:
    case VTK_QUADRATIC_QUAD:
    {
      break;
    }
    case VTK_TRIANGLE:
    case VTK_QUADRATIC_TRIANGLE:
    {
      break;
    }
    case VTK_HEXAHEDRON:
    case VTK_QUADRATIC_HEXAHEDRON:
    {
      break;
    }
    case VTK_WEDGE:
    case VTK_QUADRATIC_WEDGE:
    {
      break;
    }
    case VTK_TETRA:
    case VTK_QUADRATIC_TETRA:
    {
      break;
    }
    default:
    {
      vtkErrorMacro("Unsupported element for Finite Elements.");
      return;
    }
  }
}

void vtkvmtkFEShapeFunctions::GetInterpolationDerivs(vtkCell* cell, double* pcoords, double* derivs)
{
  switch (cell->GetCellType())
  {
    case VTK_LINE:
//      vtkLine::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      vtkErrorMacro("Error: InterpolationDerivs not defined for Line.");
      break;
    case VTK_QUADRATIC_EDGE:
//      vtkQuadraticEdge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      vtkErrorMacro("Error: InterpolationDerivs not defined for QuadraticEdge.");
      break;
    case VTK_QUAD:
      vtkQuad::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_QUAD:
      vtkQuadraticQuad::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_TRIANGLE:
      vtkTriangle::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_TRIANGLE:
      vtkQuadraticTriangle::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_HEXAHEDRON:
      vtkHexahedron::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_HEXAHEDRON:
      vtkQuadraticHexahedron::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_WEDGE:
      vtkWedge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_WEDGE:
      vtkQuadraticWedge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_TETRA:
      vtkTetra::SafeDownCast(cell)->InterpolationDerivs(derivs);
      break;
    case VTK_QUADRATIC_TETRA:
      vtkQuadraticTetra::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    default:
      vtkErrorMacro("Error: unsupported cell type.");
      return;
  }
}

double vtkvmtkFEShapeFunctions::ComputeJacobian(vtkCell* cell, double* pcoords)
{
  double jacobian = 0.0;

  int cellDimension = cell->GetCellDimension();

  if (cellDimension == 2)
  {
    int numberOfCellPoints = cell->GetNumberOfPoints();
    double* derivs = new double[2*numberOfCellPoints];
  
    this->GetInterpolationDerivs(cell,pcoords,derivs);
  
    int i, j;
  
    double jacobianMatrix[2][3];
    for (i=0; i<3; i++)
    {
      jacobianMatrix[0][i] = jacobianMatrix[1][i] = 0.0;
    }
  
    double x[3];
    for (j=0; j<numberOfCellPoints; j++)
    {
      cell->GetPoints()->GetPoint(j,x);
      for (i=0; i<3; i++)
      {
        jacobianMatrix[0][i] += x[i] * derivs[j];
        jacobianMatrix[1][i] += x[i] * derivs[numberOfCellPoints+j];
      }
    }
    delete[] derivs;

    double jacobianMatrixSquared[2][2];
    jacobianMatrixSquared[0][0] = vtkMath::Dot(jacobianMatrix[0],jacobianMatrix[0]);
    jacobianMatrixSquared[0][1] = vtkMath::Dot(jacobianMatrix[0],jacobianMatrix[1]);
    jacobianMatrixSquared[1][0] = vtkMath::Dot(jacobianMatrix[1],jacobianMatrix[0]);
    jacobianMatrixSquared[1][1] = vtkMath::Dot(jacobianMatrix[1],jacobianMatrix[1]);

    double jacobianSquared = vtkMath::Determinant2x2(jacobianMatrixSquared[0],jacobianMatrixSquared[1]);

    if (jacobianSquared < 0.0)
    {
      vtkWarningMacro("Warning: negative Jacobian, taking absolute value.");
      jacobianSquared = fabs(jacobianSquared);
    }

    jacobian = sqrt(jacobianSquared);
  }
  else if (cellDimension == 3)
  {
    int numberOfCellPoints = cell->GetNumberOfPoints();
    double* derivs = new double[3*numberOfCellPoints];
  
    this->GetInterpolationDerivs(cell,pcoords,derivs);
  
    int i, j;
  
    double jacobianMatrix[3][3];
    for (i=0; i<3; i++)
    {
      jacobianMatrix[0][i] = jacobianMatrix[1][i] = jacobianMatrix[2][i] = 0.0;
    }
  
    double x[3];
    for (j=0; j<numberOfCellPoints; j++)
    {
      cell->GetPoints()->GetPoint(j,x);
      for (i=0; i<3; i++)
      {
        jacobianMatrix[0][i] += x[i] * derivs[j];
        jacobianMatrix[1][i] += x[i] * derivs[numberOfCellPoints+j];
        jacobianMatrix[2][i] += x[i] * derivs[2*numberOfCellPoints+j];
      }
    }
    delete[] derivs;

    jacobian = vtkMath::Determinant3x3(jacobianMatrix);
  }

  return jacobian;
}

