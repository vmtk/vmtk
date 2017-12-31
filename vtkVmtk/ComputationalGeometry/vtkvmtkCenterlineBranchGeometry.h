/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBranchGeometry.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
Version:   $Revision: 1.8 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCenterlineBranchGeometry - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineBranchGeometry_h
#define __vtkvmtkCenterlineBranchGeometry_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkPoints;
class vtkDoubleArray;
class vtkIntArray;
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBranchGeometry : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBranchGeometry,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkCenterlineBranchGeometry* New();

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetStringMacro(LengthArrayName);
  vtkGetStringMacro(LengthArrayName);

  vtkSetStringMacro(CurvatureArrayName);
  vtkGetStringMacro(CurvatureArrayName);

  vtkSetStringMacro(TorsionArrayName);
  vtkGetStringMacro(TorsionArrayName);

  vtkSetStringMacro(TortuosityArrayName);
  vtkGetStringMacro(TortuosityArrayName);

  vtkSetMacro(MinSubsamplingSpacing,double);
  vtkGetMacro(MinSubsamplingSpacing,double);

  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);

  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);

  vtkSetMacro(LineSmoothing,int);
  vtkGetMacro(LineSmoothing,int);
  vtkBooleanMacro(LineSmoothing,int);

  vtkSetMacro(LineSubsampling,int);
  vtkGetMacro(LineSubsampling,int);
  vtkBooleanMacro(LineSubsampling,int);

  vtkSetMacro(SphereSubsampling,int);
  vtkGetMacro(SphereSubsampling,int);
  vtkBooleanMacro(SphereSubsampling,int);

  protected:
  vtkvmtkCenterlineBranchGeometry();
  ~vtkvmtkCenterlineBranchGeometry();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  double ComputeGroupLength(vtkPolyData* input, int branchGroupId);
  double ComputeGroupCurvature(vtkPolyData* input, int branchGroupId);
  double ComputeGroupTorsion(vtkPolyData* input, int branchGroupId);
  double ComputeGroupTortuosity(vtkPolyData* input, int branchGroupId, double groupLength);

  static void SubsampleLine(vtkPoints* linePoints, vtkPoints* subsampledLinePoints, double minSpacing = 0.01);
  void SphereSubsampleLine(vtkPolyData* input, vtkIdType cellId, vtkPoints* subsampledLinePoints);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* BlankingArrayName;

  char* LengthArrayName;
  char* CurvatureArrayName;
  char* TorsionArrayName;
  char* TortuosityArrayName;

  int LineSubsampling;
  int SphereSubsampling;
  int LineSmoothing;

  double MinSubsamplingSpacing;
  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineBranchGeometry(const vtkvmtkCenterlineBranchGeometry&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBranchGeometry&);  // Not implemented.
};

#endif
