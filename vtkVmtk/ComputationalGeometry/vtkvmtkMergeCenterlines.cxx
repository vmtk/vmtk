/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMergeCenterlines.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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

#include "vtkvmtkMergeCenterlines.h"
#include "vtkvmtkCenterlineUtilities.h"
#include "vtkSplineFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkMergeCenterlines, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkvmtkMergeCenterlines);

vtkvmtkMergeCenterlines::vtkvmtkMergeCenterlines()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->BlankingArrayName = NULL;
  this->ResamplingStepLength = 0.0;
}

vtkvmtkMergeCenterlines::~vtkvmtkMergeCenterlines()
{
  if (this->RadiusArrayName)
    {
    delete[] this->RadiusArrayName;
    this->RadiusArrayName = NULL;
    }

  if (this->GroupIdsArrayName)
    {
    delete[] this->GroupIdsArrayName;
    this->GroupIdsArrayName = NULL;
    }

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }
}

int vtkvmtkMergeCenterlines::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->RadiusArrayName)
    {
    vtkErrorMacro(<<"RadiusArrayName not specified");
    return 1;
    }

  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);

  if (!radiusArray)
    {
    vtkErrorMacro(<<"RadiusArray with name specified does not exist");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* groupIdsArray = input->GetCellData()->GetArray(this->GroupIdsArrayName);

  if (!groupIdsArray)
    {
    vtkErrorMacro(<<"GroupIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<<"BlankingArrayName not specified");
    return 1;
    }

  vtkDataArray* blankingArray = input->GetCellData()->GetArray(this->BlankingArrayName);

  if (!blankingArray)
    {
    vtkErrorMacro(<<"BlankingArray with name specified does not exist");
    return 1;
    }

  vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
  cleaner->SetInput(input);
  cleaner->Update();
  
  if (this->ResamplingStepLength < 1E-12)
    {
    this->ResamplingStepLength = 0.01 * cleaner->GetOutput()->GetLength();
    }

  vtkSplineFilter* resampler = vtkSplineFilter::New();
  resampler->SetInput(cleaner->GetOutput());
  resampler->SetSubdivideToLength();
  resampler->SetLength(this->ResamplingStepLength);
  resampler->Update();

  vtkPolyData* resampledCenterlines = vtkPolyData::New();
  resampledCenterlines->DeepCopy(resampler->GetOutput());

  resampler->Delete(); 

  radiusArray = resampledCenterlines->GetPointData()->GetArray(this->RadiusArrayName);

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputLines = vtkCellArray::New();

  output->SetPoints(outputPoints);
  output->SetLines(outputLines);

  output->GetPointData()->CopyAllocate(resampledCenterlines->GetPointData());
  output->GetCellData()->CopyAllocate(resampledCenterlines->GetCellData());

  vtkIdList* nonBlankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetNonBlankedGroupsIdList(resampledCenterlines,this->GroupIdsArrayName,this->BlankingArrayName,nonBlankedGroupIds);

  int i;
  for (i=0; i<nonBlankedGroupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = nonBlankedGroupIds->GetId(i);
    vtkIdList* groupUniqueCellIds = vtkIdList::New();

    vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(resampledCenterlines,this->GroupIdsArrayName,groupId,groupUniqueCellIds);

    int numberOfMergedCellPoints = 0;
    int j;
    for (j=0; j<groupUniqueCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType cellId = groupUniqueCellIds->GetId(j);
      int numberOfCellPoints = resampledCenterlines->GetCell(cellId)->GetNumberOfPoints();
      if ((j==0) || (numberOfCellPoints < numberOfMergedCellPoints))
        {
        numberOfMergedCellPoints = numberOfCellPoints;
        }
      }

    vtkIdType mergedCellId = outputLines->InsertNextCell(numberOfMergedCellPoints);
    int k;
    for (k=0; k<numberOfMergedCellPoints; k++)
      {
      double mergedPoint[3];
      mergedPoint[0] = 0.0;
      mergedPoint[1] = 0.0;
      mergedPoint[2] = 0.0;
      double weightSum = 0.0;
      for (j=0; j<groupUniqueCellIds->GetNumberOfIds(); j++)
        {
        vtkIdType cellId = groupUniqueCellIds->GetId(j);
        double point[3];
        vtkIdType pointId = resampledCenterlines->GetCell(cellId)->GetPointId(k);
        resampledCenterlines->GetPoint(pointId,point);
        double radius = radiusArray->GetTuple1(pointId);
        double weight = radius*radius;
        resampledCenterlines->GetCell(cellId)->GetPoints()->GetPoint(k,point);
        mergedPoint[0] += weight * point[0];
        mergedPoint[1] += weight * point[1];
        mergedPoint[2] += weight * point[2];
        weightSum += weight;
        }
      mergedPoint[0] /= weightSum;
      mergedPoint[1] /= weightSum;
      mergedPoint[2] /= weightSum;
      vtkIdType mergedPointId = outputPoints->InsertNextPoint(mergedPoint);
      outputLines->InsertCellPoint(mergedPointId);
      vtkIdType cellId = groupUniqueCellIds->GetId(0);
      vtkIdType pointId = resampledCenterlines->GetCell(cellId)->GetPointId(k);
      output->GetPointData()->CopyData(resampledCenterlines->GetPointData(),pointId,mergedPointId);
      }

    vtkIdType cellId = groupUniqueCellIds->GetId(0);
    output->GetCellData()->CopyData(resampledCenterlines->GetCellData(),cellId,mergedCellId);

    groupUniqueCellIds->Delete();
    }

  nonBlankedGroupIds->Delete();
  resampledCenterlines->Delete();
  outputPoints->Delete();
  outputLines->Delete();

  return 1;
}

void vtkvmtkMergeCenterlines::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
