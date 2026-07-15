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
 * @class   vtkvmtkPolyDataKiteRemovalFilter
 * @brief   Removes small protrusions (kites) from a surface based on a SizeFactor parameter.
 * @ingroup Misc
 *
 * A "kite" is a small triangle (or connected group of triangles) whose area is much smaller than
 * the surrounding local average, typically a meshing artifact. For every triangle, this filter
 * compares its neighborhood's local average area against the global average area (scaled by
 * SizeFactor) and removes/patches over triangles that fall below the threshold. This is the filter
 * behind the vmtksurfacekiteremoval pype script, typically run before Taubin smoothing to avoid
 * artifacts around degenerate small triangles.
 */

#ifndef __vtkvmtkPolyDataKiteRemovalFilter_h
#define __vtkvmtkPolyDataKiteRemovalFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataKiteRemovalFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataKiteRemovalFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataKiteRemovalFilter *New();
  
  ///@{
  /**
   * Set/Get the ratio between a triangle's local neighborhood average area and the surface's global
   * average triangle area, below which the triangle is considered a kite and removed. Default: 0.1.
   */
  vtkGetMacro(SizeFactor,double);
  vtkSetMacro(SizeFactor,double);
  ///@}

  protected:
  vtkvmtkPolyDataKiteRemovalFilter();
  ~vtkvmtkPolyDataKiteRemovalFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double SizeFactor;

  private:
  vtkvmtkPolyDataKiteRemovalFilter(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataKiteRemovalFilter&);  // Not implemented.
};

#endif
