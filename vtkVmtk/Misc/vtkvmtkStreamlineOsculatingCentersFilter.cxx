/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineOsculatingCentersFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
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

#include "vtkvmtkStreamlineOsculatingCentersFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkvmtkConstants.h"

#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkSplineFilter.h"
//#include "vtkPointLocator.h"
#include "vtkCellArray.h"
#include "vtkCell.h"

vtkCxxRevisionMacro(vtkvmtkStreamlineOsculatingCentersFilter, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkvmtkStreamlineOsculatingCentersFilter);

vtkvmtkStreamlineOsculatingCentersFilter::vtkvmtkStreamlineOsculatingCentersFilter()
{
}

vtkvmtkStreamlineOsculatingCentersFilter::~vtkvmtkStreamlineOsculatingCentersFilter()
{
}

int vtkvmtkStreamlineOsculatingCentersFilter::RequestData(
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

  if (input->GetNumberOfPoints() < 1)
    {
    return 1;
    }

  double resampleLength = 0.1;
  double thresholdRadius = 0.1;

  vtkSplineFilter* splineFilter = vtkSplineFilter::New();
  splineFilter->SetInput(input);
  splineFilter->SetSubdivideToLength();
  splineFilter->SetLength(resampleLength);
  splineFilter->Update();

  vtkPolyData* streamlines = splineFilter->GetOutput();

  int numberOfCells = streamlines->GetNumberOfCells();

  vtkIntArray* labelArray = vtkIntArray::New();
  labelArray->SetName("Label");

  vtkDoubleArray* radiusArray = vtkDoubleArray::New();
  radiusArray->SetName("Radius");

  vtkPoints* outputPoints = vtkPoints::New();
  vtkCellArray* outputVertices = vtkCellArray::New();

  for (int i=0; i<numberOfCells; i++)
    {
    vtkCell* streamline = streamlines->GetCell(i);
    int numberOfStreamlinePoints = streamline->GetNumberOfPoints();
    for (int ip=1; ip<numberOfStreamlinePoints-1; ip++)
      {
      double x0[3], x1[3], x2[3];
      streamlines->GetPoint(streamline->GetPointId(ip-1),x0);
      streamlines->GetPoint(streamline->GetPointId(ip),x1);
      streamlines->GetPoint(streamline->GetPointId(ip+1),x2);
      double u[3], a[3];
      for (int j=0; j<3; j++)
        {
        a[j] = 0.5 * (x1[j] + x0[j]);
        }
      double x2_x1[3], x1_x0[3];
      double x2_a[3];
      for (int j=0; j<3; j++)
        {
        x2_x1[j] = x2[j] - x1[j];
        x1_x0[j] = x1[j] - x0[j];
        x2_a[j] = x2[j] - a[j];
        }
      double a_x2_sq = vtkMath::Distance2BetweenPoints(a,x2);
      double a_x1_sq = vtkMath::Distance2BetweenPoints(a,x1);
      double x2_x1_dot_x1_x0 = vtkMath::Dot(x2_x1,x1_x0);
      double x2_x1_sq = vtkMath::Distance2BetweenPoints(x2,x1);
      for (int j=0; j<3; j++)
        {
        u[j] = (x2[j] - x1[j]) - (x1[j] - x0[j]) * (x2_x1_dot_x1_x0 / x2_x1_sq);
        }
      vtkMath::Normalize(u);
      double u_dot_x2_a = vtkMath::Dot(u, x2_a);
      if (fabs(u_dot_x2_a) < VTK_VMTK_DOUBLE_TOL)
        {
        continue;
        }
      double t = (a_x2_sq - a_x1_sq) / (2.0 * u_dot_x2_a);
      double osculatingCenter[3];
      for (int j=0; j<3; j++)
        {
        osculatingCenter[j] = a[j] + t * u[j];
        }
      double radius = sqrt(vtkMath::Distance2BetweenPoints(osculatingCenter,x1));
      int id = outputPoints->InsertNextPoint(osculatingCenter);
      outputVertices->InsertNextCell(1);
      outputVertices->InsertCellPoint(id);
      labelArray->InsertNextValue(i);
      radiusArray->InsertNextValue(radius);
      }
    }

  output->SetPoints(outputPoints);
  output->SetVerts(outputVertices);
  output->GetPointData()->AddArray(labelArray);
  output->GetPointData()->AddArray(radiusArray);

  splineFilter->Delete(); 
  outputPoints->Delete();
  outputVertices->Delete();
  labelArray->Delete();
  radiusArray->Delete();

  return 1;
}

void vtkvmtkStreamlineOsculatingCentersFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
