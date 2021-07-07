/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataClampedSmoothingFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataClampedSmoothingFilter - Apply a laplacian or curvature based smoothing filter to a surface with particular points fixed in place.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataClampedSmoothingFilter_h
#define __vtkvmtkPolyDataClampedSmoothingFilter_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataClampedSmoothingFilter : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataClampedSmoothingFilter *New();
  vtkTypeMacro(vtkvmtkPolyDataClampedSmoothingFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  vtkSetMacro(SmoothingType,int);
  vtkGetMacro(SmoothingType,int);
  void SetSmoothingTypeToLaplacian()
  { this->SmoothingType = LAPLACIAN; }
  void SetSmoothingTypeToCurvatureDiffusion()
  { this->SmoothingType = CURVATURE_DIFFUSION; }

  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);

  vtkSetMacro(Clamp,int);
  vtkGetMacro(Clamp,int);
  vtkBooleanMacro(Clamp,int);

  vtkSetMacro(ClampThreshold,double);
  vtkGetMacro(ClampThreshold,double);

  vtkSetStringMacro(ClampArrayName);
  vtkGetStringMacro(ClampArrayName);

  vtkSetMacro(TimeStepFactor,double);
  vtkGetMacro(TimeStepFactor,double);

//BTX
  enum {
    LAPLACIAN,
    CURVATURE_DIFFUSION
  };
//ETX

protected:
  vtkvmtkPolyDataClampedSmoothingFilter();
  ~vtkvmtkPolyDataClampedSmoothingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  void LaplacianIteration(vtkPolyData* surface);
  void CurvatureDiffusionIteration(vtkPolyData* surface);
  double ComputeTimeStep(vtkPolyData* surface);

  int SmoothingType;
  int NumberOfIterations;
  int Clamp;
  double ClampThreshold;
  double TimeStepFactor;

  char* ClampArrayName;

private:
  vtkvmtkPolyDataClampedSmoothingFilter(const vtkvmtkPolyDataClampedSmoothingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataClampedSmoothingFilter&);  // Not implemented.
};

#endif

