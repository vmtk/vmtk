/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSphereDistance.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
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

#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
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
#include "vtkObjectFactory.h"



vtkStandardNewMacro(vtkvmtkCenterlineSphereDistance);


void vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(vtkPolyData* centerlines, const char* radiusArrayName, vtkIdType cellId, vtkIdType subId, double pcoord, int numberOfTouchingSpheres, vtkIdType& touchingSubId, double& touchingPCoord, bool forward)
{
  vtkIdType dummySubId0, dummySubId1;
  double dummyPCoord0, dummyPCoord1;

  if (numberOfTouchingSpheres == 0)
  {
    touchingSubId = subId;
    touchingPCoord = pcoord;
    return;
  }
  
  dummySubId0 = subId;
  dummyPCoord0 = pcoord;

  for (int i=0; i<numberOfTouchingSpheres; i++)
    {
    vtkvmtkCenterlineSphereDistance::FindTouchingSphereCenter(centerlines,radiusArrayName,cellId,dummySubId0,dummyPCoord0,dummySubId1,dummyPCoord1,forward);
    if (dummySubId1 == -1)
      {
      break;
      }
    dummySubId0 = dummySubId1;
    dummyPCoord0 = dummyPCoord1;
    }

  touchingSubId = dummySubId1;
  touchingPCoord = dummyPCoord1;
}

void vtkvmtkCenterlineSphereDistance::FindTouchingSphereCenter(vtkPolyData* centerlines, const char* radiusArrayName, vtkIdType cellId, vtkIdType subId, double pcoord, vtkIdType& touchingSubId, double& touchingPCoord, bool forward)
{
  touchingSubId = -1;
  touchingPCoord = 0.0;

  vtkCell* centerline = centerlines->GetCell(cellId);
  
  if (centerline->GetCellType() != VTK_LINE && centerline->GetCellType() != VTK_POLY_LINE)
    {
    return;
    }

  if (!radiusArrayName)
    {
    return;
    }

  vtkDataArray* radiusArray = centerlines->GetPointData()->GetArray(radiusArrayName);
  
  if (!radiusArray)
    {
    return;
    }

  vtkIdList* subIds = vtkIdList::New();

  int k;
  if (forward)
    {
    for (k=0; k<subId; k++)
      {
      subIds->InsertNextId(k);
      }
    }
  else
    {
    for (k=centerline->GetNumberOfPoints()-2; k>subId; k--)
      {
      subIds->InsertNextId(k);
      }
    }

  double point[3], point0[3], point1[3];
  centerline->GetPoints()->GetPoint(subId,point0);
  centerline->GetPoints()->GetPoint(subId+1,point1);
  for (k=0; k<3; k++) 
    {
    point[k] = point0[k] + pcoord*(point1[k] - point0[k]);
    }

  vtkIdType centerId0, centerId1;
  double center0[3], center1[3], radius0, radius1;
  vtkIdType currentSubId;
  double currentPCoord;
  double sphereDistance0, sphereDistance1;

  touchingSubId = -1;
  touchingPCoord = 0.0;

  int i;
  for (i=0; i<subIds->GetNumberOfIds(); i++)
    {
    currentSubId = subIds->GetId(i);
    
    centerId0 = centerline->GetPointId(currentSubId);
    centerId1 = centerline->GetPointId(currentSubId+1);
    centerlines->GetPoint(centerId0,center0);
    centerlines->GetPoint(centerId1,center1);
    radius0 = radiusArray->GetComponent(centerId0,0);
    radius1 = radiusArray->GetComponent(centerId1,0);    
    sphereDistance0 = vtkvmtkMath::EvaluateSphereFunction(center0,radius0,point);
    sphereDistance1 = vtkvmtkMath::EvaluateSphereFunction(center1,radius1,point);

    if (forward)
      {
      if ((sphereDistance0 > 0.0) && (sphereDistance1 <= 0.0))
        {
        touchingSubId = currentSubId;
        break;
        }
      }
    else
      {
      if ((sphereDistance0 <= 0.0) && (sphereDistance1 > 0.0))
        {
        touchingSubId = currentSubId;
        break;
        }
      }
    }

  if (touchingSubId == -1)
    {
    centerId0 = centerline->GetPointId(subId);
    centerId1 = centerline->GetPointId(subId+1);
    centerlines->GetPoint(centerId0,center0);
    centerlines->GetPoint(centerId1,center1);
    radius0 = radiusArray->GetComponent(centerId0,0);
    radius1 = radiusArray->GetComponent(centerId1,0);

    sphereDistance0 = vtkvmtkMath::EvaluateSphereFunction(center0,radius0,point);
    sphereDistance1 = vtkvmtkMath::EvaluateSphereFunction(center1,radius1,point);

    if ((sphereDistance0<=0.0) && (sphereDistance1<=0.0))
      {
      touchingSubId = -1;
      return;
      }

    touchingSubId = subId;
    }

  // optimize between (touchingSubId, touchingSubId+1)

  // linear search, step proportional to sphere radius.

  centerId0 = centerline->GetPointId(touchingSubId);
  centerId1 = centerline->GetPointId(touchingSubId+1);
  centerlines->GetPoint(centerId0,center0);
  centerlines->GetPoint(centerId1,center1);
  radius0 = radiusArray->GetComponent(centerId0,0);
  radius1 = radiusArray->GetComponent(centerId1,0);

  double segmentLength, stepSize, pcoordStepSize;
  int numberOfSteps;
  double subCenter0[3], subCenter1[3], subRadius0, subRadius1;
  segmentLength = vtkMath::Distance2BetweenPoints(center0,center1);
  stepSize = 1E-6 * (radius0 + radius1) / 2.0;
  numberOfSteps = (int)ceil(segmentLength / stepSize);
  stepSize = segmentLength / numberOfSteps;
  pcoordStepSize = 1.0 / (double)numberOfSteps;

  touchingPCoord = 0.0;
  currentPCoord = 0.0;
  for (i=0; i<numberOfSteps; i++)
    {
    for (int k=0; k<3; k++) 
      {
      subCenter0[k] = center0[k] + currentPCoord*(center1[k] - center0[k]);
      subCenter1[k] = center0[k] + (currentPCoord+pcoordStepSize)*(center1[k] - center0[k]);
      }
    subRadius0 = radius0 + currentPCoord*(radius1 - radius0);
    subRadius1 = radius0 + (currentPCoord+pcoordStepSize)*(radius1 - radius0);

    sphereDistance0 = vtkvmtkMath::EvaluateSphereFunction(subCenter0,subRadius0,point);
    sphereDistance1 = vtkvmtkMath::EvaluateSphereFunction(subCenter1,subRadius1,point);

    if (forward)
      {
      if ((sphereDistance0 > 0.0) && (sphereDistance1 <= 0.0))
        {
        touchingPCoord = currentPCoord;
        break;
        }
      }
    else
      {
      if ((sphereDistance0 <= 0.0) && (sphereDistance1 > 0.0))
        {
        touchingPCoord = currentPCoord+pcoordStepSize;
        break;
        }
      }
    currentPCoord += pcoordStepSize;
    }

  if (forward)
    {
    if ((touchingSubId == subId) && (touchingPCoord > pcoord))
      {
      touchingSubId = -1;
      touchingPCoord = 0.0;
      }
    }
  else
    {
    if ((touchingSubId == subId) && (touchingPCoord < pcoord))
      {
      touchingSubId = -1;
      touchingPCoord = 0.0;
      }
    }

  subIds->Delete();
}

void vtkvmtkCenterlineSphereDistance::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
