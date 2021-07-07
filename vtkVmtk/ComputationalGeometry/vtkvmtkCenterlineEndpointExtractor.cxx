/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineEndpointExtractor.cxx,v $
Language:  C++
Date:      $Date: 2005/10/06 11:01:44 $
Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkCenterlineEndpointExtractor.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineEndpointExtractor);

vtkvmtkCenterlineEndpointExtractor::vtkvmtkCenterlineEndpointExtractor()
{
  this->NumberOfEndpointSpheres = 2;
  this->NumberOfGapSpheres = 1;
  this->ExtractionMode = VTK_VMTK_BOTH_ENDPOINTS;
}

vtkvmtkCenterlineEndpointExtractor::~vtkvmtkCenterlineEndpointExtractor()
{
}

void vtkvmtkCenterlineEndpointExtractor::ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId)
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

  vtkCell* centerline = input->GetCell(cellId);

  if (centerline->GetCellType() != VTK_LINE && centerline->GetCellType() != VTK_POLY_LINE)
    {
    return;
    }

  this->NumberOfSplittingPoints = 4;
  if (this->ExtractionMode == VTK_VMTK_FIRST_ENDPOINT || this->ExtractionMode == VTK_VMTK_LAST_ENDPOINT)
    {
    this->NumberOfSplittingPoints = 2;
    }

  this->SubIds = new vtkIdType[this->NumberOfSplittingPoints];
  this->PCoords = new double[this->NumberOfSplittingPoints];

  vtkIdType firstSubId, lastSubId;
  double firstPCoord, lastPCoord;

  firstSubId = 0;
  firstPCoord = 0.0;

  lastSubId = centerline->GetNumberOfPoints() - 2;
  lastPCoord = 1.0;

  if (this->ExtractionMode == VTK_VMTK_FIRST_ENDPOINT)
    {
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,firstSubId,firstPCoord,this->NumberOfEndpointSpheres,this->SubIds[0],this->PCoords[0],false);
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,this->SubIds[0],this->PCoords[0],this->NumberOfGapSpheres,this->SubIds[1],this->PCoords[1],false);
    }
  else if (this->ExtractionMode == VTK_VMTK_LAST_ENDPOINT)
    {
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,lastSubId,lastPCoord,this->NumberOfEndpointSpheres,this->SubIds[1],this->PCoords[1],true);
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,this->SubIds[1],this->PCoords[1],this->NumberOfGapSpheres,this->SubIds[0],this->PCoords[0],true);
    if (this->SubIds[0] == -1)
      {
      this->SubIds[0] = 0;
      this->PCoords[0] = 0.0;
      }
    if (this->SubIds[1] == -1)
      {
      this->SubIds[1] = 0;
      this->PCoords[1] = 0.0;
      }
    }
  else
    {
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,firstSubId,firstPCoord,this->NumberOfEndpointSpheres,this->SubIds[0],this->PCoords[0],false);
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,this->SubIds[0],this->PCoords[0],this->NumberOfGapSpheres,this->SubIds[1],this->PCoords[1],false);
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,lastSubId,lastPCoord,this->NumberOfEndpointSpheres,this->SubIds[3],this->PCoords[3],true);
    vtkvmtkCenterlineSphereDistance::FindNTouchingSphereCenter(input,this->RadiusArrayName,cellId,this->SubIds[3],this->PCoords[3],this->NumberOfGapSpheres,this->SubIds[2],this->PCoords[2],true);
    }

  this->TractBlanking = new int[this->NumberOfSplittingPoints+1];

  if (this->ExtractionMode == VTK_VMTK_FIRST_ENDPOINT || this->ExtractionMode == VTK_VMTK_LAST_ENDPOINT)
    {
    this->TractBlanking[0] = 0;
    this->TractBlanking[1] = 1;
    this->TractBlanking[2] = 0;
    }
  else
    {
    this->TractBlanking[0] = 0;
    this->TractBlanking[1] = 1;
    this->TractBlanking[2] = 0;
    this->TractBlanking[3] = 1;
    this->TractBlanking[4] = 0;
    }
}

void vtkvmtkCenterlineEndpointExtractor::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
