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
#include "vtkvmtkPolyBallLine.h"
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
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyBallModeller);

vtkvmtkPolyBallModeller::vtkvmtkPolyBallModeller()
{
  this->ReferenceImage = NULL;
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

  this->UsePolyBallLine = 0;
  this->NegateFunction = 0;
}

vtkvmtkPolyBallModeller::~vtkvmtkPolyBallModeller()
{
  if (this->ReferenceImage)
    {
    this->ReferenceImage->Delete();
    this->ReferenceImage = NULL;
    }

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

  vtkPolyData* input = vtkPolyData::SafeDownCast(this->GetInput());
 
  if (this->RadiusArrayName == NULL)
    {
    vtkErrorMacro(<<"Error: RadiusArrayName not specified!");
    return 1;
    }

  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);
  if (radiusArray == NULL)
    {
    vtkErrorMacro(<<"Error: RadiusArray with name specified does not exist!");
    return 1;
    }

  int extent[6];

  if (this->ReferenceImage)
    {
    this->ReferenceImage->GetOrigin(origin);
    this->ReferenceImage->GetSpacing(spacing);
    this->ReferenceImage->GetDimensions(this->SampleDimensions);
    this->ReferenceImage->GetExtent(extent);
    }
  else
    {
    double maxRadius = radiusArray->GetRange()[1]; 
 
    if ((this->ModelBounds[0] >= this->ModelBounds[1]) || (this->ModelBounds[2] >= this->ModelBounds[3]) || (this->ModelBounds[4] >= this->ModelBounds[5]))
      {
      double* bounds = input->GetBounds();
      double maxDist = 2.0 * maxRadius;

      int i;
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

    extent[0] = 0;
    extent[1] = this->SampleDimensions[0]-1;
    extent[2] = 0;
    extent[3] = this->SampleDimensions[1]-1;
    extent[4] = 0;
    extent[5] = this->SampleDimensions[2]-1;
    }

  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);
  outInfo->Set(vtkDataObject::SPACING(),spacing,3);

  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_DOUBLE, 1);
  //outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),0, this->SampleDimensions[0]-1, 0, this->SampleDimensions[1]-1, 0, this->SampleDimensions[2]-1);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);

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

  if (this->RadiusArrayName == NULL)
    {
    vtkErrorMacro(<<"Error: RadiusArrayName not specified!");
    return 1;
    }

  if (input->GetPointData()->GetArray(this->RadiusArrayName) == NULL)
    {
    vtkErrorMacro(<<"Error: RadiusArray with name specified does not exist!");
    return 1;
    }

  output->SetExtent(outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
#if (VTK_MAJOR_VERSION <= 5)
  output->AllocateScalars();
#else
  output->AllocateScalars(outInfo);
#endif

  vtkDoubleArray *functionArray = vtkDoubleArray::SafeDownCast(output->GetPointData()->GetScalars());
  functionArray->SetName(this->RadiusArrayName);

  int numberOfOutputPoints = output->GetNumberOfPoints();

#if 0
  vtkImplicitFunction* function;
  if (!this->UsePolyBallLine)
    {
    vtkvmtkPolyBall* polyBall = vtkvmtkPolyBall::New();
    polyBall->SetInput(input);
    polyBall->SetPolyBallRadiusArrayName(this->RadiusArrayName);
    function = polyBall;
    }
  else
    {
    vtkvmtkPolyBallLine* polyBall = vtkvmtkPolyBallLine::New();
    polyBall->SetInput(input);
    polyBall->SetPolyBallRadiusArrayName(this->RadiusArrayName);
    function = polyBall;
    }

  double point[3];
  int i;
  for (i=0; i<numberOfOutputPoints; i++)
    {
    output->GetPoint(i,point);
    double polyBallFunction = function->EvaluateFunction(point);
    functionArray->SetComponent(i,0,polyBallFunction);
    }

  function->Delete();
#endif

  if (!this->UsePolyBallLine)
    {
    functionArray->FillComponent(0,VTK_VMTK_LARGE_DOUBLE);
    vtkDoubleArray* radiusArray = vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->RadiusArrayName));
    int numberOfInputPoints = input->GetNumberOfPoints();
    int extent[6];
    double x[3], p[3], r;
    double boundingBox0[3], boundingBox1[3];
    int ijk0[3], ijk1[3], ijk[3];
    int ijkExtent[6];
    double pcoords[3];
    int inBounds0, inBounds1;
    double radiusFactor = 2.0;
    output->GetExtent(extent);
    double sphereFunctionValue, currentSphereFunctionValue;
    int n;
    int i, j, k;
    vtkIdType outputId;
    for (n=0; n<numberOfInputPoints; n++)
      {
      input->GetPoint(n,p);
      r = radiusArray->GetValue(n);
      boundingBox0[0] = p[0] - radiusFactor * r;
      boundingBox0[1] = p[1] - radiusFactor * r;
      boundingBox0[2] = p[2] - radiusFactor * r;
      boundingBox1[0] = p[0] + radiusFactor * r;
      boundingBox1[1] = p[1] + radiusFactor * r;
      boundingBox1[2] = p[2] + radiusFactor * r;
      inBounds0 = output->ComputeStructuredCoordinates(boundingBox0,ijk0,pcoords);
      inBounds1 = output->ComputeStructuredCoordinates(boundingBox1,ijk1,pcoords);
      if (!inBounds0 && !inBounds1)
        {
        continue;
        }
      ijkExtent[0] = ijk0[0] < extent[0] ? extent[0] : ijk0[0];
      ijkExtent[1] = ijk1[0] > extent[1] ? extent[1] : ijk1[0];
      ijkExtent[2] = ijk0[1] < extent[2] ? extent[2] : ijk0[1];
      ijkExtent[3] = ijk1[1] > extent[3] ? extent[3] : ijk1[1];
      ijkExtent[4] = ijk0[2] < extent[4] ? extent[4] : ijk0[2];
      ijkExtent[5] = ijk1[2] > extent[5] ? extent[5] : ijk1[2];
      for (k=ijkExtent[4]; k<=ijkExtent[5]; k++)
        {
        for (j=ijkExtent[2]; j<=ijkExtent[3]; j++)
          {
          for (i=ijkExtent[0]; i<=ijkExtent[1]; i++)
            {
            ijk[0] = i;
            ijk[1] = j;
            ijk[2] = k;
            outputId = output->ComputePointId(ijk);
            output->GetPoint(outputId,x);
            sphereFunctionValue = ((x[0] - p[0]) * (x[0] - p[0]) + (x[1] - p[1]) * (x[1] - p[1]) + (x[2] - p[2]) * (x[2] - p[2])) - r * r; 
            currentSphereFunctionValue = functionArray->GetComponent(outputId,0);
            if (sphereFunctionValue < currentSphereFunctionValue)
              {
              functionArray->SetComponent(outputId,0,sphereFunctionValue);
              }
            }
          }
        }
      }
    }
  else
    {
    vtkvmtkPolyBallLine* polyBallLine = vtkvmtkPolyBallLine::New();
    polyBallLine->SetInput(input);
    polyBallLine->SetPolyBallRadiusArrayName(this->RadiusArrayName);
    double point[3];
    int i;
    for (i=0; i<numberOfOutputPoints; i++)
      {
      output->GetPoint(i,point);
      double polyBallLineValue = polyBallLine->EvaluateFunction(point);
      functionArray->SetComponent(i,0,polyBallLineValue);
      }
    }

  if (this->NegateFunction)
    {
    int i;
    for (i=0; i<numberOfOutputPoints; i++)
      {
      double functionValue = functionArray->GetComponent(i,0);
      functionValue *= -1.0;
      functionArray->SetComponent(i,0,functionValue);
      }
    }

  return 1;
}

void vtkvmtkPolyBallModeller::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Sample Dimensions: (" << this->SampleDimensions[0] << ", "
     << this->SampleDimensions[1] << ", "
     << this->SampleDimensions[2] << ")\n";

  os << indent << "ModelBounds: \n";
  os << indent << "  Xmin,Xmax: (" << this->ModelBounds[0] << ", " << this->ModelBounds[1] << ")\n";
  os << indent << "  Ymin,Ymax: (" << this->ModelBounds[2] << ", " << this->ModelBounds[3] << ")\n";
  os << indent << "  Zmin,Zmax: (" << this->ModelBounds[4] << ", " << this->ModelBounds[5] << ")\n";

}
