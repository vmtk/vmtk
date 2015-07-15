/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSplittingAndGroupingFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/07 10:46:19 $
Version:   $Revision: 1.11 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkvmtkCenterlineUtilities.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkLine.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkAppendPolyData.h"
#include "vtkvmtkConstants.h"
#include "vtkMath.h"
#include "vtkvmtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkvmtkCenterlineSplittingAndGroupingFilter::vtkvmtkCenterlineSplittingAndGroupingFilter()
{
  this->RadiusArrayName = NULL;
  this->GroupIdsArrayName = NULL;
  this->CenterlineIdsArrayName = NULL;
  this->BlankingArrayName = NULL;
  this->TractIdsArrayName = NULL;

  this->NumberOfSplittingPoints = 0;
  this->SubIds = NULL;
  this->PCoords = NULL;
  this->TractBlanking = NULL;

  this->GroupingMode = POINTINTUBE;
}

vtkvmtkCenterlineSplittingAndGroupingFilter::~vtkvmtkCenterlineSplittingAndGroupingFilter()
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

  if (this->BlankingArrayName)
    {
    delete[] this->BlankingArrayName;
    this->BlankingArrayName = NULL;
    }

  if (this->TractIdsArrayName)
    {
    delete[] this->TractIdsArrayName;
    this->TractIdsArrayName = NULL;
    }

  if (this->SubIds)
    {
    delete[] this->SubIds;
    this->SubIds = NULL;
    }

  if (this->PCoords)
    {
    delete[] this->PCoords;
    this->PCoords = NULL;
    }

  if (this->TractBlanking)
    {
    delete[] this->TractBlanking;
    this->TractBlanking = NULL;
    }
}

