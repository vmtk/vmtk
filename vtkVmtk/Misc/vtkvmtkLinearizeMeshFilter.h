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
 * @class   vtkvmtkLinearizeMeshFilter
 * @brief   Convert quadratic mesh elements to linear mesh elements by discarding mid-side nodes.
 * @ingroup Misc
 *
 * This filter takes an unstructured grid whose cells may be linear or quadratic (triangles, quads,
 * tetrahedra, hexahedra, wedges, and their quadratic/biquadratic/triquadratic variants) and
 * rebuilds every cell using only its corner nodes, dropping all mid-edge/mid-face/mid-volume nodes;
 * the corresponding linear cell type (e.g. VTK_QUADRATIC_TETRA becomes VTK_TETRA) is emitted for
 * each input cell, with cell data copied unchanged. Point data is copied only for corner points
 * that survive into the output. Points that end up unused by any output cell (i.e. former mid-side
 * nodes) are dropped from the output when CleanOutput is on.
 *
 * This is the filter behind the vmtkmeshlinearize pype script, used to strip a second-order
 * (quadratic) volume or surface mesh back down to first-order (linear) elements, e.g. for tools
 * that only support linear elements.
 *
 * @sa
 * vtkvmtkLinearToQuadraticMeshFilter, vtkvmtkLinearToQuadraticSurfaceMeshFilter
 */

#ifndef __vtkvmtkLinearizeMeshFilter_h
#define __vtkvmtkLinearizeMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearizeMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearizeMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkLinearizeMeshFilter *New();
  
  ///@{
  /**
   * Toggle discarding, from the output, points that are not used by any output (linear) cell --
   * i.e. the mid-side nodes dropped during linearization. When off, all input points are kept in
   * the output even though some are no longer referenced by any cell. Default: on.
   */
  vtkSetMacro(CleanOutput,int);
  vtkGetMacro(CleanOutput,int);
  vtkBooleanMacro(CleanOutput,int);
  ///@}

  protected:
  vtkvmtkLinearizeMeshFilter();
  ~vtkvmtkLinearizeMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int CleanOutput;

  private:
  vtkvmtkLinearizeMeshFilter(const vtkvmtkLinearizeMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearizeMeshFilter&);  // Not implemented.
};

#endif
