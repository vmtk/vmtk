/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfMeshWrapper.h,v $
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
// .NAME vtkvmtkSurfMeshWrapper - wrapper around surfmesh surface mesh generator by Gordan Stuhne; May not be working. 
// .SECTION Description
// ...

#ifndef __vtkvmtkSurfMeshWrapper_h
#define __vtkvmtkSurfMeshWrapper_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkSurfMeshWrapper : public vtkPolyDataAlgorithm
{
  public: 
  static vtkvmtkSurfMeshWrapper *New();
  vtkTypeMacro(vtkvmtkSurfMeshWrapper,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  vtkSetMacro(NodeSpacing,double);
  vtkGetMacro(NodeSpacing,double);

  protected:
  vtkvmtkSurfMeshWrapper();
  ~vtkvmtkSurfMeshWrapper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double NodeSpacing;

  private:
  vtkvmtkSurfMeshWrapper(const vtkvmtkSurfMeshWrapper&);  // Not implemented.
  void operator=(const vtkvmtkSurfMeshWrapper&);  // Not implemented.
};

#endif
