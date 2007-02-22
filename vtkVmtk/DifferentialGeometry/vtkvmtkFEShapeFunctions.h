/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkFEShapeFunctions.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkFEShapeFunctions - ..
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
  vtkTypeRevisionMacro(vtkvmtkFEShapeFunctions,vtkObject);
  static vtkvmtkFEShapeFunctions* New();

  void Initialize(vtkCell* cell, vtkDoubleArray* pcoords);

  double GetJacobian(vtkIdType i)
  { return this->Jacobians->GetValue(i); }

protected:
  vtkvmtkFEShapeFunctions();
  ~vtkvmtkFEShapeFunctions();

  double ComputeJacobian(vtkCell* cell, double* pcoords);
  void GetInterpolationDerivs(vtkCell* cell, double* pcoords, double* derivs);

  vtkDoubleArray* Phi;
  vtkDoubleArray* DPhi;
  vtkDoubleArray* Jacobians;

private:  
  vtkvmtkFEShapeFunctions(const vtkvmtkFEShapeFunctions&);  // Not implemented.
  void operator=(const vtkvmtkFEShapeFunctions&);  // Not implemented.

};

#endif
