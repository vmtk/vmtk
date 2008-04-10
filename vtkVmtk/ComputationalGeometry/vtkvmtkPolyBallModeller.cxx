/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBallModeller.cxx,v $
Language:  C++
Date:      $Date: 2006/01/09 17:01:50 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkvmtkPolyBallModeller.h"

#include "vtkvmtkPolyBall.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"

vtkCxxRevisionMacro(vtkvmtkPolyBallModeller, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkPolyBallModeller);

vtkvmtkPolyBallModeller::vtkvmtkPolyBallModeller()
{
  this->MaximumDistance = 1.0;

  this->ModelBounds[0] = 0.0;
  this->ModelBounds[1] = 0.0;
  this->ModelBounds[2] = 0.0;
  this->ModelBounds[3] = 0.0;
  this->ModelBounds[4] = 0.0;
  this->ModelBounds[5] = 0.0;

  this->SampleDimensions[0] = 50;
  this->SampleDimensions[1] = 50;
  this->SampleDimensions[2] = 50;

  this->RadiusArrayName = NULL;
}

vtkvmtkPolyBallModeller::~vtkvmtkPolyBallModeller()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }
}

int vtkvmtkPolyBallModeller::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector *outputVector)
{
  double spacing[3], origin[3];

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_DOUBLE, 1);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),0, this->SampleDimensions[0]-1, 0, this->SampleDimensions[1]-1, 0, this->SampleDimensions[2]-1);

  if ((this->ModelBounds[0] >= this->ModelBounds[1]) || (this->ModelBounds[2] >= this->ModelBounds[3]) || (this->ModelBounds[4] >= this->ModelBounds[5]))
    {
    double* bounds = vtkPolyData::SafeDownCast(this->GetInput())->GetBounds();
    double maxDist = 0.0;
    int i;
    for (i=0; i<3; i++)
      {
      if ((bounds[2*i+1] - bounds[2*i]) > maxDist)
        {
        maxDist = bounds[2*i+1] - bounds[2*i];
        }
      }
    maxDist *= 0.5;

    for (i=0; i<3; i++)
      {
      this->ModelBounds[2*i] = bounds[2*i] - maxDist;
      this->ModelBounds[2*i+1] = bounds[2*i+1] + maxDist;
      }
    }

  int i;
  for (i=0; i<3; i++)
    {
    origin[i] = this->ModelBounds[2*i];
    if (this->SampleDimensions[i] <= 1)
      {
      spacing[i] = 1.0;
      }
    else
      {
      spacing[i] = (this->ModelBounds[2*i+1] - this->ModelBounds[2*i]) / (this->SampleDimensions[i] - 1);
      }
    }

  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);

  return 1;
}

int vtkvmtkPolyBallModeller::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

int vtkvmtkPolyBallModeller::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData *output = vtkImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkDoubleArray* functionArray = vtkDoubleArray::New();
  functionArray->SetName(this->RadiusArrayName);
  functionArray->SetNumberOfTuples(output->GetNumberOfPoints());

  output->GetPointData()->AddArray(functionArray);
  output->GetPointData()->SetActiveScalars(this->RadiusArrayName);

  functionArray->FillComponent(0,VTK_VMTK_LARGE_DOUBLE);

  int numberOfOutputPoints = output->GetNumberOfPoints();

  vtkvmtkPolyBall* polyBall = vtkvmtkPolyBall::New();
  polyBall->SetInput(input);
  polyBall->SetPolyBallRadiusArrayName(this->RadiusArrayName);

  double point[3];
  int i;
  for (i=0; i<numberOfOutputPoints; i++)
    {
    output->GetPoint(i,point);
    double polyBallFunction = polyBall->EvaluateFunction(point);
    functionArray->SetComponent(i,0,polyBallFunction);
    }

  polyBall->Delete();
  functionArray->Delete();

  return 1;
}

void vtkvmtkPolyBallModeller::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Maximum Distance: " << this->MaximumDistance << "\n";

  os << indent << "Sample Dimensions: (" << this->SampleDimensions[0] << ", "
     << this->SampleDimensions[1] << ", "
     << this->SampleDimensions[2] << ")\n";

  os << indent << "ModelBounds: \n";
  os << indent << "  Xmin,Xmax: (" << this->ModelBounds[0] << ", " << this->ModelBounds[1] << ")\n";
  os << indent << "  Ymin,Ymax: (" << this->ModelBounds[2] << ", " << this->ModelBounds[3] << ")\n";
  os << indent << "  Zmin,Zmax: (" << this->ModelBounds[4] << ", " << this->ModelBounds[5] << ")\n";

}
