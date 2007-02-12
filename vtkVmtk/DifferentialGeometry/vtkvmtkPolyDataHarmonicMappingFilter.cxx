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
#include "vtkvmtkLASPACKLinearSystemSolver.h"

#include "vtkvmtkDirichletBoundaryConditions.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkPolyDataHarmonicMappingFilter, "$Revision: 1.7 $");
vtkStandardNewMacro(vtkvmtkPolyDataHarmonicMappingFilter);

vtkvmtkPolyDataHarmonicMappingFilter::vtkvmtkPolyDataHarmonicMappingFilter() 
{
  this->BoundaryPointIds = NULL;
  this->BoundaryValues = NULL;
  
  this->HarmonicMappingArrayName = NULL;
  this->ConvergenceTolerance = 1E-6;
  this->SetAssemblyModeToStencils();
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

  if (this->AssemblyMode == VTK_VMTK_ASSEMBLY_STENCILS)
    {
    vtkvmtkStencils* stencils = vtkvmtkStencils::New();
    stencils->SetStencilTypeToFELaplaceBeltramiStencil();
    stencils->WeightScalingOff();
    stencils->NegateWeightsOn();
    stencils->SetDataSet(input);
    stencils->Build();

    sparseMatrix->CopyRowsFromStencils(stencils);

    stencils->Delete();
    }
  else if (this->AssemblyMode == VTK_VMTK_ASSEMBLY_FINITEELEMENTS)
    {
    vtkvmtkPolyDataFELaplaceAssembler* assembler = vtkvmtkPolyDataFELaplaceAssembler::New();
    assembler->SetDataSet(input);
    assembler->SetMatrix(sparseMatrix);
    assembler->Build();

    assembler->Delete();
    }
    
  vtkvmtkDoubleVector* rhsVector = vtkvmtkDoubleVector::New();
  rhsVector->SetNormTypeToLInf();
  rhsVector->Allocate(numberOfInputPoints);
  rhsVector->Fill(0.0);
    
  vtkvmtkDoubleVector* solutionVector = vtkvmtkDoubleVector::New();
  solutionVector->SetNormTypeToLInf();
  solutionVector->Allocate(numberOfInputPoints);
  solutionVector->Fill(0.0);
    
  vtkvmtkLinearSystem* linearSystem = vtkvmtkLinearSystem::New();
  linearSystem->SetA(sparseMatrix);
  linearSystem->SetB(rhsVector);
  linearSystem->SetX(solutionVector);

  vtkvmtkDirichletBoundaryConditions* dirichetBoundaryConditions = vtkvmtkDirichletBoundaryConditions::New();
  dirichetBoundaryConditions->SetLinearSystem(linearSystem);
  dirichetBoundaryConditions->SetBoundaryNodes(this->BoundaryPointIds);
  dirichetBoundaryConditions->SetBoundaryValues(this->BoundaryValues);
  dirichetBoundaryConditions->Apply();

  vtkvmtkLASPACKLinearSystemSolver* solver = vtkvmtkLASPACKLinearSystemSolver::New();
  solver->SetLinearSystem(linearSystem);
  solver->SetConvergenceTolerance(this->ConvergenceTolerance);
  solver->SetMaximumNumberOfIterations(numberOfInputPoints);
  solver->SetSolverType(vtkvmtkLASPACKLinearSystemSolver::VTK_VMTK_LASPACK_SOLVER_CG);
  solver->SetPreconditionerType(vtkvmtkLASPACKLinearSystemSolver::VTK_VMTK_LASPACK_PRECONDITIONER_NONE);
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
