/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataRigidSurfaceModelling.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataRigidSurfaceModelling - Not implemented; no utility developed yet.
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataRigidSurfaceModelling_h
#define __vtkvmtkPolyDataRigidSurfaceModelling_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataRigidSurfaceModelling : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataRigidSurfaceModelling *New();
  vtkTypeMacro(vtkvmtkPolyDataRigidSurfaceModelling,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

protected:
  vtkvmtkPolyDataRigidSurfaceModelling();
  ~vtkvmtkPolyDataRigidSurfaceModelling();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkvmtkPolyDataRigidSurfaceModelling(const vtkvmtkPolyDataRigidSurfaceModelling&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataRigidSurfaceModelling&);  // Not implemented.
};

#endif

