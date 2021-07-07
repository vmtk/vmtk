/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearToQuadraticMeshFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
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
// .NAME vtkvmtkLinearToQuadraticMeshFilter - Converts linear mesh elements to quadratic mesh elements (optionally) by executing by projecting mid side nodes onto the surface and relaxing projection if Jacobian is negative, otherwise does not project nodes.
// .SECTION Description
// ...

#ifndef __vtkvmtkLinearToQuadraticMeshFilter_h
#define __vtkvmtkLinearToQuadraticMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkCell.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearToQuadraticMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearToQuadraticMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkLinearToQuadraticMeshFilter *New();
  
  vtkSetMacro(UseBiquadraticWedge,int);
  vtkGetMacro(UseBiquadraticWedge,int);
  vtkBooleanMacro(UseBiquadraticWedge,int);

  vtkSetMacro(NumberOfNodesHexahedra,int);
  vtkGetMacro(NumberOfNodesHexahedra,int);

  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(ProjectedCellEntityId,int);
  vtkGetMacro(ProjectedCellEntityId,int);

  vtkSetMacro(QuadratureOrder,int);
  vtkGetMacro(QuadratureOrder,int);

  vtkSetMacro(NegativeJacobianTolerance,double);
  vtkGetMacro(NegativeJacobianTolerance,double);

  vtkSetMacro(JacobianRelaxation,int);
  vtkGetMacro(JacobianRelaxation,int);
  vtkBooleanMacro(JacobianRelaxation,int);

  vtkSetMacro(TestFinalJacobians,int);
  vtkGetMacro(TestFinalJacobians,int);
  vtkBooleanMacro(TestFinalJacobians,int);

  protected:
  vtkvmtkLinearToQuadraticMeshFilter();
  ~vtkvmtkLinearToQuadraticMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  bool HasJacobianChangedSign(vtkCell* linearVolumeCell, vtkCell* quadraticVolumeCell);
  double ComputeJacobian(vtkCell* cell, double pcoords[3]);

  int UseBiquadraticWedge;

  int NumberOfNodesHexahedra;

  vtkPolyData* ReferenceSurface;

  char* CellEntityIdsArrayName;
  int ProjectedCellEntityId;
  int QuadratureOrder;
  double NegativeJacobianTolerance;

  int JacobianRelaxation;
  int TestFinalJacobians;

  private:
  vtkvmtkLinearToQuadraticMeshFilter(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
};

#endif
