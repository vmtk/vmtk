/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkIterativeClosestPointTransform.cxx,v $
Language:  C++
Date:      $Date: 2010/05/30 11:32:56 $
Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkIterativeClosestPointTransform.h"
#include "vtkDataSet.h"
#include "vtkCellLocator.h"
#include "vtkPoints.h"
#include "vtkTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkIterativeClosestPointTransform);

vtkvmtkIterativeClosestPointTransform::vtkvmtkIterativeClosestPointTransform()
{
  this->FarThreshold = 1.0;
  this->UseFarThreshold = 0;
}

vtkvmtkIterativeClosestPointTransform::~vtkvmtkIterativeClosestPointTransform()
{
}

void vtkvmtkIterativeClosestPointTransform::InternalUpdate()
{
  // Check source, target

  if (this->Source == NULL || !this->Source->GetNumberOfPoints())
    {
    vtkErrorMacro(<<"Can't execute with NULL or empty input");
    return;
    }

  if (this->Target == NULL || !this->Target->GetNumberOfPoints())
    {
    vtkErrorMacro(<<"Can't execute with NULL or empty target");
    return;
    }

  // Create locator

  this->CreateDefaultLocator();
  this->Locator->SetDataSet(this->Target);
  this->Locator->SetNumberOfCellsPerBucket(1);
  this->Locator->BuildLocator();

  // Create two sets of points to handle iteration

  int step = 1;
  if (this->Source->GetNumberOfPoints() > this->MaximumNumberOfLandmarks)
    {
    step = this->Source->GetNumberOfPoints() / this->MaximumNumberOfLandmarks;
    vtkDebugMacro(<< "Landmarks step is now : " << step);
    }

  vtkIdType nb_points = this->Source->GetNumberOfPoints() / step;

  // Allocate some points.
  // - closestp is used so that the internal state of LandmarkTransform remains
  //   correct whenever the iteration process is stopped (hence its source
  //   and landmark points might be used in a vtkThinPlateSplineTransform).
  // - points2 could have been avoided, but do not ask me why 
  //   InternalTransformPoint is not working correctly on my computer when
  //   in and out are the same pointer.

  vtkPoints *points1 = vtkPoints::New();
  points1->SetNumberOfPoints(nb_points);

  vtkPoints *closestp = vtkPoints::New();
  closestp->SetNumberOfPoints(nb_points);

  vtkPoints *points2 = vtkPoints::New();
  points2->SetNumberOfPoints(nb_points);

  // Fill with initial positions (sample dataset using step)

  vtkTransform *accumulate = vtkTransform::New();
  accumulate->PostMultiply();

  vtkIdType i;
  int j;
  double p1[3], p2[3];

  if (StartByMatchingCentroids)
    {
    double source_centroid[3] = {0,0,0};
    for (i = 0; i < this->Source->GetNumberOfPoints(); i++)
      {
      this->Source->GetPoint(i, p1);
      source_centroid[0] += p1[0];
      source_centroid[1] += p1[1];
      source_centroid[2] += p1[2];
      }
    source_centroid[0] /= this->Source->GetNumberOfPoints();
    source_centroid[1] /= this->Source->GetNumberOfPoints();
    source_centroid[2] /= this->Source->GetNumberOfPoints();

    double target_centroid[3] = {0,0,0};
    for (i = 0; i < this->Target->GetNumberOfPoints(); i++)
      {
      this->Target->GetPoint(i, p2);
      target_centroid[0] += p2[0];
      target_centroid[1] += p2[1];
      target_centroid[2] += p2[2];
      }
    target_centroid[0] /= this->Target->GetNumberOfPoints();
    target_centroid[1] /= this->Target->GetNumberOfPoints();
    target_centroid[2] /= this->Target->GetNumberOfPoints();

    accumulate->Translate(target_centroid[0] - source_centroid[0],
                          target_centroid[1] - source_centroid[1],
                          target_centroid[2] - source_centroid[2]);
    accumulate->Update();

    for (i = 0, j = 0; i < nb_points; i++, j += step)
      {
      double outPoint[3];
      accumulate->InternalTransformPoint(this->Source->GetPoint(j),
                                         outPoint);
      points1->SetPoint(i, outPoint);
      }
    }
  else 
    {
    for (i = 0, j = 0; i < nb_points; i++, j += step)
      {
      points1->SetPoint(i, this->Source->GetPoint(j));
      }
    }

  // Go
  
  vtkIdType cell_id;
  int sub_id;
  double dist2, totaldist = 0;
  double outPoint[3];

  vtkPoints *temp, *a = points1, *b = points2;

  this->NumberOfIterations = 0;

  double tooFarThreshold2 = this->FarThreshold * this->FarThreshold;
  do 
    {
    // Fill points with the closest points to each vertex in input
    if (!this->UseFarThreshold)
      {
      closestp->SetNumberOfPoints(nb_points); // It was set above, but at this point it should be removed from there
      for(i = 0; i < nb_points; i++)
        {
        this->Locator->FindClosestPoint(a->GetPoint(i),
                                        outPoint,
                                        cell_id,
                                        sub_id,
                                        dist2);
        closestp->SetPoint(i, outPoint);
        }
      this->LandmarkTransform->SetSourceLandmarks(a);
      }
    else //use a FarThreshold
      {
      vtkPoints* sourceLandmarks = vtkPoints::New();
      closestp->Initialize();
      for(i = 0; i < nb_points; i++)
        {
        this->Locator->FindClosestPoint(a->GetPoint(i),
                                        outPoint,
                                        cell_id,
                                        sub_id,
                                        dist2);
        if(dist2 < tooFarThreshold2)
          {
          closestp->InsertNextPoint(outPoint);
          sourceLandmarks->InsertNextPoint(a->GetPoint(i));
          }
        }
      this->LandmarkTransform->SetSourceLandmarks(sourceLandmarks);
      sourceLandmarks->Delete();
      }
    // Build the landmark transform

    this->LandmarkTransform->SetTargetLandmarks(closestp);
    this->LandmarkTransform->Update();

    this->NumberOfIterations++;
    vtkDebugMacro(<< "Iteration: " << this->NumberOfIterations);
    if (this->NumberOfIterations > this->MaximumNumberOfIterations)
      {
      break;
      }

    // Concatenate (can't use this->Concatenate directly)
    accumulate->Concatenate(this->LandmarkTransform->GetMatrix());

    // Move mesh and compute mean distance if needed

    if (this->CheckMeanDistance)
      {
      totaldist = 0.0;
      }

    for(i = 0; i < nb_points; i++)
      {
      a->GetPoint(i, p1);
      this->LandmarkTransform->InternalTransformPoint(p1, p2);
      b->SetPoint(i, p2);
      if (this->CheckMeanDistance)
        {
        if (this->MeanDistanceMode == VTK_ICP_MODE_RMS) 
          {
          totaldist += vtkMath::Distance2BetweenPoints(p1, p2);
          } else {
          totaldist += sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
          }
        }
      }
    
    if (this->CheckMeanDistance)
      {
      if (this->MeanDistanceMode == VTK_ICP_MODE_RMS) 
        {
        this->MeanDistance = sqrt(totaldist / (double)nb_points);
        } else {
        this->MeanDistance = totaldist / (double)nb_points;
        }
      vtkDebugMacro("Mean distance: " << this->MeanDistance);
      if (this->MeanDistance <= this->MaximumMeanDistance)
        {
        break;
        }
      }

    temp = a;
    a = b;
    b = temp;

    } 
  while (1);

  // Now recover accumulated result

  this->Matrix->DeepCopy(accumulate->GetMatrix());

  accumulate->Delete();
  points1->Delete();
  closestp->Delete();
  points2->Delete();
}

void vtkvmtkIterativeClosestPointTransform::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FarThreshold: " << this->FarThreshold << "\n";
  os << indent << "UseFarThreshold: " << this->UseFarThreshold << "\n";
}
