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
 * @class   vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter
 * @brief   Compute a harmonic mapping over each branch of a bifurcated surface, treating every
 * branch as an independent open cylinder.
 * @ingroup DifferentialGeometry
 *
 * Given an input surface whose points are tagged with a per-branch group id (see
 * GroupIdsArrayName, typically produced while splitting a vascular surface into branches), this
 * filter extracts each group in turn, treats it as an independent open cylindrical surface, and
 * computes a harmonic mapping over it with vtkvmtkPolyDataCylinderHarmonicMappingFilter (a
 * longitudinal scalar field ranging between the two open boundaries of the cylinder). The
 * per-branch results are merged into a single point data array (HarmonicMappingArrayName) on the
 * output, which retains all points/cells of the input. This is used by the vmtkbranchmapping pype
 * script as the first step in building a longitudinal metric for centerline-based surface
 * parameterization, later stretched to correctly account for the presence of insertion regions at
 * bifurcations.
 *
 * @sa vtkvmtkPolyDataCylinderHarmonicMappingFilter, vtkvmtkUnstructuredGridHarmonicMappingFilter
 */

#ifndef __vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter_h
#define __vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the name of the point data array where the computed per-branch harmonic mapping
   * scalar is stored on the output surface.
   * Commonly named "HarmonicMapping".
   */
  vtkSetStringMacro(HarmonicMappingArrayName);
  vtkGetStringMacro(HarmonicMappingArrayName);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data array of the input surface holding the branch/group id
   * that each point belongs to. Each distinct group id is extracted and processed independently
   * as its own open cylinder.
   * Commonly named "GroupIds".
   */
  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);
  ///@}

protected:
  vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter();
  ~vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* HarmonicMappingArrayName;
  char* GroupIdsArrayName;

private:
  vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter(const vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataMultipleCylinderHarmonicMappingFilter&);  // Not implemented.
};

#endif

