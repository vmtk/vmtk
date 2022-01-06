/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataGeodesicRBFInterpolation.cxx,v $
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

#include "vtkvmtkPolyDataGeodesicRBFInterpolation.h"

#include "vtkVersion.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIOStream.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"

#include <vector>

#include "vtkvmtkConstants.h"

#include "vtkDijkstraGraphGeodesicPath.h"

vtkStandardNewMacro(vtkvmtkPolyDataGeodesicRBFInterpolation);

vtkvmtkPolyDataGeodesicRBFInterpolation::vtkvmtkPolyDataGeodesicRBFInterpolation() 
{
  this->InterpolatedArrayName = NULL;

  this->SeedIds = NULL;
  this->SeedValues = NULL;
  
  this->RBFType = THIN_PLATE_SPLINE;
}

vtkvmtkPolyDataGeodesicRBFInterpolation::~vtkvmtkPolyDataGeodesicRBFInterpolation()
{
  if (this->InterpolatedArrayName)
    {
    delete[] this->InterpolatedArrayName;
    this->InterpolatedArrayName = NULL;
    }

  if (this->SeedIds)
    {
    this->SeedIds->Delete();
    this->SeedIds = NULL;
    }
    
  if (this->SeedValues)
    {
    this->SeedValues->Delete();
    this->SeedValues = NULL;
    }
}

double vtkvmtkPolyDataGeodesicRBFInterpolation::EvaluateRBF(double r)
{
  if (this->RBFType == THIN_PLATE_SPLINE)
    {
    double r2 = r*r;
    if (!r2)
      {
      return 0.0;
      }
    return r2 * log(sqrt(r2));
    }
  else if (this->RBFType == BIHARMONIC)
    {
    return r;
    }
  else if (this->RBFType == TRIHARMONIC)
    {
    return r*r*r;
    }
  else
    {
    vtkErrorMacro(<<"Error: Unsupported RBFType!");
    return 0.0;
    }
}


int vtkvmtkPolyDataGeodesicRBFInterpolation::RequestData(
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

  if (!this->SeedIds)
    {
    vtkErrorMacro(<<"SeedIds not set");
    return 1;
    }
    
  if (!this->SeedValues)
    {
    vtkErrorMacro(<<"SeedValues not set");
    return 1;
    }


  int numberOfSeeds = this->SeedIds->GetNumberOfIds();

  int numberOfInputPoints = input->GetNumberOfPoints();

  if ((numberOfSeeds != this->SeedValues->GetNumberOfTuples()) || (numberOfSeeds>numberOfInputPoints) || (numberOfSeeds<2))
    {
    vtkErrorMacro(<<"Incorrect number of seed values");
    return 1;
    }
  
  output->DeepCopy(input);

  
  vtkDataArray* interpolatedArray =  output->GetPointData()->GetArray(this->InterpolatedArrayName);
  
  bool createArray = !interpolatedArray;
  
  if (createArray) 
    {
    interpolatedArray = vtkDoubleArray::New();
    interpolatedArray->SetName(this->InterpolatedArrayName);
    interpolatedArray->SetNumberOfComponents(1);
    interpolatedArray->SetNumberOfTuples(numberOfInputPoints);
  
    output->GetPointData()->AddArray(interpolatedArray);
    }

  
  
  vtkDijkstraGraphGeodesicPath *dijkstraAlgo = vtkDijkstraGraphGeodesicPath::New();
  dijkstraAlgo->SetInputData(input);
  dijkstraAlgo->StopWhenEndReachedOff();
  dijkstraAlgo->UseScalarWeightsOff();
  
  std::vector<vtkDoubleArray *> geodesicDistances;
  
  
  //Compute the geodesic distances
  for (int i=0; i<numberOfSeeds; i++)
    {
    dijkstraAlgo->SetStartVertex(SeedIds->GetId(i));
    dijkstraAlgo->Update();
    vtkDoubleArray *seedDistances = vtkDoubleArray::New();
    dijkstraAlgo->GetCumulativeWeights(seedDistances);
    geodesicDistances.push_back(seedDistances);
    }
    
  //Compute the coefficients  
  double **A, *x;
  x = new double[numberOfSeeds];
  A = new double* [numberOfSeeds];

  int i;
  for (i=0; i<numberOfSeeds; i++)
    {
    A[i] = new double[numberOfSeeds];
    x[i] = this->SeedValues->GetComponent(i,0);
    }

  int j;
  for (i=0; i<numberOfSeeds; i++)
    {
    for (j=0; j<numberOfSeeds; j++)
      {
      double dist = geodesicDistances[i]->GetValue(this->SeedIds->GetId(j));
      A[i][j] = this->EvaluateRBF(dist);
      }
    } 

  cout << "A " << endl;
  for (int i=0;i<numberOfSeeds;i++) {
    for (int j=0;j<numberOfSeeds;j++) {
      cout << A[i][j] << " ";
    }
    cout << endl;
  }
  
  cout << "b " << endl;
  for (int i=0;i<numberOfSeeds;i++) {
    cout << x[i] << endl;
  }

  int ret = vtkMath::SolveLinearSystem(A,x,numberOfSeeds);
  
  cout << "x " << endl;
  for (int i=0;i<numberOfSeeds;i++) {
    cout << x[i] << endl;
  }
  
  if (!ret)
    {
    vtkErrorMacro(<<"Cannot compute coefficients: error during linear system solve");
    }

  
  for (i=0; i<numberOfSeeds; i++)
    {
    delete[] A[i];
    }
  delete[] A;
  
  
  //Interpolate the values at all points using the coefficients
  for (int i=0;i<numberOfInputPoints;i++) 
    {
    double rbfValue = 0.;
    for (int j=0;j<numberOfSeeds;j++) {
      double dist = geodesicDistances[j]->GetValue(i);
      rbfValue += x[j]*this->EvaluateRBF(dist);
    }
    interpolatedArray->SetComponent(i,0,rbfValue);
    }
  
  delete[] x;
  for (int i=0;i<numberOfSeeds;i++)
    {
    geodesicDistances[i]->Delete();
    }
  
  geodesicDistances.clear();


  if (createArray) interpolatedArray->Delete();

  return 1;
}
