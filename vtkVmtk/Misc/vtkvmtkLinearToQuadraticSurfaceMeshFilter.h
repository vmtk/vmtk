/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearToQuadraticSurfaceMeshFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
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
// .NAME vtkvmtkLinearToQuadraticSurfaceMeshFilter - Converts linear surface elements to quadratic surface elements via either linear subdivision of laplacian (butterfly) subdivision.
// .SECTION Description
// ...

#ifndef __vtkvmtkLinearToQuadraticSurfaceMeshFilter_h
#define __vtkvmtkLinearToQuadraticSurfaceMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearToQuadraticSurfaceMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkLinearToQuadraticSurfaceMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkLinearToQuadraticSurfaceMeshFilter *New();
  
  vtkSetMacro(SubdivisionMethod,int);
  vtkGetMacro(SubdivisionMethod,int);
  void SetSubdivisionMethodToLinear()
  { this->SubdivisionMethod = LINEAR_SUBDIVISION; }
  void SetSubdivisionMethodToButterfly()
  { this->SubdivisionMethod = BUTTERFLY_SUBDIVISION; }

  protected:
  vtkvmtkLinearToQuadraticSurfaceMeshFilter();
  ~vtkvmtkLinearToQuadraticSurfaceMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int SubdivisionMethod;

  //BTX
  enum
  {
    LINEAR_SUBDIVISION,
    BUTTERFLY_SUBDIVISION
  };
  //ETX

  private:
  vtkvmtkLinearToQuadraticSurfaceMeshFilter(const vtkvmtkLinearToQuadraticSurfaceMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearToQuadraticSurfaceMeshFilter&);  // Not implemented.
};

#endif
