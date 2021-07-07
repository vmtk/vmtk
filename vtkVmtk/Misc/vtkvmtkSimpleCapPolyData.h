/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSimpleCapPolyData.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkSimpleCapPolyData - Add caps to boundaries.
// .SECTION Description
// This class closes the boundaries of a surface with a cap.

#ifndef __vtkvmtkSimpleCapPolyData_h
#define __vtkvmtkSimpleCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkSimpleCapPolyData : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSimpleCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSimpleCapPolyData *New();

  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  
  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);

  protected:
  vtkvmtkSimpleCapPolyData();
  ~vtkvmtkSimpleCapPolyData();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  private:
  vtkvmtkSimpleCapPolyData(const vtkvmtkSimpleCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkSimpleCapPolyData&);  // Not implemented.
};

#endif
