/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkPolyDataMeanCurvature
 * @brief   Compute the mean curvature and mean curvature normals of surface point neighborhoods with a particular stencil applied.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataMeanCurvature estimates the discrete mean curvature (and/or the mean curvature
 * normal vector) at every point of an input surface using the mean-curvature-normal identity
 * H*n = 0.5 * L(x), where L is a discrete Laplace-Beltrami operator built as a stencil
 * (vtkvmtkStencils, selected by StencilType) over each point's neighborhood. For every point, the
 * stencil weights are applied to the neighbor and center point coordinates
 * (ComputePointMeanCurvatureVector) to obtain the mean curvature normal vector; its norm is written,
 * when ComputeMeanCurvatureScalars is on, to the point data scalar array named
 * MeanCurvatureScalarsArrayName, and the normalized vector is written, when
 * ComputeMeanCurvatureNormals is on, to the point data vector array named
 * MeanCurvatureNormalsArrayName.
 *
 * @sa vtkvmtkStencils, vtkvmtkPolyDataLaplaceBeltramiStencil, vtkvmtkPolyDataAreaWeightedUmbrellaStencil
 */

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
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/get the stencil type used to build the discrete Laplace-Beltrami operator at each point:
   * VTK_VMTK_UMBRELLA_STENCIL (uniform-weight umbrella operator),
   * VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL, VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL (cotangent weights
   * with finite-element area normalization, the default), or
   * VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL (cotangent weights with finite-volume/Voronoi area
   * normalization). See vtkvmtkStencils for the corresponding stencil classes.
   */
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
  ///@}

  ///@{
  /**
   * Set/get the name of the point data scalar array where the mean curvature magnitude is stored on
   * the output. Required whenever ComputeMeanCurvatureScalars is on.
   */
  vtkSetStringMacro(MeanCurvatureScalarsArrayName);
  vtkGetStringMacro(MeanCurvatureScalarsArrayName);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data vector array where the (unit) mean curvature normal is
   * stored on the output. Required whenever ComputeMeanCurvatureNormals is on.
   */
  vtkSetStringMacro(MeanCurvatureNormalsArrayName);
  vtkGetStringMacro(MeanCurvatureNormalsArrayName);
  ///@}

  ///@{
  /**
   * Toggle computation of the mean curvature scalar (magnitude of the mean curvature normal) at
   * each point, written to MeanCurvatureScalarsArrayName. Default: off.
   */
  vtkSetMacro(ComputeMeanCurvatureScalars,int);
  vtkGetMacro(ComputeMeanCurvatureScalars,int);
  vtkBooleanMacro(ComputeMeanCurvatureScalars,int);
  ///@}

  ///@{
  /**
   * Toggle computation of the (unit) mean curvature normal vector at each point, written to
   * MeanCurvatureNormalsArrayName. Default: off.
   */
  vtkSetMacro(ComputeMeanCurvatureNormals,int);
  vtkGetMacro(ComputeMeanCurvatureNormals,int);
  vtkBooleanMacro(ComputeMeanCurvatureNormals,int);
  ///@}

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

