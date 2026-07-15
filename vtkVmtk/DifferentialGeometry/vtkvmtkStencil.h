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
 * @class   vtkvmtkStencil
 * @brief   Serves as an abstract base for a weighting scheme applied to a single point's
 * neighborhood of a data set.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkStencil extends vtkvmtkNeighborhood by attaching a weight to every neighbor point (and to
 * the center point itself, via CenterWeight), supporting multiple weight components per point
 * (NumberOfComponents) for vector-valued stencils such as gradient operators. Concrete subclasses
 * implement Build() to compute the actual weights for a given point according to a specific
 * numerical scheme (e.g. uniform umbrella/Laplacian weights in vtkvmtkPolyDataUmbrellaStencil,
 * area-weighted, or finite-element/finite-volume Laplace-Beltrami weights). Weights can optionally
 * be sign-negated (NegateWeights) and rescaled (WeightScaling, via ScaleWeights) once computed.
 * Stencils are the per-point building blocks copied into the rows of a vtkvmtkSparseMatrix (see
 * vtkvmtkSparseMatrixRow::CopyStencil) to assemble discrete differential operators over a mesh.
 *
 * @sa vtkvmtkNeighborhood, vtkvmtkStencils, vtkvmtkSparseMatrix, vtkvmtkPolyDataUmbrellaStencil
 */

#ifndef __vtkvmtkStencil_h
#define __vtkvmtkStencil_h

#include "vtkObject.h"
#include "vtkvmtkNeighborhood.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkStencil : public vtkvmtkNeighborhood 
{
public:

  vtkTypeMacro(vtkvmtkStencil,vtkvmtkNeighborhood);

  ///@{
  /**
   * Set/get the number of weight components stored per neighbor point (and per center weight
   * component); 1 for a scalar stencil (e.g. Laplacian smoothing), 3 for a vector-valued stencil
   * (e.g. gradient). Default: 1.
   */
  vtkSetMacro(NumberOfComponents,vtkIdType);
  vtkGetMacro(NumberOfComponents,vtkIdType);
  ///@}

  ///@{
  /**
   * Get/set the (single-component) weight of the i-th neighbor point of the stencil (0-based
   * index into the neighborhood's point list, as with vtkvmtkNeighborhood::GetPointId).
   */
  double GetWeight(vtkIdType i) {return this->Weights[i];};
  void SetWeight(vtkIdType i, double weight) {this->Weights[i] = weight;};
  ///@}

  ///@{
  /**
   * Get/set the weight of the given component of the i-th neighbor point, for multi-component
   * stencils (see NumberOfComponents).
   */
  double GetWeight(vtkIdType i, vtkIdType component) {return this->Weights[this->NumberOfComponents * i + component];};
  void SetWeight(vtkIdType i, vtkIdType component, double weight) {this->Weights[this->NumberOfComponents * i + component] = weight;};
  ///@}

  /**
   * Get the total number of weight values stored for this stencil (NumberOfComponents times the
   * number of neighbor points).
   */
  vtkIdType GetNumberOfWeights() {return this->NumberOfComponents * this->NPoints;};

  /**
   * Multiply every neighbor weight and center weight component by factor (only if WeightScaling
   * is on; a no-op otherwise). Guards against overflow/underflow: near-zero factors zero out all
   * weights, near-infinite factors clamp them to VTK_VMTK_LARGE_DOUBLE, and any resulting weight
   * below VTK_VMTK_DOUBLE_TOL is snapped to zero.
   */
  void ScaleWeights(double factor);

  ///@{
  /**
   * Get/set the (single-component) weight assigned to the stencil's center point.
   */
  double GetCenterWeight() {return this->CenterWeight[0];};
  void SetCenterWeight(double weight) {this->CenterWeight[0] = weight;};
  ///@}

  /**
   * Get a pointer to the full center-weight tuple (NumberOfComponents values).
   */
  const double* GetCenterWeightTuple() {return this->CenterWeight;};

  ///@{
  /**
   * Get/set the weight of the given component of the stencil's center point, for multi-component
   * stencils (see NumberOfComponents).
   */
  double GetCenterWeight(vtkIdType component) {return this->CenterWeight[component];};
  void SetCenterWeight(vtkIdType component, double weightComponent) {this->CenterWeight[component] = weightComponent;};
  ///@}

//  vtkSetMacro(CenterWeight,double);
//  vtkGetMacro(CenterWeight,double);

  /**
   * Build the stencil.
   */
  virtual void Build() override = 0;

  /**
   * Standard DeepCopy method.
   */
  virtual void DeepCopy(vtkvmtkItem *src) override;

  ///@{
  /**
   * Toggle whether ScaleWeights() actually rescales the stencil weights when called; if off,
   * ScaleWeights() is a no-op. Default: on.
   */
  vtkSetMacro(WeightScaling,int)
  vtkGetMacro(WeightScaling,int)
  vtkBooleanMacro(WeightScaling,int)
  ///@}

  ///@{
  /**
   * Toggle whether neighbor weights are sign-negated by ChangeWeightSign() after being computed
   * by a subclass's Build(); most differential-operator stencils are built with a natural sign
   * convention and are negated here so that applying the stencil yields the desired displacement/
   * operator sign. Default: on.
   */
  vtkSetMacro(NegateWeights,int)
  vtkGetMacro(NegateWeights,int)
  vtkBooleanMacro(NegateWeights,int)
  ///@}

protected:
  vtkvmtkStencil();
  ~vtkvmtkStencil();

  void ResizePointList(vtkIdType ptId, int size);

  void ChangeWeightSign();

  vtkIdType NumberOfComponents;
  double* Weights;
  double* CenterWeight;

  int WeightScaling;

  int NegateWeights;
  
private:
  vtkvmtkStencil(const vtkvmtkStencil&);  // Not implemented.
  void operator=(const vtkvmtkStencil&);  // Not implemented.
};

#endif

