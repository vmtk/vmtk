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
 * @class   vtkvmtkPolyDataDiscreteElasticaFilter
 * @brief   Not implemented; smooths a surface via an elastic-forces method.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataDiscreteElasticaFilter is a placeholder vtkPolyDataAlgorithm: its RequestData body
 * (an iterative mean/Gaussian-curvature-driven point displacement scheme, intended as a discrete
 * "elastica" surface-smoothing method) is currently disabled at compile time (guarded out with
 * "#if 0" in the .cxx file), so this class has no functional implementation and should not be used.
 */

#ifndef __vtkvmtkPolyDataDiscreteElasticaFilter_h
#define __vtkvmtkPolyDataDiscreteElasticaFilter_h

#include "vtkObject.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataDiscreteElasticaFilter : public vtkPolyDataAlgorithm
{
public:

  static vtkvmtkPolyDataDiscreteElasticaFilter *New();
  vtkTypeMacro(vtkvmtkPolyDataDiscreteElasticaFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

protected:
  vtkvmtkPolyDataDiscreteElasticaFilter();
  ~vtkvmtkPolyDataDiscreteElasticaFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkvmtkPolyDataDiscreteElasticaFilter(const vtkvmtkPolyDataDiscreteElasticaFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDiscreteElasticaFilter&);  // Not implemented.
};

#endif

