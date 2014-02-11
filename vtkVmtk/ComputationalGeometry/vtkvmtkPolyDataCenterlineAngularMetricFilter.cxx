/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineAngularMetricFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.9 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataCenterlineAngularMetricFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyLine.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkvmtkConstants.h"
#include "vtkvmtkMath.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineAngularMetricFilter);

vtkvmtkPolyDataCenterlineAngularMetricFilter::vtkvmtkPolyDataCenterlineAngularMetricFilter() 
{
  this->CenterlineNormalsArrayName = NULL;
}

vtkvmtkPolyDataCenterlineAngularMetricFilter::~vtkvmtkPolyDataCenterlineAngularMetricFilter()
{
  if (this->CenterlineNormalsArrayName)
    {
    delete[] this->CenterlineNormalsArrayName;
    this->CenterlineNormalsArrayName = NULL;
    }
}

void vtkvmtkPolyDataCenterlineAngularMetricFilter::EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) 
{
  vtkDataArray* centerlineGroupIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineGroupIdsArrayName);
  vtkDataArray* centerlineTractIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineTractIdsArrayName);
  vtkDataArray* blankingArray = this->Centerlines->GetCellData()->GetArray(this->BlankingArrayName);
  vtkDataArray* centerlineIdsArray = this->Centerlines->GetCellData()->GetArray(this->CenterlineIdsArrayName);

  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  vtkIdList* centerlineGroupCellIds = vtkIdList::New();

  tube->SetInput(this->Centerlines);
  tube->SetInputCellIds(centerlineGroupCellIds);
  tube->SetUseRadiusInformation(this->UseRadiusInformation);
  tube->SetPolyBallRadiusArrayName(this->RadiusArrayName);

  double averagePoint[3];
  averagePoint[0] = averagePoint[1] = averagePoint[2] = 0.0;
  double averageNormal[3];
  averageNormal[0] = averageNormal[1] = averageNormal[2] = 0.0;
  double averageTangent[3];
  averageTangent[0] = averageTangent[1] = averageTangent[2] = 0.0;
  double weightSum = 0.0;

  int numberOfCenterlineCells = this->Centerlines->GetNumberOfCells();
  for (int i=0; i<numberOfCenterlineCells; i++)
    {
    int centerlineGroupId = static_cast<int>(centerlineGroupIdsArray->GetComponent(i,0));
    if (centerlineGroupId != groupId)
      {
      continue;
      }

    centerlineGroupCellIds->Initialize();
    centerlineGroupCellIds->InsertNextId(i);
    
    if (this->IncludeBifurcations)
      {
      int centerlineId = static_cast<int>(centerlineIdsArray->GetComponent(i,0));
      int centerlineTractId = static_cast<int>(centerlineTractIdsArray->GetComponent(i,0));

      for (int j=0; j<numberOfCenterlineCells; j++)
        {
        if (j==i)
          {
          continue;
          }

        int adjacentCenterlineGroupId = static_cast<int>(centerlineGroupIdsArray->GetComponent(j,0));
        int adjacentCenterlineId = static_cast<int>(centerlineIdsArray->GetComponent(j,0));
        int adjacentCenterlineTractId = static_cast<int>(centerlineTractIdsArray->GetComponent(j,0));
        int adjacentBlanking = static_cast<int>(blankingArray->GetComponent(j,0));
        
        if (adjacentBlanking == 0)
          {
          continue;
          }

        if (adjacentCenterlineGroupId == centerlineGroupId)
          {
          continue;
          }

        if (adjacentCenterlineId != centerlineId)
          {
          continue;
          }
        
        if (!((adjacentCenterlineTractId == centerlineTractId - 1) || 
              (adjacentCenterlineTractId == centerlineTractId + 1)))
          {
          continue;
          }

        centerlineGroupCellIds->InsertNextId(j);
        }
      }

    tube->EvaluateFunction(point);

    vtkIdType centerlineCellId = tube->GetLastPolyBallCellId();
    vtkIdType centerlineSubId = tube->GetLastPolyBallCellSubId();
    double centerlinePCoord = tube->GetLastPolyBallCellPCoord();

    vtkCell* polyLine = this->Centerlines->GetCell(centerlineCellId);

    if (polyLine->GetCellType() != VTK_LINE && polyLine->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }

    double radius = 0.0;
    vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->RadiusArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,radius);
    double weight = radius * radius;
    double centerlinePoint[3];
    vtkvmtkCenterlineUtilities::InterpolatePoint(this->Centerlines,centerlineCellId,centerlineSubId,centerlinePCoord,centerlinePoint);

    averagePoint[0] += weight * centerlinePoint[0];
    averagePoint[1] += weight * centerlinePoint[1];
    averagePoint[2] += weight * centerlinePoint[2];

    double centerlinePoint0[3], centerlinePoint1[3];
    polyLine->GetPoints()->GetPoint(centerlineSubId,centerlinePoint0);
    polyLine->GetPoints()->GetPoint(centerlineSubId+1,centerlinePoint1);

    if (vtkMath::Distance2BetweenPoints(centerlinePoint0,centerlinePoint1) < VTK_VMTK_DOUBLE_TOL)
      {
      if (centerlineSubId > 0)
        {
        polyLine->GetPoints()->GetPoint(centerlineSubId-1,centerlinePoint0);
        }
      else if (centerlineSubId+1 < polyLine->GetNumberOfPoints()-1)
        {
        polyLine->GetPoints()->GetPoint(centerlineSubId+2,centerlinePoint1);
        }
      }

    double tangent[3];
    tangent[0] = centerlinePoint1[0] - centerlinePoint0[0];
    tangent[1] = centerlinePoint1[1] - centerlinePoint0[1];
    tangent[2] = centerlinePoint1[2] - centerlinePoint0[2];

    averageTangent[0] += weight * tangent[0];
    averageTangent[1] += weight * tangent[1];
    averageTangent[2] += weight * tangent[2];
    double normal[3];
    vtkvmtkCenterlineUtilities::InterpolateTuple3(this->Centerlines,this->CenterlineNormalsArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,normal);

    averageNormal[0] += weight * normal[0];
    averageNormal[1] += weight * normal[1];
    averageNormal[2] += weight * normal[2];

    weightSum += weight;
    }

  averagePoint[0] /= weightSum;
  averagePoint[1] /= weightSum;
  averagePoint[2] /= weightSum;
  vtkMath::Normalize(averageNormal);
  vtkMath::Normalize(averageTangent);

  double projectedPoint[3];
  vtkPlane::ProjectPoint(point,averagePoint,averageTangent,projectedPoint);

  double positionVector[3];
  positionVector[0] = projectedPoint[0] - averagePoint[0];
  positionVector[1] = projectedPoint[1] - averagePoint[1];
  positionVector[2] = projectedPoint[2] - averagePoint[2];
  vtkMath::Normalize(positionVector);

  double normalPoint[3];
  normalPoint[0] = averagePoint[0] + averageNormal[0];
  normalPoint[1] = averagePoint[1] + averageNormal[1];
  normalPoint[2] = averagePoint[2] + averageNormal[2];

  double projectedNormalPoint[3];
  vtkPlane::ProjectPoint(normalPoint,averagePoint,averageTangent,projectedNormalPoint);

  double projectedNormal[3];
  projectedNormal[0] = projectedNormalPoint[0] - averagePoint[0];
  projectedNormal[1] = projectedNormalPoint[1] - averagePoint[1];
  projectedNormal[2] = projectedNormalPoint[2] - averagePoint[2];
  vtkMath::Normalize(projectedNormal);

  double cross[3];
  vtkMath::Cross(positionVector,projectedNormal,cross);

  double tangentDot = vtkMath::Dot(averageTangent,cross);

  double angle = vtkvmtkMath::AngleBetweenNormals(positionVector,projectedNormal);

  if (tangentDot < 0.0)
    {
    angle *= -1.0;
    }

  metricArray->SetComponent(pointId,0,angle);

  centerlineGroupCellIds->Delete();
  tube->Delete();
}
