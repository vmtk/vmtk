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
 * @class   vtkvmtkPolyDataToUnstructuredGridFilter
 * @brief   Repackage a vtkPolyData surface as a vtkUnstructuredGrid, unchanged otherwise.
 * @ingroup Misc
 *
 * This filter copies the points, cells (of whatever type they are -- vertices, lines,
 * polygons, triangle strips), point data, and cell data of the input poly data into an
 * unstructured grid output, without altering geometry, topology, or connectivity. It performs
 * no interpolation, subdivision, or tetrahedralization: the output has exactly the same cells
 * as the input, only represented with the more general vtkUnstructuredGrid data structure.
 *
 * This is used by the vmtksurfacetomesh pype script to turn a (typically triangulated) surface
 * into the mesh representation ("vtkUnstructuredGrid") expected by vmtk's mesh I/O and mesh
 * processing scripts, e.g. as a lightweight way to obtain a "surface mesh" prior to volumetric
 * mesh generation.
 */

#ifndef __vtkvmtkPolyDataToUnstructuredGridFilter_h
#define __vtkvmtkPolyDataToUnstructuredGridFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataToUnstructuredGridFilter : public vtkUnstructuredGridAlgorithm
{
public:
  vtkTypeMacro(vtkvmtkPolyDataToUnstructuredGridFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataToUnstructuredGridFilter *New();

protected:
  vtkvmtkPolyDataToUnstructuredGridFilter() {}
  ~vtkvmtkPolyDataToUnstructuredGridFilter() {}

  int FillInputPortInformation(int, vtkInformation *info) override;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  
private:
  vtkvmtkPolyDataToUnstructuredGridFilter(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataToUnstructuredGridFilter&);  // Not implemented.
};

#endif


