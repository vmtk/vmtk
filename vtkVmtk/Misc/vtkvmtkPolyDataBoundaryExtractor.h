/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBoundaryExtractor.h,v $
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
// .NAME vtkvmtkPolyDataBoundaryExtractor - Extract boundary edges as poly lines.
// .SECTION Description
// This class identifies boundary edges and organizes them into poly lines based on connectivity. It also provides the output with a point data vtkIdTypeArray (set as active scalars) in which the ids of boundary points in the input dataset are stored.

#ifndef __vtkvmtkPolyDataBoundaryExtractor_h
#define __vtkvmtkPolyDataBoundaryExtractor_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataBoundaryExtractor : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBoundaryExtractor,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkPolyDataBoundaryExtractor *New();
  
  protected:
  vtkvmtkPolyDataBoundaryExtractor();
  ~vtkvmtkPolyDataBoundaryExtractor() {}

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
 
  private:
  vtkvmtkPolyDataBoundaryExtractor(const vtkvmtkPolyDataBoundaryExtractor&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBoundaryExtractor&);  // Not implemented.
};

#endif
