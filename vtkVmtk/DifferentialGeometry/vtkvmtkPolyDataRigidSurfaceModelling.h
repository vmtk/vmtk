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
 * @class   vtkvmtkPolyDataRigidSurfaceModelling
 * @brief   Serves as a placeholder filter for rigid surface modelling; not yet implemented.
 * @ingroup DifferentialGeometry
 *
 * This class is a stub reserved for a future rigid-surface-modelling filter (e.g. fitting or
 * deforming a surface under rigid-body constraints). Its RequestData() currently only reports an
 * error ("hasn't been implemented yet") and produces no output; it performs no useful processing
 * and has no pype script wrapper.
 */

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
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

protected:
  vtkvmtkPolyDataRigidSurfaceModelling();
  ~vtkvmtkPolyDataRigidSurfaceModelling();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkvmtkPolyDataRigidSurfaceModelling(const vtkvmtkPolyDataRigidSurfaceModelling&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataRigidSurfaceModelling&);  // Not implemented.
};

#endif

