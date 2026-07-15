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
 * @class   vtkvmtkReferenceSystemUtilities
 * @brief   Utility functions for looking up branch reference system points by group id.
 * @ingroup ComputationalGeometry
 *
 * A reference systems poly data (as produced e.g. by vtkvmtkCenterlineBranchExtractor /
 * vtkvmtkCenterlineBifurcationReferenceSystems) stores one point per branch group, tagged with the
 * group's id in a point data array. This class provides a static helper to find the point id
 * corresponding to a given group id, used by other filters (e.g.
 * vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter) that need to relate a branch to its reference
 * system origin.
 *
 * @sa
 * vtkvmtkPolyDataReferenceSystemBoundaryMetricFilter, vtkvmtkCenterlineBifurcationReferenceSystems
 */

#ifndef __vtkvmtkReferenceSystemUtilities_h
#define __vtkvmtkReferenceSystemUtilities_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkReferenceSystemUtilities : public vtkObject
{
public: 
  vtkTypeMacro(vtkvmtkReferenceSystemUtilities,vtkObject);
  static vtkvmtkReferenceSystemUtilities* New();

  /**
   * Return the id of the point of referenceSystems whose groupIdsArrayName point data value equals
   * groupId, or -1 if no such point is found.
   */
  static vtkIdType GetReferenceSystemPointId(vtkPolyData* referenceSystems, const char* groupIdsArrayName, vtkIdType groupId);
  
protected:
  vtkvmtkReferenceSystemUtilities() {};
  ~vtkvmtkReferenceSystemUtilities() {};

private:
  vtkvmtkReferenceSystemUtilities(const vtkvmtkReferenceSystemUtilities&);  // Not implemented.
  void operator=(const vtkvmtkReferenceSystemUtilities&);  // Not implemented.
};

#endif
