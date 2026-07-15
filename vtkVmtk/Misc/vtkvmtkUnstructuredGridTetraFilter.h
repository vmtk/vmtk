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
 * @class   vtkvmtkUnstructuredGridTetraFilter
 * @brief   Convert the elements of a mesh to linear tetrahedra.
 * @ingroup Misc
 *
 * Subdivides every 3D cell of the input unstructured grid (hexahedra, wedges, pyramids, as well as
 * higher-order/quadratic cells) into linear tetrahedra, using vtkOrderedTriangulator for
 * non-tetrahedral cell types. This is the filter behind the vmtkmeshtetrahedralize pype script,
 * used e.g. to prepare a mesh (or its boundary quads) for solvers that require a pure tetrahedral
 * mesh.
 */

#ifndef __vtkvmtkUnstructuredGridTetraFilter_h
#define __vtkvmtkUnstructuredGridTetraFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkOrderedTriangulator;
class vtkIdList;

class VTK_VMTK_MISC_EXPORT vtkvmtkUnstructuredGridTetraFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridTetraFilter *New();
  vtkTypeMacro(vtkvmtkUnstructuredGridTetraFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Toggle discarding non-tetrahedral output cells (e.g. leftover triangles from boundary quad
   * splitting) so the output contains only tetrahedra. Default: off.
   */
  vtkSetMacro(TetrahedraOnly, int);
  vtkGetMacro(TetrahedraOnly, int);
  vtkBooleanMacro(TetrahedraOnly, int);
  ///@}

protected:
  vtkvmtkUnstructuredGridTetraFilter();
  ~vtkvmtkUnstructuredGridTetraFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int TriangulateQuad(vtkUnstructuredGrid* output, vtkIdList *quadPtIds, vtkIdList *ptIds);

  vtkOrderedTriangulator *Triangulator;

  void Execute(vtkUnstructuredGrid *, vtkUnstructuredGrid *);

  int TetrahedraOnly;

private:
  vtkvmtkUnstructuredGridTetraFilter(const vtkvmtkUnstructuredGridTetraFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridTetraFilter&);  // Not implemented.
};

#endif

