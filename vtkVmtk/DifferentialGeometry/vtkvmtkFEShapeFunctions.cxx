/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkFEShapeFunctions.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/15 17:39:25 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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
#include "vtkBiQuadraticQuad.h"
#include "vtkTriangle.h"
#include "vtkQuadraticTriangle.h"
#include "vtkHexahedron.h"
#include "vtkQuadraticHexahedron.h"
#include "vtkTriQuadraticHexahedron.h"
#include "vtkWedge.h"
#include "vtkQuadraticWedge.h"
#include "vtkVersion.h"
#include "vtkBiQuadraticQuadraticWedge.h"
#include "vtkTetra.h"
#include "vtkQuadraticTetra.h"
#include "vtkPoints.h"
#include "vtkMath.h"

//#define VTKVMTKFESHAPEFUNCTIONS_NEGATIVE_JACOBIAN_WARNING

vtkStandardNewMacro(vtkvmtkFEShapeFunctions);


vtkvmtkFEShapeFunctions::vtkvmtkFEShapeFunctions()
{
  this->Phi = vtkDoubleArray::New();
  this->DPhi = vtkDoubleArray::New();
  this->Jacobians = vtkDoubleArray::New();
  this->NumberOfCellPoints = -1;
}

vtkvmtkFEShapeFunctions::~vtkvmtkFEShapeFunctions()
{
  this->Phi->Delete();
  this->DPhi->Delete();
  this->Jacobians->Delete();
}

void vtkvmtkFEShapeFunctions::Initialize(vtkCell* cell, vtkDoubleArray* pcoords)
{
  vtkIdType cellDimension = cell->GetCellDimension();
  vtkIdType numberOfPCoords = pcoords->GetNumberOfTuples();
  vtkIdType numberOfCellPoints = cell->GetNumberOfPoints();
  this->NumberOfCellPoints = numberOfCellPoints;

  this->Phi->Initialize();
  this->Phi->SetNumberOfTuples(numberOfCellPoints*numberOfPCoords);

  this->DPhi->Initialize();
  this->DPhi->SetNumberOfComponents(3);
  this->DPhi->SetNumberOfTuples(numberOfCellPoints*numberOfPCoords);

  this->Jacobians->Initialize();
  this->Jacobians->SetNumberOfTuples(numberOfPCoords);

  int i;

  //Phi
  for (i=0; i<numberOfPCoords; i++)
  {
    double* sf = new double[numberOfCellPoints];
    this->GetInterpolationFunctions(cell,pcoords->GetTuple(i),sf);
    int j;
    for (j=0; j<numberOfCellPoints; j++)
    {
      this->Phi->SetValue(i*numberOfCellPoints+j,sf[j]);
    }
    delete[] sf;

    //DPhi
    double* derivs = new double[cellDimension*numberOfCellPoints];
    this->GetInterpolationDerivs(cell,pcoords->GetTuple(i),derivs);
    if (cellDimension == 2)
    {
      double inverseJacobianMatrix[2][3];
      this->ComputeInverseJacobianMatrix2D(cell,pcoords->GetTuple(i),inverseJacobianMatrix);
      for (j=0; j<numberOfCellPoints; j++)
      {
        int k;
        for (k=0; k<3; k++)
        {
          double dphik = derivs[j] * inverseJacobianMatrix[0][k] + derivs[j+numberOfCellPoints] * inverseJacobianMatrix[1][k];
          this->DPhi->SetComponent(i*numberOfCellPoints+j,k,dphik);
        }
      }
    }
    else if (cellDimension == 3)
    {
      double inverseJacobianMatrix[3][3];
      this->ComputeInverseJacobianMatrix3D(cell,pcoords->GetTuple(i),inverseJacobianMatrix);
      for (j=0; j<numberOfCellPoints; j++)
      {
        int k;
        for (k=0; k<3; k++)
        {
          double dphik = derivs[j] * inverseJacobianMatrix[0][k] + derivs[j+numberOfCellPoints] * inverseJacobianMatrix[1][k] + derivs[j+2*numberOfCellPoints] * inverseJacobianMatrix[2][k];
          this->DPhi->SetComponent(i*numberOfCellPoints+j,k,dphik);
        }
      }
    }
    delete[] derivs;

    //Jacobians
    double jacobian = this->ComputeJacobian(cell,pcoords->GetTuple(i));
    this->Jacobians->SetValue(i,jacobian);
  }
}

