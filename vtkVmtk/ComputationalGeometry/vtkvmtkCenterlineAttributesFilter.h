/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineAttributesFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCenterlineAttributesFilter - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineAttributesFilter_h
#define __vtkvmtkCenterlineAttributesFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineAttributesFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineAttributesFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkCenterlineAttributesFilter* New();

  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);

  vtkSetStringMacro(ParallelTransportNormalsArrayName);
  vtkGetStringMacro(ParallelTransportNormalsArrayName);

  protected:
  vtkvmtkCenterlineAttributesFilter();
  ~vtkvmtkCenterlineAttributesFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  void ComputeAbscissas(vtkPolyData* input, vtkDoubleArray* abscissasArray);
  void ComputeParallelTransportNormals(vtkPolyData* input, vtkDoubleArray* parallelTransportNormalsArray);

  char* AbscissasArrayName;
  char* ParallelTransportNormalsArrayName;

  private:
  vtkvmtkCenterlineAttributesFilter(const vtkvmtkCenterlineAttributesFilter&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineAttributesFilter&);  // Not implemented.
};

#endif
