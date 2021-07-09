/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataGeodesicRBFInterpolation.h,v $
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
// .NAME vtkvmtkPolyDataGeodesicRBFInterpolation - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataGeodesicRBFInterpolation_h
#define __vtkvmtkPolyDataGeodesicRBFInterpolation_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataGeodesicRBFInterpolation : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataGeodesicRBFInterpolation* New();
  vtkTypeMacro(vtkvmtkPolyDataGeodesicRBFInterpolation,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(SeedIds,vtkIdList);
  vtkGetObjectMacro(SeedIds,vtkIdList);
  
  vtkSetObjectMacro(SeedValues,vtkDataArray);
  vtkGetObjectMacro(SeedValues,vtkDataArray);
        
  vtkSetStringMacro(InterpolatedArrayName);
  vtkGetStringMacro(InterpolatedArrayName);
  
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
    
protected:
  vtkvmtkPolyDataGeodesicRBFInterpolation();
  ~vtkvmtkPolyDataGeodesicRBFInterpolation();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double EvaluateRBF(double r);
  
  char* InterpolatedArrayName;

  vtkIdList* SeedIds;
  vtkDataArray* SeedValues;

  int RBFType;
  
private:
  vtkvmtkPolyDataGeodesicRBFInterpolation(const vtkvmtkPolyDataGeodesicRBFInterpolation&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataGeodesicRBFInterpolation&);  // Not implemented.
};

#endif

