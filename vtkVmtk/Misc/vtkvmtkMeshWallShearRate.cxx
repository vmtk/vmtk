/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshWallShearRate.cxx,v $
Language:  C++
Date:      $Date: 2020/02/17 10:20:00 $
Version:   $Revision: 2.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkMeshWallShearRate.h"

#include "vtkvmtkUnstructuredGridGradientFilter.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkMeshWallShearRate);

vtkvmtkMeshWallShearRate::vtkvmtkMeshWallShearRate()
{
  this->VelocityArrayName = NULL;
  this->WallShearRateArrayName = NULL;
  this->ComputeIndividualPartialDerivatives = 0;
  this->ConvergenceTolerance = 1E-6;
  this->QuadratureOrder = 3;
  this->UseFullStrainRateTensor = 0;
}

vtkvmtkMeshWallShearRate::~vtkvmtkMeshWallShearRate()
{
  if (this->VelocityArrayName)
    {
    delete[] this->VelocityArrayName;
    this->VelocityArrayName = NULL;
    }
  if (this->WallShearRateArrayName)
    {
    delete[] this->WallShearRateArrayName;
    this->WallShearRateArrayName = NULL;
    }
}

int vtkvmtkMeshWallShearRate::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

int vtkvmtkMeshWallShearRate::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
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
#if (VTK_MAJOR_VERSION <= 5)
  gradientFilter->SetInput(input);
#else
  gradientFilter->SetInputData(input);
#endif
  gradientFilter->SetInputArrayName(this->VelocityArrayName);
  gradientFilter->SetGradientArrayName(gradientArrayName);
  gradientFilter->SetQuadratureOrder(this->QuadratureOrder);
  gradientFilter->SetConvergenceTolerance(this->ConvergenceTolerance);
  gradientFilter->SetComputeIndividualPartialDerivatives(this->ComputeIndividualPartialDerivatives);
  gradientFilter->Update();

  vtkGeometryFilter* geometryFilter = vtkGeometryFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
  geometryFilter->SetInput(gradientFilter->GetOutput());
#else
  geometryFilter->SetInputConnection(gradientFilter->GetOutputPort());
#endif
  geometryFilter->Update();

  vtkPolyDataNormals* normalsFilter = vtkPolyDataNormals::New();
#if (VTK_MAJOR_VERSION <= 5)
  normalsFilter->SetInput(geometryFilter->GetOutput());
#else
  normalsFilter->SetInputConnection(geometryFilter->GetOutputPort());
#endif
  normalsFilter->AutoOrientNormalsOn();
  normalsFilter->ConsistencyOn();
  normalsFilter->SplittingOff();
  normalsFilter->Update();

  vtkPolyData* outputSurface = normalsFilter->GetOutput();

  vtkDataArray* velocityGradientArray = outputSurface->GetPointData()->GetArray(gradientArrayName);
  vtkDataArray* normalsArray = outputSurface->GetPointData()->GetNormals();

  int numberOfPoints = outputSurface->GetNumberOfPoints();
  
  vtkDoubleArray* wallShearRateArray = vtkDoubleArray::New();
  if (this->WallShearRateArrayName)
    {
    wallShearRateArray->SetName(this->WallShearRateArrayName);
    }
  else
    {
    wallShearRateArray->SetName("WallShearRate");
    }
  wallShearRateArray->SetNumberOfComponents(3);
  wallShearRateArray->SetNumberOfTuples(numberOfPoints);

  double velocityGradient[9];
  double normal[3];
  double wallShearRate[3];
 
  int i, j, k;

  if (!this->UseFullStrainRateTensor)
    {
    for (i=0; i<numberOfPoints; i++)
      { 
      velocityGradientArray->GetTuple(i,velocityGradient);
      normalsArray->GetTuple(i,normal);
      for (j=0; j<3; j++)
        {
        wallShearRate[j] = -normal[0] * velocityGradient[3*j + 0] - normal[1] * velocityGradient[3*j + 1] - normal[2] * velocityGradient[3*j + 2];  
        }
      wallShearRateArray->SetTuple(i,wallShearRate);
      }
    }
  else
    {

    /**********************************************************************
      Calculate strain rate tensor: E = 0.5 * (\nabla u + (\nabla u)^T)
      Calculate wall shear rate vector: tau = -2 * E*n * (1-n^T*n)
      Reference: Matyka et al., http://dx.doi.org/10.1016/j.compfluid.2012.12.018
    **********************************************************************/

    double normalShear, shearVector[3], strainRateTensor[9];

    for (i=0; i<numberOfPoints; i++)
      {

      // compute strain rate tensor
      velocityGradientArray->GetTuple(i,velocityGradient);
      for (j=0; j<3; j++)
        {
	for (k=0; k<3; k++)
          {
	  strainRateTensor[3*j + k] = 0.5 * (velocityGradient[3*j + k] + velocityGradient[3*k + j]);
	  }
        }

      // compute shear rate vector and normal projection
      normalsArray->GetTuple(i,normal);
      normalShear = 0.0;
      for (j=0; j<3; j++)
        {
	shearVector[j] = 0.0;
	for (k=0; k<3; k++)
          {
	  shearVector[j] += strainRateTensor[3*j + k] * normal[k];
	  }
	normalShear += shearVector[j] * normal[j];
        }

      // compute wall shear rate
      for (j=0; j<3; j++)
        {
	// sign due to normals pointing outwards
	wallShearRate[j] = -2.0 * (shearVector[j] - normalShear*normal[j]);
        }

      wallShearRateArray->SetTuple(i,wallShearRate);
      }
    }

  output->DeepCopy(outputSurface);
  output->GetPointData()->AddArray(wallShearRateArray);
  
  wallShearRateArray->Delete();
  geometryFilter->Delete();
  normalsFilter->Delete();
  
  return 1;
}

void vtkvmtkMeshWallShearRate::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
