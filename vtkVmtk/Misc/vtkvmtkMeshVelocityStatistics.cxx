/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshVelocityStatistics.cxx,v $
Language:  C++
Date:      $Date: 2006/07/27 08:28:36 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkMeshVelocityStatistics.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkMeshVelocityStatistics);

vtkvmtkMeshVelocityStatistics::vtkvmtkMeshVelocityStatistics()
{
  this->VelocityArrayIds = NULL;
}

vtkvmtkMeshVelocityStatistics::~vtkvmtkMeshVelocityStatistics()
{
  if (this->VelocityArrayIds)
    {
    this->VelocityArrayIds->Delete();
    this->VelocityArrayIds = NULL;
    }
}

int vtkvmtkMeshVelocityStatistics::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
 
  vtkPointData* inputPointData = input->GetPointData();
  
  if (this->VelocityArrayIds == NULL)
    {
    vtkErrorMacro("No VelocityArrayIds specified");
    return 1;
    }
  
  int numberOfArrayIds = this->VelocityArrayIds->GetNumberOfIds();
  
  if (numberOfArrayIds < 2)
    {
    vtkWarningMacro("Only 1 VelocityArrayIds specified. No point in computing statistics.");
    return 1;
    }
 
  int i;
  for (i=0; i<numberOfArrayIds; i++)
    {
    if (inputPointData->GetArray(this->VelocityArrayIds->GetId(i)) == NULL)
      {
      vtkErrorMacro("Id in VelocityArrayIds is not a PointData array.");
      return 1;
      }
    }
 
  int numberOfPoints = input->GetNumberOfPoints();
  
  vtkDoubleArray* avgVelocityArray = vtkDoubleArray::New();
  avgVelocityArray->SetName("AVGVelocity");
  avgVelocityArray->SetNumberOfComponents(3);
  avgVelocityArray->SetNumberOfTuples(numberOfPoints);
  avgVelocityArray->FillComponent(0,0.0);
  avgVelocityArray->FillComponent(1,0.0);
  avgVelocityArray->FillComponent(2,0.0);

  vtkDoubleArray* rmsVelocityArray = vtkDoubleArray::New();
  rmsVelocityArray->SetName("RMSVelocity");
  rmsVelocityArray->SetNumberOfComponents(3);
  rmsVelocityArray->SetNumberOfTuples(numberOfPoints);
  rmsVelocityArray->FillComponent(0,0.0);
  rmsVelocityArray->FillComponent(1,0.0);
  rmsVelocityArray->FillComponent(2,0.0);

  double avgVelocity[3], rmsVelocity[3], velocity[3];
  
  double weight = 1.0 / double(numberOfArrayIds);
  
  int j;
  for (i=0; i<numberOfPoints; i++)
    {
    for (j=0; j<numberOfArrayIds; j++)
      {
      vtkDataArray* velocityArray = inputPointData->GetArray(this->VelocityArrayIds->GetId(j));
      avgVelocityArray->GetTuple(i,avgVelocity);
      velocityArray->GetTuple(i,velocity);
      avgVelocity[0] += weight * velocity[0];
      avgVelocity[1] += weight * velocity[1];
      avgVelocity[2] += weight * velocity[2];
      avgVelocityArray->SetTuple(i,avgVelocity);
      }
    }

  for (i=0; i<numberOfPoints; i++)
    {
    for (j=0; j<numberOfArrayIds; j++)
      {
      vtkDataArray* velocityArray = inputPointData->GetArray(this->VelocityArrayIds->GetId(j));
      rmsVelocityArray->GetTuple(i,rmsVelocity);
      avgVelocityArray->GetTuple(i,avgVelocity);
      velocityArray->GetTuple(i,velocity);
      rmsVelocity[0] += weight * (velocity[0] - avgVelocity[0]) * (velocity[0] - avgVelocity[0]);
      rmsVelocity[1] += weight * (velocity[1] - avgVelocity[1]) * (velocity[1] - avgVelocity[1]);
      rmsVelocity[2] += weight * (velocity[2] - avgVelocity[2]) * (velocity[2] - avgVelocity[2]);
      rmsVelocityArray->SetTuple(i,rmsVelocity);
      }
    }

  for (i=0; i<numberOfPoints; i++)
    {
    rmsVelocityArray->GetTuple(i,rmsVelocity);
    rmsVelocity[0] = sqrt(rmsVelocity[0]);
    rmsVelocity[1] = sqrt(rmsVelocity[1]);
    rmsVelocity[2] = sqrt(rmsVelocity[2]);
    rmsVelocityArray->SetTuple(i,rmsVelocity);
    }

  output->GetPointData()->AddArray(avgVelocityArray);
  output->GetPointData()->AddArray(rmsVelocityArray);
  
  avgVelocityArray->Delete();
  rmsVelocityArray->Delete();
  
  return 1;
}

void vtkvmtkMeshVelocityStatistics::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
