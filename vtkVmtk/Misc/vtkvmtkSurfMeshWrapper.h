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
 * @class   vtkvmtkSurfMeshWrapper
 * @brief   Wraps the surfmesh surface mesh generator by Gordan Stuhne; may not be working.
 * @ingroup Misc
 *
 * Intended to remesh the input surface to an approximately uniform triangle edge length of
 * NodeSpacing using the third-party "surfmesh" mesh generator by Gordan Stuhne. See
 * vtkvmtkPolyDataSurfaceRemeshing for an actively used alternative surface remeshing filter.
 *
 * @sa vtkvmtkPolyDataSurfaceRemeshing
 */

#ifndef __vtkvmtkSurfMeshWrapper_h
#define __vtkvmtkSurfMeshWrapper_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkSurfMeshWrapper : public vtkPolyDataAlgorithm
{
  public: 
  static vtkvmtkSurfMeshWrapper *New();
  vtkTypeMacro(vtkvmtkSurfMeshWrapper,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the target (approximately uniform) triangle edge length of the remeshed output.
   */
  vtkSetMacro(NodeSpacing,double);
  vtkGetMacro(NodeSpacing,double);
  ///@}

  protected:
  vtkvmtkSurfMeshWrapper();
  ~vtkvmtkSurfMeshWrapper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double NodeSpacing;

  private:
  vtkvmtkSurfMeshWrapper(const vtkvmtkSurfMeshWrapper&);  // Not implemented.
  void operator=(const vtkvmtkSurfMeshWrapper&);  // Not implemented.
};

#endif
