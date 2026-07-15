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
 * @class   vtkvmtkPolyDataScissors
 * @brief   Not implemented / experimental. Cuts a surface open along a polyline that follows existing mesh edges, duplicating points so the two sides of the cut become topologically disconnected.
 * @ingroup ComputationalGeometry
 *
 * For each internal segment of CutLine (a polyline whose points map to input surface points through
 * CutLinePointIdsArrayName), this class attempts to duplicate the corresponding input point and
 * reassign it, on one side of the cut, to the cells lying on that side, so that the surface can be
 * opened up along the line (as with a pair of scissors). This is not part of any released pype script
 * and should be considered experimental/unfinished.
 */

#ifndef __vtkvmtkPolyDataScissors_h
#define __vtkvmtkPolyDataScissors_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataScissors : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataScissors,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;
  
  static vtkvmtkPolyDataScissors *New();

  ///@{
  /**
   * Set/Get the poly data holding the line (polyline cells) along which the input surface should be
   * cut open. Every point of CutLine must lie on an edge of the input mesh and carry, in
   * CutLinePointIdsArrayName, the id of the corresponding input surface point.
   */
  vtkSetObjectMacro(CutLine,vtkPolyData);
  vtkGetObjectMacro(CutLine,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array on CutLine giving, for each cut line point, the id of
   * the corresponding point on the input surface. Required input.
   */
  vtkSetStringMacro(CutLinePointIdsArrayName);
  vtkGetStringMacro(CutLinePointIdsArrayName);
  ///@}
  
  protected:
  vtkvmtkPolyDataScissors();
  ~vtkvmtkPolyDataScissors();  
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int GetCellsOnSameSide(vtkPolyData* input, vtkIdType targetCellId0, vtkIdType targetCellId1, vtkIdType referenceCellId, vtkIdType linePointId0, vtkIdType linePointId1, vtkIdType linePointId2, vtkIdList *cellsOnSameSide);
  int IsEdgeInCell(vtkPolyData *input, vtkIdType edgePointId0, vtkIdType edgePointId1, vtkIdType cellId);

  char *CutLinePointIdsArrayName;
  vtkPolyData *CutLine;
  
private:
  vtkvmtkPolyDataScissors(const vtkvmtkPolyDataScissors&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataScissors&);  // Not implemented.
};

#endif
