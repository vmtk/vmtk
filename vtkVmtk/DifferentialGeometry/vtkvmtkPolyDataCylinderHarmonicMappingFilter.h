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
 * @class   vtkvmtkPolyDataCylinderHarmonicMappingFilter
 * @brief   Create a single instance wrapping a harmonic function over a cylinder so that mapping can be applied with stretching of the longitudinal metric.
 * @ingroup DifferentialGeometry
 *
 * vtkvmtkPolyDataCylinderHarmonicMappingFilter specializes vtkvmtkPolyDataHarmonicMappingFilter for
 * an input surface that is topologically a cylinder (i.e. has exactly two open boundaries, as
 * detected with vtkvmtkPolyDataBoundaryExtractor). It automatically builds the Dirichlet boundary
 * conditions expected by the base class (BoundaryPointIds / BoundaryValues) by assigning value 0 to
 * every point on the first boundary and value 1 to every point on the second, then delegates to
 * vtkvmtkPolyDataHarmonicMappingFilter::RequestData to solve the harmonic (Laplace) problem. The
 * result is a scalar field, stored under HarmonicMappingArrayName, that increases monotonically from
 * one end of the cylinder to the other and can be used as a longitudinal (streamwise) surface
 * parameterization. If the input does not have exactly two boundaries, the input is passed through
 * unmodified and a warning is issued.
 *
 * @sa vtkvmtkPolyDataHarmonicMappingFilter, vtkvmtkPolyDataBoundaryExtractor
 */

#ifndef __vtkvmtkPolyDataCylinderHarmonicMappingFilter_h
#define __vtkvmtkPolyDataCylinderHarmonicMappingFilter_h

#include "vtkvmtkPolyDataHarmonicMappingFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataCylinderHarmonicMappingFilter : public vtkvmtkPolyDataHarmonicMappingFilter
{
public:
  static vtkvmtkPolyDataCylinderHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataCylinderHarmonicMappingFilter,vtkvmtkPolyDataHarmonicMappingFilter);

protected:
  vtkvmtkPolyDataCylinderHarmonicMappingFilter();
  ~vtkvmtkPolyDataCylinderHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkvmtkPolyDataCylinderHarmonicMappingFilter(const vtkvmtkPolyDataCylinderHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCylinderHarmonicMappingFilter&);  // Not implemented.
};

#endif

