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
 * @class   vtkvmtkCenterlineAttributesFilter
 * @brief   Computes the abscissa and parallel transport normal along a centerline.
 * @ingroup ComputationalGeometry
 *
 * Abscissas are easy to define: they measure the distances along a line. This class generates abscissas relative to the starting point of the centerline and can be offset to a different location at a later time.
 *
 * Parallel Transport Normals are used to help construct an osculating reference system along a centerline. We generate a pair of arbitrary normal and binormal orthogonal vectors are generated for the first point along the centerline.  The reference system created by the orthogonal normal, binormal, and tangent vectors is propagated down the centerline by rotating it (in the osculating plane) by an amount equal to the change in orientation of the line tangent between neighboring pairs of points.
 *
 * This is the filter behind the vmtkcenterlineattributes pype script; its output (abscissas and
 * parallel transport normals) is a required input for centerline-based surface mapping/patching
 * (e.g. vtkvmtkPolyDataStretchMappingFilter).
 *
 * @sa
 * vtkvmtkCenterlineGeometry, vtkvmtkPolyDataStretchMappingFilter
 */

#ifndef __vtkvmtkCenterlineAttributesFilter_h
#define __vtkvmtkCenterlineAttributesFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineAttributesFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineAttributesFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineAttributesFilter* New();

  ///@{
  /**
   * Set/Get the name of the point data array where the abscissa (arc-length distance from the
   * centerline's starting point) is stored for each point.
   * Commonly named "Abscissas".
   */
  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 3-component point data array where the parallel-transported normal
   * vector is stored for each point (see class description for how it's propagated along the
   * centerline).
   * Commonly named "ParallelTransportNormals".
   */
  vtkSetStringMacro(ParallelTransportNormalsArrayName);
  vtkGetStringMacro(ParallelTransportNormalsArrayName);
  ///@}

  protected:
  vtkvmtkCenterlineAttributesFilter();
  ~vtkvmtkCenterlineAttributesFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeAbscissas(vtkPolyData* input, vtkDoubleArray* abscissasArray);
  void ComputeParallelTransportNormals(vtkPolyData* input, vtkDoubleArray* parallelTransportNormalsArray);

  char* AbscissasArrayName;
  char* ParallelTransportNormalsArrayName;

  private:
  vtkvmtkCenterlineAttributesFilter(const vtkvmtkCenterlineAttributesFilter&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineAttributesFilter&);  // Not implemented.
};

#endif
