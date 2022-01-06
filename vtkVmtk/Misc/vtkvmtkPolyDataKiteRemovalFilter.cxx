/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataKiteRemovalFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkPolyDataKiteRemovalFilter.h"
#include "vtkvmtkConstants.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkTriangle.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkVersion.h"


vtkStandardNewMacro(vtkvmtkPolyDataKiteRemovalFilter);

vtkvmtkPolyDataKiteRemovalFilter::vtkvmtkPolyDataKiteRemovalFilter()
{
  this->SizeFactor = 0.1;
}

vtkvmtkPolyDataKiteRemovalFilter::~vtkvmtkPolyDataKiteRemovalFilter()
{
}

int vtkvmtkPolyDataKiteRemovalFilter::RequestData(
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

  vtkTriangleFilter* triangleFilter = vtkTriangleFilter::New();
  triangleFilter->SetInputData(input);
  triangleFilter->PassLinesOff();
  triangleFilter->PassVertsOff();
  triangleFilter->Update();
  
  vtkPolyData* surface = vtkPolyData::New();
  surface->CopyStructure(triangleFilter->GetOutput());
  surface->GetPointData()->PassData(triangleFilter->GetOutput()->GetPointData());
  surface->GetCellData()->PassData(triangleFilter->GetOutput()->GetCellData());
  surface->GetFieldData()->PassData(triangleFilter->GetOutput()->GetFieldData());
  surface->BuildCells();
  surface->BuildLinks();

  double averageTriangleArea = 0.0;
  int numberOfCells = surface->GetNumberOfCells();
  double point[3], point1[3], point2[3];
  vtkIdList* cellPointIds = vtkIdList::New();
  for (vtkIdType cellId=0; cellId<numberOfCells; cellId++)
    {
    cellPointIds->Initialize();
    surface->GetCellPoints(cellId,cellPointIds);
    surface->GetPoint(cellPointIds->GetId(0),point);
    surface->GetPoint(cellPointIds->GetId(1),point1);
    surface->GetPoint(cellPointIds->GetId(2),point2);
    averageTriangleArea += vtkTriangle::TriangleArea(point,point1,point2);
    }
  averageTriangleArea /= numberOfCells;

  int numberOfPoints = surface->GetNumberOfPoints();
  vtkIdList* cellIds = vtkIdList::New();
  for (int pointId=0; pointId<numberOfPoints; pointId++)
    {
    surface->GetPoint(pointId,point);
    cellIds->Initialize();
    surface->GetPointCells(pointId,cellIds);
    if (cellIds->GetNumberOfIds() != 4)
      {
      continue;
      }
    double localAverageTriangleArea = 0.0;
    int numberOfCellIds = cellIds->GetNumberOfIds();
    int i, j;
    for (i=0; i<numberOfCellIds; i++)
      {
      vtkIdType cellId = cellIds->GetId(i);
      cellPointIds->Initialize();
      surface->GetCellPoints(cellId,cellPointIds);
      surface->GetPoint(cellPointIds->GetId(0),point);
      surface->GetPoint(cellPointIds->GetId(1),point1);
      surface->GetPoint(cellPointIds->GetId(2),point2);
      localAverageTriangleArea += vtkTriangle::TriangleArea(point,point1,point2);
      }
    localAverageTriangleArea /= numberOfCellIds;
    if (localAverageTriangleArea < VTK_VMTK_DOUBLE_TOL)
      {
      continue;
      }
    if (localAverageTriangleArea > this->SizeFactor * averageTriangleArea)
      {
      continue;
      }
    for (i=0; i<numberOfCellIds; i++)
      {
      vtkIdType cellId = cellIds->GetId(i);
      cellPointIds->Initialize();
      surface->GetCellPoints(cellId,cellPointIds);
      surface->GetPoint(cellPointIds->GetId(0),point);
      surface->GetPoint(cellPointIds->GetId(1),point1);
      surface->GetPoint(cellPointIds->GetId(2),point2);
      double area = vtkTriangle::TriangleArea(point,point1,point2);
      if (area > this->SizeFactor * averageTriangleArea)
        {
        continue;
        }
      int numberOfCellPointIds = cellPointIds->GetNumberOfIds();
      for (j=0; j<numberOfCellPointIds; j++)
        {
        vtkIdType cellPointId = cellPointIds->GetId(j);
        surface->GetPoints()->SetPoint(cellPointId,point);
        }
      }
    }

  vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
  cleaner->SetInputData(surface);

  vtkTriangleFilter* triangleFilter2 = vtkTriangleFilter::New();
  triangleFilter2->SetInputConnection(cleaner->GetOutputPort());
  triangleFilter2->PassLinesOff();
  triangleFilter2->PassVertsOff();
  triangleFilter2->Update();

  output->CopyStructure(triangleFilter2->GetOutput());
  output->GetPointData()->PassData(triangleFilter2->GetOutput()->GetPointData());
  output->GetCellData()->PassData(triangleFilter2->GetOutput()->GetCellData());
  output->GetFieldData()->PassData(triangleFilter2->GetOutput()->GetFieldData());

  triangleFilter->Delete();
  triangleFilter2->Delete();
  cleaner->Delete();
  surface->Delete();
  cellPointIds->Delete();
  cellIds->Delete();

  return 1;
}

void vtkvmtkPolyDataKiteRemovalFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
