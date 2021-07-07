/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkAnnularCapPolyData.h,v $
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
// .NAME vtkvmtkAnnularCapPolyData - Add annular caps between the boundaries of a walled surface.
// .SECTION Description
// This class closes the boundaries between the surfaces of a walled surface with caps. The surfaces are required to be dense for the algorithm to produce legal caps.

#ifndef __vtkvmtkAnnularCapPolyData_h
#define __vtkvmtkAnnularCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkAnnularCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkAnnularCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkvmtkAnnularCapPolyData *New();

  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);

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
