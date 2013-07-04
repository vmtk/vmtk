/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkConcaveAnnularCapPolyData.h,v $
Language:  C++
Date:      $Date: 2012/09/19 $
Version:   $Revision: 1.0 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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
  // .NAME vtkvmtkConcaveAnnularCapPolyData - Add annular caps between the boundaries of a walled surface.
  // .SECTION Description
  // This class closes the boundaries between the surfaces of a walled surface with caps. The
  // surfaces are required to be dense for the algorithm to produce legal caps.

#ifndef __vtkvmtkConcaveAnnularCapPolyData_h
#define __vtkvmtkConcaveAnnularCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkConcaveAnnularCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeRevisionMacro(vtkvmtkConcaveAnnularCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkvmtkConcaveAnnularCapPolyData *New();

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);

  protected:
  vtkvmtkConcaveAnnularCapPolyData();
  ~vtkvmtkConcaveAnnularCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char* CellEntityIdsArrayName;

  int CellEntityIdOffset;

  private:
  vtkvmtkConcaveAnnularCapPolyData(const vtkvmtkConcaveAnnularCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkConcaveAnnularCapPolyData&);  // Not implemented.
};

#endif
