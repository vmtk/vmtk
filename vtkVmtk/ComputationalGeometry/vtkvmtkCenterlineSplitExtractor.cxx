/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSplitExtractor.cxx,v $
Language:  C++
Date:      $Date: 2005/10/06 11:01:44 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineSplitExtractor.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkvmtkPolyBallLine.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineSplitExtractor);

vtkvmtkCenterlineSplitExtractor::vtkvmtkCenterlineSplitExtractor()
{
  this->SplitPoint[0] = 0.0;
  this->SplitPoint[1] = 0.0;
  this->SplitPoint[2] = 0.0;

  this->SplitPoint2[0] = 0.0;
  this->SplitPoint2[1] = 0.0;
  this->SplitPoint2[2] = 0.0;

  this->Tolerance = 1E-4;

  this->GapLength = 1.0;

  this->SplittingMode = POINTANDGAP;
  this->GroupingMode = FIRSTPOINT;
}

vtkvmtkCenterlineSplitExtractor::~vtkvmtkCenterlineSplitExtractor()
{
}

void vtkvmtkCenterlineSplitExtractor::ComputePointAndGapCenterlineSplitting(vtkPolyData* input, vtkIdType cellId)
{
  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(input);
  tube->SetInputCellId(cellId);
  tube->UseRadiusInformationOff();
  double tubeFunctionValue = tube->EvaluateFunction(this->SplitPoint);

  if (tubeFunctionValue > this->Tolerance)
    {
    this->NumberOfSplittingPoints = 0;
    return;
    }

  vtkIdType centerlineSubId = tube->GetLastPolyBallCellSubId();
  double centerlinePCoord = tube->GetLastPolyBallCellPCoord();
  double centerlinePoint[3];
  tube->GetLastPolyBallCenter(centerlinePoint);

  this->NumberOfSplittingPoints = 2;

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

  vtkCell* centerline = input->GetCell(cellId);

  if (centerline->GetCellType() != VTK_LINE && centerline->GetCellType() != VTK_POLY_LINE)
    {
    return;
    }

  this->SubIds = new vtkIdType[this->NumberOfSplittingPoints];
  this->PCoords = new double[this->NumberOfSplittingPoints];

  int numberOfCenterlinePoints = centerline->GetNumberOfPoints();
  int numberOfCenterlineSubIds = centerline->GetNumberOfPoints() - 1;

  double currentAbscissa = 0.0;
  vtkDoubleArray* abscissas = vtkDoubleArray::New();
  abscissas->InsertNextValue(currentAbscissa);
  double point[3], previousPoint[3];
  centerline->GetPoints()->GetPoint(0,previousPoint);
  int i;
  for (i=1; i<numberOfCenterlinePoints; i++)
    {
    centerline->GetPoints()->GetPoint(i,point);
    currentAbscissa += sqrt(vtkMath::Distance2BetweenPoints(previousPoint,point));
    abscissas->InsertNextValue(currentAbscissa);
    previousPoint[0] = point[0];
    previousPoint[1] = point[1];
    previousPoint[2] = point[2];
    }

  double halfGapLength = this->GapLength * 0.5;

  double centerlinePointAbscissa = (1.0 - centerlinePCoord) * abscissas->GetValue(centerlineSubId) + centerlinePCoord * abscissas->GetValue(centerlineSubId+1);

  double abscissa;
  int lastSubId = numberOfCenterlineSubIds - 1;
  for (i=centerlineSubId; i<numberOfCenterlineSubIds; i++)
    {
    abscissa = abscissas->GetValue(i+1);
    if (abscissa - centerlinePointAbscissa > halfGapLength)
      {
      lastSubId = i;
      break;
      }
    }
  double lastPCoord = (centerlinePointAbscissa + halfGapLength - abscissas->GetValue(lastSubId)) / (abscissas->GetValue(lastSubId+1) - abscissas->GetValue(lastSubId));
  if (lastPCoord > 1.0)
    {
    lastPCoord = 1.0 - 1E-1;
    }

  int firstSubId = 0;
  for (i=centerlineSubId; i>=0; i--)
    {
    abscissa = abscissas->GetValue(i);
    if (centerlinePointAbscissa - abscissa > halfGapLength)
      {
      firstSubId = i;
      break;
      }
    }
  double firstPCoord = (centerlinePointAbscissa - halfGapLength - abscissas->GetValue(firstSubId)) / (abscissas->GetValue(firstSubId+1) - abscissas->GetValue(firstSubId));
  if (firstPCoord < 0.0)
    {
    firstPCoord = 0.0 + 1E-1;
    }

  this->SubIds[0] = firstSubId;
  this->PCoords[0] = firstPCoord;

  this->SubIds[1] = lastSubId;
  this->PCoords[1] = lastPCoord;

  this->TractBlanking = new int[this->NumberOfSplittingPoints+1];

  this->TractBlanking[0] = 0;
  this->TractBlanking[1] = 1;
  this->TractBlanking[2] = 0;

  abscissas->Delete();
}

