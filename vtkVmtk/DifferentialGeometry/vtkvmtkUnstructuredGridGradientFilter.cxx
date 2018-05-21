/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridGradientFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridGradientFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkUnstructuredGridFEGradientAssembler.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkOpenNLLinearSystemSolver.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridGradientFilter);

vtkvmtkUnstructuredGridGradientFilter::vtkvmtkUnstructuredGridGradientFilter() 
{
  this->InputArrayName = NULL;
  this->GradientArrayName = NULL;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
  this->ComputeIndividualPartialDerivatives = 0;
}

vtkvmtkUnstructuredGridGradientFilter::~vtkvmtkUnstructuredGridGradientFilter()
{
  if (this->InputArrayName)
    {
    delete[] this->InputArrayName;
    this->InputArrayName = NULL;
    }
  if (this->GradientArrayName)
    {
    delete[] this->GradientArrayName;
    this->GradientArrayName = NULL;
    }
}

int vtkvmtkUnstructuredGridGradientFilter::RequestData(
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

  if (!this->InputArrayName)
    {
    vtkErrorMacro(<<"InputArrayName not set.");
    return 1;
    }

  if (!this->GradientArrayName)
    {
    vtkErrorMacro(<<"GradientArrayName not set.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkDataArray* inputArray = input->GetPointData()->GetArray(this->InputArrayName);

  if (!inputArray)
    {
    vtkErrorMacro("InputArray with name specified doesn not exist!");
    return 0;
    }

  int numberOfInputComponents = inputArray->GetNumberOfComponents();

  vtkDoubleArray* gradientArray = vtkDoubleArray::New();
  gradientArray->SetName(this->GradientArrayName);
  gradientArray->SetNumberOfComponents(3*numberOfInputComponents);
  gradientArray->SetNumberOfTuples(numberOfInputPoints);

  int i;
  for (i=0; i<numberOfInputComponents; i++)
    {
    vtkvmtkSparseMatrix* sparseMatrix = vtkvmtkSparseMatrix::New();
  
    vtkvmtkDoubleVector* rhsVector = vtkvmtkDoubleVector::New();
    rhsVector->SetNormTypeToLInf();
      
    vtkvmtkDoubleVector* solutionVector = vtkvmtkDoubleVector::New();
    solutionVector->SetNormTypeToLInf();
     
    if (!this->ComputeIndividualPartialDerivatives)
      {
      vtkvmtkUnstructuredGridFEGradientAssembler* assembler = vtkvmtkUnstructuredGridFEGradientAssembler::New();
      assembler->SetDataSet(input);
      assembler->SetScalarsArrayName(this->InputArrayName);
      assembler->SetScalarsComponent(i);
      assembler->SetMatrix(sparseMatrix);
      assembler->SetRHSVector(rhsVector);
      assembler->SetSolutionVector(solutionVector);
      assembler->SetQuadratureOrder(this->QuadratureOrder);
      assembler->SetAssemblyModeToGradient();
      cout<<"Assembling system"<<endl;
      assembler->Build();
      cout<<"Done"<<endl;
        
      vtkvmtkLinearSystem* linearSystem = vtkvmtkLinearSystem::New();
      linearSystem->SetA(sparseMatrix);
      linearSystem->SetB(rhsVector);
      linearSystem->SetX(solutionVector);
    
      vtkvmtkOpenNLLinearSystemSolver* solver = vtkvmtkOpenNLLinearSystemSolver::New();
      solver->SetLinearSystem(linearSystem);
      solver->SetConvergenceTolerance(this->ConvergenceTolerance);
      solver->SetMaximumNumberOfIterations(numberOfInputPoints);
      solver->SetSolverTypeToCG();
      solver->SetPreconditionerTypeToJacobi();
      cout<<"Solving system"<<endl;
      solver->Solve();
      cout<<"Done"<<endl;

      solutionVector->CopyVariableIntoArrayComponent(gradientArray,0,3*i+0);
      solutionVector->CopyVariableIntoArrayComponent(gradientArray,1,3*i+1);
      solutionVector->CopyVariableIntoArrayComponent(gradientArray,2,3*i+2);
      
      assembler->Delete(); 
      linearSystem->Delete();
      solver->Delete();
      }
    else
      {
      int j;
      for (j=0; j<3; j++)
        {
        vtkvmtkUnstructuredGridFEGradientAssembler* assembler = vtkvmtkUnstructuredGridFEGradientAssembler::New();
        assembler->SetDataSet(input);
        assembler->SetScalarsArrayName(this->InputArrayName);
        assembler->SetScalarsComponent(i);
        assembler->SetMatrix(sparseMatrix);
        assembler->SetRHSVector(rhsVector);
        assembler->SetSolutionVector(solutionVector);
        assembler->SetQuadratureOrder(this->QuadratureOrder);
        assembler->SetAssemblyModeToPartialDerivative();
        assembler->SetDirection(j);
        cout<<"Assembling system"<<endl;
        assembler->Build();
        cout<<"Done"<<endl;
          
        vtkvmtkLinearSystem* linearSystem = vtkvmtkLinearSystem::New();
        linearSystem->SetA(sparseMatrix);
        linearSystem->SetB(rhsVector);
        linearSystem->SetX(solutionVector);
      
        vtkvmtkOpenNLLinearSystemSolver* solver = vtkvmtkOpenNLLinearSystemSolver::New();
        solver->SetLinearSystem(linearSystem);
        solver->SetConvergenceTolerance(this->ConvergenceTolerance);
        solver->SetMaximumNumberOfIterations(numberOfInputPoints);
        solver->SetSolverTypeToCG();
        solver->SetPreconditionerTypeToJacobi();
        cout<<"Solving system"<<endl;
        solver->Solve();
        cout<<"Done"<<endl;
      
        solutionVector->CopyIntoArrayComponent(gradientArray,3*i+j);
       
        assembler->Delete();
        linearSystem->Delete();
        solver->Delete();
        }
      }

    solutionVector->Delete();
    rhsVector->Delete();
    sparseMatrix->Delete();
    }

  output->DeepCopy(input);  
  output->GetPointData()->AddArray(gradientArray);
    
  gradientArray->Delete();
 
  return 1;
}
