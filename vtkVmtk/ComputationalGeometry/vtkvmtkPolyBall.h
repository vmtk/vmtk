/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBall.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyBall - An instance of vtkImplicitFunction which evaluates the minimum sphere function from a collection of points with attached sphere radii as point data.
// .SECTION Description
// Implicit functions are real valued functions defined in 3D space, w = F(x,y,z). Two primitive operations are required: the ability to evaluate the function, and the function gradient at a given point. The implicit function divides space into three regions: on the surface (F(x,y,z)=w), outside of the surface (F(x,y,z)>c), and inside the surface (F(x,y,z)<c). (When c is zero, positive values are outside, negative values are inside, and zero is on the surface. Note also that the function gradient points from inside to outside.)
//  
// A polyball is just an implicit function which takes a data set containing a bunch of points in R^3 with sphere radii defined on top of them and evaluates the minimum sphere function across the entire collection for a particular point. The sphere function is zero at the sphere surface, negative inside the sphere, and positive outside the sphere. This is implemented as a brute force calculation; in order to find the minimum sphere function across the collection, the sphere function is evaluated at the query point for every point in the input data set. 

#ifndef __vtkvmtkPolyBall_h
#define __vtkvmtkPolyBall_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBall : public vtkImplicitFunction
{
  public:
  vtkTypeMacro(vtkvmtkPolyBall,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyBall *New();

  // Description
  // Evaluate polyball.
  double EvaluateFunction(double x[3]) override;
  double EvaluateFunction(double x, double y, double z) override
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  // Description
  // Evaluate polyball gradient.
  void EvaluateGradient(double x[3], double n[3]) override;

  // Description:
  // Set / get input poly data.
  vtkSetObjectMacro(Input,vtkPolyData);
  vtkGetObjectMacro(Input,vtkPolyData);
  void SetInputData(vtkPolyData* input) { SetInput(input); }
  vtkPolyData* GetInputData() { return GetInput(); }

  // Description:
  // Set / get poly ball radius array name.
  vtkSetStringMacro(PolyBallRadiusArrayName);
  vtkGetStringMacro(PolyBallRadiusArrayName);

  // Description:
  // Get the id of the last nearest poly ball center.
  vtkGetMacro(LastPolyBallCenterId,vtkIdType);

  protected:
  vtkvmtkPolyBall();
  ~vtkvmtkPolyBall();

  vtkPolyData* Input;
  char* PolyBallRadiusArrayName;
  vtkIdType LastPolyBallCenterId;

  private:
  vtkvmtkPolyBall(const vtkvmtkPolyBall&);  // Not implemented.
  void operator=(const vtkvmtkPolyBall&);  // Not implemented.
};

#endif


