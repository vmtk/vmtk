/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataMeanCurvature.h,v $
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
// .NAME vtkvmtkPolyDataMeanCurvature - Compute the mean curvature and mean curvature normals of surface point neighborhoods with a particular stencil applied.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataMeanCurvature_h
#define __vtkvmtkPolyDataMeanCurvature_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkStencils.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataMeanCurvature : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataMeanCurvature *New();
  vtkTypeMacro(vtkvmtkPolyDataMeanCurvature,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  vtkSetMacro(StencilType,int);
  vtkGetMacro(StencilType,int);
  void SetStencilTypeToUmbrellaStencil() 
    {this->SetStencilType(VTK_VMTK_UMBRELLA_STENCIL);};
  void SetStencilTypeToAreaWeightedUmbrellaStencil() 
    {this->SetStencilType(VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL);};
  void SetStencilTypeToFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL);};
  void SetStencilTypeToFVFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL);};

  vtkSetStringMacro(MeanCurvatureScalarsArrayName);
  vtkGetStringMacro(MeanCurvatureScalarsArrayName);
  vtkSetStringMacro(MeanCurvatureNormalsArrayName);
  vtkGetStringMacro(MeanCurvatureNormalsArrayName);

  vtkSetMacro(ComputeMeanCurvatureScalars,int);
  vtkGetMacro(ComputeMeanCurvatureScalars,int);
  vtkBooleanMacro(ComputeMeanCurvatureScalars,int);

  vtkSetMacro(ComputeMeanCurvatureNormals,int);
  vtkGetMacro(ComputeMeanCurvatureNormals,int);
  vtkBooleanMacro(ComputeMeanCurvatureNormals,int);

protected:
  vtkvmtkPolyDataMeanCurvature();
  ~vtkvmtkPolyDataMeanCurvature();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputePointMeanCurvatureVector(vtkPolyData* input, vtkIdType pointId, double* meanCurvatureVector);
  void ReleaseStencils();

  char* MeanCurvatureScalarsArrayName;
  char* MeanCurvatureNormalsArrayName;
  int StencilType;
  vtkvmtkStencils* Stencils;

  int ComputeMeanCurvatureScalars;
  int ComputeMeanCurvatureNormals;

private:
  vtkvmtkPolyDataMeanCurvature(const vtkvmtkPolyDataMeanCurvature&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataMeanCurvature&);  // Not implemented.
};

#endif

