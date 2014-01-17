/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataSampleFunction.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/

#include "vtkvmtkPolyDataSampleFunction.h"

#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIOStream.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataSampleFunction);
vtkCxxSetObjectMacro(vtkvmtkPolyDataSampleFunction,ImplicitFunction,vtkImplicitFunction);

vtkvmtkPolyDataSampleFunction::vtkvmtkPolyDataSampleFunction() 
{
  this->SampleArrayName = NULL;
  this->ImplicitFunction = NULL;
}

vtkvmtkPolyDataSampleFunction::~vtkvmtkPolyDataSampleFunction()
{
  if (this->SampleArrayName)
    {
    delete[] this->SampleArrayName;
    this->SampleArrayName = NULL;
    }

  this->ImplicitFunction = NULL;
}



int vtkvmtkPolyDataSampleFunction::RequestData(
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

  if (!this->SampleArrayName)
    {
    vtkErrorMacro(<<"DistanceToSpheresArrayName not set.");
    return 1;
    }
    
  if (!this->ImplicitFunction)
    {
    vtkErrorMacro(<<"ImplicitFunction not set.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  
  vtkDataArray* sampleArray =  output->GetPointData()->GetArray(this->SampleArrayName);
  
  bool createArray = !sampleArray;
  
  if (createArray) 
    {
    sampleArray = vtkDoubleArray::New();
    sampleArray->SetName(this->SampleArrayName);
    sampleArray->SetNumberOfComponents(1);
    sampleArray->SetNumberOfTuples(numberOfInputPoints);
    sampleArray->FillComponent(0, 0);
    output->GetPointData()->AddArray(sampleArray);
    }
  


  double point[3];
  for (int i=0; i<numberOfInputPoints; i++)
    {
    input->GetPoint(i,point);
    sampleArray->SetComponent(i,0,this->ImplicitFunction->EvaluateFunction(point));
    }

  if (createArray) sampleArray->Delete();

  return 1;
}
