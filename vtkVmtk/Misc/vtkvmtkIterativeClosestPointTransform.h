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
 * @class   vtkvmtkIterativeClosestPointTransform
 * @brief   Implements the ICP algorithm, with a FarThreshold variant.
 * @ingroup Misc
 *
 * This class extends VTK's vtkIterativeClosestPointTransform (rigid/similarity registration of a
 * source point set onto a target point set by iteratively matching closest points and solving a
 * landmark transform) with an optional far-point rejection step. When UseFarThreshold is on,
 * source points whose closest target point is farther than FarThreshold are excluded from the
 * landmark correspondences used to update the transform at each iteration; this makes the
 * registration robust to partially overlapping surfaces, where forcing every source point to match
 * some target point would bias the result.
 *
 * This is the transform behind the vmtkicpregistration pype script, used to rigidly align a
 * surface onto a reference surface (e.g. two segmentations of the same vessel from different
 * imaging sessions).
 */

#ifndef __vtkvmtkIterativeClosestPointTransform_h
#define __vtkvmtkIterativeClosestPointTransform_h

#include "vtkIterativeClosestPointTransform.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkIterativeClosestPointTransform : public vtkIterativeClosestPointTransform
{
public:
  static vtkvmtkIterativeClosestPointTransform *New();
  vtkTypeMacro(vtkvmtkIterativeClosestPointTransform,vtkIterativeClosestPointTransform);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the threshold to declare a point to not have a corresponding point in the other point set.
   * This value is only used if UseFarThreshold is True (not the default). This is useful to align
   * partially overlapping surfaces. If this value is negative, all points are considered to have a
   * corresponding point in the other point set. The default is 1.0.
   */
  vtkSetMacro(FarThreshold,double);
  vtkGetMacro(FarThreshold,double);
  ///@}

  ///@{
  /**
   * Determine whether or not to use the FarThreshold. The default is 0.
   */
  vtkSetMacro(UseFarThreshold,int);
  vtkGetMacro(UseFarThreshold,int);
  vtkBooleanMacro(UseFarThreshold,int);
  ///@}

protected:

  vtkvmtkIterativeClosestPointTransform();
  ~vtkvmtkIterativeClosestPointTransform();

  void InternalUpdate() override;

  double FarThreshold;
  int UseFarThreshold;

private:
  vtkvmtkIterativeClosestPointTransform(const vtkvmtkIterativeClosestPointTransform&);  // Not implemented.
  void operator=(const vtkvmtkIterativeClosestPointTransform&);  // Not implemented.
};

#endif
