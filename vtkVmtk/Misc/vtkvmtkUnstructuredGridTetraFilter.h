/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridTetraFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkUnstructuredGridTetraFilter - Convert the elements of a mesh to linear tetrahedra.
  // .SECTION Description
  // .

#ifndef __vtkvmtkUnstructuredGridTetraFilter_h
#define __vtkvmtkUnstructuredGridTetraFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkOrderedTriangulator;
class vtkIdList;

class VTK_VMTK_MISC_EXPORT vtkvmtkUnstructuredGridTetraFilter : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkvmtkUnstructuredGridTetraFilter *New();
  vtkTypeMacro(vtkvmtkUnstructuredGridTetraFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(TetrahedraOnly, int);
  vtkGetMacro(TetrahedraOnly, int);
  vtkBooleanMacro(TetrahedraOnly, int);

protected:
  vtkvmtkUnstructuredGridTetraFilter();
  ~vtkvmtkUnstructuredGridTetraFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int TriangulateQuad(vtkUnstructuredGrid* output, vtkIdList *quadPtIds, vtkIdList *ptIds);

  vtkOrderedTriangulator *Triangulator;

  void Execute(vtkUnstructuredGrid *, vtkUnstructuredGrid *);

  int TetrahedraOnly;

private:
  vtkvmtkUnstructuredGridTetraFilter(const vtkvmtkUnstructuredGridTetraFilter&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridTetraFilter&);  // Not implemented.
};

#endif

