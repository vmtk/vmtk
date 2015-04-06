/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter.h"
#include "vtkvmtkPolyDataCylinderHarmonicMappingFilter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"

#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter);

vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter::vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter() 
{
  this->HarmonicMappingArrayName = NULL;
  this->GroupIdsArrayName = NULL;
}

vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter::~vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter()
{
  if (this->HarmonicMappingArrayName)
    {
    delete[] this->HarmonicMappingArrayName;
    this->HarmonicMappingArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }
}

int vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->HarmonicMappingArrayName)
    {
    vtkErrorMacro(<<"HarmonicMappingArrayName not set.");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not set.");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetPointData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist.");
    return 1;
    }

  int numberOfInputPoints = input->GetNumberOfPoints();

  vtkDoubleArray* harmonicMappingArray = vtkDoubleArray::New();
  harmonicMappingArray->SetName(this->HarmonicMappingArrayName);
  harmonicMappingArray->SetNumberOfComponents(1);
  harmonicMappingArray->SetNumberOfTuples(numberOfInputPoints);
  harmonicMappingArray->FillComponent(0,0.0);
  output->DeepCopy(input);

  vtkIdList* groupIds = vtkIdList::New();
  vtkvmtkPolyDataBranchUtilities::GetGroupsIdList(input,this->GroupIdsArrayName,groupIds);

  int i, j;
  for (i=0; i<groupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = groupIds->GetId(i);
    vtkPolyData* cylinder = vtkPolyData::New();
    vtkvmtkPolyDataBranchUtilities::ExtractGroup(input,this->GroupIdsArrayName,groupId,false,cylinder);
  
    vtkvmtkPolyDataCylinderHarmonicMappingFilter* mappingFilter = vtkvmtkPolyDataCylinderHarmonicMappingFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
    mappingFilter->SetInput(cylinder);
#else
    mappingFilter->SetInputData(cylinder);
#endif
    mappingFilter->SetHarmonicMappingArrayName(this->HarmonicMappingArrayName);
    mappingFilter->Update();

    vtkDataArray* cylinderMappingArray = mappingFilter->GetOutput()->GetPointData()->GetArray(this->HarmonicMappingArrayName);

    if (!cylinderMappingArray)
      {
      mappingFilter->Delete();
      continue;
      }

    double mappingValue;
    for (j=0; j<numberOfInputPoints; j++)
      {
      vtkIdType currentGroupId = static_cast<int>(groupIdsArray->GetComponent(j,0));
      if (currentGroupId == groupId)
        {
        mappingValue = cylinderMappingArray->GetComponent(j,0);
        harmonicMappingArray->SetComponent(j,0,mappingValue);
        }
      }

    mappingFilter->Delete();
    cylinder->Delete();
    }

  output->GetPointData()->AddArray(harmonicMappingArray);
  
  groupIds->Delete();
  harmonicMappingArray->Delete();

  return 1;
}
