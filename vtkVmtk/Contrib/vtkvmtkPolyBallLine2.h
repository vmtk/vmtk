/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyBallLine2.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

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
// .NAME vtkvmtkPolyBallLine2 - Second version of polyline where distance to the line is first computed and then the radius at the closest point is subtracted.
// .SECTION Description
// This is another version of a polyball line. Here the distance to the line is first computed and then the radius at the closest point is subtracted. This way each line segment only influences its voronoi cell.

#ifndef __vtkvmtkPolyBallLine2_h
#define __vtkvmtkPolyBallLine2_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkTriangleFilter.h"
#include "vtkCellLocator.h"

//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyBallLine2 : public vtkImplicitFunction
{
  public:

  static vtkvmtkPolyBallLine2 *New();
  vtkTypeMacro(vtkvmtkPolyBallLine2,vtkImplicitFunction);
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
  virtual void SetInput(vtkPolyData *inp);
  vtkGetObjectMacro(Input,vtkPolyData);
  void SetInputData(vtkPolyData* input) { SetInput(input); }
  vtkPolyData* GetInputData() { return GetInput(); }

  // Description:
  // Set / get input cell ids used for the function.
  virtual void SetInputCellIds(vtkIdList *cellIds);
  vtkGetObjectMacro(InputCellIds,vtkIdList);

  // Description:
  // Set / get a single input cell id used for the function.
  virtual void SetInputCellId(vtkIdType cellId);
  vtkGetMacro(InputCellId,vtkIdType);

  // Description:
  // Set / get poly ball radius array name.
  vtkSetStringMacro(PolyBallRadiusArrayName);
  vtkGetStringMacro(PolyBallRadiusArrayName);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  static double ComplexDot(double x[4], double y[4]);

  protected:
  vtkvmtkPolyBallLine2();
  ~vtkvmtkPolyBallLine2();

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
  
  // Used to triangulate the polylines
  vtkTriangleFilter *Triangulator;
  
  //For faster distance computation
  vtkCellLocator *Locator;
  
  //Local copy of the cells used in the input
  vtkPolyData *LocalInput;
  
  //Triangulated input (for increased performance)
  vtkPolyData *TriangulatedInput;
  
  //Should the locator be rebuild
  bool ShouldBuildLocator;
  
  //MTime of input last time the locator was built
  unsigned long LocatorMTime;
  
  //Rebuild the locator
  void BuildLocator();

  private:
  vtkvmtkPolyBallLine2(const vtkvmtkPolyBallLine2&);  // Not implemented.
  void operator=(const vtkvmtkPolyBallLine2&);  // Not implemented.
};

#endif


