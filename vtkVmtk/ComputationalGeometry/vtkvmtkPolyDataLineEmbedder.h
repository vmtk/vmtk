/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataLineEmbedder.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataLineEmbedder - not implemented. Takes a number of lines and embed them in a mesh.
// .SECTION Description
// The idea was to use this to try to partition triangles, but it became to messy (luca reports).

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataLineEmbedder *New();
  
  vtkSetObjectMacro(Lines,vtkPolyData);
  vtkGetObjectMacro(Lines,vtkPolyData);

  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);

  vtkSetStringMacro(EdgePCoordArrayName);
  vtkGetStringMacro(EdgePCoordArrayName);

  vtkSetMacro(SnapToMeshTolerance,double);
  vtkGetMacro(SnapToMeshTolerance,double);

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
