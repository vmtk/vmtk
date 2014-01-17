/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataMeanCurvature.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataMeanCurvature.h"
#include "vtkvmtkStencils.h"
#include "vtkvmtkStencil.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyDataMeanCurvature);

vtkvmtkPolyDataMeanCurvature::vtkvmtkPolyDataMeanCurvature()
{
  this->StencilType = VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL;
  this->MeanCurvatureScalarsArrayName = NULL;
  this->MeanCurvatureNormalsArrayName = NULL;

  this->ComputeMeanCurvatureScalars = 0;
  this->ComputeMeanCurvatureNormals = 0;
}

vtkvmtkPolyDataMeanCurvature::~vtkvmtkPolyDataMeanCurvature()
{
  this->ReleaseStencils();
}

void vtkvmtkPolyDataMeanCurvature::ReleaseStencils()
{
  if (this->Stencils)
    {
    this->Stencils->UnRegister(this);
    this->Stencils = NULL;
    }
}

void vtkvmtkPolyDataMeanCurvature::ComputePointMeanCurvatureVector(vtkPolyData* input, vtkIdType pointId, double* meanCurvatureVector)
{
  double point[3], stencilPoint[3];
  vtkIdType j;
  vtkvmtkStencil* stencil;

  meanCurvatureVector[0] = 0.0;
  meanCurvatureVector[1] = 0.0;
  meanCurvatureVector[2] = 0.0;

  input->GetPoint(pointId,point);

  stencil = this->Stencils->GetStencil(pointId);

  for (j=0; j<stencil->GetNumberOfPoints(); j++)
    {
    input->GetPoint(stencil->GetPointId(j),stencilPoint);

    meanCurvatureVector[0] += stencil->GetWeight(j) * stencilPoint[0];
    meanCurvatureVector[1] += stencil->GetWeight(j) * stencilPoint[1];
    meanCurvatureVector[2] += stencil->GetWeight(j) * stencilPoint[2];
    }

  meanCurvatureVector[0] += stencil->GetCenterWeight() * point[0];
  meanCurvatureVector[1] += stencil->GetCenterWeight() * point[1];
  meanCurvatureVector[2] += stencil->GetCenterWeight() * point[2];

  meanCurvatureVector[0] *= 0.5;
  meanCurvatureVector[1] *= 0.5;
  meanCurvatureVector[2] *= 0.5;

}

int vtkvmtkPolyDataMeanCurvature::RequestData(
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

  vtkIdType pointId, numberOfPoints;
  double meanCurvatureVector[3], meanCurvature;
  vtkDoubleArray* meanCurvatureScalarsArray = NULL;
  vtkDoubleArray* meanCurvatureNormalsArray = NULL;

  if ((this->ComputeMeanCurvatureScalars) && (!this->MeanCurvatureScalarsArrayName))
    {
    vtkErrorMacro(<< "No mean curvature scalars array name specified!");
    return 1;
    }

  if ((this->ComputeMeanCurvatureNormals) && (!this->MeanCurvatureNormalsArrayName))
    {
    vtkErrorMacro(<< "No mean curvature vectors array name specified!");
    return 1;
    }

  this->Stencils = vtkvmtkStencils::New();
  this->Stencils->SetItemType(this->StencilType);
  this->Stencils->SetDataSet(input);
  this->Stencils->Build();

  if (this->ComputeMeanCurvatureScalars)
    {
    meanCurvatureScalarsArray = vtkDoubleArray::New();
    meanCurvatureScalarsArray->SetName(this->MeanCurvatureScalarsArrayName);
    meanCurvatureScalarsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    meanCurvatureScalarsArray->FillComponent(0,0.0);
    meanCurvatureScalarsArray->Register(this);
    meanCurvatureScalarsArray->Delete();
    }

  if (this->ComputeMeanCurvatureNormals)
    {
    meanCurvatureNormalsArray = vtkDoubleArray::New();
    meanCurvatureNormalsArray->SetName(this->MeanCurvatureNormalsArrayName);
    meanCurvatureNormalsArray->SetNumberOfComponents(3);
    meanCurvatureNormalsArray->SetNumberOfTuples(input->GetNumberOfPoints());
    meanCurvatureNormalsArray->FillComponent(0,0.0);
    meanCurvatureNormalsArray->FillComponent(1,0.0);
    meanCurvatureNormalsArray->FillComponent(2,0.0);
    meanCurvatureNormalsArray->Register(this);
    meanCurvatureNormalsArray->Delete();
    }

  numberOfPoints = input->GetNumberOfPoints();

  for (pointId=0; pointId<numberOfPoints; pointId++)
    {
    this->ComputePointMeanCurvatureVector(input,pointId,meanCurvatureVector);

    if (this->ComputeMeanCurvatureScalars)
      {
      meanCurvature = vtkMath::Norm(meanCurvatureVector);
      meanCurvatureScalarsArray->SetComponent(pointId,0,meanCurvature);
      }

    if (this->ComputeMeanCurvatureNormals)
      {
      vtkMath::Normalize(meanCurvatureVector);
      meanCurvatureNormalsArray->SetTuple(pointId,meanCurvatureVector);
      }
    }

  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  if (this->ComputeMeanCurvatureScalars)
    {
    output->GetPointData()->AddArray(meanCurvatureScalarsArray);
    output->GetPointData()->SetActiveScalars(this->MeanCurvatureScalarsArrayName);
    }

  if (this->ComputeMeanCurvatureNormals)
    {
    output->GetPointData()->AddArray(meanCurvatureNormalsArray);
    output->GetPointData()->SetActiveNormals(this->MeanCurvatureNormalsArrayName);
    }
 
  return 1;
}

void vtkvmtkPolyDataMeanCurvature::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

