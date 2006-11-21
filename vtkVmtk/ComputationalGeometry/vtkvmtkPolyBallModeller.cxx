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
  this->MaximumDistance = 2.0;

  this->ModelBounds[0] = 0.0;
  this->ModelBounds[1] = 0.0;
  this->ModelBounds[2] = 0.0;
  this->ModelBounds[3] = 0.0;
  this->ModelBounds[4] = 0.0;
  this->ModelBounds[5] = 0.0;

  this->SampleDimensions[0] = 50;
  this->SampleDimensions[1] = 50;
  this->SampleDimensions[2] = 50;

}

// Compute ModelBounds from input geometry.
double vtkvmtkPolyBallModeller::ComputeModelBounds(double origin[3], double spacing[3])
{
  double *bounds, maxDist;
  int i;

  bounds = this->ModelBounds;

  for (maxDist=0.0, i=0; i<3; i++)
    {
    if ((bounds[2*i+1] - bounds[2*i]) > maxDist)
      {
      maxDist = bounds[2*i+1] - bounds[2*i];
      }
    }
  maxDist *= 0.5;

  return maxDist;

#if 0
  double *bounds, maxDist;
  int i, adjustBounds=0;

  // compute model bounds if not set previously
  if ((this->ModelBounds[0] >= this->ModelBounds[1]) || (this->ModelBounds[2] >= this->ModelBounds[3]) || (this->ModelBounds[4] >= this->ModelBounds[5]))
    {
    adjustBounds = 1;
    bounds = this->GetInput()->GetBounds();
    }
  else
    {
    bounds = this->ModelBounds;
    }

  for (maxDist=0.0, i=0; i<3; i++)
    {
    if ((bounds[2*i+1] - bounds[2*i]) > maxDist)
      {
      maxDist = bounds[2*i+1] - bounds[2*i];
      }
    }
  maxDist *= 0.5;

  // adjust bounds so model fits strictly inside (only if not set previously)
  if (adjustBounds)
    {
    for (i=0; i<3; i++)
      {
      this->ModelBounds[2*i] = bounds[2*i] - maxDist;
      this->ModelBounds[2*i+1] = bounds[2*i+1] + maxDist;
      }
    }

  // Set volume origin and data spacing
  for (i=0; i<3; i++)
    {
    origin[i] = this->ModelBounds[2*i];
    spacing[i] = (this->ModelBounds[2*i+1] - this->ModelBounds[2*i]) / (this->SampleDimensions[i] - 1);
    }

  this->GetOutput()->SetOrigin(origin);
  this->GetOutput()->SetSpacing(spacing);

  return maxDist;  
#endif
}

int vtkvmtkPolyBallModeller::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector *outputVector)
{
  int i;
  double ar[3], origin[3];

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_DOUBLE, 1);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),0, this->SampleDimensions[0]-1, 0, this->SampleDimensions[1]-1, 0, this->SampleDimensions[2]-1);

  for (i=0; i<3; i++)
    {
    origin[i] = this->ModelBounds[2*i];
    if (this->SampleDimensions[i] <= 1)
      {
      ar[i] = 1.0;
      }
    else
      {
      ar[i] = (this->ModelBounds[2*i+1] - this->ModelBounds[2*i]) / (this->SampleDimensions[i] - 1);
      }
    }

  outInfo->Set(vtkDataObject::ORIGIN(),origin,3);
  outInfo->Set(vtkDataObject::SPACING(),ar,3);

  return 1;
}

int vtkvmtkPolyBallModeller::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

