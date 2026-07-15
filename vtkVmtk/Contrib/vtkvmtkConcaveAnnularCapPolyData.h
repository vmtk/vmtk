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

  Note: this class was contributed by
    Martin Sandve Alnaes
	Simula Research Laboratory
  Based on vtkvmtkAnnularCapPolyData by Tangui Morvan.

=========================================================================*/
/**
 * @class   vtkvmtkConcaveAnnularCapPolyData
 * @brief   Add annular caps between the boundaries of a walled surface.
 * @ingroup Contrib
 *
 * This class closes the boundaries between the surfaces of a walled surface with caps. The surfaces are required to be dense for the algorithm to produce legal caps.
 *
 * Unlike vtkvmtkAnnularCapPolyData, boundaries are not required to be
 * paired in input order: each open boundary is automatically paired with
 * its geometrically closest remaining boundary (by barycenter distance),
 * and the (possibly concave/non-convex) annular region between each pair
 * is triangulated directly, without an intermediate flattening step.
 *
 * @sa vtkvmtkAnnularCapPolyData
 */

#ifndef __vtkvmtkConcaveAnnularCapPolyData_h
#define __vtkvmtkConcaveAnnularCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkConcaveAnnularCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkConcaveAnnularCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkConcaveAnnularCapPolyData *New();

  ///@{
  /**
   * Set/get the (even-sized) subset of boundary indices to pair up and
   * cap, restricting which boundaries are considered by the closest-
   * barycenter pairing. If NULL (default), all boundaries of the input
   * surface are considered and must number an even count.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the name of the cell data array used to tag the newly created
   * cap triangles with an integer id, one distinct value per boundary
   * pairing (the pairing index, starting from 1, plus CellEntityIdOffset).
   * If the array already exists on the input, existing cell values are
   * preserved and only the new cap cells are appended with the new tag.
   * If left NULL (default), no cell entity id array is created.
   * Commonly named "CellEntityIds".
   */
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);
  ///@}

  ///@{
  /**
   * Set/get the base offset added to the ids written into
   * CellEntityIdsArrayName. The id assigned to the cap of the i-th
   * boundary pairing is (i + 1 + CellEntityIdOffset). Default: 1.
   */
  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);
  ///@}

  protected:
  vtkvmtkConcaveAnnularCapPolyData();
  ~vtkvmtkConcaveAnnularCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  private:
  vtkvmtkConcaveAnnularCapPolyData(const vtkvmtkConcaveAnnularCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkConcaveAnnularCapPolyData&);  // Not implemented.
};

#endif