int vtkvmtkCenterlineSplittingAndGroupingFilter::RequestData(
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

  vtkIdType numberOfInputCells;

  if (!this->RadiusArrayName)
    {
    vtkErrorMacro(<<"RadiusArrayName not specified");
    return 1;
    }

  if (!input->GetPointData()->GetArray(this->RadiusArrayName))
    {
    vtkErrorMacro(<<"RadiusArray with name specified does not exist");
    return 1;
    }

  if (!this->GroupIdsArrayName)
    {
    vtkErrorMacro(<<"GroupIdsArrayName not specified");
    return 1;
    }

  if (!this->CenterlineIdsArrayName)
    {
    vtkErrorMacro(<<"CenterlineIdsArrayName not specified");
    return 1;
    }

  if (!this->BlankingArrayName)
    {
    vtkErrorMacro(<<"BlankingArrayName not specified");
    return 1;
    }

  if (!this->TractIdsArrayName)
    {
    vtkErrorMacro(<<"TractIdsArrayName not specified");
    return 1;
    }

  numberOfInputCells = input->GetNumberOfCells();

  vtkAppendPolyData* appendCenterlinesFilter  = vtkAppendPolyData::New();

  for (int i=0; i<numberOfInputCells; i++)
    {
    this->ComputeCenterlineSplitting(input,i);
    vtkPolyData* splitCenterline = vtkPolyData::New();
    this->SplitCenterline(input,i,this->NumberOfSplittingPoints,this->SubIds,this->PCoords,this->TractBlanking,splitCenterline);
#if (VTK_MAJOR_VERSION <= 5)
    appendCenterlinesFilter->AddInput(splitCenterline);
#else
    appendCenterlinesFilter->AddInputData(splitCenterline);
#endif
    splitCenterline->Delete();
    }
  
  appendCenterlinesFilter->Update();

  vtkPolyData* centerlineTracts = vtkPolyData::New();
  centerlineTracts->DeepCopy(appendCenterlinesFilter->GetOutput());

  appendCenterlinesFilter->Delete();

  this->GroupTracts(input,centerlineTracts);
  this->MergeTracts(centerlineTracts); 
  //FIXME: now, ill situations may still occour after merging: a branch that was inbetween two same-group and has been merged, might have had a buddy that didn't need to be merged on another centerline. In that case ehat should we do? Leave the other one alone? Or rething the whole thing once for all?
  this->MakeGroupIdsAdjacent(centerlineTracts);
  this->MakeTractIdsAdjacent(centerlineTracts);

  output->DeepCopy(centerlineTracts);

  centerlineTracts->Delete();

  return 1;
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::MergeTracts(vtkPolyData* centerlineTracts)
{
  int numberOfCenterlineCells = centerlineTracts->GetNumberOfCells();
  vtkPolyData* mergedCenterlineTracts = vtkPolyData::New();
  vtkPoints* mergedCenterlineTractsPoints = vtkPoints::New();
  mergedCenterlineTractsPoints->DeepCopy(centerlineTracts->GetPoints());
  mergedCenterlineTracts->SetPoints(mergedCenterlineTractsPoints);
  mergedCenterlineTracts->GetPointData()->DeepCopy(centerlineTracts->GetPointData());
  mergedCenterlineTracts->GetCellData()->CopyAllocate(centerlineTracts->GetCellData());
  
  vtkCellArray* mergedCellArray = vtkCellArray::New();

  vtkIntArray* groupIdsArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->GroupIdsArrayName));
  vtkIdList* centerlineCellIds = vtkIdList::New();
  int i;
  for (i=0; i<numberOfCenterlineCells; i++)
    {
    centerlineCellIds->Initialize();
    vtkvmtkCenterlineUtilities::GetCenterlineCellIds(centerlineTracts,this->CenterlineIdsArrayName,this->TractIdsArrayName,i,centerlineCellIds);
    int numberOfCenterlineTracts = centerlineCellIds->GetNumberOfIds();
    //loop over groupids and keep track of merges (a bit redundant, but clearer)
    int j;
    for (j=0; j<numberOfCenterlineTracts; j++)
      {
      int tractCellId = centerlineCellIds->GetId(j);
      int groupId = groupIdsArray->GetValue(tractCellId);
      int k;
      for (k=numberOfCenterlineTracts-1; k > j; k--)
        {
        int downstreamTractCellId = centerlineCellIds->GetId(k);
        if (groupId == groupIdsArray->GetValue(downstreamTractCellId))
          {
          //merge from j to k
          int l;
          for (l=j+1; l<k; l++)
            {
            groupIdsArray->SetValue(centerlineCellIds->GetId(l),groupId);
            }
          }   
        } 
      }
    
    // at this point we have to merge based on the repeated groupIds (which are now adjacent)
    vtkIdList* tractCellPointIds = vtkIdList::New();
    vtkIdList* mergedCellPointIds = vtkIdList::New();
    int previousGroupId = -1;
    int currentMergedCellId = -1;
    int firstTractCellId = 0;
    for (j=0; j<numberOfCenterlineTracts; j++)
      {
      // copy cell if not repeated, copy first cell and keep adding points if repeated. 
      // Note that point data remains the same, only cells change
      int tractCellId = centerlineCellIds->GetId(j);
      centerlineTracts->GetCellPoints(tractCellId,tractCellPointIds);
      int groupId = groupIdsArray->GetValue(tractCellId);
      if ((groupId != previousGroupId) || (j == numberOfCenterlineTracts-1))
        {
        if (previousGroupId != -1)
          {
          currentMergedCellId = mergedCellArray->InsertNextCell(mergedCellPointIds);
          mergedCenterlineTracts->GetCellData()->CopyData(centerlineTracts->GetCellData(),firstTractCellId,currentMergedCellId);
          }
        firstTractCellId = tractCellId;
        mergedCellPointIds->DeepCopy(tractCellPointIds);
        if (j == numberOfCenterlineTracts-1)
          {
          currentMergedCellId = mergedCellArray->InsertNextCell(mergedCellPointIds);
          mergedCenterlineTracts->GetCellData()->CopyData(centerlineTracts->GetCellData(),firstTractCellId,currentMergedCellId);
          }
        }
      else
        {
        int numberOfTractCellPoints = tractCellPointIds->GetNumberOfIds();
        int k;
        for (k=1; k<numberOfTractCellPoints; k++)
          {
          mergedCellPointIds->InsertNextId(tractCellPointIds->GetId(k));
          }
        }
      previousGroupId = groupId;
      }
    tractCellPointIds->Delete();
    mergedCellPointIds->Delete();
    }

  mergedCenterlineTracts->SetLines(mergedCellArray);
    
  centerlineTracts->DeepCopy(mergedCenterlineTracts);
  
  mergedCellArray->Delete();
  mergedCenterlineTracts->Delete();
  mergedCenterlineTractsPoints->Delete();
  centerlineCellIds->Delete();
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts)
{
  switch (this->GroupingMode)
    {
    case FIRSTPOINT:
      this->CoincidentExtremePointGroupTracts(input,centerlineTracts,true);
      break;
    case LASTPOINT:
      this->CoincidentExtremePointGroupTracts(input,centerlineTracts,false);
      break;
    case POINTINTUBE:
      this->PointInTubeGroupTracts(input,centerlineTracts);
      break;
    default:
      vtkErrorMacro("Unknown GroupingMode");
    }
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::CoincidentExtremePointGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts, bool first)
{
  vtkIntArray* groupIdsArray = vtkIntArray::New();
  groupIdsArray->SetName(this->GroupIdsArrayName);
  int numberOfCells = centerlineTracts->GetNumberOfCells();
  groupIdsArray->SetNumberOfComponents(1);
  groupIdsArray->SetNumberOfTuples(numberOfCells);

  int i;
  for (i=0; i<numberOfCells; i++)
    {
    groupIdsArray->SetValue(i,i);
    }

  // group based on point-in-tube criterion
  for (i=0; i<numberOfCells; i++)
    {
    if (centerlineTracts->GetCell(i)->GetCellType() != VTK_LINE && centerlineTracts->GetCell(i)->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    int tractGroupId = groupIdsArray->GetValue(i);
    double point[3];
    if (first)
      {
      centerlineTracts->GetCell(i)->GetPoints()->GetPoint(0,point);
      }
    else
      {
      int numberOfCellPoints = centerlineTracts->GetCell(i)->GetNumberOfPoints();
      centerlineTracts->GetCell(i)->GetPoints()->GetPoint(numberOfCellPoints-1,point);
      }

    int j;
    for (j=i; j<numberOfCells; j++)
      {
      double currentPoint[3];
      if (first)
        {
        centerlineTracts->GetCell(j)->GetPoints()->GetPoint(0,currentPoint);
        }
      else
        {
        int numberOfCellPoints = centerlineTracts->GetCell(j)->GetNumberOfPoints();
        centerlineTracts->GetCell(j)->GetPoints()->GetPoint(numberOfCellPoints-1,currentPoint);
        }

      if (sqrt(vtkMath::Distance2BetweenPoints(point,currentPoint)) < VTK_VMTK_DOUBLE_TOL)
        {
        groupIdsArray->SetValue(j,tractGroupId);
        }
      } 
    }
 
  centerlineTracts->GetCellData()->AddArray(groupIdsArray);

  groupIdsArray->Delete();
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::PointInTubeGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts)
{
  vtkIntArray* blankingArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->BlankingArrayName));
  vtkIntArray* centerlineIdsArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->CenterlineIdsArrayName));

  vtkIntArray* groupIdsArray = vtkIntArray::New();
  groupIdsArray->SetName(this->GroupIdsArrayName);
  int numberOfCells = centerlineTracts->GetNumberOfCells();
  groupIdsArray->SetNumberOfComponents(1);
  groupIdsArray->SetNumberOfTuples(numberOfCells);

  int i;
  for (i=0; i<numberOfCells; i++)
    {
    groupIdsArray->SetValue(i,i);
    }

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(centerlineTracts);
  tube->SetPolyBallRadiusArrayName(this->RadiusArrayName);

  vtkIdList* tubeCellIds = vtkIdList::New();

  vtkvmtkPolyBallLine* centerlineTube = vtkvmtkPolyBallLine::New();
  centerlineTube->SetInput(centerlineTracts);
  centerlineTube->SetPolyBallRadiusArrayName(this->RadiusArrayName);

  vtkIdList* centerlineTubeCellIds = vtkIdList::New();

  int numberOfCenterlineCells = input->GetNumberOfCells();

  // group based on point-in-tube criterion
  for (i=0; i<numberOfCells; i++)
    {
    // for all points in the present cell, check if there's one which contains it. If yes, put current id equal to the other's group.
    // Alternative: for the first point present cell (i.e. the first reference point), check if there's one tube which contains it. If yes, put current id equal to the other's group.
    if (centerlineTracts->GetCell(i)->GetCellType() != VTK_LINE && centerlineTracts->GetCell(i)->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    centerlineTubeCellIds->Initialize();
    centerlineTubeCellIds->InsertNextId(i);
    centerlineTube->SetInputCellIds(centerlineTubeCellIds);

    int centerlineGroupId = groupIdsArray->GetValue(i);
    
    int centerlineId = centerlineIdsArray->GetValue(i);

    for (int k=0; k<numberOfCenterlineCells; k++)
      {
      if (k == centerlineId)
        {
        continue;
        }
      
      bool centerlineTractAlreadyInGroup = false;
      int j;
      for (j=0; j<numberOfCells; j++)
        {
        if (j==i)
          {
          continue;
          }

        int tubeCenterlineId = centerlineIdsArray->GetValue(j);

        if (tubeCenterlineId != k)
          {
          continue;
          }

        int tubeCenterlineGroupId = groupIdsArray->GetValue(j);
        if (tubeCenterlineGroupId == centerlineGroupId)
          {
          centerlineTractAlreadyInGroup = true;
          }
        }
      
      if (centerlineTractAlreadyInGroup)
        {
        continue;
        }
      
      int sameGroupTubeGroupId = -1;

      for (j=0; j<numberOfCells; j++)
        {
        if (j==i)
          {
          continue;
          }

        int tubeCenterlineId = centerlineIdsArray->GetValue(j);

        if (tubeCenterlineId != k)
          {
          continue;
          }

        // don't group blanked tracts with non-blanked ones and vice-versa
        if (blankingArray->GetValue(i) != blankingArray->GetValue(j))
          {
          continue;
          }

        if ((centerlineTracts->GetCell(j)->GetCellType() != VTK_POLY_LINE) && (centerlineTracts->GetCell(j)->GetCellType() != VTK_LINE))
          {
          continue;
          }
 
        int tubeGroupId = groupIdsArray->GetValue(j);

        if (tubeGroupId == centerlineGroupId)
          {
          continue;
          }

        tubeCellIds->Initialize();
        tubeCellIds->InsertNextId(j);
        tube->SetInputCellIds(tubeCellIds);
        bool pointInTube = false;
        double minTubeValue = VTK_VMTK_LARGE_DOUBLE;
        double minCenterlineTubeValue = VTK_VMTK_LARGE_DOUBLE;
        int l;
//        for (l=0; l<centerlineTracts->GetCell(i)->GetNumberOfPoints(); l++)  // any point in tube criterion
        for (l=0; l<=0; l++)  // first point in tube criterion (divergent networks only)
          {
          double tubeValue = tube->EvaluateFunction(centerlineTracts->GetCell(i)->GetPoints()->GetPoint(l));
          double centerlineTubeValue = centerlineTube->EvaluateFunction(tube->GetLastPolyBallCenter());

          if ((tubeValue < -VTK_VMTK_DOUBLE_TOL) && (centerlineTubeValue < -VTK_VMTK_DOUBLE_TOL))
            {
            pointInTube = true;
            if (tubeValue < minTubeValue)
              {
              minTubeValue = tubeValue;
              }
            if (centerlineTubeValue < minCenterlineTubeValue)
              {
              minCenterlineTubeValue = centerlineTubeValue;
              sameGroupTubeGroupId = tubeGroupId;
              }
            }
          }
        }

      if (sameGroupTubeGroupId == -1)
        {
        continue;
        }

      for (j=0; j<groupIdsArray->GetNumberOfTuples(); j++)
        {
        if (groupIdsArray->GetValue(j) == sameGroupTubeGroupId)
          {
          groupIdsArray->SetValue(j,centerlineGroupId);
          }
        }
      }
    }

  centerlineTracts->GetCellData()->AddArray(groupIdsArray);

  groupIdsArray->Delete();
  tube->Delete();
  tubeCellIds->Delete();
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::MakeGroupIdsAdjacent(vtkPolyData* centerlineTracts)
{
  vtkIntArray* groupIdsArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->GroupIdsArrayName));

  int currentGroupId = 0;
  for (int i=0; i<groupIdsArray->GetNumberOfTuples(); i++)
    {
    int minGroupId = VTK_VMTK_LARGE_INTEGER;
    int j;
    for (j=0; j<groupIdsArray->GetNumberOfTuples(); j++)
      {
      int groupId = groupIdsArray->GetValue(j);
      if ((groupId<minGroupId) && (groupId >= currentGroupId))
        {
        minGroupId = groupId;
        }
      }
    for (j=0; j<groupIdsArray->GetNumberOfTuples(); j++)
      {
      if (groupIdsArray->GetValue(j)==minGroupId)
        {
        groupIdsArray->SetValue(j,currentGroupId);
        }
      }
    ++currentGroupId;
    }
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::MakeTractIdsAdjacent(vtkPolyData* centerlineTracts)
{
  vtkIntArray* centerlineIdsArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->CenterlineIdsArrayName));
  vtkIntArray* tractIdsArray = vtkIntArray::SafeDownCast(centerlineTracts->GetCellData()->GetArray(this->TractIdsArrayName));

  vtkIdList* centerlineIds = vtkIdList::New();
  for (int i=0; i<centerlineIdsArray->GetNumberOfTuples(); i++)
    {
    int centerlineId = centerlineIdsArray->GetValue(i);
    centerlineIds->InsertUniqueId(centerlineId);
    }

  for (int i=0; i<centerlineIds->GetNumberOfIds(); i++)
    {
    int centerlineId = centerlineIds->GetId(i);
    vtkIdList* tractIdsMap = vtkIdList::New();
    int maxTractId = -1;
    for (int j=0; j<tractIdsArray->GetNumberOfTuples(); j++)
      {
      if (centerlineIdsArray->GetValue(j) != centerlineId)
        {
        continue;
        }
      int tractId = tractIdsArray->GetValue(j);
      if (tractId > maxTractId)
        {
        maxTractId = tractId;
        }
      }
    tractIdsMap->SetNumberOfIds(maxTractId+1);
    for (int j=0; j<tractIdsMap->GetNumberOfIds(); j++)
      {
      tractIdsMap->SetId(j,0);
      }
    for (int j=0; j<tractIdsArray->GetNumberOfTuples(); j++)
      {
      if (centerlineIdsArray->GetValue(j) != centerlineId)
        {
        continue;
        }
      int tractId = tractIdsArray->GetValue(j);
      tractIdsMap->SetId(tractId,1);
      }
    int currentTractId = 0;
    for (int j=0; j<tractIdsMap->GetNumberOfIds(); j++)
      {
      if (tractIdsMap->GetId(j) == 0)
        {
        continue;
        }
      tractIdsMap->SetId(j,currentTractId);
      currentTractId += 1;
      }
    for (int j=0; j<tractIdsArray->GetNumberOfTuples(); j++)
      {
      if (centerlineIdsArray->GetValue(j) != centerlineId)
        {
        continue;
        }
      int tractId = tractIdsArray->GetValue(j);
      tractIdsArray->SetValue(j,tractIdsMap->GetId(tractId));
      }
    tractIdsMap->Delete();
    }
  centerlineIds->Delete();
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::SplitCenterline(vtkPolyData* input, vtkIdType cellId, int numberOfSplittingPoints, const vtkIdType* subIds, const double* pcoords, const int* tractBlanking, vtkPolyData* splitCenterline)
{
  vtkPointData* inputPD = input->GetPointData();
  vtkCell* centerline = input->GetCell(cellId);

  if (centerline->GetCellType() != VTK_POLY_LINE && centerline->GetCellType() != VTK_LINE)
    {
    return;
    }

  vtkIdType numberOfCenterlinePoints = centerline->GetNumberOfPoints();

  int numberOfTractPoints;

  splitCenterline->Initialize();
  vtkPoints* splitCenterlinePoints = vtkPoints::New();
  vtkCellArray* splitCenterlineCellArray = vtkCellArray::New();
  vtkIdList* splitCenterlineCellPointIds = vtkIdList::New();
  vtkPointData* splitCenterlinePD = splitCenterline->GetPointData();

  vtkIntArray* centerlineIdsArray = vtkIntArray::New();
  centerlineIdsArray->SetNumberOfComponents(1);
  centerlineIdsArray->SetName(this->CenterlineIdsArrayName);

  vtkIntArray* tractIdsArray = vtkIntArray::New();
  tractIdsArray->SetNumberOfComponents(1);
  tractIdsArray->SetName(this->TractIdsArrayName);

  vtkIntArray* blankingArray = vtkIntArray::New();
  blankingArray->SetNumberOfComponents(1);
  blankingArray->SetName(this->BlankingArrayName);

  int i;
  if (numberOfSplittingPoints == 0)
    {
    splitCenterlinePoints->DeepCopy(centerline->GetPoints());
    splitCenterlineCellArray->InsertNextCell(numberOfCenterlinePoints);
    splitCenterlinePD->CopyAllocate(inputPD,numberOfCenterlinePoints);
    for (i=0; i<numberOfCenterlinePoints; i++)
      {
      splitCenterlinePD->CopyData(inputPD,centerline->GetPointId(i),i);
      splitCenterlineCellArray->InsertCellPoint(i);
      } 
    centerlineIdsArray->InsertNextValue(cellId);
    tractIdsArray->InsertNextValue(0);
    blankingArray->InsertNextValue(0);

    splitCenterline->SetPoints(splitCenterlinePoints);
    splitCenterline->SetLines(splitCenterlineCellArray);
    splitCenterline->GetCellData()->CopyAllocate(input->GetCellData(),1);
    splitCenterline->GetCellData()->CopyData(input->GetCellData(),cellId,0);
    splitCenterline->GetCellData()->AddArray(centerlineIdsArray);
    splitCenterline->GetCellData()->AddArray(tractIdsArray);
    splitCenterline->GetCellData()->AddArray(blankingArray);

    splitCenterlinePoints->Delete();
    splitCenterlineCellArray->Delete();
    splitCenterlineCellPointIds->Delete();
    centerlineIdsArray->Delete();
    blankingArray->Delete();
    return;
    }

  int numberOfSplitCenterlinePoints = 0;
  for (i=0; i<=numberOfSplittingPoints; i++)
    {
    if (tractBlanking[i] == 1)
      {
      continue; 
      }
    vtkIdType lowerId, higherId;

    numberOfTractPoints = 0;

    if (i==0)
      {
      lowerId = 0;
      }
    else
      {
      lowerId = subIds[i-1]+1;
      numberOfTractPoints += 1;
      }
    if (i==numberOfSplittingPoints)
      {
      higherId = numberOfCenterlinePoints-1;
      }
    else
      {
      higherId = subIds[i];
      numberOfTractPoints += 1;
      }
    if (higherId - lowerId > 0)
      {
      numberOfTractPoints += higherId - lowerId;
      }
    numberOfSplitCenterlinePoints += numberOfTractPoints;
    }

  splitCenterlinePD->InterpolateAllocate(inputPD,numberOfSplitCenterlinePoints);
  splitCenterline->GetCellData()->CopyAllocate(input->GetCellData(),numberOfSplittingPoints+1);

  for (i=0; i<=numberOfSplittingPoints; i++)
    {
//     if (tractBlanking[i] == 1)
//       {
//       // don't skip here! [Kept as a caveat!]
//       continue;
//       }

    splitCenterlineCellPointIds->Initialize();

    vtkIdType lowerId, higherId;

    lowerId = (i == 0) ? 0 : subIds[i-1]+1;
    higherId = (i == numberOfSplittingPoints) ? numberOfCenterlinePoints-1 : subIds[i];

    double point[3], point0[3], point1[3];
    vtkIdType pointId;

    // insert first interpolated point if necessary
    int j, k;
    if (i>0)
      {
      centerline->GetPoints()->GetPoint(subIds[i-1],point0);
      centerline->GetPoints()->GetPoint(subIds[i-1]+1,point1);
      for (k=0; k<3; k++) 
        {
        point[k] = point0[k] + pcoords[i-1]*(point1[k] - point0[k]);
        }
      if (vtkMath::Distance2BetweenPoints(point,centerline->GetPoints()->GetPoint(lowerId))>VTK_VMTK_FLOAT_TOL)
        {
        pointId = splitCenterlinePoints->InsertNextPoint(point);
        splitCenterlineCellPointIds->InsertNextId(pointId);
        splitCenterlinePD->InterpolateEdge(inputPD,pointId,centerline->GetPointId(subIds[i-1]),centerline->GetPointId(subIds[i-1]+1),pcoords[i-1]);
        }
      }

    for (j=lowerId; j<=higherId; j++)
      {
      pointId = splitCenterlinePoints->InsertNextPoint(centerline->GetPoints()->GetPoint(j));
      splitCenterlineCellPointIds->InsertNextId(pointId);
      splitCenterlinePD->CopyData(inputPD,centerline->GetPointId(j),pointId);
      }
    
    // insert last interpolated point if necessary
    if (i<numberOfSplittingPoints)
      {
      centerline->GetPoints()->GetPoint(subIds[i],point0);
      centerline->GetPoints()->GetPoint(subIds[i]+1,point1);
      for (k=0; k<3; k++) 
        {
        point[k] = point0[k] + pcoords[i]*(point1[k] - point0[k]);
        }
      if (vtkMath::Distance2BetweenPoints(point,centerline->GetPoints()->GetPoint(higherId))>VTK_VMTK_FLOAT_TOL)
        {
        pointId = splitCenterlinePoints->InsertNextPoint(point);
        splitCenterlineCellPointIds->InsertNextId(pointId);
        splitCenterlinePD->InterpolateEdge(inputPD,pointId,centerline->GetPointId(subIds[i]),centerline->GetPointId(subIds[i]+1),pcoords[i]);
        }
      }

    splitCenterlineCellArray->InsertNextCell(splitCenterlineCellPointIds);
    centerlineIdsArray->InsertNextValue(cellId);
    tractIdsArray->InsertNextValue(i);
    blankingArray->InsertNextValue(tractBlanking[i]);
    splitCenterline->GetCellData()->CopyData(input->GetCellData(),cellId,i);
    }

  splitCenterline->SetPoints(splitCenterlinePoints);
  splitCenterline->SetLines(splitCenterlineCellArray);
  splitCenterline->GetCellData()->AddArray(centerlineIdsArray);
  splitCenterline->GetCellData()->AddArray(tractIdsArray);
  splitCenterline->GetCellData()->AddArray(blankingArray);

  splitCenterlinePoints->Delete();
  splitCenterlineCellArray->Delete();
  splitCenterlineCellPointIds->Delete();
  centerlineIdsArray->Delete();
  blankingArray->Delete();
}

void vtkvmtkCenterlineSplittingAndGroupingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
