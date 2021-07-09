/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCardinalSpline.cxx,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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


#include "vtkvmtkCardinalSpline.h"
#include "vtkPiecewiseFunction.h"

#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkCardinalSpline);

vtkvmtkCardinalSpline::vtkvmtkCardinalSpline()
{
}

double vtkvmtkCardinalSpline::EvaluateDerivative(double t)
{
  int index;
  double *intervals;
  double *coefficients;

  if (this->ComputeTime < this->GetMTime())
    {
    this->Compute();
    }

  int size = this->PiecewiseFunction->GetSize();
  if (size < 2)
    {
    return 0.0;
    }

  intervals = this->Intervals;
  coefficients = this->Coefficients;

  if (this->Closed)
    {
    size = size + 1;
    }

  if (t < intervals[0])
    {
    t = intervals[0];
    }
  if (t > intervals[size - 1])
    {
    t = intervals[size - 1];
    }

  index = this->FindIndex(size,t);

  t = (t - intervals[index]);

  return 3 * t * t * *(coefficients + index * 4 + 3) +
             2 * t * *(coefficients + index * 4 + 2) +
                   + *(coefficients + index * 4 + 1);
}

double vtkvmtkCardinalSpline::EvaluateSecondDerivative(double t)
{
  int index;
  double *intervals;
  double *coefficients;

  if (this->ComputeTime < this->GetMTime())
    {
    this->Compute();
    }

  int size = this->PiecewiseFunction->GetSize();
  if (size < 2)
    {
    return 0.0;
    }

  intervals = this->Intervals;
  coefficients = this->Coefficients;

  if (this->Closed)
    {
    size = size + 1;
    }

  if (t < intervals[0])
    {
    t = intervals[0];
    }
  if (t > intervals[size - 1])
    {
    t = intervals[size - 1];
    }

  index = this->FindIndex(size,t);

  t = (t - intervals[index]);

  return 6 * t * *(coefficients + index * 4 + 3) +
             2 * *(coefficients + index * 4 + 2);
}

void vtkvmtkCardinalSpline::EvaluateValueAndDerivatives(double t, double valueAndDerivatives[3])
{
  int index;
  double *intervals;
  double *coefficients;

  if (this->ComputeTime < this->GetMTime())
    {
    this->Compute();
    }

  int size = this->PiecewiseFunction->GetSize();
  if (size < 2)
    {
    valueAndDerivatives[0] = 0.0;
    valueAndDerivatives[1] = 0.0;
    valueAndDerivatives[2] = 0.0;
    return;
    }

  intervals = this->Intervals;
  coefficients = this->Coefficients;

  if (this->Closed)
    {
    size = size + 1;
    }

  if (t < intervals[0])
    {
    t = intervals[0];
    }
  if (t > intervals[size - 1])
    {
    t = intervals[size - 1];
    }

  index = this->FindIndex(size,t);

  t = (t - intervals[index]);

  valueAndDerivatives[0] = (t * (t * (t * *(coefficients + index * 4 + 3)
                                       + *(coefficients + index * 4 + 2))
                                       + *(coefficients + index * 4 + 1))
                                       + *(coefficients + index * 4));

  valueAndDerivatives[1] = 3 * t * t * *(coefficients + index * 4 + 3) +
                              2 * t * *(coefficients + index * 4 + 2) +
                                    + *(coefficients + index * 4 + 1);

  valueAndDerivatives[2] = 6 * t * *(coefficients + index * 4 + 3) +
                              2 * *(coefficients + index * 4 + 2);
}

void vtkvmtkCardinalSpline::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

