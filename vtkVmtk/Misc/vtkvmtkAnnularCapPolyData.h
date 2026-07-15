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
 * @class   vtkvmtkAnnularCapPolyData
 * @brief   Add annular caps between the boundaries of a walled surface.
 * @ingroup Misc
 *
 * This class closes the boundaries between the surfaces of a walled surface with caps. The
 * surfaces are required to be dense for the algorithm to produce legal caps.
 *
 * Unlike vtkvmtkCapPolyData (which caps each open boundary independently with a single-vertex
 * triangle fan), this filter is meant for surfaces that have two "parallel" boundaries bounding a
 * wall thickness (e.g. an inner lumen boundary and an outer wall boundary produced by offsetting
 * a vessel surface) -- it pairs up the closest boundaries by barycenter distance and connects them
 * with an annular strip of triangles, without inserting new points. This is one of the capping
 * strategies used by the vmtksurfacecapper pype script (its "annular" Method), producing a closed
 * surface out of a "walled" (double-boundary) input. Boundaries can be restricted with
 * BoundaryIds. If CellEntityIdsArrayName is set, the newly created cap triangles are tagged with
 * per-boundary-pair ids in that cell data array.
 *
 * @sa
 * vtkvmtkCapPolyData, vtkvmtkPolyDataBoundaryExtractor, vtkvmtkBoundaryReferenceSystems
 */

#ifndef __vtkvmtkAnnularCapPolyData_h
#define __vtkvmtkAnnularCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkAnnularCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkAnnularCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkAnnularCapPolyData *New();

  ///@{
  /**
   * Set/Get the ids (into the list of open boundaries extracted from the input, in the order
   * returned by vtkvmtkPolyDataBoundaryExtractor) of the boundaries to pair up and cap. If not
   * set (default, NULL), every open boundary of the input surface is considered, paired with its
   * closest (by barycenter distance) unpaired boundary. The number of boundaries considered must
   * be even.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array used to tag the newly created annular cap triangles
   * with an integer id, one distinct value per capped boundary pair (offset by
   * CellEntityIdOffset, then further offset by boundary index + 1). If the array already exists
   * on the input, existing cell values are preserved and only the new cap cells are appended with
   * the new tag. If left NULL (default), no cell entity id array is created.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the base offset added to the ids written into CellEntityIdsArrayName. The id assigned
   * to the cap connecting the i-th processed boundary pair is (i + 1 + CellEntityIdOffset).
   * Default: 1.
   */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  protected:
  vtkvmtkAnnularCapPolyData();
  ~vtkvmtkAnnularCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  private:
  vtkvmtkAnnularCapPolyData(const vtkvmtkAnnularCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkAnnularCapPolyData&);  // Not implemented.
};

#endif
