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
#include "vtkvmtkCenterlineBifurcationReferenceSystems.h"
#include "vtkvmtkReferenceSystemUtilities.h"
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
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkMergeCenterlines);

vtkvmtkMergeCenterlines::vtkvmtkMergeCenterlines()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->TractIdsArrayName = NULL;
  this->BlankingArrayName = NULL;
  this->ResamplingStepLength = 0.0;
  this->MergeBlanked = 1;
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

  if (this->CenterlineIdsArrayName)
    {
    delete[] this->CenterlineIdsArrayName;
    this->CenterlineIdsArrayName = NULL;
    }

  if (this->TractIdsArrayName)
    {
    delete[] this->TractIdsArrayName;
    this->TractIdsArrayName = NULL;
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

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* centerlineIdsArray = input->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  if (!centerlineIdsArray)
    {
    vtkErrorMacro(<<"CenterlineIdsArray with name specified does not exist");
    return 1;
    }

  if (!this->TractIdsArrayName)
    {
    vtkErrorMacro(<<"TractIdsArrayName not specified");
    return 1;
    }

  vtkDataArray* tractIdsArray = input->GetCellData()->GetArray(this->TractIdsArrayName);

  if (!tractIdsArray)
    {
    vtkErrorMacro(<<"TractIdsArray with name specified does not exist");
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
#if (VTK_MAJOR_VERSION <= 5)
  cleaner->SetInput(input);
#else
  cleaner->SetInputData(input);
#endif
  cleaner->Update();
  
  if (this->ResamplingStepLength < 1E-12)
    {
    this->ResamplingStepLength = 0.01 * cleaner->GetOutput()->GetLength();
    }

  vtkSplineFilter* resampler = vtkSplineFilter::New();
#if (VTK_MAJOR_VERSION <= 5)
  resampler->SetInput(cleaner->GetOutput());
#else
  resampler->SetInputConnection(cleaner->GetOutputPort());
#endif
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

  vtkIdList* groupIdsToMergedCells = vtkIdList::New();
  vtkIdType maxGroupId = vtkvmtkCenterlineUtilities::GetMaxGroupId(resampledCenterlines,this->GroupIdsArrayName);
  groupIdsToMergedCells->SetNumberOfIds(maxGroupId);
  int i;
  for (i=0; i<maxGroupId; i++)
    {
    groupIdsToMergedCells->SetId(i,-1);
    }

  vtkIdTypeArray* cellEndPointIds = vtkIdTypeArray::New();
  cellEndPointIds->SetNumberOfComponents(2);

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
    groupIdsToMergedCells->InsertId(groupId,mergedCellId);
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
      if (k==0 || k==numberOfMergedCellPoints-1)
        {
        cellEndPointIds->InsertNextValue(mergedPointId);
        }
      }

    vtkIdType cellId = groupUniqueCellIds->GetId(0);
    output->GetCellData()->CopyData(resampledCenterlines->GetCellData(),cellId,mergedCellId);

    groupUniqueCellIds->Delete();
    }

  if (!this->MergeBlanked)
    {
    nonBlankedGroupIds->Delete();
    resampledCenterlines->Delete();
    outputPoints->Delete();
    outputLines->Delete();
    groupIdsToMergedCells->Delete();
    cellEndPointIds->Delete();
    return 1;
    }

  vtkvmtkCenterlineBifurcationReferenceSystems* referenceSystemsFilter = vtkvmtkCenterlineBifurcationReferenceSystems::New();
#if (VTK_MAJOR_VERSION <= 5)
  referenceSystemsFilter->SetInput(resampledCenterlines);
#else
  referenceSystemsFilter->SetInputData(resampledCenterlines);
