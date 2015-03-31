/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineToParticlesFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkStreamlineToParticlesFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkPointData.h"
#include "vtkMaskPolyData.h"
#include "vtkvmtkCenterlineUtilities.h"
#include "vtkIdList.h"
#include "vtkPolyLine.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"


vtkStandardNewMacro(vtkvmtkStreamlineToParticlesFilter);

vtkvmtkStreamlineToParticlesFilter::vtkvmtkStreamlineToParticlesFilter()
{
  this->NumberOfInjections = 10;
  this->NumberOfParticlesPerInjection = 10;
  this->InjectionStart = 0.0;
  this->InjectionEnd = 1.0;
  this->TracingEnd = 1.0;
  this->DeltaT = 1E-2;
  this->IntegrationTimeArrayName = NULL;
  this->TimeArrayName = NULL;
  this->VelocityArrayName = NULL;
}

vtkvmtkStreamlineToParticlesFilter::~vtkvmtkStreamlineToParticlesFilter()
{
  if (this->IntegrationTimeArrayName)
    {
    delete[] this->IntegrationTimeArrayName;
    this->IntegrationTimeArrayName = NULL;
    }
  if (this->TimeArrayName)
    {
    delete[] this->TimeArrayName;
    this->TimeArrayName = NULL;
    }
  if (this->VelocityArrayName)
    {
    delete[] this->VelocityArrayName;
    this->VelocityArrayName = NULL;
    }
}

int vtkvmtkStreamlineToParticlesFilter::RequestData(
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

  if (input->GetNumberOfPoints() < 1)
    {
    return 1;
    }

  if (!this->IntegrationTimeArrayName)
    {
    vtkErrorMacro("Error: IntegrationTimeArrayName not specified");
    return 1;
    }

  vtkDataArray* integrationTimeArray = input->GetPointData()->GetArray(this->IntegrationTimeArrayName);
  
  if (!integrationTimeArray)
    {
    vtkErrorMacro("Error: IntegrationTimeArray with name specified does not exist");
    return 1;
    }

  vtkDataArray* velocityArray = NULL;

  if (this->VelocityArrayName)
    {
    velocityArray = input->GetPointData()->GetArray(this->VelocityArrayName);
  
    if (!velocityArray)
      {
      vtkErrorMacro("Error: VelocityArray with name specified does not exist");
      return 1;
      }
    }

  int numberOfCells = input->GetNumberOfCells();

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputVerts = vtkCellArray::New();

  vtkDoubleArray* outputTimeArray = vtkDoubleArray::New();
  outputTimeArray->SetName(this->TimeArrayName);

  vtkDoubleArray* outputVelocityArray = vtkDoubleArray::New();
  outputVelocityArray->SetName(this->VelocityArrayName);
  outputVelocityArray->SetNumberOfComponents(3);

  int i;
  for (i=0; i<this->NumberOfInjections; i++)
    {
    double injectionOffset = this->InjectionStart + i * (this->InjectionEnd - this->InjectionStart) / this->NumberOfInjections;

    vtkMaskPolyData* mask = vtkMaskPolyData::New();
#if (VTK_MAJOR_VERSION <= 5)
    mask->SetInput(input);
#else
    mask->SetInputData(input);
#endif
    mask->SetOnRatio(numberOfCells/this->NumberOfParticlesPerInjection);
    mask->SetOffset(i);
    mask->Update();

    mask->GetOutput()->GetPointData()->SetActiveScalars(this->IntegrationTimeArrayName);

    vtkPolyData* subset = mask->GetOutput();

    int numberOfSubsetCells = subset->GetNumberOfCells();
    int j;
    for (j=0; j<numberOfSubsetCells; j++)
      {
      vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(subset->GetCell(j));
      if (!polyLine)
        {
        continue;
        }
      
      vtkIdList* pointIds = polyLine->GetPointIds();
      int numberOfPointIds = pointIds->GetNumberOfIds();

      double integrationTime = injectionOffset;
      int k;
      for (k=0; k<numberOfPointIds-1; k++)
        {
        double time0 = integrationTimeArray->GetTuple1(pointIds->GetId(k));
        double time1 = integrationTimeArray->GetTuple1(pointIds->GetId(k+1));
        if (time0 < integrationTime && time1 < integrationTime)
          {
          continue;
          }
        if (time0 > integrationTime && time1 > integrationTime)
          {
          break;
          }
        bool valid = true;
        while (valid)
          {
          double pcoord = (integrationTime - time0) / (time1 - time0);
          double interpolatedPoint[3];
          vtkvmtkCenterlineUtilities::InterpolatePoint(subset,j,k,pcoord,interpolatedPoint);
          vtkIdType pointId = outputPoints->InsertNextPoint(interpolatedPoint);
          outputVerts->InsertNextCell(1);
          outputVerts->InsertCellPoint(pointId);
          outputTimeArray->InsertNextValue(integrationTime + injectionOffset);
          if (velocityArray)
            {
            double velocity[3];
            vtkvmtkCenterlineUtilities::InterpolateTuple3(subset,this->VelocityArrayName,j,k,pcoord,velocity);
            outputVelocityArray->InsertNextTuple(velocity);
            }
          integrationTime += this->DeltaT;
          if (integrationTime > time1)
            {
            valid = false;
            }
          if (integrationTime + injectionOffset > this->TracingEnd)
            {
            break;
            }
          }
        if (integrationTime + injectionOffset > this->TracingEnd)
          {
          break;
          }
        }
      }
    }

  output->SetPoints(outputPoints);
  output->SetVerts(outputVerts);
  output->GetPointData()->AddArray(outputTimeArray);
  if (velocityArray)
    {
    output->GetPointData()->AddArray(outputVelocityArray);
    }

  outputPoints->Delete();
  outputVerts->Delete();
  outputTimeArray->Delete();
  if (velocityArray)
    {
    outputVelocityArray->Delete();
    }

  return 1;
}

void vtkvmtkStreamlineToParticlesFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
