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
 * @class   vtkvmtkMeshProjection
 * @brief   Project point data from a reference mesh onto a query mesh.
 * @ingroup Misc
 *
 * For each point of the input (query) mesh, finds the enclosing cell (within Tolerance) of
 * ReferenceMesh and interpolates every point data array present there onto the query point. Used,
 * e.g., to transfer a computed solution (velocity, pressure, wall shear stress) from one mesh
 * discretization onto another (differently meshed or remeshed) geometry.
 */

#ifndef __vtkvmtkMeshProjection_h
#define __vtkvmtkMeshProjection_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkMeshProjection : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkMeshProjection,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkMeshProjection *New();

  ///@{
  /**
   * Set/Get the mesh whose point data is projected onto the input (query) mesh. Required input.
   */
  vtkSetObjectMacro(ReferenceMesh,vtkUnstructuredGrid);
  vtkGetObjectMacro(ReferenceMesh,vtkUnstructuredGrid);
  ///@}

  ///@{
  /**
   * Set/Get the parametric-coordinate tolerance used when locating the ReferenceMesh cell enclosing
   * each query point.
   */
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);
  ///@}

  protected:
  vtkvmtkMeshProjection();
  ~vtkvmtkMeshProjection();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkUnstructuredGrid *ReferenceMesh;
  double Tolerance;

  private:
  vtkvmtkMeshProjection(const vtkvmtkMeshProjection&);  // Not implemented.
  void operator=(const vtkvmtkMeshProjection&);  // Not implemented.
};

#endif
