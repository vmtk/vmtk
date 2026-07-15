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
 * @class   vtkvmtkPolyDataSizingFunction
 * @brief   Construct a mesh-size field, sampled on the input surface points, from the local
 * surface triangle size.
 * @ingroup Misc
 *
 * For every point of the input surface, this filter computes the average area of the triangles
 * incident to that point, takes its square root as a local characteristic length, and stores
 * the result -- scaled by ScaleFactor -- as a new point data array named
 * SizingFunctionArrayName on a copy of the input, left unchanged otherwise. The resulting field
 * is a local edge-length target that mimics the input surface's own triangle sizing.
 *
 * This is used by the volume meshing pipeline (e.g. vmtkmeshgenerator) as an element-size field
 * consumed by vtkvmtkTetGenWrapper (via vmtkTetGen's UseSizingFunction option) to grade the
 * volume mesh element size according to the surface discretization, typically after the surface
 * has been remeshed to the desired resolution.
 *
 * @sa
 * vtkvmtkTetGenWrapper
 */

#ifndef __vtkvmtkPolyDataSizingFunction_h
#define __vtkvmtkPolyDataSizingFunction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataSizingFunction : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataSizingFunction,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataSizingFunction *New();
  
  ///@{
  /**
   * Set/Get the name of the point data array where the computed sizing function values are
   * stored on the output. Must be specified before Update() is called.
   * Commonly named "VolumeSizingFunction".
   */
  vtkSetStringMacro(SizingFunctionArrayName);
  vtkGetStringMacro(SizingFunctionArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the factor multiplying the square root of the local average incident-triangle area
   * to produce the sizing function value at each point. Values greater than 1 coarsen the
   * target element size, values less than 1 refine it. Default: 1.0.
   */
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);
  ///@}

  protected:
  vtkvmtkPolyDataSizingFunction();
  ~vtkvmtkPolyDataSizingFunction();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* SizingFunctionArrayName;
  double ScaleFactor;

  private:
  vtkvmtkPolyDataSizingFunction(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataSizingFunction&);  // Not implemented.
};

#endif
