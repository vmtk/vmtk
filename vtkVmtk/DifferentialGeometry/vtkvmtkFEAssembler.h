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
 * @class   vtkvmtkFEAssembler
 * @brief   Serves as the base class on which specific finite element routines are built.
 * @ingroup DifferentialGeometry
 *
 * Supports common operations needed during setup of a finite element solver run.
 *
 * vtkvmtkFEAssembler holds the mesh (DataSet) and the finite-element linear-system pieces built
 * from it: the sparse system Matrix and the RHSVector / SolutionVector double vectors, all sized
 * for NumberOfVariables unknowns per mesh point. Concrete subclasses override Build() to loop over
 * the cells of DataSet, evaluate element-level contributions (typically with the help of
 * vtkvmtkGaussQuadrature and vtkvmtkFEShapeFunctions, at the quadrature order given by
 * QuadratureOrder), and assemble them into Matrix and RHSVector. Initialize() is a protected helper
 * that subclasses call from their constructor/Build() to allocate Matrix and the vectors for a
 * given number of variables.
 *
 * @sa
 * vtkvmtkGaussQuadrature, vtkvmtkFEShapeFunctions, vtkvmtkLinearSystem, vtkvmtkDoubleVector
 */

#ifndef __vtkvmtkFEAssembler_h
#define __vtkvmtkFEAssembler_h

#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkvmtkSparseMatrix.h"
#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkFEAssembler : public vtkObject
{
public:

  vtkTypeMacro(vtkvmtkFEAssembler,vtkObject);

  ///@{
  /**
   * Set/get the mesh (data set) that the finite element system is assembled over.
   */
  vtkSetObjectMacro(DataSet,vtkDataSet);
  vtkGetObjectMacro(DataSet,vtkDataSet);
  ///@}

  ///@{
  /**
   * Set/get the sparse matrix that Build() assembles the finite-element system matrix into.
   */
  vtkSetObjectMacro(Matrix,vtkvmtkSparseMatrix);
  vtkGetObjectMacro(Matrix,vtkvmtkSparseMatrix);
  ///@}

  ///@{
  /**
   * Set/get the vector that Build() assembles the finite-element system's right-hand side into.
   */
  vtkSetObjectMacro(RHSVector,vtkvmtkDoubleVector);
  vtkGetObjectMacro(RHSVector,vtkvmtkDoubleVector);
  ///@}

  ///@{
  /**
   * Set/get the vector holding the (unknown, to be solved for) solution of the assembled system.
   */
  vtkSetObjectMacro(SolutionVector,vtkvmtkDoubleVector);
  vtkGetObjectMacro(SolutionVector,vtkvmtkDoubleVector);
  ///@}

  /**
   * Get the number of unknowns per mesh point, as set by the last call to Initialize().
   */
  vtkGetMacro(NumberOfVariables,int);

  ///@{
  /**
   * Set/get the order of the Gauss quadrature rule used to numerically integrate element
   * contributions during assembly (see vtkvmtkGaussQuadrature). Default: 1.
   */
  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);
  ///@}

  /**
   * Assemble the finite-element system (Matrix and RHSVector) from DataSet. Implemented by
   * subclasses for a specific PDE/discretization.
   */
  virtual void Build() = 0;

  /**
   * Deep-copy DataSet, Matrix, RHSVector, SolutionVector, NumberOfVariables and QuadratureOrder
   * from src into this assembler.
   */
  void DeepCopy(vtkvmtkFEAssembler *src);

  /**
   * Share (reference-count, without copying) DataSet, Matrix, RHSVector and SolutionVector with
   * src, and copy NumberOfVariables and QuadratureOrder.
   */
  void ShallowCopy(vtkvmtkFEAssembler *src);

protected:
  vtkvmtkFEAssembler();
  ~vtkvmtkFEAssembler();

  void Initialize(int numberOfVariables);

  vtkDataSet* DataSet;
  vtkvmtkSparseMatrix* Matrix;
  vtkvmtkDoubleVector* RHSVector;
  vtkvmtkDoubleVector* SolutionVector;

  int NumberOfVariables;
  int QuadratureOrder;

private:
  vtkvmtkFEAssembler(const vtkvmtkFEAssembler&);  // Not implemented.
  void operator=(const vtkvmtkFEAssembler&);  // Not implemented.
};

#endif
