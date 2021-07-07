/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkIterativeClosestPointTransform.h,v $
Language:  C++
Date:      $Date: 2010/05/30 11:29:48 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkIterativeClosestPointTransform - Implementation of the ICP algorithm with FarThreshold variant
// .SECTION Description
// ..

#ifndef __vtkvmtkIterativeClosestPointTransform_h
#define __vtkvmtkIterativeClosestPointTransform_h

#include "vtkIterativeClosestPointTransform.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkIterativeClosestPointTransform : public vtkIterativeClosestPointTransform
{
public:
  static vtkvmtkIterativeClosestPointTransform *New();
  vtkTypeMacro(vtkvmtkIterativeClosestPointTransform,vtkIterativeClosestPointTransform);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description: 
  // Set/Get the threshold to declare a point to not have a corresponding
  // point in the other point set. This value is only used if
  // UseFarThreshold is True (not the default).
  // This is useful to align partially overlapping surfaces.
  // If this value is negative, all points are considered to have a
  // corresponding point in the other point set.
  // The default is 1.0.
  vtkSetMacro(FarThreshold,double);
  vtkGetMacro(FarThreshold,double);

  // Description: 
  // Determine whether or not to use the FarThreshold.
  // The default is 0.
  vtkSetMacro(UseFarThreshold,int);
  vtkGetMacro(UseFarThreshold,int);
  vtkBooleanMacro(UseFarThreshold,int);

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
