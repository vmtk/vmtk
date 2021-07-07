/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkRBFInterpolation2.h,v $
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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
  // .NAME vtkvmtkRBFInterpolation2 - 
  // .SECTION Description
  // ..

#ifndef __vtkvmtkRBFInterpolation2_h
#define __vtkvmtkRBFInterpolation2_h

#include "vtkImplicitFunction.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkRBFInterpolation2 : public vtkImplicitFunction
{
  public:
  vtkTypeMacro(vtkvmtkRBFInterpolation2,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkRBFInterpolation2 *New();

  // Description
  // Evaluate polyball.
  double EvaluateFunction(double x[3]) override;
  double EvaluateFunction(double x, double y, double z) override
  {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  // Description
  // Evaluate polyball gradient.
  void EvaluateGradient(double x[3], double n[3]) override;

  void ComputeCoefficients();

  // Description:
  // Set / get source poly data.
  vtkSetObjectMacro(Source,vtkPolyData);
  vtkGetObjectMacro(Source,vtkPolyData);

  vtkSetMacro(RBFType,int);
  vtkGetMacro(RBFType,int);
  void SetRBFTypeToThinPlateSpline()
  { this->SetRBFType(THIN_PLATE_SPLINE); }
  void SetRBFTypeToBiharmonic()
  { this->SetRBFType(BIHARMONIC); }
  void SetRBFTypeToTriharmonic()
  { this->SetRBFType(TRIHARMONIC); }
  

//BTX
  enum 
  {
    THIN_PLATE_SPLINE,
    BIHARMONIC,
    TRIHARMONIC
  };
//ETX

  #ifdef VTK_HAS_MTIME_TYPE
    vtkMTimeType GetMTime() override;
  #else
    unsigned long GetMTime();
  #endif

  protected:
  vtkvmtkRBFInterpolation2();
  ~vtkvmtkRBFInterpolation2();

  double EvaluateRBF(double c[3], double x[3]);

  vtkPolyData* Source;
  int RBFType;

  vtkDoubleArray* Coefficients;
  
  private:
  vtkvmtkRBFInterpolation2(const vtkvmtkRBFInterpolation2&);  // Not implemented.
  void operator=(const vtkvmtkRBFInterpolation2&);  // Not implemented.
};

#endif


