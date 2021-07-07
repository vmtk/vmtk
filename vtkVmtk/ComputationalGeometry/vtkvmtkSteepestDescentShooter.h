/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSteepestDescentShooter.h,v $
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
// .NAME vtkvmtkSteepestDescentShooter - experimental method attempting movement across voronoi vertices; implement with caution. 
// .SECTION Description
// For each voronoi vertex, create lines that go from that point to the centerline by running an Eikonal equation from every point in the centerline outwards over the spikes of the voronoi diagram. then back-project similar to a steepest decent algorithm from the Voronoi back to the centerline. 


#ifndef __vtkvmtkSteepestDescentShooter_h
#define __vtkvmtkSteepestDescentShooter_h

#include "vtkvmtkNonManifoldSteepestDescent.h"
#include "vtkPolyData.h"
#include "vtkEdgeTable.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkSteepestDescentShooter : public vtkvmtkNonManifoldSteepestDescent
{
  public: 
  vtkTypeMacro(vtkvmtkSteepestDescentShooter,vtkvmtkNonManifoldSteepestDescent);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSteepestDescentShooter *New();

  vtkSetObjectMacro(Target,vtkPolyData);
  vtkGetObjectMacro(Target,vtkPolyData);

  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);

  vtkSetStringMacro(EdgeArrayName);
  vtkGetStringMacro(EdgeArrayName);

  vtkSetStringMacro(TargetVectorsArrayName);
  vtkGetStringMacro(TargetVectorsArrayName);

  vtkSetStringMacro(TargetCellIdsArrayName);
  vtkGetStringMacro(TargetCellIdsArrayName);

  vtkSetStringMacro(TargetPCoordsArrayName);
  vtkGetStringMacro(TargetPCoordsArrayName);
 
  protected:
  vtkvmtkSteepestDescentShooter();
  ~vtkvmtkSteepestDescentShooter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void Backtrace(vtkPolyData* input, vtkIdType seedId);

  void FindNearestPolyLinePoint(double* currentPoint, double* lineClosestPoint, int &lineCellId, int &lineCellSubId, double &linePCoord);

  vtkPolyData* Target;
  vtkIdList* Seeds;

  vtkIntArray* TargetArray;
  vtkDataArray* EdgeArray;

  vtkDoubleArray* TargetVectors;
  vtkIntArray* TargetCellIds;
  vtkDoubleArray* TargetPCoords;

  char* EdgeArrayName;

  char* TargetVectorsArrayName;
  char* TargetCellIdsArrayName;
  char* TargetPCoordsArrayName;

  private:
  vtkvmtkSteepestDescentShooter(const vtkvmtkSteepestDescentShooter&);  // Not implemented.
  void operator=(const vtkvmtkSteepestDescentShooter&);  // Not implemented.
};

#endif
