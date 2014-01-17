/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBranchExtractor.cxx,v $
Language:  C++
Date:      $Date: 2006/07/07 10:46:19 $
Version:   $Revision: 1.10 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineBranchExtractor.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkPointData.h"
#include "vtkIdList.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineBranchExtractor);

vtkvmtkCenterlineBranchExtractor::vtkvmtkCenterlineBranchExtractor()
{
}

vtkvmtkCenterlineBranchExtractor::~vtkvmtkCenterlineBranchExtractor()
{
}

void vtkvmtkCenterlineBranchExtractor::ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId)
{
  this->NumberOfSplittingPoints = 0;

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

  if (!this->RadiusArrayName)
    {
    return;
    }

  if (input->GetCell(cellId)->GetCellType() != VTK_LINE && input->GetCell(cellId)->GetCellType() != VTK_POLY_LINE)
    {
    return;
    }

  vtkDataArray* radiusArray = input->GetPointData()->GetArray(this->RadiusArrayName);
  
  if (!radiusArray)
    {
    return;
    }

  // TODO: remove assumption: centerlines organized in a tree, parent branches as sources and children as targets

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();

  tube->SetInput(input);
  tube->SetPolyBallRadiusArrayName(this->RadiusArrayName);

  vtkIdList* tubeCellIds = vtkIdList::New();

  vtkIdList* intersectionSubIds = vtkIdList::New();
  vtkDoubleArray* intersectionPCoords = vtkDoubleArray::New();

  vtkIdType numberOfCells = input->GetNumberOfCells();

  //for every other cell than cellId, find intersection of cellId with the tube function of the other cell
  int j;
  for (j=0; j<numberOfCells; j++)
    {
    if (j==cellId)
      {
      continue;
      }

    if (input->GetCell(j)->GetCellType() != VTK_LINE && input->GetCell(j)->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    tubeCellIds->Initialize();
    tubeCellIds->InsertNextId(j);
    tube->SetInputCellIds(tubeCellIds);

    int numberOfCellPoints = input->GetCell(cellId)->GetNumberOfPoints();

    double point0[3], point1[3];
    double tubeValue0, tubeValue1;
    for (int k=0; k<numberOfCellPoints-1; k++)
      {
      input->GetCell(cellId)->GetPoints()->GetPoint(k,point0);
      input->GetCell(cellId)->GetPoints()->GetPoint(k+1,point1);
      tubeValue0 = tube->EvaluateFunction(point0);
      tubeValue1 = tube->EvaluateFunction(point1);

//       if (tubeValue0 * tubeValue1 <= 0.0)
      if ((tubeValue0 <= 0.0) && (tubeValue1 > 0.0))  // this is for divergent networks
        {
        double radius0, radius1;
        radius0 = radiusArray->GetComponent(input->GetCell(cellId)->GetPointId(k),0);
        radius1 = radiusArray->GetComponent(input->GetCell(cellId)->GetPointId(k+1),0);

        //TODO: use Newton iterations
        double segmentLength, stepSize, pcoordStepSize;
        int numberOfSteps;
        double subPoint0[3], subPoint1[3];
        double subTubeValue0, subTubeValue1;
        segmentLength = vtkMath::Distance2BetweenPoints(point0,point1);
        stepSize = 1E-2 * (radius0 + radius1) / 2.0;
        numberOfSteps = (int)ceil(segmentLength / stepSize);
        stepSize = segmentLength / numberOfSteps;
        pcoordStepSize = 1.0 / (double)numberOfSteps;
        
        double pcoord = 0.0;
        int s;
        for (s=0; s<numberOfSteps; s++)
          {
          for (int d=0; d<3; d++) 
            {
            subPoint0[d] = point0[d] + pcoord * (point1[d] - point0[d]);
            subPoint1[d] = point0[d] + (pcoord + pcoordStepSize) * (point1[d] - point0[d]);
            }
          
          subTubeValue0 = tube->EvaluateFunction(subPoint0);
          subTubeValue1 = tube->EvaluateFunction(subPoint1);

//           if (subTubeValue0 * subTubeValue1 <= 0.0)
          if ((subTubeValue0 <= 0.0) && (subTubeValue1 > 0.0))  // this is for divergent networks
            {
            pcoord += pcoordStepSize;
            break;
            }

          pcoord += pcoordStepSize; 
          }

        //since we are considering each other cell separately, we need to decide where to put the intersection point in order to have them sorted along cellId
        int numberOfIntersections = intersectionSubIds->GetNumberOfIds();
        int insertLocation = 0;
        for (s=0; s<numberOfIntersections; s++)
          {
          bool afterThis = (k > intersectionSubIds->GetId(s)) || (k==intersectionSubIds->GetId(s) && pcoord > intersectionPCoords->GetValue(s));
          bool beforeNext = true;
          if (s<numberOfIntersections-1)
            {
            beforeNext = (k < intersectionSubIds->GetId(s+1)) || (k==intersectionSubIds->GetId(s+1) && pcoord <= intersectionPCoords->GetValue(s+1));
            }
          if (afterThis && beforeNext)
            {
            insertLocation = s+1;
            break;
            }
          }
        for (s=numberOfIntersections-1; s>=insertLocation; s--)
          {
          intersectionSubIds->InsertId(s+1,intersectionSubIds->GetId(s));
          intersectionPCoords->InsertValue(s+1,intersectionPCoords->GetValue(s));
          }
        intersectionSubIds->InsertId(insertLocation,k);
        intersectionPCoords->InsertValue(insertLocation,pcoord);
        }
      }
    }
  
  // For each branch, find point at one-sphere distance upstream (i.e. touching forward). 

  vtkIdType touchingSubId;
  double touchingPCoord;

  const int numberOfGapSpheres = 1;
  const bool forward = true;

  vtkIdList* touchingSubIds = vtkIdList::New();
  vtkDoubleArray* touchingPCoords = vtkDoubleArray::New();

  int i;
  for (i=0; i<intersectionSubIds->GetNumberOfIds(); i++)
    {
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,intersectionSubIds->GetId(i),intersectionPCoords->GetValue(i),numberOfGapSpheres,touchingSubId,touchingPCoord,forward);
    if (touchingSubId == -1)
      {
      touchingSubId = 0;
      touchingPCoord = 0.0;
      touchingSubIds->InsertNextId(touchingSubId);
      touchingPCoords->InsertNextValue(touchingPCoord);
      continue;
      }

    touchingSubIds->InsertNextId(touchingSubId);
    touchingPCoords->InsertNextValue(touchingPCoord);
    }

  vtkIdList* splittingSubIds = vtkIdList::New();
  vtkDoubleArray* splittingPCoords = vtkDoubleArray::New();
  vtkIdList* blankingFlags = vtkIdList::New();

  blankingFlags->InsertNextId(0);
/*
  for (i=0; i<intersectionSubIds->GetNumberOfIds(); i++)
    {
    // put one intersection every touching. Blank out between touching and intersection.
    if (i==0)
      {
      splittingSubIds->InsertNextId(touchingSubIds->GetId(i));
      splittingPCoords->InsertNextValue(touchingPCoords->GetValue(i)); 
      blankingFlags->InsertNextId(1);
      }
    else
      {
      // check if this touching is before previous intersection. If it is, skip the touching.
      if (!((touchingSubIds->GetId(i) < intersectionSubIds->GetId(i-1)) || 
            ((touchingSubIds->GetId(i) == intersectionSubIds->GetId(i-1)) && (touchingPCoords->GetValue(i) <= intersectionPCoords->GetValue(i-1)))))
        {
        splittingSubIds->InsertNextId(touchingSubIds->GetId(i));
        splittingPCoords->InsertNextValue(touchingPCoords->GetValue(i)); 
        blankingFlags->InsertNextId(1);
        }
      }

    if (i==intersectionSubIds->GetNumberOfIds()-1)
      {
      splittingSubIds->InsertNextId(intersectionSubIds->GetId(i));
      splittingPCoords->InsertNextValue(intersectionPCoords->GetValue(i));
      blankingFlags->InsertNextId(0);
      }
    else
      {
      // check if subsequent touching is before this intersection. If it is, and if subsequent intersection is after this intersection, skip the intersection.
      if (!
           (((touchingSubIds->GetId(i+1) < intersectionSubIds->GetId(i)) || 
            ((touchingSubIds->GetId(i+1) == intersectionSubIds->GetId(i)) && (touchingPCoords->GetValue(i+1) <= intersectionPCoords->GetValue(i))))) &&
           (((intersectionSubIds->GetId(i+1) > intersectionSubIds->GetId(i)) || 
            ((intersectionSubIds->GetId(i+1) == intersectionSubIds->GetId(i)) && (intersectionPCoords->GetValue(i+1) > intersectionPCoords->GetValue(i))))) )
        {
        splittingSubIds->InsertNextId(intersectionSubIds->GetId(i));
        splittingPCoords->InsertNextValue(intersectionPCoords->GetValue(i));
        blankingFlags->InsertNextId(0);
        }
      }
    }
*/

  //for every touching, put one intersection, and blank in between. If a subsequent touching falls between a previous touching and the corresponding intersection, take the farthest intersection
  vtkIdType prevIntersectionId = -1;
  for (i=0; i<touchingSubIds->GetNumberOfIds(); i++)
    {
    if (i > 0)
      {
      if ((touchingSubIds->GetId(i) < intersectionSubIds->GetId(prevIntersectionId)) || 
         ((touchingSubIds->GetId(i) == intersectionSubIds->GetId(prevIntersectionId)) && (touchingPCoords->GetValue(i) <= intersectionPCoords->GetValue(prevIntersectionId))))
        {
        continue;
        }
      }

    splittingSubIds->InsertNextId(touchingSubIds->GetId(i));
    splittingPCoords->InsertNextValue(touchingPCoords->GetValue(i)); 
    blankingFlags->InsertNextId(1);

    vtkIdType maxIntersectionId = i;

    if (i < touchingSubIds->GetNumberOfIds()-1)
      {
      for (j=i+1; j<touchingSubIds->GetNumberOfIds(); j++)
        {
        if ((touchingSubIds->GetId(j) < intersectionSubIds->GetId(maxIntersectionId)) || 
           ((touchingSubIds->GetId(j) == intersectionSubIds->GetId(maxIntersectionId)) && (touchingPCoords->GetValue(j) <= intersectionPCoords->GetValue(maxIntersectionId))))
          {
          if ((intersectionSubIds->GetId(j) > intersectionSubIds->GetId(maxIntersectionId)) || 
              ((intersectionSubIds->GetId(j) == intersectionSubIds->GetId(maxIntersectionId)) && (intersectionPCoords->GetValue(j) >= intersectionPCoords->GetValue(maxIntersectionId))))
            {
            maxIntersectionId = j;
            }
          }
        else
          {
          break;
          }
        }
      }
 
    splittingSubIds->InsertNextId(intersectionSubIds->GetId(maxIntersectionId));
    splittingPCoords->InsertNextValue(intersectionPCoords->GetValue(maxIntersectionId)); 
    blankingFlags->InsertNextId(0);

    prevIntersectionId = maxIntersectionId;
    }

  this->NumberOfSplittingPoints = splittingSubIds->GetNumberOfIds();

  this->SubIds = new vtkIdType[this->NumberOfSplittingPoints];
  this->PCoords = new double[this->NumberOfSplittingPoints];

  for (i=0; i<splittingSubIds->GetNumberOfIds(); i++)
    {
    this->SubIds[i] = splittingSubIds->GetId(i);
    this->PCoords[i] = splittingPCoords->GetValue(i);
    }

  this->TractBlanking = new int[this->NumberOfSplittingPoints+1];

  for (i=0; i<this->NumberOfSplittingPoints+1; i++)
    {
    this->TractBlanking[i] = blankingFlags->GetId(i);
    }

  tube->Delete();
  tubeCellIds->Delete();

  intersectionSubIds->Delete();
  intersectionPCoords->Delete();

  touchingSubIds->Delete();
  touchingPCoords->Delete();

  splittingSubIds->Delete();
  splittingPCoords->Delete();
  blankingFlags->Delete();
}

void vtkvmtkCenterlineBranchExtractor::GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts)
{
  Superclass::GroupTracts(input,centerlineTracts);

  //New ideas:
  // loop over group ids, if blanked group, if same centerlineId as another tract in same group, make it another group. And what about the rest? No, better: assume net is a tree. For every group, look at which tracts of each centerline are downstream (via TractIdsArray) and group them in a bifurcation. In order to relax the assumption on the tree, for every group, for every direction, look for all the groups to which the next next tracts belong (tractId + 2 or -2).
}

void vtkvmtkCenterlineBranchExtractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
