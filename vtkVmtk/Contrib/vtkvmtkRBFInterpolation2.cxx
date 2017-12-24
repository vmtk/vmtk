/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkRBFInterpolation2.cxx,v $
Language:  C++
Date:      $Date: 2005/03/04 11:07:28 $
Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
#include "vtkvmtkRBFInterpolation2.h"
#include "vtkvmtkConstants.h"
#include "vtkPointData.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkRBFInterpolation2);

vtkvmtkRBFInterpolation2::vtkvmtkRBFInterpolation2()
{
  this->Source = NULL;
  this->RBFType = THIN_PLATE_SPLINE;
  this->Coefficients = NULL;
}

vtkvmtkRBFInterpolation2::~vtkvmtkRBFInterpolation2()
{
  if (this->Source)
    {
    this->Source->Delete();
    this->Source = NULL;
    }

  if (this->Coefficients)
    {
    this->Coefficients->Delete();
    this->Coefficients = NULL;
    }
}

double vtkvmtkRBFInterpolation2::EvaluateRBF(double c[3], double x[3])
{
  if (this->RBFType == THIN_PLATE_SPLINE)
    {
    double r2 = vtkMath::Distance2BetweenPoints(c,x);
    if (!r2)
      {
      return 0.0;
      }
    return r2 * log(sqrt(r2));
    }
  else if (this->RBFType == BIHARMONIC)
    {
    return sqrt(vtkMath::Distance2BetweenPoints(c,x));
    }
  else if (this->RBFType == TRIHARMONIC)
    {
    return pow(vtkMath::Distance2BetweenPoints(c,x),1.5);
    }
  else
    {
    vtkErrorMacro(<<"Error: Unsupported RBFType!");
    return 0.0;
    }
}

void vtkvmtkRBFInterpolation2::ComputeCoefficients()
{
  if (this->Coefficients)
    {
    this->Coefficients->Delete();
    this->Coefficients = NULL;
    }

  if (!this->Source)
    {
    vtkErrorMacro("No Source specified!");
    return;
    }

  vtkDataArray *sourceScalars = this->Source->GetPointData()->GetScalars();

  if (!sourceScalars)
    {
    vtkErrorMacro("No scalars to interpolate!");
    return;
    }
  
  int numberOfPoints = this->Source->GetNumberOfPoints();
  if (!numberOfPoints)
    {
    vtkWarningMacro("Empty Source specified!");
    return;
    }
  
  this->Coefficients = vtkDoubleArray::New();
  this->Coefficients->SetNumberOfValues(numberOfPoints);

  double **A, *x;
  x = new double[numberOfPoints];
  A = new double* [numberOfPoints];

  int i;
  for (i=0; i<numberOfPoints; i++)
    {
    A[i] = new double[numberOfPoints];
    x[i] = sourceScalars->GetComponent(i,0);
    }

  double center[3];
  int j;
  for (i=0; i<numberOfPoints; i++)
    {
    this->Source->GetPoint(i,center);
    for (j=0; j<numberOfPoints; j++)
      {
      A[i][j] = this->EvaluateRBF(center,this->Source->GetPoint(j));
      }
    } 

  int ret = vtkMath::SolveLinearSystem(A,x,numberOfPoints);
  
  if (!ret)
    {
    vtkErrorMacro(<<"Cannot compute coefficients: error during linear system solve");
    }

  for (i=0; i<numberOfPoints; i++)
    {
    this->Coefficients->SetValue(i,x[i]);
    }

  delete[] x;
  for (i=0; i<numberOfPoints; i++)
    {
    delete[] A[i];
    }
  delete[] A;
  
}

double vtkvmtkRBFInterpolation2::EvaluateFunction(double x[3])
{
  if (!this->Source)
    {
    vtkErrorMacro("No Source specified!");
    return 0.0;
    }

  int numberOfPoints = this->Source->GetNumberOfPoints();

  if (!numberOfPoints)
    {
    vtkWarningMacro("Empty Source specified!");
    return 0.0;
    }

  if (!this->Coefficients)
    {
    this->ComputeCoefficients();
    }

  double rbfValue = 0.0;
  double center[3];
  int i;
  for (i=0; i<numberOfPoints; i++)
    {
    this->Source->GetPoint(i,center);
    rbfValue += this->Coefficients->GetValue(i) * this->EvaluateRBF(center,x);
    }

  return rbfValue;
}

void vtkvmtkRBFInterpolation2::EvaluateGradient(double x[3], double n[3])
{
  vtkWarningMacro("RBF gradient computation not implemented.");
}

#ifdef VTK_HAS_MTIME_TYPE
vtkMTimeType vtkvmtkRBFInterpolation2::GetMTime()
#else
unsigned long vtkvmtkRBFInterpolation2::GetMTime()
#endif
{
  unsigned long mTime=this->Superclass::GetMTime();
  unsigned long sourceMTime;

  if (this->Source != NULL)
    {
    sourceMTime = this->Source->GetMTime();
    mTime = ( sourceMTime > mTime ? sourceMTime : mTime );
    }

  return mTime;
}

void vtkvmtkRBFInterpolation2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