#endif
  referenceSystemsFilter->SetRadiusArrayName(this->RadiusArrayName);
  referenceSystemsFilter->SetGroupIdsArrayName(this->GroupIdsArrayName);
  referenceSystemsFilter->SetBlankingArrayName(this->BlankingArrayName);
  referenceSystemsFilter->SetNormalArrayName("Normal");
  referenceSystemsFilter->SetUpNormalArrayName("UpNormal");
  referenceSystemsFilter->Update();

  vtkPolyData* referenceSystems = referenceSystemsFilter->GetOutput();

  int numberOfMergedCells = outputLines->GetNumberOfCells();

  vtkIdList* blankedGroupIds = vtkIdList::New();
  vtkvmtkCenterlineUtilities::GetBlankedGroupsIdList(resampledCenterlines,this->GroupIdsArrayName,this->BlankingArrayName,blankedGroupIds);
  vtkIdList* groupIdsToBifurcationPointIds = vtkIdList::New();
  vtkIdTypeArray* cellAdditionalEndPointIds = vtkIdTypeArray::New();
  cellAdditionalEndPointIds->SetNumberOfComponents(2);
  cellAdditionalEndPointIds->SetNumberOfTuples(numberOfMergedCells);
  vtkIdType tupleValue[2];
  tupleValue[0] = tupleValue[1] = -1;
  for (i=0; i<numberOfMergedCells; i++)
    {
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
    cellAdditionalEndPointIds->SetTypedTuple(i, tupleValue);
#else
    cellAdditionalEndPointIds->SetTupleValue(i, tupleValue);
#endif
    }

  for (i=0; i<blankedGroupIds->GetNumberOfIds(); i++)
    {
    vtkIdType groupId = blankedGroupIds->GetId(i);

    vtkIdType referenceSystemPointId = vtkvmtkReferenceSystemUtilities::GetReferenceSystemPointId(referenceSystems,this->GroupIdsArrayName,groupId);

    vtkIdList* upStreamGroupIds = vtkIdList::New();
    vtkIdList* downStreamGroupIds = vtkIdList::New();

    vtkvmtkCenterlineUtilities::FindAdjacentCenterlineGroupIds(resampledCenterlines,this->GroupIdsArrayName,this->CenterlineIdsArrayName,this->TractIdsArrayName,groupId,upStreamGroupIds,downStreamGroupIds);

    double bifurcationPoint[3];
    referenceSystems->GetPoint(referenceSystemPointId,bifurcationPoint);
    
    vtkIdType bifurcationPointId = outputPoints->InsertNextPoint(bifurcationPoint);

    vtkIdType sourcePointId = -1;
    int j;
    for (j=0; j<upStreamGroupIds->GetNumberOfIds(); j++)
      {
      vtkIdType mergedCellId = groupIdsToMergedCells->GetId(upStreamGroupIds->GetId(j));
      if (mergedCellId == -1)
        {
        continue;
        }
      if (sourcePointId == -1)
        {
        vtkIdList* groupUniqueCellIds = vtkIdList::New();
        vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(resampledCenterlines,this->GroupIdsArrayName,upStreamGroupIds->GetId(j),groupUniqueCellIds);
        vtkCell* resampledCell = resampledCenterlines->GetCell(groupUniqueCellIds->GetId(0));
        sourcePointId = resampledCell->GetPointId(resampledCell->GetNumberOfPoints()-1);
        groupUniqueCellIds->Delete();
        }
      vtkIdType tupleValue[2];
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
      cellAdditionalEndPointIds->GetTypedTuple(mergedCellId,tupleValue);
#else
      cellAdditionalEndPointIds->GetTupleValue(mergedCellId, tupleValue);
#endif
      tupleValue[1] = bifurcationPointId;
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
      cellAdditionalEndPointIds->SetTypedTuple(mergedCellId, tupleValue);
#else
      cellAdditionalEndPointIds->SetTupleValue(mergedCellId, tupleValue);
#endif

      }

    for (j=0; j<downStreamGroupIds->GetNumberOfIds(); j++)
      {
      vtkIdType mergedCellId = groupIdsToMergedCells->GetId(downStreamGroupIds->GetId(j));
      if (mergedCellId == -1)
        {
        continue;
        }
      if (sourcePointId == -1)
        {
        vtkIdList* groupUniqueCellIds = vtkIdList::New();
        vtkvmtkCenterlineUtilities::GetGroupUniqueCellIds(resampledCenterlines,this->GroupIdsArrayName,downStreamGroupIds->GetId(j),groupUniqueCellIds);
        vtkCell* resampledCell = resampledCenterlines->GetCell(groupUniqueCellIds->GetId(0));
        sourcePointId = resampledCell->GetPointId(0);
        groupUniqueCellIds->Delete();
        }
      vtkIdType tupleValue[2];
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
      cellAdditionalEndPointIds->GetTypedTuple(mergedCellId, tupleValue);
#else
      cellAdditionalEndPointIds->GetTupleValue(mergedCellId,tupleValue);
#endif
      tupleValue[0] = bifurcationPointId;
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
      cellAdditionalEndPointIds->SetTypedTuple(mergedCellId, tupleValue);
#else
      cellAdditionalEndPointIds->SetTupleValue(mergedCellId,tupleValue);
#endif
    }
    if (sourcePointId == -1)
      {
      upStreamGroupIds->Delete();
      downStreamGroupIds->Delete();
      continue;
      }

    // TODO: interpolate point data instead of copying from first upstream point - good enough for now
    output->GetPointData()->CopyData(resampledCenterlines->GetPointData(),sourcePointId,bifurcationPointId);

    upStreamGroupIds->Delete();
    downStreamGroupIds->Delete();
    }
 
  vtkCellArray* extendedOutputLines = vtkCellArray::New();
  outputLines->InitTraversal();
  for (i=0; i<numberOfMergedCells; i++)
    {
    vtkIdType npts, *pts;
    npts = 0;
    pts = NULL;
    outputLines->GetNextCell(npts,pts);
    vtkIdType tupleValue[2];
#if VTK_MAJOR_VERSION >= 8  || (VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1)
    cellAdditionalEndPointIds->GetTypedTuple(i, tupleValue);
#else
    cellAdditionalEndPointIds->GetTupleValue(i,tupleValue);
#endif
    vtkIdType extendedNpts = npts;
    if (tupleValue[0] != -1)
      {
      extendedNpts += 1;
      }
    if (tupleValue[1] != -1)
      {
      extendedNpts += 1;
      }
    extendedOutputLines->InsertNextCell(extendedNpts);
    if (tupleValue[0] != -1)
      {
      extendedOutputLines->InsertCellPoint(tupleValue[0]);
      }
    int j;
    for (j=0; j<npts; j++)
      {
      extendedOutputLines->InsertCellPoint(pts[j]);
      }
    if (tupleValue[1] != -1)
      {
      extendedOutputLines->InsertCellPoint(tupleValue[1]);
      }
    }

  output->SetLines(extendedOutputLines);

  nonBlankedGroupIds->Delete();
  resampledCenterlines->Delete();
  outputPoints->Delete();
  outputLines->Delete();
  groupIdsToMergedCells->Delete();
  cellEndPointIds->Delete();

  referenceSystemsFilter->Delete();
  blankedGroupIds->Delete();
  groupIdsToBifurcationPointIds->Delete();
  cellAdditionalEndPointIds->Delete();
  extendedOutputLines->Delete();
 
  return 1;
}

void vtkvmtkMergeCenterlines::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