void vtkvmtkPolyBallModeller::ExecuteData(vtkDataObject *outp)
{
  vtkIdType ptId, i;
  int j, k, jkFactor;
  double *px, x[3], spacing[3], origin[3], sphereFunctionValue;
  
  double maxDistance, ballRadius;
  vtkDataArray *inScalars;
  vtkIdType numPts, idx;
  int min[3], max[3];
  vtkDataSet *input = vtkDataSet::SafeDownCast(this->GetInput());
  vtkImageData *output = this->AllocateOutputData(outp);
  vtkDoubleArray *newScalars = vtkDoubleArray::SafeDownCast(output->GetPointData()->GetScalars());

  // Check input
  //
  if ((numPts=input->GetNumberOfPoints()) < 1)
    {
    vtkErrorMacro(<<"Points must be defined!");
    return;
    }

  if ((inScalars = input->GetPointData()->GetScalars()) == NULL)
    {
    vtkErrorMacro(<<"Scalars must be defined!");
    return;
    }

  // Allocate
  //
  newScalars->FillComponent(0,VTK_VMTK_LARGE_DOUBLE);

  this->ComputeModelBounds(origin,spacing);

  // Traverse all input points. 
  // Each input point affects voxels within maxDistance.
  //
  int abortExecute=0;
  for (ptId=0; (ptId<numPts) && (!abortExecute); ptId++)
    {
    if (!(ptId % 1000))
      {
      vtkDebugMacro(<<"Inserting point #" << ptId);
      this->UpdateProgress (ptId/numPts);
      if (this->GetAbortExecute())
        {
        abortExecute = 1;
        break;
        }
      }

    px = input->GetPoint(ptId);
    ballRadius = inScalars->GetComponent(ptId,0);
    maxDistance = ballRadius * this->MaximumDistance;

    for (i=0; i<3; i++) //compute dimensional bounds in data set
      {
      double amin = static_cast<double>(((px[i] - maxDistance) - origin[i]) / spacing[i]);
      double amax = static_cast<double>(((px[i] + maxDistance) - origin[i]) / spacing[i]);
      min[i] = static_cast<int>(amin);
      max[i] = static_cast<int>(amax);
      
      if (min[i] < amin)
        {
        min[i]++; // round upward to nearest integer to get min[i]
        }
      if (max[i] > amax)
        {
        max[i]--; // round downward to nearest integer to get max[i]
        }

      if (min[i] < 0)
        {
        min[i] = 0; // valid range check
        }
      if (max[i] >= this->SampleDimensions[i]) 
        {
        max[i] = this->SampleDimensions[i] - 1;
        }
      }

    for (i=0; i<3; i++) //compute dimensional bounds in data set
      {
      min[i] = static_cast<int>(static_cast<double>(((px[i] - maxDistance) - origin[i]) / spacing[i]));
      max[i] = static_cast<int>(static_cast<double>(((px[i] + maxDistance) - origin[i]) / spacing[i]));
      if (min[i] < 0)
        {
        min[i] = 0;
        }
      if (max[i] >= this->SampleDimensions[i]) 
        {
        max[i] = this->SampleDimensions[i] - 1;
        }
      }

    jkFactor = this->SampleDimensions[0]*this->SampleDimensions[1];
    for (k=min[2]; k<=max[2]; k++) 
      {
      x[2] = spacing[2] * k + origin[2];
      for (j=min[1]; j<=max[1]; j++)
        {
        x[1] = spacing[1] * j + origin[1];
        for (i=min[0]; i<=max[0]; i++) 
          {
          x[0] = spacing[0] * i + origin[0];

          idx = jkFactor*k + this->SampleDimensions[0]*j + i;

          sphereFunctionValue = ((x[0] - px[0]) * (x[0] - px[0]) + (x[1] - px[1]) * (x[1] - px[1]) + (x[2] - px[2]) * (x[2] - px[2])) - ballRadius*ballRadius;

          if (sphereFunctionValue<newScalars->GetComponent(idx,0))
            {
            newScalars->SetComponent(idx,0,sphereFunctionValue);
            }
          }
        }
      }
    }
}

// Set the i-j-k dimensions on which to sample the distance function.
void vtkvmtkPolyBallModeller::SetSampleDimensions(int i, int j, int k)
{
  int dim[3];

  dim[0] = i;
  dim[1] = j;
  dim[2] = k;

  this->SetSampleDimensions(dim);
}

// Set the i-j-k dimensions on which to sample the distance function.
void vtkvmtkPolyBallModeller::SetSampleDimensions(int dim[3])
{
  int dataDim, i;

  vtkDebugMacro(<< " setting SampleDimensions to (" << dim[0] << ","  << dim[1] << "," << dim[2] << ")");

  if ((dim[0] != this->SampleDimensions[0]) || (dim[1] != this->SampleDimensions[1]) || (dim[2] != this->SampleDimensions[2]))
    {
    if ((dim[0]<1) || (dim[1]<1) || (dim[2]<1))
      {
      vtkErrorMacro (<< "Bad Sample Dimensions, retaining previous values");
      return;
      }

    for (dataDim=0, i=0; i<3 ; i++)
      {
      if (dim[i] > 1)
        {
        dataDim++;
        }
      }

    if (dataDim  < 3)
      {
      vtkErrorMacro(<<"Sample dimensions must define a volume!");
      return;
      }

    for (i=0; i<3; i++)
      {
      this->SampleDimensions[i] = dim[i];
      }

    this->Modified();
    }
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