void vtkvmtkFEShapeFunctions::ComputeInverseJacobianMatrix2D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[2][3])
{
  int cellDimension = cell->GetCellDimension();

  if (cellDimension != 2)
  {
    vtkGenericWarningMacro("Error: ComputeInverseJacobian2D only works for 2D cells.");
    return;
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();
  double* derivs = new double[2*numberOfCellPoints];

  vtkvmtkFEShapeFunctions::GetInterpolationDerivs(cell,pcoords,derivs);

  int i, j;

  double jacobianMatrixTr[2][3];
  for (i=0; i<3; i++)
  {
    jacobianMatrixTr[0][i] = jacobianMatrixTr[1][i] = 0.0;
  }

  double x[3];
  for (j=0; j<numberOfCellPoints; j++)
  {
    cell->GetPoints()->GetPoint(j,x);
    for (i=0; i<3; i++)
    {
      jacobianMatrixTr[0][i] += x[i] * derivs[j];
      jacobianMatrixTr[1][i] += x[i] * derivs[numberOfCellPoints+j];
    }
  }
  delete[] derivs;

  double jacobianMatrixSquared[2][2];
  jacobianMatrixSquared[0][0] = vtkMath::Dot(jacobianMatrixTr[0],jacobianMatrixTr[0]);
  jacobianMatrixSquared[0][1] = vtkMath::Dot(jacobianMatrixTr[0],jacobianMatrixTr[1]);
  jacobianMatrixSquared[1][0] = vtkMath::Dot(jacobianMatrixTr[1],jacobianMatrixTr[0]);
  jacobianMatrixSquared[1][1] = vtkMath::Dot(jacobianMatrixTr[1],jacobianMatrixTr[1]);

  double jacobianSquared = vtkMath::Determinant2x2(jacobianMatrixSquared[0],jacobianMatrixSquared[1]);

  if (jacobianSquared < 0.0)
  {
#ifdef VTKVMTKFESHAPEFUNCTIONS_NEGATIVE_JACOBIAN_WARNING 
    vtkGenericWarningMacro("Warning: negative determinant of squared Jacobian, taking absolute value.");
#endif
    jacobianSquared = fabs(jacobianSquared);
  }

  double inverseJacobianSquared = 1.0 / jacobianSquared;

  double inverseJacobianMatrixSquared[2][2];
  inverseJacobianMatrixSquared[0][0] =  jacobianMatrixSquared[1][1] * inverseJacobianSquared;
  inverseJacobianMatrixSquared[0][1] = -jacobianMatrixSquared[0][1] * inverseJacobianSquared;
  inverseJacobianMatrixSquared[1][0] = -jacobianMatrixSquared[1][0] * inverseJacobianSquared;
  inverseJacobianMatrixSquared[1][1] =  jacobianMatrixSquared[0][0] * inverseJacobianSquared;

  inverseJacobianMatrix[0][0] = inverseJacobianMatrixSquared[0][0] * jacobianMatrixTr[0][0] + inverseJacobianMatrixSquared[0][1] * jacobianMatrixTr[1][0];
  inverseJacobianMatrix[0][1] = inverseJacobianMatrixSquared[0][0] * jacobianMatrixTr[0][1] + inverseJacobianMatrixSquared[0][1] * jacobianMatrixTr[1][1];
  inverseJacobianMatrix[0][2] = inverseJacobianMatrixSquared[0][0] * jacobianMatrixTr[0][2] + inverseJacobianMatrixSquared[0][1] * jacobianMatrixTr[1][2];
  inverseJacobianMatrix[1][0] = inverseJacobianMatrixSquared[1][0] * jacobianMatrixTr[0][0] + inverseJacobianMatrixSquared[1][1] * jacobianMatrixTr[1][0];
  inverseJacobianMatrix[1][1] = inverseJacobianMatrixSquared[1][0] * jacobianMatrixTr[0][1] + inverseJacobianMatrixSquared[1][1] * jacobianMatrixTr[1][1];
  inverseJacobianMatrix[1][2] = inverseJacobianMatrixSquared[1][0] * jacobianMatrixTr[0][2] + inverseJacobianMatrixSquared[1][1] * jacobianMatrixTr[1][2];
}

void vtkvmtkFEShapeFunctions::ComputeInverseJacobianMatrix3D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[3][3])
{
  int cellDimension = cell->GetCellDimension();

  if (cellDimension != 3)
  {
    vtkGenericWarningMacro("Error: ComputeInverseJacobian3D only works for 3D cells.");
    return;
  }

  int numberOfCellPoints = cell->GetNumberOfPoints();
  double* derivs = new double[3*numberOfCellPoints];

  vtkvmtkFEShapeFunctions::GetInterpolationDerivs(cell,pcoords,derivs);

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

  vtkMath::Invert3x3(jacobianMatrix,inverseJacobianMatrix);

  vtkMath::Transpose3x3(inverseJacobianMatrix,inverseJacobianMatrix);
}

