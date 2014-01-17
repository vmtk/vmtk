/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkConstrainedLaplacianPolyDataFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkConstrainedLaplacianPolyDataFilter.h"

#include "vtkvmtkConstants.h"
#include "vtkbvgNeighborhoods.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkConstrainedLaplacianPolyDataFilter);

vtkvmtkConstrainedLaplacianPolyDataFilter::vtkvmtkConstrainedLaplacianPolyDataFilter()
{
  this->Convergence = 0.0;
  this->RelaxationFactor = 0.0;
  this->NumberOfIterations = VTK_VMTK_LARGE_INTEGER;
  this->BoundarySmoothing = 0;

  this->ConstrainedPointIds = NULL;
}

vtkvmtkConstrainedLaplacianPolyDataFilter::~vtkvmtkConstrainedLaplacianPolyDataFilter()
{
  if (this->ConstrainedPointIds)
    {
    this->ConstrainedPointIds->Delete();
    this->ConstrainedPointIds = NULL;
    }
}

vtkCxxSetObjectMacro(vtkvmtkConstrainedLaplacianPolyDataFilter,ConstrainedPointIds,vtkIdList);

void vtkvmtkConstrainedLaplacianPolyDataFilter::Execute()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *newPoints;
  vtkbvgNeighborhoods* neighborhoods;
  vtkbvgNeighborhood *neighborhood;
  vtkIdType n, i, j, step;
  vtkIdType neighborhoodPointId;
  double point[3], newPoint[3];
  double neighborhoodPoint[3];
  double laplacianPoint[3];
  double displacement[3], displacementNorm;
  double maxDisplacementNorm;
  vtkIdType numberOfPoints;
  vtkIdType numberOfNeighborhoodPoints;
  vtkIdType numberOfConstrainedPoints;
  vtkIdType constrainedStatus;
  vtkIdType constrainedPointId;
  vtkCharArray *constrainedStatusArray;

  newPoints = vtkPoints::New();
  constrainedStatusArray = vtkCharArray::New();
  constrainedStatusArray->SetNumberOfComponents(1);

  input = this->GetInput();
  output = this->GetOutput();

  newPoints->DeepCopy(input->GetPoints());

  numberOfPoints = input->GetNumberOfPoints();

  constrainedStatusArray->SetNumberOfTuples(numberOfPoints);
  constrainedStatusArray->FillComponent(0,0.0);

  if (this->ConstrainedPointIds)
    {
    numberOfConstrainedPoints = this->ConstrainedPointIds->GetNumberOfIds();
    for (i=0; i<numberOfConstrainedPoints; i++)
      {
      constrainedPointId = this->ConstrainedPointIds->GetId(i);
      constrainedStatusArray->SetValue(constrainedPointId,1);
      }
    }

  output->SetPoints(newPoints);
  output->SetVerts(input->GetVerts());
  output->SetLines(input->GetLines());
  output->SetPolys(input->GetPolys());
  output->SetStrips(input->GetStrips());
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());

  neighborhoods = vtkbvgNeighborhoods::New();
  neighborhoods->SetDataSet(input);
  neighborhoods->SetNeighborhoodTypeToPolyDataManifoldNeighborhood();
  neighborhoods->Build();

  for (n=0; n<this->NumberOfIterations; n++)
    {
    maxDisplacementNorm = 0.0;
    for (step=0; step<2; step++)
      {
      for (i=0; i<numberOfPoints; i++)
        {
        constrainedStatus = constrainedStatusArray->GetValue(i);
        if (constrainedStatus == 1)
          {
          continue;
          }

        neighborhood = neighborhoods->GetNeighborhood(i);
        
        if (neighborhood->GetIsBoundary() && !this->BoundarySmoothing)
          {
          continue;
          }

        newPoints->GetPoint(i,point);
        laplacianPoint[0] = laplacianPoint[1] = laplacianPoint[2] = 0.0;
        numberOfNeighborhoodPoints = neighborhood->GetNumberOfPoints();
        for (j=0; j<numberOfNeighborhoodPoints; j++)
          {
          newPoints->GetPoint(neighborhood->GetPointId(j),neighborhoodPoint);
          laplacianPoint[0] += neighborhoodPoint[0];
          laplacianPoint[1] += neighborhoodPoint[1];
          laplacianPoint[2] += neighborhoodPoint[2];
          }
        laplacianPoint[0] /= (double)numberOfNeighborhoodPoints;
        laplacianPoint[1] /= (double)numberOfNeighborhoodPoints;
        laplacianPoint[2] /= (double)numberOfNeighborhoodPoints;

        displacement[0] = laplacianPoint[0] - point[0];
        displacement[1] = laplacianPoint[1] - point[1];
        displacement[2] = laplacianPoint[2] - point[2];
        
        displacementNorm = vtkMath::Norm(displacement);
        if (displacementNorm > maxDisplacementNorm)
          {
          maxDisplacementNorm = displacementNorm;
          }

        if (step == 0)
          {
          newPoint[0] = point[0] + this->RelaxationFactor * displacement[0];
          newPoint[1] = point[1] + this->RelaxationFactor * displacement[1];
          newPoint[2] = point[2] + this->RelaxationFactor * displacement[2];
          newPoints->SetPoint(i,newPoint);
          }
        else if (step == 1)
          {
          for (j=0; j<numberOfNeighborhoodPoints; j++)
            {
            neighborhoodPointId = neighborhood->GetPointId(j);
            
            if ((constrainedStatusArray->GetValue(neighborhoodPointId) == 1) || neighborhoods->GetNeighborhood(neighborhoodPointId)->GetIsBoundary())
              {
              continue;
              }

            newPoints->GetPoint(neighborhoodPointId,neighborhoodPoint);
            
            newPoint[0] = neighborhoodPoint[0] - this->RelaxationFactor * displacement[0] / (double)numberOfNeighborhoodPoints;
            newPoint[1] = neighborhoodPoint[1] - this->RelaxationFactor * displacement[1] / (double)numberOfNeighborhoodPoints;
            newPoint[2] = neighborhoodPoint[2] - this->RelaxationFactor * displacement[2] / (double)numberOfNeighborhoodPoints;
            
            newPoints->SetPoint(neighborhoodPointId,newPoint);
            }
          }
        }
      }
    
    if (maxDisplacementNorm < this->Convergence)
      {
      break;
      }
    }
  
  neighborhoods->Delete();
  newPoints->Delete();
  constrainedStatusArray->Delete();
}

void vtkvmtkConstrainedLaplacianPolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

