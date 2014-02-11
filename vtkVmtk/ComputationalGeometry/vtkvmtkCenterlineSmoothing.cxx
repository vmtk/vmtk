/*=========================================================================

Program:   VTK Blood Vessel Smoothing
Module:    $RCSfile: vtkvmtkCenterlineSmoothing.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
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

#include "vtkvmtkCenterlineSmoothing.h"
#include "vtkvmtkConstants.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyLine.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCenterlineSmoothing);

vtkvmtkCenterlineSmoothing::vtkvmtkCenterlineSmoothing()
{
  this->SmoothingFactor = 0.01;
  this->NumberOfSmoothingIterations = 100;
}

vtkvmtkCenterlineSmoothing::~vtkvmtkCenterlineSmoothing()
{
}

int vtkvmtkCenterlineSmoothing::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->DeepCopy(input);

  for (int i=0; i<input->GetNumberOfCells(); i++)
    {
    vtkCell* line = input->GetCell(i);
    if (line->GetCellType() != VTK_LINE && line->GetCellType() != VTK_POLY_LINE)
      {
      continue;
      }
 
    vtkPoints* smoothLinePoints = vtkPoints::New();
    this->SmoothLine(line->GetPoints(),smoothLinePoints,this->NumberOfSmoothingIterations,this->SmoothingFactor);
   
    int numberOfLinePoints = smoothLinePoints->GetNumberOfPoints();

    for (int j=0; j<numberOfLinePoints; j++)
      {
      vtkIdType pointId = line->GetPointId(j);
      output->GetPoints()->SetPoint(pointId,smoothLinePoints->GetPoint(j));
      }

    smoothLinePoints->Delete();
    }

  return 1;
}

void vtkvmtkCenterlineSmoothing::SmoothLine(vtkPoints* linePoints, vtkPoints* smoothLinePoints, int numberOfIterations, double relaxation)
{
  int numberOfPoints = linePoints->GetNumberOfPoints();

  smoothLinePoints->DeepCopy(linePoints);

  double point0[3];
  double point1[3];
  double point2[3];

  for (int i=0; i<numberOfIterations; i++)
    {
    for (int j=1; j<numberOfPoints-1; j++)
      {
      smoothLinePoints->GetPoint(j-1,point0);
      smoothLinePoints->GetPoint(j  ,point1);
      smoothLinePoints->GetPoint(j+1,point2);

      point1[0] += relaxation * (0.5 * (point0[0] + point2[0]) - point1[0]);
      point1[1] += relaxation * (0.5 * (point0[1] + point2[1]) - point1[1]);
      point1[2] += relaxation * (0.5 * (point0[2] + point2[2]) - point1[2]);

      smoothLinePoints->SetPoint(j,point1);
      }
    }
}

void vtkvmtkCenterlineSmoothing::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
