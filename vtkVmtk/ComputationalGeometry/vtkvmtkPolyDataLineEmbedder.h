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
 * @class   vtkvmtkPolyDataLineEmbedder
 * @brief   Not implemented. Takes a number of lines and embeds them in a mesh.
 * @ingroup ComputationalGeometry
 *
 * The idea was to use this to try to partition triangles, but it became to messy (luca reports).
 */

#ifndef __vtkvmtkPolyDataLineEmbedder_h
#define __vtkvmtkPolyDataLineEmbedder_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataLineEmbedder : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataLineEmbedder,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataLineEmbedder *New();

  ///@{
  /**
   * Set/Get the poly data holding the line(s) (VTK_LINE / VTK_POLY_LINE cells) to be embedded into
   * the input surface mesh. Each point of Lines must carry, in EdgeArrayName and EdgePCoordArrayName,
   * the mesh edge (pair of input point ids) it lies on and its parametric position along that edge.
   */
  vtkSetObjectMacro(Lines,vtkPolyData);
  vtkGetObjectMacro(Lines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 2-component point data array on Lines giving, for each line point, the
   * ids of the two input mesh points bounding the edge it lies on (both -1 if the line point is not
   * located on a mesh edge). Required input.
   * Commonly named "EdgeArray".
   */
  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array on Lines giving, for each line point, its parametric
   * coordinate (0 to 1) along the mesh edge identified by EdgeArrayName. Required input.
   * Commonly named "EdgePCoordArray".
   */
  vtkSetStringMacro(EdgePCoordArrayName);
  vtkGetStringMacro(EdgePCoordArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the parametric-coordinate tolerance, along a mesh edge, within which a line point is
   * snapped onto the existing mesh vertex at that end of the edge instead of inserting a new point.
   * A value of 0 (default) disables snapping and always inserts a new point.
   */
  vtkSetMacro(SnapToMeshTolerance,double);
  vtkGetMacro(SnapToMeshTolerance,double);
  ///@}

  /**
   * Get the ids, in the output mesh, of the points corresponding to each point of Lines (either newly
   * inserted points or, where snapping occurred, ids of pre-existing mesh points). Valid only after
   * Update() has been called.
   */
  vtkGetObjectMacro(EmbeddedLinePointIds,vtkIdList);

  protected:
  vtkvmtkPolyDataLineEmbedder();
  ~vtkvmtkPolyDataLineEmbedder();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdType GetCellId(vtkPolyData* input, vtkIdList* pointIds);
  void GetNeighbors(vtkIdType pointId, vtkIdList* neighborPointIds);
  void OrderNeighborhood(vtkIdList* cellPointIds, vtkIdList* neighborIds, vtkIdList* addedPointIds, vtkIdList* snapToMeshIds, vtkDataArray* edgeArray, vtkDataArray* edgePCoordArray, vtkIdList* orderedNeighborIds);
  void Triangulate(vtkIdList* cellPointIds, vtkIdList* orderedNeighborIds, vtkIdList* triangulationIds);

  char* EdgeArrayName;
  char* EdgePCoordArrayName;
  double SnapToMeshTolerance;
  vtkPolyData* Lines;

  vtkIdList* EmbeddedLinePointIds;

  private:
  vtkvmtkPolyDataLineEmbedder(const vtkvmtkPolyDataLineEmbedder&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataLineEmbedder&);  // Not implemented.
};

#endif
