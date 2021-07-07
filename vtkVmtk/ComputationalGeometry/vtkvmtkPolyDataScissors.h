/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataScissors.h,v $
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
// .NAME vtkvmtkPolyDataScissors - not implemented, try to cut a surface along a line. 
// .SECTION Description
// ...

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 
  
  static vtkvmtkPolyDataScissors *New();

  vtkSetObjectMacro(CutLine,vtkPolyData);
  vtkGetObjectMacro(CutLine,vtkPolyData);

  vtkSetStringMacro(CutLinePointIdsArrayName);
  vtkGetStringMacro(CutLinePointIdsArrayName);
  
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
