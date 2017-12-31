/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkMeshProjection.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkMeshProjection - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkMeshProjection_h
#define __vtkvmtkMeshProjection_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshProjection : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshProjection,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkMeshProjection *New();

  vtkSetObjectMacro(ReferenceMesh,vtkUnstructuredGrid);
  vtkGetObjectMacro(ReferenceMesh,vtkUnstructuredGrid);

  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  protected:
  vtkvmtkMeshProjection();
  ~vtkvmtkMeshProjection();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  vtkUnstructuredGrid *ReferenceMesh;
  double Tolerance;

  private:
  vtkvmtkMeshProjection(const vtkvmtkMeshProjection&);  // Not implemented.
  void operator=(const vtkvmtkMeshProjection&);  // Not implemented.
};

#endif