void vtkvmtkFEShapeFunctions::GetInterpolationFunctions(vtkCell* cell, double* pcoords, double* sf)
{
  switch (cell->GetCellType())
  {
    case VTK_QUAD:
      vtkQuad::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_QUADRATIC_QUAD:
      vtkQuadraticQuad::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_TRIANGLE:
      vtkTriangle::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_QUADRATIC_TRIANGLE:
      vtkQuadraticTriangle::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_HEXAHEDRON:
      vtkHexahedron::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_QUADRATIC_HEXAHEDRON:
      vtkQuadraticHexahedron::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_TRIQUADRATIC_HEXAHEDRON:
      vtkTriQuadraticHexahedron::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_WEDGE:
      vtkWedge::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_QUADRATIC_WEDGE:
      vtkQuadraticWedge::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
      vtkBiQuadraticQuadraticWedge::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_TETRA:
      vtkTetra::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    case VTK_QUADRATIC_TETRA:
      vtkQuadraticTetra::SafeDownCast(cell)->InterpolationFunctions(pcoords,sf);
      break;
    default:
      vtkGenericWarningMacro("Error: unsupported cell type.");
      return;
  }
}

void vtkvmtkFEShapeFunctions::GetInterpolationDerivs(vtkCell* cell, double* pcoords, double* derivs)
{
  switch (cell->GetCellType())
  {
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
    case VTK_TRIQUADRATIC_HEXAHEDRON:
      vtkTriQuadraticHexahedron::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_WEDGE:
      vtkWedge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_WEDGE:
      vtkQuadraticWedge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
      vtkBiQuadraticQuadraticWedge::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    case VTK_TETRA:
      vtkTetra::SafeDownCast(cell)->InterpolateDerivs(pcoords,derivs);
      break;
    case VTK_QUADRATIC_TETRA:
      vtkQuadraticTetra::SafeDownCast(cell)->InterpolationDerivs(pcoords,derivs);
      break;
    default:
      vtkGenericWarningMacro("Error: unsupported cell type.");
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
  
    vtkvmtkFEShapeFunctions::GetInterpolationDerivs(cell,pcoords,derivs);
  
    int i, j;
  
    double jacobianMatrixTr[2][3];
    for (i=0; i<3; i++)
    {
      jacobianMatrixTr[0][i] = jacobianMatrixTr[1][i] = 0.0;
    }
  
    double x[3];
    for (j=0; j<numberOfCellPoints; j++)
    {
      cell->GetPoints()->GetPoint(j,x);
      for (i=0; i<3; i++)
      {
        jacobianMatrixTr[0][i] += x[i] * derivs[j];
        jacobianMatrixTr[1][i] += x[i] * derivs[numberOfCellPoints+j];
      }
    }
    delete[] derivs;

    double jacobianMatrixSquared[2][2];
    jacobianMatrixSquared[0][0] = vtkMath::Dot(jacobianMatrixTr[0],jacobianMatrixTr[0]);
    jacobianMatrixSquared[0][1] = vtkMath::Dot(jacobianMatrixTr[0],jacobianMatrixTr[1]);
    jacobianMatrixSquared[1][0] = vtkMath::Dot(jacobianMatrixTr[1],jacobianMatrixTr[0]);
    jacobianMatrixSquared[1][1] = vtkMath::Dot(jacobianMatrixTr[1],jacobianMatrixTr[1]);

    double jacobianSquared = vtkMath::Determinant2x2(jacobianMatrixSquared[0],jacobianMatrixSquared[1]);

    if (jacobianSquared < 0.0)
    {
#ifdef VTKVMTKFESHAPEFUNCTIONS_NEGATIVE_JACOBIAN_WARNING 
      vtkGenericWarningMacro("Warning: negative determinant of squared Jacobian, taking absolute value.");
#endif
      jacobianSquared = fabs(jacobianSquared);
    }

    jacobian = sqrt(jacobianSquared);
  }
  else if (cellDimension == 3)
  {
    int numberOfCellPoints = cell->GetNumberOfPoints();
    double* derivs = new double[3*numberOfCellPoints];
  
    vtkvmtkFEShapeFunctions::GetInterpolationDerivs(cell,pcoords,derivs);
  
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

    if (jacobian < 0.0)
    {
#ifdef VTKVMTKFESHAPEFUNCTIONS_NEGATIVE_JACOBIAN_WARNING 
      vtkGenericWarningMacro("Warning: negative Jacobian, taking absolute value.");
#endif
      jacobian = fabs(jacobian);
    }
  }

  return jacobian;
}

