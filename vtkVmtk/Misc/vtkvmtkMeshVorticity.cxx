/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshVorticity.cxx,v $
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

#include "vtkvmtkMeshVorticity.h"

#include "vtkvmtkUnstructuredGridGradientFilter.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkMeshVorticity);

vtkvmtkMeshVorticity::vtkvmtkMeshVorticity()
{
  this->VelocityArrayName = NULL;
  this->VorticityArrayName = NULL;
  this->ComputeIndividualPartialDerivatives = 0;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
}

vtkvmtkMeshVorticity::~vtkvmtkMeshVorticity()
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
}

int vtkvmtkMeshVorticity::RequestData(
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
  
  vtkDoubleArray* vorticityArray = vtkDoubleArray::New();
  if (this->VorticityArrayName)
    {
    vorticityArray->SetName(this->VorticityArrayName);
    }
  else
    {
    vorticityArray->SetName("Vorticity");
    }
  vorticityArray->SetNumberOfComponents(3);
  vorticityArray->SetNumberOfTuples(numberOfPoints);

  double velocityGradient[9];
  double vorticity[3];
  
  int i;
  for (i=0; i<numberOfPoints; i++)
    {
    velocityGradientArray->GetTuple(i,velocityGradient);
    vorticity[0] = velocityGradient[7] - velocityGradient[5];
    vorticity[1] = velocityGradient[2] - velocityGradient[6];
    vorticity[2] = velocityGradient[3] - velocityGradient[1];
    vorticityArray->SetTuple(i,vorticity);
    }

  output->DeepCopy(input);
  output->GetPointData()->AddArray(vorticityArray);
  
  vorticityArray->Delete();
  
  return 1;
}

void vtkvmtkMeshVorticity::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
