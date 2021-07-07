/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineBranchExtractor.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineBranchExtractor - Split and group centerlines.
// .SECTION Description
// A centerline is split into individual tracts and the tracts are merged together into branches. 
//
// Tracts are formed (in the example of a bifurcation, though this is generalized to a n-furcation), by identifying two points on each centerline (which we termed the reference points). The first is located where the centerline intersects another centerline’s tube (in the vascular tree assumption, each centerline will intersect all the other centerlines’ tube once). The second is located one maximum inscribed sphere upstream. The second reference point may be thought of as the start of the bifurcation region. We can then identify three tracts along each centerline, the first preceding the bifurcation, the second lying in-between the two points and the last following the bifurcation
//
// Note that in the first tract the two centerlines are one in each other’s tube. The same thing holds true for the second tract, since by definition the second tract ends where the centerlines exit the other centerline’s tube. The last tracts are instead outside each other’s tube.
//
// We can therefore group the tracts based on these tube containment relationships: the first group will be composed of the first tracts of the two centerlines, the second of the second tracts of the two centerlines, the ones located at the bifurcation region in-between the reference points, the third of the second tract of the first centerline and the fourth of the second tract of the second centerline. Each group will basically represent a branch, except for the second group, which will be relative to the bifurcation.
//
// This process gives us the "Blanking" label for tracts free of charge. Tracts which lie between the two reference points (within the bifurcation region), are said to be Blanked. This is label will become important during processing steps downstream in the pipeline.

#ifndef __vtkvmtkCenterlineBranchExtractor_h
#define __vtkvmtkCenterlineBranchExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineBranchExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineBranchExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineBranchExtractor *New();
  
  protected:
  vtkvmtkCenterlineBranchExtractor();
  ~vtkvmtkCenterlineBranchExtractor();  

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) override;
  virtual void GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts) override;

  private:
  vtkvmtkCenterlineBranchExtractor(const vtkvmtkCenterlineBranchExtractor&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineBranchExtractor&);  // Not implemented.
};

#endif
