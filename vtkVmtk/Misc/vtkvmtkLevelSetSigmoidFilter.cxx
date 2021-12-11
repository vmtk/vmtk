/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLevelSetSigmoidFilter.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkLevelSetSigmoidFilter.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include <cmath>


vtkStandardNewMacro(vtkvmtkLevelSetSigmoidFilter);

vtkvmtkLevelSetSigmoidFilter::vtkvmtkLevelSetSigmoidFilter()
{
  this->LevelSetsImage = NULL;
  this->Sigma = 1.0;
  this->ScaleValue = 0.02;
  this->ComputeScaleValueFromInput = 1;
}

vtkvmtkLevelSetSigmoidFilter::~vtkvmtkLevelSetSigmoidFilter()
{
  if (this->LevelSetsImage)
    {
    this->LevelSetsImage->Delete();
    this->LevelSetsImage = NULL;
    }
}

void vtkvmtkLevelSetSigmoidFilter::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  if (!this->LevelSetsImage)
    {
    vtkErrorMacro(<<"Error: no LevelSetsImage has been set!");
    return;
    }

  vtkDataArray* inputScalars = input->GetPointData()->GetScalars();
  vtkDataArray* levelSetsScalars = this->LevelSetsImage->GetPointData()->GetScalars();
  vtkDataArray* outputScalars = output->GetPointData()->GetScalars();

  double spacing[3];
  input->GetSpacing(spacing);

  double minSpacing = spacing[0];
  if (spacing[1] < minSpacing)
    {
    minSpacing = spacing[1];
    }
  if (spacing[2] < minSpacing)
    {
    minSpacing = spacing[2];
    }

  double windowValue = 4.0;
  double windowValueReal = windowValue * minSpacing;
  double sigmaReal = this->Sigma * minSpacing;

  double scaleValue = this->ScaleValue;

  int numberOfPoints = input->GetNumberOfPoints();

  int i;
  if (this->ComputeScaleValueFromInput)
    {
    scaleValue = 0.0;
    for (i=0; i<numberOfPoints; i++)
      {
      scaleValue += inputScalars->GetComponent(i,0);
      }
    scaleValue /= numberOfPoints;
    }

  for (i=0; i<numberOfPoints; i++)
    {
    double featureValue = inputScalars->GetComponent(i,0);
    double levelSetsValue = levelSetsScalars->GetComponent(i,0);
    if (levelSetsValue < windowValueReal)
      {
      double sigmoidValue = scaleValue * 1.0 / (1.0 + std::exp((levelSetsValue-sigmaReal)/(0.5*sigmaReal)));
      featureValue += sigmoidValue;
      }
    outputScalars->SetComponent(i,0,featureValue);
    }
}

void vtkvmtkLevelSetSigmoidFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
