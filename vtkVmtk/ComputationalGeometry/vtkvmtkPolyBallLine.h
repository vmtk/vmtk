/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBallLine.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkPolyBallLine - Instance of vtkImplicitFunction which evaluates the minimum sphere function for an envelope created by an interpolation of sphere radii along a centerline line.
// .SECTION Description
//  See detailed description of implicit function in the documentation for vtkvmtkPolyBall class. 
//
//  Similar to vtkvmtkPolyBall, the core function of this class is to evaluate the minimum sphere function from on input centerline with associated sphere radii and a query point location. Unlike vtkvmtkPolyBall, this class constructs a continuous tubular envelope whose shape is defined by the linear linear interpolation of the circular boundary profiles (with radius equal to the associated sphere radii) between every consecutive point on the line. As the boundary profiles are constructed from the centerline sphere radii, the tubular envelope generated is guaranteed to lie completely within the surface volume. When evaluated, this is essentially equivalent to evaluating a polyball function for an infinity large collection of spheres along an input dataset. 

#ifndef __vtkvmtkPolyBallLine_h
#define __vtkvmtkPolyBallLine_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyBallLine : public vtkImplicitFunction
{
  public:

  static vtkvmtkPolyBallLine *New();
  vtkTypeMacro(vtkvmtkPolyBallLine,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent) override;

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

  // Description:
  // Set / get input cell ids used for the function.
  vtkSetObjectMacro(InputCellIds,vtkIdList);
  vtkGetObjectMacro(InputCellIds,vtkIdList);

  // Description:
  // Set / get a single input cell id used for the function.
  vtkSetMacro(InputCellId,vtkIdType);
  vtkGetMacro(InputCellId,vtkIdType);

  // Description:
  // Set / get poly ball radius array name.
  vtkSetStringMacro(PolyBallRadiusArrayName);
  vtkGetStringMacro(PolyBallRadiusArrayName);

  // Description:
  // Get the id of the last nearest poly ball center.
  vtkGetMacro(LastPolyBallCellId,vtkIdType);
  vtkGetMacro(LastPolyBallCellSubId,vtkIdType);
  vtkGetMacro(LastPolyBallCellPCoord,double);
  vtkGetVectorMacro(LastPolyBallCenter,double,3);
  vtkGetMacro(LastPolyBallCenterRadius,double);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  static double ComplexDot(double x[4], double y[4]);

  protected:
  vtkvmtkPolyBallLine();
  ~vtkvmtkPolyBallLine();

  vtkPolyData* Input;
  vtkIdList* InputCellIds;
  vtkIdType InputCellId;

  char* PolyBallRadiusArrayName;
  vtkIdType LastPolyBallCellId;
  vtkIdType LastPolyBallCellSubId;
  double LastPolyBallCellPCoord;
  double LastPolyBallCenter[3];
  double LastPolyBallCenterRadius;

  int UseRadiusInformation;

  private:
  vtkvmtkPolyBallLine(const vtkvmtkPolyBallLine&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallLine&);  // Not implemented.
};

#endif


