/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkUnstructuredGridVorticityFilter.cxx,v $
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

#include "vtkvmtkUnstructuredGridVorticityFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkUnstructuredGridFEVorticityAssembler.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkLinearSystem.h"
#include "vtkvmtkOpenNLLinearSystemSolver.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkUnstructuredGridVorticityFilter);

vtkvmtkUnstructuredGridVorticityFilter::vtkvmtkUnstructuredGridVorticityFilter() 
{
  this->VelocityArrayName = NULL;
  this->VorticityArrayName = NULL;
  this->HelicityFactorArrayName = NULL;
  this->ComputeHelicityFactor = 0;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
}

vtkvmtkUnstructuredGridVorticityFilter::~vtkvmtkUnstructuredGridVorticityFilter()
{
  if (this->VelocityArrayName)
    {
    delete[] this->VelocityArrayName;
    this->VelocityArrayName = NULL;
    }
  if (this->VorticityArrayName)
    {
    delete[] this->VorticityArrayName;
    this->VorticityArrayName = NULL;
    }
  if (this->HelicityFactorArrayName)
    {
    delete[] this->HelicityFactorArrayName;
    this->HelicityFactorArrayName = NULL;
    }
}

int vtkvmtkUnstructuredGridVorticityFilter::RequestData(
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

  if (!this->VelocityArrayName)
    {
    vtkErrorMacro(<<"VelocityArrayName not set.");
    return 1;
    }

  if (!this->VorticityArrayName)
    {
    vtkErrorMacro(<<"VorticityArrayName not set.");
    return 1;
    }

  if (this->ComputeHelicityFactor)
    {
    if (!this->HelicityFactorArrayName)
      {
      vtkErrorMacro(<<"HelicityFactorArrayName not set.");
      return 1;
      }
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkDataArray* velocityArray = input->GetPointData()->GetArray(this->VelocityArrayName);

  if (!velocityArray)
    {
    vtkErrorMacro("VelocityArray with name specified doesn not exist!");
    return 0;
    }

  int numberOfInputComponents = velocityArray->GetNumberOfComponents();

  if (numberOfInputComponents != 3)
    {
    vtkErrorMacro("VelocityArray must have 3 components!");
    return 0;
    }

  vtkDoubleArray* vorticityArray = vtkDoubleArray::New();
  vorticityArray->SetName(this->VorticityArrayName);
  vorticityArray->SetNumberOfComponents(numberOfInputComponents);
  vorticityArray->SetNumberOfTuples(numberOfInputPoints);

  vtkvmtkSparseMatrix* sparseMatrix = vtkvmtkSparseMatrix::New();
  
  vtkvmtkDoubleVector* rhsVector = vtkvmtkDoubleVector::New();
  rhsVector->SetNormTypeToLInf();
    
  vtkvmtkDoubleVector* solutionVector = vtkvmtkDoubleVector::New();
  solutionVector->SetNormTypeToLInf();
  
  int i;
  for (i=0; i<3; i++)
    {
    vtkvmtkUnstructuredGridFEVorticityAssembler* assembler = vtkvmtkUnstructuredGridFEVorticityAssembler::New();
    assembler->SetDataSet(input);
    assembler->SetVelocityArrayName(this->VelocityArrayName);
    assembler->SetMatrix(sparseMatrix);
    assembler->SetRHSVector(rhsVector);
    assembler->SetSolutionVector(solutionVector);
    assembler->SetQuadratureOrder(this->QuadratureOrder);
    assembler->SetDirection(i);
    assembler->Build();
    
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
    solver->Solve();
    
//    solutionVector->CopyVariableIntoArrayComponent(vorticityArray,0,0);
//    solutionVector->CopyVariableIntoArrayComponent(vorticityArray,1,1);
//    solutionVector->CopyVariableIntoArrayComponent(vorticityArray,2,2);
    solutionVector->CopyIntoArrayComponent(vorticityArray,i);
 
    assembler->Delete(); 
    linearSystem->Delete();
    solver->Delete();
    }

  sparseMatrix->Delete();
  rhsVector->Delete();

  solutionVector->Delete();

  output->DeepCopy(input);  
  output->GetPointData()->AddArray(vorticityArray);

  if (this->ComputeHelicityFactor)
    {
    vtkDoubleArray* helicityFactorArray = vtkDoubleArray::New();
    helicityFactorArray->SetName(this->HelicityFactorArrayName);
    helicityFactorArray->SetNumberOfTuples(numberOfInputPoints);

    double velocity[3], vorticity[3], velocityMagnitude, vorticityMagnitude, helicityFactor;
    int i;
    for (i=0; i<numberOfInputPoints; i++)
      {
      velocityArray->GetTuple(i,velocity);
      vorticityArray->GetTuple(i,vorticity);
      velocityMagnitude = vtkMath::Norm(velocity);
      vorticityMagnitude = vtkMath::Norm(vorticity);
      helicityFactor = 0.0;
      if (velocityMagnitude > 0.0 && vorticityMagnitude > 0.0)
        {
        helicityFactor = vtkMath::Dot(velocity,vorticity) / (velocityMagnitude * vorticityMagnitude);
        }
      helicityFactorArray->SetValue(i,helicityFactor);
      }
    output->GetPointData()->AddArray(helicityFactorArray);
    helicityFactorArray->Delete();
    }  

  vorticityArray->Delete();
 
  return 1;
}
