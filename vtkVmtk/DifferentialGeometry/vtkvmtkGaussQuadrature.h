/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkGaussQuadrature.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkGaussQuadrature - Finite element quadrature rule; Approximation of the definite integral of a function, as a weighted sum of function values at specified points.
// .SECTION Description
// ..

#ifndef __vtkvmtkGaussQuadrature_h
#define __vtkvmtkGaussQuadrature_h

#include "vtkObject.h"
#include "vtkvmtkWin32Header.h"

#include "vtkCell.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkGaussQuadrature : public vtkObject
{
public:
  vtkTypeMacro(vtkvmtkGaussQuadrature,vtkObject);
  static vtkvmtkGaussQuadrature* New();

  vtkGetObjectMacro(QuadraturePoints,vtkDoubleArray);
  vtkGetObjectMacro(QuadratureWeights,vtkDoubleArray);

  vtkSetMacro(Order,int);
  vtkGetMacro(Order,int);
  
  int GetNumberOfQuadraturePoints()
  {
    return this->QuadraturePoints->GetNumberOfTuples();
  }
 
  double* GetQuadraturePoint(vtkIdType id)
  {
    return this->QuadraturePoints->GetTuple(id);
  }
 
  void GetQuadraturePoint(vtkIdType id, double* quadraturePoint)
  {
    this->QuadraturePoints->GetTuple(id,quadraturePoint);
  }
 
  double GetQuadraturePoint(vtkIdType id, int c)
  {
    return this->QuadraturePoints->GetComponent(id,c);
  }
 
  double GetQuadratureWeight(vtkIdType id)
  {
    return this->QuadratureWeights->GetValue(id);
  }
 
  void Initialize(vtkIdType cellType);

  void Initialize(vtkCell* cell)
  {
    this->Initialize(cell->GetCellType());
  }
 
  void Initialize1DGauss();
  void Initialize1DJacobi(int alpha, int beta);
  void ScaleTo01();
 
protected:
  vtkvmtkGaussQuadrature();
  ~vtkvmtkGaussQuadrature();

  void TensorProductQuad(vtkvmtkGaussQuadrature* q1D);
  void TensorProductTriangle(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D);
  
  void TensorProductHexahedron(vtkvmtkGaussQuadrature* q1D);
  void TensorProductWedge(vtkvmtkGaussQuadrature* q1D, vtkvmtkGaussQuadrature* q2D);
  void TensorProductTetra(vtkvmtkGaussQuadrature* gauss1D, vtkvmtkGaussQuadrature* jacA1D, vtkvmtkGaussQuadrature* jacB1D);
 
  vtkDoubleArray* QuadraturePoints;
  vtkDoubleArray* QuadratureWeights;
 
  int Order;
  int QuadratureType;
  vtkIdType CellType;
  int PreviousOrder;

private:  
  vtkvmtkGaussQuadrature(const vtkvmtkGaussQuadrature&);  // Not implemented.
  void operator=(const vtkvmtkGaussQuadrature&);  // Not implemented.

};

#endif
