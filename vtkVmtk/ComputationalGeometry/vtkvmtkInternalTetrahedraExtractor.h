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
 * @class   vtkvmtkInternalTetrahedraExtractor
 * @brief   Extract internal tetrahedra from a Delaunay tessellation of a surface.
 * @ingroup ComputationalGeometry
 *
 * This class takes in input the Delaunay tessellation of a point set and extracts internal tetrahedra based on outward oriented point normals (to be provided as input point data array). A tetrahedron \f$T_i\f$ is retained if \f[(x_j - c_i) \cdot n_j \geq 0  \qquad  \forall x_j \in T_i \f] where \f$x_i\f$ are the vertices of \f$T_i\f$, \f$c_i\f$ its circumcenter and \f$n_j\f$ the normals at the vertices. It is possible to properly handle capped regions (generated with vtkCapPolyData) by activating UseCaps and providing the ids of cap centers.
 *
 * @sa
 * vtkCapPolyData
 */

#ifndef __vtkvmtkInternalTetrahedraExtractor_h
#define __vtkvmtkInternalTetrahedraExtractor_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkInternalTetrahedraExtractor : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkInternalTetrahedraExtractor,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkInternalTetrahedraExtractor *New();
  
  ///@{
  /**
   * Set/Get the name of the array containing outward oriented point normals.
   */
  vtkSetStringMacro(OutwardNormalsArrayName);
  vtkGetStringMacro(OutwardNormalsArrayName);
  ///@}

  ///@{
  /**
   * Turn on/off special handling of caps.
   */
  vtkSetMacro(UseCaps,int);
  vtkGetMacro(UseCaps,int);
  vtkBooleanMacro(UseCaps,int);
  ///@}

  ///@{
  /**
   * Set/Get the ids of cap centers.
   */
  vtkSetObjectMacro(CapCenterIds,vtkIdList);
  vtkGetObjectMacro(CapCenterIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the numerical tolerance used when testing the retention inequality for each tetrahedron.
   * Default: VTK_VMTK_DOUBLE_TOL.
   */
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);
  ///@}

  ///@{
  /**
   * Turn on/off removal of surface slivers.
   */
  vtkSetMacro(RemoveSubresolutionTetrahedra,int);
  vtkGetMacro(RemoveSubresolutionTetrahedra,int);
  vtkBooleanMacro(RemoveSubresolutionTetrahedra,int);
  ///@}

  ///@{
  /**
   * Set/Get the factor, relative to local feature size, below which a surface tetrahedron is
   * considered a sub-resolution sliver and removed when RemoveSubresolutionTetrahedra is on.
   * Requires Surface to be set. Default: 1.0.
   */
  vtkSetMacro(SubresolutionFactor,double);
  vtkGetMacro(SubresolutionFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the original (uncapped or capped) surface used as reference when
   * RemoveSubresolutionTetrahedra is on, to identify and discard degenerate surface slivers.
   */
  vtkSetObjectMacro(Surface,vtkPolyData);
  vtkGetObjectMacro(Surface,vtkPolyData);
  ///@}

  protected:
  vtkvmtkInternalTetrahedraExtractor();
  ~vtkvmtkInternalTetrahedraExtractor();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int UseCaps;
  vtkIdList* CapCenterIds;
  char* OutwardNormalsArrayName;

  double Tolerance;

  int RemoveSubresolutionTetrahedra;
  vtkPolyData* Surface;
  double SubresolutionFactor;

  private:
  vtkvmtkInternalTetrahedraExtractor(const vtkvmtkInternalTetrahedraExtractor&);  // Not implemented.
  void operator=(const vtkvmtkInternalTetrahedraExtractor&);  // Not implemented.
};

#endif
