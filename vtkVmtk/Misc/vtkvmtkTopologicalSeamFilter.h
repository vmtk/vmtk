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
 * @class   vtkvmtkTopologicalSeamFilter
 * @brief   Create a feature edge on a surface from a point and implicit plane function.
 * @ingroup Misc
 *
 * Evaluates SeamFunction (typically a vtkPlane) at every point of the input surface, walking
 * outward from the point closest to ClosestPoint, and marks the points forming the resulting
 * "seam" -- where the surface crosses the implicit function -- in the output point data array
 * named SeamScalarsArrayName. This is used e.g. to cut a topological seam into a tubular surface
 * prior to operations that require a disk-like (simply connected) topology, such as flattening/
 * unrolling for 2D visualization.
 */

#ifndef __vtkvmtkTopologicalSeamFilter_h
#define __vtkvmtkTopologicalSeamFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkImplicitFunction.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkTopologicalSeamFilter : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkTopologicalSeamFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkTopologicalSeamFilter *New();

  ///@{
  /**
   * Set/Get the (x,y,z) location used to find the starting point of the seam (the closest surface
   * point is used). Default: (0,0,0).
   */
  vtkSetVectorMacro(ClosestPoint,double,3);
  vtkGetVectorMacro(ClosestPoint,double,3);
  ///@}

  ///@{
  /**
   * Set/Get the name of the output point data array where the seam marker (side of SeamFunction
   * each point falls on) is stored.
   * Commonly named "SeamScalars".
   */
  vtkSetStringMacro(SeamScalarsArrayName);
  vtkGetStringMacro(SeamScalarsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the implicit function (typically a vtkPlane) that defines the cutting surface used to
   * locate the seam. Required input.
   */
  vtkSetObjectMacro(SeamFunction,vtkImplicitFunction);
  vtkGetObjectMacro(SeamFunction,vtkImplicitFunction);
  ///@}

  protected:
  vtkvmtkTopologicalSeamFilter();
  ~vtkvmtkTopologicalSeamFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double ClosestPoint[3];
  char* SeamScalarsArrayName;
  vtkImplicitFunction* SeamFunction;

  private:
  vtkvmtkTopologicalSeamFilter(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
  void operator=(const vtkvmtkTopologicalSeamFilter&);  // Not implemented.
};

#endif
