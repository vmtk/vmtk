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
 * @class   vtkvmtkDoubleVector
 * @brief   Serves as the base class for vectors utilized in finite-element and other PDE evaluations.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkDoubleVector wraps a plain double array with the bookkeeping needed by vmtk's
 * finite-element machinery: it can be Allocate()d for a given number of elements per variable and
 * number of variables (so that, e.g., a solution with several unknowns per node can be stored as
 * one contiguous array, see CopyVariableIntoArrayComponent), and it provides basic linear-algebra
 * operations (Add, Subtract, MultiplyBy, Dot, ComputeNorm) used by iterative linear system solvers.
 * It is used throughout the DifferentialGeometry classes as the concrete type for right-hand-side
 * and solution vectors (see vtkvmtkLinearSystem, vtkvmtkFEAssembler).
 *
 * @sa
 * vtkvmtkLinearSystem, vtkvmtkFEAssembler, vtkvmtkLinearSystemSolver
 */

#ifndef __vtkvmtkDoubleVector_h
#define __vtkvmtkDoubleVector_h

#include "vtkObject.h"
#include "vtkDataArray.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#define VTK_VMTK_L2_NORM 0
#define VTK_VMTK_LINF_NORM 1

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkDoubleVector : public vtkObject
{
public:

  static vtkvmtkDoubleVector* New();
  vtkTypeMacro(vtkvmtkDoubleVector,vtkObject);

  ///@{
  /**
   * Set/get the norm type computed by ComputeNorm(): VTK_VMTK_L2_NORM (Euclidean norm, default) or
   * VTK_VMTK_LINF_NORM (maximum element value).
   */
  vtkSetMacro(NormType,int);
  vtkGetMacro(NormType,int);
  void SetNormTypeToL2()
    {this->SetNormType(VTK_VMTK_L2_NORM);};
  void SetNormTypeToLInf()
    {this->SetNormType(VTK_VMTK_LINF_NORM);};
  ///@}

  /**
   * Get the total number of elements in the vector, i.e. NumberOfElementsPerVariable *
   * NumberOfVariables.
   */
  vtkGetMacro(NumberOfElements,vtkIdType);

  /**
   * Get the number of elements per variable, as set by the last call to Allocate().
   */
  vtkGetMacro(NumberOfElementsPerVariable,vtkIdType);

  /**
   * Get the number of variables (i.e. the number of consecutive blocks of
   * NumberOfElementsPerVariable elements), as set by the last call to Allocate().
   */
  vtkGetMacro(NumberOfVariables,vtkIdType);

  /**
   * Allocate the internal array to hold numberOfElementsPerVariable * numberOfVariables elements,
   * discarding any previous content. The array is laid out as numberOfVariables consecutive blocks
   * of numberOfElementsPerVariable elements each (see CopyVariableIntoArrayComponent).
   */
  void Allocate(vtkIdType numberOfElementsPerVariable, vtkIdType numberOfVariables=1);

  /**
   * Set every element of the vector to value.
   */
  void Fill(double value);

  /**
   * Copy the contents of src into this vector. The two vectors must already have the same number of
   * elements.
   */
  void Assign(vtkvmtkDoubleVector *src);

  /**
   * Copy numberOfElements values from the raw C array into this vector's internal array (which must
   * already be allocated with at least that many elements).
   */
  void Assign(vtkIdType numberOfElements, const double *array);

  /**
   * Get the value of the i-th element.
   */
  double GetElement(vtkIdType i) {return this->Array[i];};

  /**
   * Set the value of the i-th element.
   */
  void SetElement(vtkIdType i, double value) {this->Array[i] = value;};

  /**
   * Add value to the current value of the i-th element.
   */
  void AddElement(vtkIdType i, double value) {this->Array[i] += value;};

  //bool GetLocked(vtkIdType i) {return this->Locked[i];}
  //void SetLocked(vtkIdType i, bool locked) {this->Locked[i] = locked;}
  //void UnlockAll();

  /**
   * Get direct read-only access to the underlying raw double array.
   */
  const double* GetArray() {return this->Array;};

  /**
   * Copy this vector's elements into the given component of array, growing array's number of
   * components and/or tuples as needed to fit.
   */
  void CopyIntoArrayComponent(vtkDataArray *array, int component);

  /**
   * Copy the block of NumberOfElementsPerVariable elements belonging to the given variable index
   * into the given component of array, growing array's number of components and/or tuples as
   * needed to fit.
   */
  void CopyVariableIntoArrayComponent(vtkDataArray *array, int variable, int component);

  /**
   * Compute and return the norm of the vector, according to NormType.
   */
  double ComputeNorm();

  /**
   * Add vectorToAdd element-wise to this vector (in place). The two vectors must have the same
   * number of elements.
   */
  void Add(vtkvmtkDoubleVector* vectorToAdd);

  /**
   * Subtract vectorToSubtract element-wise from this vector (in place). The two vectors must have
   * the same number of elements.
   */
  void Subtract(vtkvmtkDoubleVector* vectorToSubtract);

  /**
   * Multiply every element of this vector by scalar (in place).
   */
  void MultiplyBy(double scalar);

  /**
   * Compute and return the dot product of this vector with vectorToDotWith. The two vectors must
   * have the same number of elements.
   */
  double Dot(vtkvmtkDoubleVector* vectorToDotWith);

  /**
   * Reallocate this vector's internal array and copy into it the size and contents of src.
   */
  void DeepCopy(vtkvmtkDoubleVector *src);

protected:
  vtkvmtkDoubleVector();
  ~vtkvmtkDoubleVector();

  vtkIdType NumberOfElements;
  vtkIdType NumberOfElementsPerVariable;
  vtkIdType NumberOfVariables;

  int NormType;

  double* Array;
  //bool* Locked;

private:
  vtkvmtkDoubleVector(const vtkvmtkDoubleVector&);  // Not implemented.
  void operator=(const vtkvmtkDoubleVector&);  // Not implemented.
};

#endif

