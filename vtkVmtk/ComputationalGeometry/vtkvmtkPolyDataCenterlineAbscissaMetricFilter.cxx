/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineAbscissaMetricFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
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

#include "vtkvmtkPolyDataCenterlineAbscissaMetricFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineAbscissaMetricFilter);

vtkvmtkPolyDataCenterlineAbscissaMetricFilter::vtkvmtkPolyDataCenterlineAbscissaMetricFilter() 
{
  this->AbscissasArrayName = NULL;
}

vtkvmtkPolyDataCenterlineAbscissaMetricFilter::~vtkvmtkPolyDataCenterlineAbscissaMetricFilter()
{
  if (this->AbscissasArrayName)
    {
    delete[] this->AbscissasArrayName;
    this->AbscissasArrayName = NULL;
    }
}

void vtkvmtkPolyDataCenterlineAbscissaMetricFilter::EvaluateMetric(vtkIdType pointId, double point[3], vtkIdType groupId, vtkDataArray* metricArray) 
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
  if (this->UseRadiusInformation)
    {
    tube->SetPolyBallRadiusArrayName(this->RadiusArrayName);
    }

  double averageAbscissa = 0.0;
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
    double radius = 0.0;
    vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->RadiusArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,radius);
    double weight = radius * radius;
    double abscissa = 0.0;
    vtkvmtkCenterlineUtilities::InterpolateTuple1(this->Centerlines,this->AbscissasArrayName,centerlineCellId,centerlineSubId,centerlinePCoord,abscissa);

    averageAbscissa += weight * abscissa;

    weightSum += weight;
    }

  averageAbscissa /= weightSum;

  metricArray->SetComponent(pointId,0,averageAbscissa);

  centerlineGroupCellIds->Delete();
  tube->Delete();
}