void vtkvmtkCenterlineSplitExtractor::ComputeBetweenPointsCenterlineSplitting(vtkPolyData* input, vtkIdType cellId)
{
  vtkvmtkPolyBallLine* tube = vtkvmtkPolyBallLine::New();
  tube->SetInput(input);
  tube->SetInputCellId(cellId);
  tube->UseRadiusInformationOff();
  double tubeFunctionValue = tube->EvaluateFunction(this->SplitPoint);

  if (tubeFunctionValue > this->Tolerance)
    {
    this->NumberOfSplittingPoints = 0;
    return;
    }

  vtkIdType centerlineSubId = tube->GetLastPolyBallCellSubId();
  double centerlinePCoord = tube->GetLastPolyBallCellPCoord();
  double centerlinePoint[3];
  tube->GetLastPolyBallCenter(centerlinePoint);

  double tubeFunctionValue2 = tube->EvaluateFunction(this->SplitPoint2);

  if (tubeFunctionValue2 > this->Tolerance)
    {
    this->NumberOfSplittingPoints = 0;
    return;
    }

  vtkIdType centerlineSubId2 = tube->GetLastPolyBallCellSubId();
  double centerlinePCoord2 = tube->GetLastPolyBallCellPCoord();
  double centerlinePoint2[3];
  tube->GetLastPolyBallCenter(centerlinePoint2);

  this->NumberOfSplittingPoints = 2;

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

  vtkCell* centerline = input->GetCell(cellId);

  if (centerline->GetCellType() != VTK_LINE && centerline->GetCellType() != VTK_POLY_LINE)
    {
    return;
    }

  this->SubIds = new vtkIdType[this->NumberOfSplittingPoints];
  this->PCoords = new double[this->NumberOfSplittingPoints];

  if (centerlineSubId > centerlineSubId2 || (centerlineSubId == centerlineSubId2 && centerlinePCoord > centerlinePCoord2))
    {
    this->SubIds[0] = centerlineSubId;
    this->PCoords[0] = centerlinePCoord;

    this->SubIds[1] = centerlineSubId2;
    this->PCoords[1] = centerlinePCoord2;
    }
  else
    {
    this->SubIds[0] = centerlineSubId2;
    this->PCoords[0] = centerlinePCoord2;

    this->SubIds[1] = centerlineSubId;
    this->PCoords[1] = centerlinePCoord;
    }

  this->TractBlanking = new int[this->NumberOfSplittingPoints+1];

  this->TractBlanking[0] = 0;
  this->TractBlanking[1] = 1;
  this->TractBlanking[2] = 0;
}

void vtkvmtkCenterlineSplitExtractor::ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId)
{
  if (this->SplittingMode == POINTANDGAP)
    {
    this->ComputePointAndGapCenterlineSplitting(input,cellId);
    }
  else if (this->SplittingMode == BETWEENPOINTS)
    {
    this->ComputeBetweenPointsCenterlineSplitting(input,cellId);
    }
  else
    {
    vtkErrorMacro("Error: unsupported splitting mode");
    }
}

void vtkvmtkCenterlineSplitExtractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
