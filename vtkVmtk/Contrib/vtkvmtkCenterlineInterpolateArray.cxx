/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkCenterlineInterpolateArray.cxx,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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

#include "vtkvmtkCenterlineInterpolateArray.h"

#include <algorithm>
#include <vector>

#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkMath.h"
#include "vtkBitArray.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIOStream.h"
#include "vtkVersion.h"

#include "vtkvmtkConstants.h"
#include "vtkvmtkCenterlineAttributesFilter.h"


vtkStandardNewMacro(vtkvmtkCenterlineInterpolateArray);

vtkvmtkCenterlineInterpolateArray::vtkvmtkCenterlineInterpolateArray() 
{
  this->InterpolatedArrayName = NULL;

  this->DefaultValue = 0;
  this->Values = NULL;
  this->ValuesIds = NULL;
}

vtkvmtkCenterlineInterpolateArray::~vtkvmtkCenterlineInterpolateArray()
{
  if (this->InterpolatedArrayName)
    {
    delete[] this->InterpolatedArrayName;
    this->InterpolatedArrayName = NULL;
    }
    
  if (this->Values)
    {
    this->Values->Delete();
    this->Values = NULL;
    }
    
  if (this->ValuesIds)
    {
    this->ValuesIds->Delete();
    this->ValuesIds = NULL;
    }
}

int vtkvmtkCenterlineInterpolateArray::RequestData(
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

  if (!this->InterpolatedArrayName)
    {
    vtkErrorMacro(<<"InterpolatedArrayName not set.");
    return 1;
    }

  if (!this->Values)
    {
    vtkErrorMacro(<<"Values not set");
    return 1;
    }
    
  if (!this->ValuesIds)
    {
    vtkErrorMacro(<<"ValuesIds not set");
    return 1;
    }
    
  if (this->Values->GetNumberOfTuples() != this->ValuesIds->GetNumberOfIds())
    {
    vtkErrorMacro(<<"Inconsistent number of values/ids");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  
  vtkDataArray* interpolatedArray =  output->GetPointData()->GetArray(this->InterpolatedArrayName);
  
  bool createArray = !interpolatedArray;
  
  if (createArray) 
    {
    interpolatedArray = vtkDoubleArray::New();
    interpolatedArray->SetName(this->InterpolatedArrayName);
    interpolatedArray->SetNumberOfComponents(this->Values->GetNumberOfComponents());
    interpolatedArray->SetNumberOfTuples(numberOfInputPoints);
    for (int i=0; i<interpolatedArray->GetNumberOfComponents(); i++)
      {
      interpolatedArray->FillComponent(i, this->DefaultValue);
      }
    output->GetPointData()->AddArray(interpolatedArray);
    }
    
  int numberOfComponents = std::min(interpolatedArray->GetNumberOfComponents(),this->Values->GetNumberOfComponents());
    
  //Compute abscissas for the centerlines
  vtkvmtkCenterlineAttributesFilter *attribFilter = vtkvmtkCenterlineAttributesFilter::New();
  attribFilter->SetInputData(input);
  attribFilter->SetAbscissasArrayName("Abscissa");
  attribFilter->SetParallelTransportNormalsArrayName("ParallelTransportNormals");
  attribFilter->Update();
  vtkDataArray *abscissaArray = attribFilter->GetOutput()->GetPointData()->GetArray("Abscissa");
  
  //Indicates which point have values
  vtkBitArray *hasValueArray = vtkBitArray::New();
  hasValueArray->SetNumberOfComponents(1);
  hasValueArray->SetNumberOfTuples(numberOfInputPoints);
  hasValueArray->FillComponent(0,false);
  
  for (int i=0; i<this->ValuesIds->GetNumberOfIds(); i++)
    {
    hasValueArray->SetValue(this->ValuesIds->GetId(i),true);
    for (int j=0; j<numberOfComponents; j++)
      {
      interpolatedArray->SetComponent(this->ValuesIds->GetId(i),j,this->Values->GetComponent(i,j));
      }
    }
  
  int numberOfInputCells = input->GetNumberOfCells();
 
  
  for (int i=0; i<numberOfInputCells; i++)
    {
    vtkPolyLine* polyLine = vtkPolyLine::SafeDownCast(input->GetCell(i));
    if (!polyLine)
      {
      continue;
      }
      
    int numberOfLinePoints = polyLine->GetNumberOfPoints();
      
    bool foundValue = false;
    int startInd = 0;
    //Find the first point having a value
    while ((startInd<numberOfLinePoints) && (!foundValue)) 
      {
      foundValue = hasValueArray->GetValue(polyLine->GetPointId(startInd++));
      }
      
    //If no point with a value has been found skip to next line
    if ((startInd==numberOfLinePoints) && (!foundValue))
      {
      continue;
      }
      
    //Otherwise we have our first value fill the previous points with this value
    startInd--;
    std::vector<double> startVal(numberOfComponents);
    for (int j=0; j<numberOfComponents; j++)
      {
      startVal[j] = interpolatedArray->GetComponent(polyLine->GetPointId(startInd),j);
      }
    double startAbs = abscissaArray->GetComponent(polyLine->GetPointId(startInd),0);
    
    for (int j=0; j<startInd; j++)
      {
      for (int k=0; k<numberOfComponents; k++)
        {
        interpolatedArray->SetComponent(polyLine->GetPointId(j),k,startVal[k]);
        }
      hasValueArray->SetValue(polyLine->GetPointId(j),true);
      }
    
    int endInd = startInd + 1;
    foundValue = false;
    while (endInd<numberOfLinePoints)
      {
      foundValue = hasValueArray->GetValue(polyLine->GetPointId(endInd));
      if (foundValue)
        {
        //Interpolate between the start and end values
        std::vector<double> endVal(numberOfComponents);
        for (int j=0; j<numberOfComponents; j++)
          {
          endVal[j] = interpolatedArray->GetComponent(polyLine->GetPointId(endInd),j);
          }
        double endAbs = abscissaArray->GetComponent(polyLine->GetPointId(endInd),0);
        double absDiff = endAbs - startAbs;
        if (absDiff!=0.) 
          {
          float invAbsDiff = 1./absDiff;
          for (int j=startInd+1;j<endInd;j++)
            {
              double currAbs = abscissaArray->GetComponent(polyLine->GetPointId(j),0);
              for (int k=0; k<numberOfComponents; k++)
                {
                double newVal = startVal[k] + (currAbs - startAbs)*invAbsDiff*(endVal[k] - startVal[k]);
                interpolatedArray->SetComponent(polyLine->GetPointId(j),k,newVal);
                }
              hasValueArray->SetValue(polyLine->GetPointId(j),true);
            }
          }
          //Replace the end value by the start value
          startInd = endInd;
          startVal = endVal;
          startAbs = endAbs;
        }
      endInd++;
      }
      
    //Fill the remaining values
    for (int j=startInd+1; j<numberOfLinePoints; j++)
      {
      for (int k=0; k<numberOfComponents; k++)
        {
        interpolatedArray->SetComponent(polyLine->GetPointId(j),k,startVal[k]);
        }
      hasValueArray->SetValue(polyLine->GetPointId(j),true);
      }
    }

  if (createArray) interpolatedArray->Delete();
  attribFilter->Delete();
  hasValueArray->Delete();

  return 1;
}
