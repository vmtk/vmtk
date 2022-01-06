/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshLambda2.cxx,v $
Language:  C++
Date:      $Date: 2006/07/27 08:28:36 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkMeshLambda2.h"

#include "vtkvmtkUnstructuredGridGradientFilter.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkMeshLambda2);

vtkvmtkMeshLambda2::vtkvmtkMeshLambda2()
{
  this->VelocityArrayName = NULL;
  this->Lambda2ArrayName = NULL;
  this->ComputeIndividualPartialDerivatives = 0;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
  this->ForceBoundaryToNegative = 0;
}

vtkvmtkMeshLambda2::~vtkvmtkMeshLambda2()
{
  if (this->VelocityArrayName)
    {
    delete[] this->VelocityArrayName;
    this->VelocityArrayName = NULL;
    }
  if (this->Lambda2ArrayName)
    {
    delete[] this->Lambda2ArrayName;
    this->Lambda2ArrayName = NULL;
    }
}

int vtkvmtkMeshLambda2::RequestData(
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

  if (this->VelocityArrayName == NULL)
    {
    vtkErrorMacro("VelocityArrayName not specified");
    return 1;
    }

  vtkDataArray* velocityArray = input->GetPointData()->GetArray(this->VelocityArrayName); 
  
  if (velocityArray == NULL)
    {
    vtkErrorMacro("VelocityArray with name specified does not exist");
    return 1;
    }

  char gradientArrayName[] = "VelocityGradient";

  vtkvmtkUnstructuredGridGradientFilter* gradientFilter = vtkvmtkUnstructuredGridGradientFilter::New();
  gradientFilter->SetInputData(input);
  gradientFilter->SetInputArrayName(this->VelocityArrayName);
  gradientFilter->SetGradientArrayName(gradientArrayName);
  gradientFilter->SetQuadratureOrder(this->QuadratureOrder);
  gradientFilter->SetConvergenceTolerance(this->ConvergenceTolerance);
  gradientFilter->SetComputeIndividualPartialDerivatives(this->ComputeIndividualPartialDerivatives);
  gradientFilter->Update();

  vtkDataArray* velocityGradientArray = gradientFilter->GetOutput()->GetPointData()->GetArray(gradientArrayName);

  int numberOfPoints = input->GetNumberOfPoints();
  
  vtkDoubleArray* lambda2Array = vtkDoubleArray::New();
  if (this->Lambda2ArrayName)
    {
    lambda2Array->SetName(this->Lambda2ArrayName);
    }
  else
    {
    lambda2Array->SetName("Lambda2");
    }
  lambda2Array->SetNumberOfComponents(1);
  lambda2Array->SetNumberOfTuples(numberOfPoints);

  double velocityGradient[9];
  double symmetricVelocityGradient[3][3];
  double antiSymmetricVelocityGradient[3][3];
  double A[3][3];
  
  int i, j, k, l;
  for (i=0; i<numberOfPoints; i++)
    {
    velocityGradientArray->GetTuple(i,velocityGradient);
    for (j=0; j<3; j++)
      {
      for (k=0; k<3; k++)
        {
        int index0 = k + j*3;
        int index1 = j + k*3;
        symmetricVelocityGradient[j][k] = 0.5 * (velocityGradient[index0] + velocityGradient[index1]);
        antiSymmetricVelocityGradient[j][k] = 0.5 * (velocityGradient[index0] - velocityGradient[index1]);
        }
      } 

    for (j=0; j<3; j++)
      {
      for (k=0; k<3; k++)
        {
        A[j][k] = 0.0;
        for (l=0; l<3; l++)
          {
          A[j][k] += symmetricVelocityGradient[j][l]*symmetricVelocityGradient[l][k] + 
                     antiSymmetricVelocityGradient[j][l]*antiSymmetricVelocityGradient[l][k];
          }
        }
      } 

    double eigenVectors[3][3];
    double eigenValues[3];
    vtkMath::Diagonalize3x3(A,eigenValues,eigenVectors);

    bool done = false;
    while (!done)
      {
      done = true;
      for (j=0; j<2; j++)
        {
        if (eigenValues[j] > eigenValues[j+1])
          {
          done = false;
          double tmp = eigenValues[j+1];
          eigenValues[j+1] = eigenValues[j];
          eigenValues[j] = tmp;
          }
        }
      }
    double lambda2 = eigenValues[1];

    lambda2Array->SetTuple1(i,lambda2);
    }

  output->DeepCopy(input);
  output->GetPointData()->AddArray(lambda2Array);
  
  lambda2Array->Delete();
  
  return 1;
}

void vtkvmtkMeshLambda2::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
