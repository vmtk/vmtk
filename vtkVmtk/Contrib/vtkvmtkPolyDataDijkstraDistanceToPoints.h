/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDijkstraDistanceToPoints.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
// .NAME vtkvmtkPolyDataDijkstraDistanceToPoints - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataDijkstraDistanceToPoints_h
#define __vtkvmtkPolyDataDijkstraDistanceToPoints_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkIdList.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataDijkstraDistanceToPoints : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataDijkstraDistanceToPoints* New();
  vtkTypeMacro(vtkvmtkPolyDataDijkstraDistanceToPoints,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(SeedIds,vtkIdList);
  vtkGetObjectMacro(SeedIds,vtkIdList);

  vtkSetMacro(DistanceOffset,double);
  vtkGetMacro(DistanceOffset,double);
  
  vtkSetMacro(DistanceScale,double);
  vtkGetMacro(DistanceScale,double);
  
  vtkSetMacro(MinDistance,double);
  vtkGetMacro(MinDistance,double);
  
  vtkSetMacro(MaxDistance,double);
  vtkGetMacro(MaxDistance,double);

  vtkSetStringMacro(DijkstraDistanceToPointsArrayName);
  vtkGetStringMacro(DijkstraDistanceToPointsArrayName);

protected:
  vtkvmtkPolyDataDijkstraDistanceToPoints();
  ~vtkvmtkPolyDataDijkstraDistanceToPoints();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* DijkstraDistanceToPointsArrayName;

  vtkIdList* SeedIds;

  double DistanceOffset;
  double DistanceScale;
  double MinDistance;
  double MaxDistance;

private:
  vtkvmtkPolyDataDijkstraDistanceToPoints(const vtkvmtkPolyDataDijkstraDistanceToPoints&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDijkstraDistanceToPoints&);  // Not implemented.
};

#endif

