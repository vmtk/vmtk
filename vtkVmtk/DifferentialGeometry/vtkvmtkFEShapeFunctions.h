/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkFEShapeFunctions.h,v $
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
// .NAME vtkvmtkFEShapeFunctions - Set of common utility (calculate jacobian, interpolation function, and interpolation function derivatives) functions used during assembly of certain finite element solutions.
// .SECTION Description
// ..

#ifndef __vtkvmtkFEShapeFunctions_h
#define __vtkvmtkFEShapeFunctions_h

#include "vtkObject.h"
#include "vtkvmtkWin32Header.h"

#include "vtkCell.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkFEShapeFunctions : public vtkObject
{
public:
  vtkTypeMacro(vtkvmtkFEShapeFunctions,vtkObject);
  static vtkvmtkFEShapeFunctions* New();

  void Initialize(vtkCell* cell, vtkDoubleArray* pcoords);

  double GetPhi(vtkIdType id, vtkIdType i)
  { return this->Phi->GetValue(id*this->NumberOfCellPoints+i); }

  double* GetDPhi(vtkIdType id, vtkIdType i)
  { return this->DPhi->GetTuple(id*this->NumberOfCellPoints+i); }

  void GetDPhi(vtkIdType id, vtkIdType i, double* dphi)
  { this->DPhi->GetTuple(id*this->NumberOfCellPoints+i,dphi); }

  double GetDPhi(vtkIdType id, vtkIdType i, int c)
  { return this->DPhi->GetComponent(id*this->NumberOfCellPoints+i,c); }

  double GetJacobian(vtkIdType i)
  { return this->Jacobians->GetValue(i); }

  static void GetInterpolationFunctions(vtkCell* cell, double* pcoords, double* sf);
  static void GetInterpolationDerivs(vtkCell* cell, double* pcoords, double* derivs);
 
  static double ComputeJacobian(vtkCell* cell, double* pcoords);

protected:
  vtkvmtkFEShapeFunctions();
  ~vtkvmtkFEShapeFunctions();
 
  static void ComputeInverseJacobianMatrix2D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[2][3]);
  static void ComputeInverseJacobianMatrix3D(vtkCell* cell, double* pcoords, double inverseJacobianMatrix[3][3]);

  vtkDoubleArray* Phi;
  vtkDoubleArray* DPhi;
  vtkDoubleArray* Jacobians;
  vtkIdType NumberOfCellPoints;

private:  
  vtkvmtkFEShapeFunctions(const vtkvmtkFEShapeFunctions&);  // Not implemented.
  void operator=(const vtkvmtkFEShapeFunctions&);  // Not implemented.

};

#endif
