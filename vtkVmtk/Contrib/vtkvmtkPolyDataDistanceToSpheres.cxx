/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDistanceToSpheres.cxx,v $
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

#include "vtkvmtkPolyDataDistanceToSpheres.h"

#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkIOStream.h"

#include "vtkvmtkConstants.h"


vtkStandardNewMacro(vtkvmtkPolyDataDistanceToSpheres);

vtkvmtkPolyDataDistanceToSpheres::vtkvmtkPolyDataDistanceToSpheres() 
{
  this->DistanceToSpheresArrayName = NULL;

  this->Spheres = NULL;

  this->DistanceOffset = 0.;
  this->DistanceScale = 1.;
  this->MinDistance = 0.;
  this->MaxDistance = -1.;
}

vtkvmtkPolyDataDistanceToSpheres::~vtkvmtkPolyDataDistanceToSpheres()
{
  if (this->DistanceToSpheresArrayName)
    {
    delete[] this->DistanceToSpheresArrayName;
    this->DistanceToSpheresArrayName = NULL;
    }

  if (this->Spheres)
    {
    this->Spheres->Delete();
    this->Spheres = NULL;
    }
}

int vtkvmtkPolyDataDistanceToSpheres::RequestData(
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

  if (!this->DistanceToSpheresArrayName)
    {
    vtkErrorMacro(<<"DistanceToSpheresArrayName not set.");
    return 1;
    }

  if (!this->Spheres)
    {
    vtkErrorMacro(<<"Spheres not set");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  output->DeepCopy(input);

  
  vtkDataArray* distanceToSpheresArray =  output->GetPointData()->GetArray(this->DistanceToSpheresArrayName);
  
  bool createArray = !distanceToSpheresArray;
  
  if (createArray) 
    {
    distanceToSpheresArray = vtkDoubleArray::New();
    distanceToSpheresArray->SetName(this->DistanceToSpheresArrayName);
    distanceToSpheresArray->SetNumberOfComponents(1);
    distanceToSpheresArray->SetNumberOfTuples(numberOfInputPoints);
    distanceToSpheresArray->FillComponent(0, VTK_VMTK_LARGE_DOUBLE);
  
    output->GetPointData()->AddArray(distanceToSpheresArray);
    }

  int numberOfSpheres = this->Spheres->GetNumberOfPoints();
  
  vtkDataArray* scalarArray = this->Spheres->GetPointData()->GetScalars();

  double maxd = this->MaxDistance > 0 ? this->MaxDistance : VTK_VMTK_LARGE_DOUBLE;

  double point[3], sphereCenter[3];
  double sphereRadius = 0.;
  double distanceToSpheres;
  for (int i=0; i<numberOfInputPoints; i++)
    {
    distanceToSpheres=distanceToSpheresArray->GetComponent(i,0);
    input->GetPoint(i,point);
    for (int j=0; j<numberOfSpheres; j++)
      {
        this->Spheres->GetPoint(j,sphereCenter);
        if (scalarArray) sphereRadius = scalarArray->GetComponent(j,0);
        double newDist = sqrt(vtkMath::Distance2BetweenPoints(point,sphereCenter)) - sphereRadius;
        if (newDist<0.) newDist = 0.;
        //Scale and offset the distance
        newDist = this->DistanceOffset + this->DistanceScale*newDist;
        if (newDist<this->MinDistance) newDist = this->MinDistance;
        if (newDist>maxd) newDist = maxd;
        if (newDist<distanceToSpheres) distanceToSpheres = newDist;
      } 
    distanceToSpheresArray->SetComponent(i,0,distanceToSpheres);
    }

  if (createArray) distanceToSpheresArray->Delete();

  return 1;
}
