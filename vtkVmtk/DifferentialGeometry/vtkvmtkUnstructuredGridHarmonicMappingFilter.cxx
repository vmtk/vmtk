/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridHarmonicMappingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridHarmonicMappingFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkUnstructuredGridFELaplaceAssembler.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkOpenNLLinearSystemSolver.h"

#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridHarmonicMappingFilter);

vtkvmtkUnstructuredGridHarmonicMappingFilter::vtkvmtkUnstructuredGridHarmonicMappingFilter() 
{
  this->BoundaryPointIds = NULL;
  this->BoundaryValues = NULL;
  
  this->HarmonicMappingArrayName = NULL;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
}

vtkvmtkUnstructuredGridHarmonicMappingFilter::~vtkvmtkUnstructuredGridHarmonicMappingFilter()
{
  if (this->BoundaryPointIds)
    {
    this->BoundaryPointIds->Delete();
    this->BoundaryPointIds = NULL;
    }

  if (this->BoundaryValues)
    {
    this->BoundaryValues->Delete();
    this->BoundaryValues = NULL;
    }

  if (this->HarmonicMappingArrayName)
    {
    delete[] this->HarmonicMappingArrayName;
    this->HarmonicMappingArrayName = NULL;
    }
}

int vtkvmtkUnstructuredGridHarmonicMappingFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->BoundaryPointIds)
    {
    vtkErrorMacro(<<"BoundaryPointIds not set.");
    return 1;
    }

  if (!this->BoundaryValues)
    {
    vtkErrorMacro(<<"BoundaryValues not set.");
    return 1;
    }

  if (!this->HarmonicMappingArrayName)
    {
    vtkErrorMacro(<<"HarmonicMappingArrayName not set.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkvmtkSparseMatrix* sparseMatrix = vtkvmtkSparseMatrix::New();

  vtkvmtkDoubleVector* rhsVector = vtkvmtkDoubleVector::New();
  rhsVector->SetNormTypeToLInf();
    
  vtkvmtkDoubleVector* solutionVector = vtkvmtkDoubleVector::New();
  solutionVector->SetNormTypeToLInf();
   
  vtkvmtkUnstructuredGridFELaplaceAssembler* assembler = vtkvmtkUnstructuredGridFELaplaceAssembler::New();
  assembler->SetDataSet(input);
  assembler->SetMatrix(sparseMatrix);
  assembler->SetRHSVector(rhsVector);
  assembler->SetSolutionVector(solutionVector);
  assembler->SetQuadratureOrder(this->QuadratureOrder);
  assembler->Build();
 
  vtkvmtkLinearSystem* linearSystem = vtkvmtkLinearSystem::New();
  linearSystem->SetA(sparseMatrix);
  linearSystem->SetB(rhsVector);
  linearSystem->SetX(solutionVector);

  vtkvmtkDirichletBoundaryConditions* dirichetBoundaryConditions = vtkvmtkDirichletBoundaryConditions::New();
  dirichetBoundaryConditions->SetLinearSystem(linearSystem);
  dirichetBoundaryConditions->SetBoundaryNodes(this->BoundaryPointIds);
  dirichetBoundaryConditions->SetBoundaryValues(this->BoundaryValues);
  dirichetBoundaryConditions->Apply();

  vtkvmtkOpenNLLinearSystemSolver* solver = vtkvmtkOpenNLLinearSystemSolver::New();
  solver->SetLinearSystem(linearSystem);
  solver->SetConvergenceTolerance(this->ConvergenceTolerance);
  solver->SetMaximumNumberOfIterations(numberOfInputPoints);
  solver->SetSolverTypeToCG();
  solver->SetPreconditionerTypeToNone();
  solver->Solve();

  vtkDoubleArray* harmonicMappingArray = vtkDoubleArray::New();
  harmonicMappingArray->SetName(this->HarmonicMappingArrayName);
  harmonicMappingArray->SetNumberOfComponents(1);
  harmonicMappingArray->SetNumberOfTuples(numberOfInputPoints);

  solutionVector->CopyIntoArrayComponent(harmonicMappingArray,0);
 
  output->DeepCopy(input);

  output->GetPointData()->AddArray(harmonicMappingArray);

  assembler->Delete();
  solver->Delete();
  harmonicMappingArray->Delete();
  sparseMatrix->Delete();
  rhsVector->Delete();
  solutionVector->Delete();
  linearSystem->Delete();
  dirichetBoundaryConditions->Delete();

  return 1;
}
