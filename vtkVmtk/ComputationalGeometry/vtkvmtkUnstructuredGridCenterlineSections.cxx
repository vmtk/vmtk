/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridCenterlineSections.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkUnstructuredGridCenterlineSections.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyLine.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkStripper.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkMath.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"

vtkCxxRevisionMacro(vtkvmtkUnstructuredGridCenterlineSections, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkvmtkUnstructuredGridCenterlineSections);

vtkvmtkUnstructuredGridCenterlineSections::vtkvmtkUnstructuredGridCenterlineSections()
{
  this->Centerlines = NULL;
}

vtkvmtkUnstructuredGridCenterlineSections::~vtkvmtkUnstructuredGridCenterlineSections()
{
  if (this->Centerlines)
    {
    this->Centerlines->Delete();
    this->Centerlines = NULL;
    }
}

int vtkvmtkUnstructuredGridCenterlineSections::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

int vtkvmtkUnstructuredGridCenterlineSections::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->Centerlines)
    {
    vtkErrorMacro(<<"Centerlines not set");
    return 1;
    }

  vtkAppendPolyData* appendFilter = vtkAppendPolyData::New();

  int numberOfCenterlineCells = this->Centerlines->GetNumberOfCells();
  int i;
  for (i=0; i<numberOfCenterlineCells; i++)
  {
    vtkCell* centerlineCell = this->Centerlines->GetCell(i);
    vtkPoints* centerlineCellPoints = centerlineCell->GetPoints();
    int numberOfCellPoints = centerlineCellPoints->GetNumberOfPoints();
  
    int j;
    for (j=0; j<numberOfCellPoints; j++)
    {
      double point[3];
      centerlineCellPoints->GetPoint(j,point);
  
      double tangent[3];
      tangent[0] = tangent[1] = tangent[2] = 0.0;
  
      double weightSum = 0.0;
      if (j>0)
      {
        double point0[3], point1[3];
        centerlineCellPoints->GetPoint(j-1,point0);
        centerlineCellPoints->GetPoint(j,point1);
        double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        tangent[0] += (point1[0] - point0[0]) / distance;
        tangent[1] += (point1[1] - point0[1]) / distance;
        tangent[2] += (point1[2] - point0[2]) / distance;
        weightSum += 1.0;
      }
  
      if (j<numberOfCellPoints-1)
      {
        double point0[3], point1[3];
        centerlineCellPoints->GetPoint(j,point0);
        centerlineCellPoints->GetPoint(j+1,point1);
        double distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
        tangent[0] += (point1[0] - point0[0]) / distance;
        tangent[1] += (point1[1] - point0[1]) / distance;
        tangent[2] += (point1[2] - point0[2]) / distance;
        weightSum += 1.0;
      }
  
      tangent[0] /= weightSum;
      tangent[1] /= weightSum;
      tangent[2] /= weightSum;
  
      vtkMath::Normalize(tangent);
  
      vtkPlane* plane = vtkPlane::New();
      plane->SetOrigin(point);
      plane->SetNormal(tangent);
    
      vtkCutter* cutter = vtkCutter::New();
      cutter->SetInput(input);
      cutter->SetCutFunction(plane);
      cutter->GenerateCutScalarsOff();
      cutter->SetValue(0,0.0);
      cutter->Update();
    
      vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
      cleaner->SetInput(cutter->GetOutput());
      cleaner->Update();
    
      if (cleaner->GetOutput()->GetNumberOfPoints() == 0)
        {
        plane->Delete();
        cutter->Delete();
        cleaner->Delete();
        continue;
        }
    
      vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
      connectivityFilter->SetInput(cleaner->GetOutput());
      connectivityFilter->SetExtractionModeToClosestPointRegion();
      connectivityFilter->SetClosestPoint(point);
      connectivityFilter->Update();
  
      appendFilter->AddInput(connectivityFilter->GetOutput());
  
      plane->Delete();
      cutter->Delete();
      cleaner->Delete();
      connectivityFilter->Delete();
    }  
  }

  appendFilter->Update();
  output->DeepCopy(appendFilter->GetOutput());
  appendFilter->Delete();

  return 1;
}

void vtkvmtkUnstructuredGridCenterlineSections::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
