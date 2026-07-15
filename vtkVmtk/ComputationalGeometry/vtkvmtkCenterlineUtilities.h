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
 * @class   vtkvmtkCenterlineUtilities
 * @brief   Provides a collection of functions used when working with split and grouped centerlines.
 * @ingroup ComputationalGeometry
 *
 * Purely a static utility class (never instantiated), providing common query/interpolation helpers
 * on split and grouped centerlines -- looking up group/branch cell ids, testing blanking status,
 * walking adjacent branches at a bifurcation, and interpolating point positions/array values at an
 * arbitrary (cellId, subId, pcoord) location along a centerline. Used internally throughout the
 * "branch extractor" family of filters and by higher-level scripts operating on split centerlines.
 *
 * @sa
 * vtkvmtkCenterlineSplittingAndGroupingFilter
 */

#ifndef __vtkvmtkCenterlineUtilities_h
#define __vtkvmtkCenterlineUtilities_h

#include "vtkObject.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkPoints;
class vtkIdList;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineUtilities : public vtkObject
{
public: 
  vtkTypeMacro(vtkvmtkCenterlineUtilities,vtkObject);
  static vtkvmtkCenterlineUtilities* New(); 

  /**
   * Return the highest group id found in the groupIdsArrayName cell data array of centerlines.
   */
  static vtkIdType GetMaxGroupId(vtkPolyData* centerlines, const char* groupIdsArrayName);

  /**
   * Fill groupIds with the list of all distinct group ids found in the groupIdsArrayName cell data
   * array of centerlines.
   */
  static void GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdList* groupIds);

  /**
   * Fill groupIds with the list of distinct group ids among cells whose blankingArrayName value
   * equals blanked (0 or 1).
   */
  static void GetGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, int blanked, vtkIdList* groupIds);

  /**
   * Fill groupIds with the list of distinct group ids among the non-blanked cells of centerlines.
   */
  static void GetNonBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds);

  /**
   * Fill groupIds with the list of distinct group ids among the blanked cells of centerlines.
   */
  static void GetBlankedGroupsIdList(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdList* groupIds);

  /**
   * Fill groupCellIds with the ids of all cells of centerlines belonging to groupId.
   */
  static void GetGroupCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds);

  /**
   * Like GetGroupCellIds, but additionally de-duplicates cells that represent the same tract
   * repeated across multiple original centerlines (keeping one representative cell per unique
   * tract).
   */
  static void GetGroupUniqueCellIds(vtkPolyData* centerlines, const char* groupIdsArrayName, vtkIdType groupId, vtkIdList* groupCellIds);

  /**
   * Fill centerlineCellIds with the ids of all cells of centerlines belonging to the original,
   * unsplit centerline centerlineId.
   */
  static void GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds);

  /**
   * Like the two-array overload, but also sorts/filters the result using tractIdsArrayName so that
   * cells come back in tract order along the original centerline.
   */
  static void GetCenterlineCellIds(vtkPolyData* centerlines, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType centerlineId, vtkIdList* centerlineCellIds);

  /**
   * Return non-zero if every cell of group groupId is marked blanked in the blankingArrayName cell
   * data array.
   */
  static int IsGroupBlanked(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* blankingArrayName, vtkIdType groupId);

  /**
   * Return the blankingArrayName value (0 or non-zero) of the given cell.
   */
  static int IsCellBlanked(vtkPolyData* centerlines, const char* blankingArrayName, vtkIdType cellId);

  /**
   * Fill upStreamGroupIds and downStreamGroupIds with the group ids immediately upstream and
   * downstream, respectively, of groupId along each original centerline passing through it.
   */
  static void FindAdjacentCenterlineGroupIds(vtkPolyData* centerlines, const char* groupIdsArrayName, const char* centerlineIdsArrayName, const char* tractIdsArrayName, vtkIdType groupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds);

  /**
   * Compute the (x,y,z) position at the given parametric location (cellId, subId, pcoord) along
   * centerlines, linearly interpolated between the segment's two endpoints.
   */
  static void InterpolatePoint(vtkPolyData* centerlines, int cellId, int subId, double pcoord, double interpolatedPoint[3]);

  /**
   * Linearly interpolate the point data array arrayName of centerlines at the given parametric
   * location (cellId, subId, pcoord), writing the result (one value per array component) into
   * interpolatedTuple.
   */
  static void InterpolateTuple(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double* interpolatedTuple);

  /**
   * Convenience overload of InterpolateTuple for single-component (scalar) arrays.
   */
  static void InterpolateTuple1(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double& interpolatedTuple1)
  {
    InterpolateTuple(centerlines,arrayName,cellId,subId,pcoord,&interpolatedTuple1);
  }

  /**
   * Convenience overload of InterpolateTuple for 3-component (vector) arrays.
   */
  static void InterpolateTuple3(vtkPolyData* centerlines, const char* arrayName, int cellId, int subId, double pcoord, double interpolatedTuple3[3])
  {
    InterpolateTuple(centerlines,arrayName,cellId,subId,pcoord,interpolatedTuple3);
  }

  /**
   * Find points shared (within tolerance) by more than one cell of centerlines -- typically
   * bifurcation points where multiple branches meet -- and append them to mergingPoints.
   */
  static void FindMergingPoints(vtkPolyData* centerlines, vtkPoints* mergingPoints, double tolerance);

protected:
  vtkvmtkCenterlineUtilities() {};
  ~vtkvmtkCenterlineUtilities() {};

private:
  vtkvmtkCenterlineUtilities(const vtkvmtkCenterlineUtilities&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineUtilities&);  // Not implemented.
};

#endif
