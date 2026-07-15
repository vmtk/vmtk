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
 * @class   vtkvmtkLinearToQuadraticSurfaceMeshFilter
 * @brief   Converts linear surface elements to quadratic surface elements via either linear subdivision of laplacian (butterfly) subdivision.
 * @ingroup Misc
 *
 * This filter takes a pure surface (triangulated) mesh, given as an unstructured grid, and
 * produces the corresponding 6-node quadratic triangle mesh (VTK_QUADRATIC_TRIANGLE) by inserting
 * one mid-edge node per triangle edge. Internally it uses one subdivision pass of a VTK
 * interpolating subdivision filter -- either vtkLinearSubdivisionFilter (new nodes placed exactly
 * at edge midpoints) or vtkButterflySubdivisionFilter (new nodes placed using the butterfly
 * stencil, which also perturbs the existing vertex positions towards a smoother, curved surface)
 * -- selected through SubdivisionMethod, and reassembles the subdivided triangles into quadratic
 * cells while carrying over point and cell data.
 *
 * This is one of the filters behind the vmtklineartoquadratic pype script (used, instead of
 * vtkvmtkLinearToQuadraticMeshFilter, when the input is a pure surface mesh rather than a
 * volumetric mesh), typically used to upgrade a linear triangulated surface to a curved,
 * second-order surface mesh.
 *
 * @sa
 * vtkvmtkLinearToQuadraticMeshFilter
 */

#ifndef __vtkvmtkLinearToQuadraticSurfaceMeshFilter_h
#define __vtkvmtkLinearToQuadraticSurfaceMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearToQuadraticSurfaceMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearToQuadraticSurfaceMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkLinearToQuadraticSurfaceMeshFilter *New();
  
  ///@{
  /**
   * Set/Get the subdivision method used to place the new mid-edge nodes: LINEAR_SUBDIVISION (exact
   * edge midpoints) or BUTTERFLY_SUBDIVISION (butterfly-stencil interpolation, which also smooths
   * the existing vertex positions). Prefer the SetSubdivisionMethodToLinear /
   * SetSubdivisionMethodToButterfly convenience methods over setting this enum value directly.
   */
  vtkSetMacro(SubdivisionMethod,int);
  vtkGetMacro(SubdivisionMethod,int);
  ///@}
  /**
   * Select LINEAR_SUBDIVISION (new nodes placed exactly at edge midpoints) as the SubdivisionMethod.
   */
  void SetSubdivisionMethodToLinear()
  { this->SubdivisionMethod = LINEAR_SUBDIVISION; }
  /**
   * Select BUTTERFLY_SUBDIVISION (new nodes placed using the butterfly stencil, which also smooths
   * existing vertex positions) as the SubdivisionMethod.
   */
  void SetSubdivisionMethodToButterfly()
  { this->SubdivisionMethod = BUTTERFLY_SUBDIVISION; }

  protected:
  vtkvmtkLinearToQuadraticSurfaceMeshFilter();
  ~vtkvmtkLinearToQuadraticSurfaceMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int SubdivisionMethod;

  //BTX
  enum
  {
    LINEAR_SUBDIVISION,
    BUTTERFLY_SUBDIVISION
  };
  //ETX

  private:
  vtkvmtkLinearToQuadraticSurfaceMeshFilter(const vtkvmtkLinearToQuadraticSurfaceMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearToQuadraticSurfaceMeshFilter&);  // Not implemented.
};

#endif
