/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBall.cxx,v $
Language:  C++
Date:      $Date: 2005/03/04 11:07:28 $
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
#include "vtkvmtkPolyBall.h"
#include "vtkvmtkConstants.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkvmtkPolyBall);

vtkvmtkPolyBall::vtkvmtkPolyBall()
{
  this->Input = NULL;
  this->PolyBallRadiusArrayName = NULL;
  this->LastPolyBallCenterId = -1;
}

vtkvmtkPolyBall::~vtkvmtkPolyBall()
{
  if (this->Input)
    {
    this->Input->Delete();
    this->Input = NULL;
    }

  if (this->PolyBallRadiusArrayName)
    {
    delete[] this->PolyBallRadiusArrayName;
    this->PolyBallRadiusArrayName = NULL;
    }
}

double vtkvmtkPolyBall::EvaluateFunction(double x[3])
{
  double px[3], pr;
  int i;
  double sphereFunctionValue, minSphereFunctionValue;
  vtkDataArray* polyballRadiusArray;

  if (!this->Input)
    {
    vtkErrorMacro("No Input specified!");
    return 0.0;
    }

  if (this->Input->GetNumberOfPoints()==0)
    {
    vtkWarningMacro("Empty Input specified!");
    }

  if (!this->PolyBallRadiusArrayName)
    {
    vtkErrorMacro("No PolyBallRadiusArrayName specified!");
    return 0.0;
    }

  if (!this->Input->GetPointData()->GetArray(this->PolyBallRadiusArrayName))
    {
    vtkErrorMacro("PolyBallRadiusArray with name specified does not exist!");
    return 0.0;
    }

  polyballRadiusArray = this->Input->GetPointData()->GetArray(this->PolyBallRadiusArrayName);
  minSphereFunctionValue = VTK_VMTK_LARGE_DOUBLE;
  for (i=0; i<this->Input->GetNumberOfPoints(); i++)
    {
    // this next line actually copies the xyz location of point i into px[3].
    // luca reports that odd bug happened when only retrieving pointers.
    this->Input->GetPoint(i,px);
    pr = polyballRadiusArray->GetComponent(i,0);
    // x, y, z -> location of query point in R3
    // px, py, pz => location of point with sphere radius == pr
    // f = ((x - px)^2 + (y - py)^2 + (z - pz)^2) - pr^2
    sphereFunctionValue = ((x[0] - px[0]) * (x[0] - px[0]) + (x[1] - px[1]) * (x[1] - px[1]) + (x[2] - px[2]) * (x[2] - px[2])) - pr*pr;
    if (sphereFunctionValue - minSphereFunctionValue < VTK_VMTK_DOUBLE_TOL)
      {
      minSphereFunctionValue = sphereFunctionValue;
      this->LastPolyBallCenterId = i;
      }
    }

  return minSphereFunctionValue;
}

void vtkvmtkPolyBall::EvaluateGradient(double x[3], double n[3])
{
  vtkWarningMacro("Poly ball gradient computation not implemented yet!");

  // TODO
  //   n[0] = 2.0 * (x[0] - this->Center[0]);
  //   n[1] = 2.0 * (x[1] - this->Center[1]);
  //   n[2] = 2.0 * (x[2] - this->Center[2]);
}

void vtkvmtkPolyBall::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
