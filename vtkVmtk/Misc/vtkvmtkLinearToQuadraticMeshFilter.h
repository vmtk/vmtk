/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkLinearToQuadraticMeshFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkLinearToQuadraticMeshFilter - Converts linear elements to quadratic.
  // .SECTION Description
  // ...

#ifndef __vtkvmtkLinearToQuadraticMeshFilter_h
#define __vtkvmtkLinearToQuadraticMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkLinearToQuadraticMeshFilter : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeRevisionMacro(vtkvmtkLinearToQuadraticMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkLinearToQuadraticMeshFilter *New();
  
  vtkSetMacro(SubdivisionMethod,int);
  vtkGetMacro(SubdivisionMethod,int);
  void SetSubdivisionMethodToLinear()
  { this->SubdivisionMethod = LINEAR_SUBDIVISION; }
  void SetSubdivisionMethodToButterfly()
  { this->SubdivisionMethod = BUTTERFLY_SUBDIVISION; }

  protected:
  vtkvmtkLinearToQuadraticMeshFilter();
  ~vtkvmtkLinearToQuadraticMeshFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int SubdivisionMethod;

  //BTX
  enum
  {
    LINEAR_SUBDIVISION,
    BUTTERFLY_SUBDIVISION
  };
  //ETX

  private:
  vtkvmtkLinearToQuadraticMeshFilter(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
  void operator=(const vtkvmtkLinearToQuadraticMeshFilter&);  // Not implemented.
};

#endif
