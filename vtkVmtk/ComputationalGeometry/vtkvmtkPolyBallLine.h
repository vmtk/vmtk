/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyBallLine.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkPolyBallLine - 
  // .SECTION Description
  // ..

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
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Description
  // Evaluate polyball.
  double EvaluateFunction(double x[3]) VTK_OVERRIDE;
  double EvaluateFunction(double x, double y, double z) VTK_OVERRIDE
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  // Description
  // Evaluate polyball gradient.
  void EvaluateGradient(double x[3], double n[3]) VTK_OVERRIDE;

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


