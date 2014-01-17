/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataHarmonicMappingFilter.cxx,v $
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

#include "vtkvmtkPolyDataHarmonicMappingFilter.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkStencils.h"
#include "vtkvmtkPolyDataFELaplaceAssembler.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkSparseMatrixRow.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkOpenNLLinearSystemSolver.h"

#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataHarmonicMappingFilter);

vtkvmtkPolyDataHarmonicMappingFilter::vtkvmtkPolyDataHarmonicMappingFilter() 
{
  this->BoundaryPointIds = NULL;
  this->BoundaryValues = NULL;
  
  this->HarmonicMappingArrayName = NULL;
  this->ConvergenceTolerance = 1E-6;
  this->SetAssemblyModeToFiniteElements();
  this->QuadratureOrder = 1;
}

vtkvmtkPolyDataHarmonicMappingFilter::~vtkvmtkPolyDataHarmonicMappingFilter()
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

int vtkvmtkPolyDataHarmonicMappingFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
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
   
  if (this->AssemblyMode == VTK_VMTK_ASSEMBLY_STENCILS)
    {
    vtkvmtkStencils* stencils = vtkvmtkStencils::New();
    stencils->SetStencilTypeToFELaplaceBeltramiStencil();
    stencils->WeightScalingOff();
    stencils->NegateWeightsOn();
    stencils->SetDataSet(input);
    stencils->Build();

    sparseMatrix->CopyRowsFromStencils(stencils);
    rhsVector->Allocate(numberOfInputPoints);
    rhsVector->Fill(0.0);
    solutionVector->Allocate(numberOfInputPoints);
    solutionVector->Fill(0.0);

    stencils->Delete();
    }
  else if (this->AssemblyMode == VTK_VMTK_ASSEMBLY_FINITEELEMENTS)
    {
    vtkvmtkPolyDataFELaplaceAssembler* assembler = vtkvmtkPolyDataFELaplaceAssembler::New();
    assembler->SetDataSet(input);
    assembler->SetMatrix(sparseMatrix);
    assembler->SetRHSVector(rhsVector);
    assembler->SetSolutionVector(solutionVector);
    assembler->SetQuadratureOrder(this->QuadratureOrder);
    assembler->Build();
    assembler->Delete();
    }
    
  vtkvmtkLinearSystem* linearSystem = vtkvmtkLinearSystem::New();
  linearSystem->SetA(sparseMatrix);
  linearSystem->SetB(rhsVector);
  linearSystem->SetX(solutionVector);

  //TODO: deal with NumberOfVariables > 1
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

  solver->Delete();
  harmonicMappingArray->Delete();
  sparseMatrix->Delete();
  rhsVector->Delete();
  solutionVector->Delete();
  linearSystem->Delete();
  dirichetBoundaryConditions->Delete();

  return 1;
}
