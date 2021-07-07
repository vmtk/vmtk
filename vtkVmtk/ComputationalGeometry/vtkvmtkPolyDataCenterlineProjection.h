/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataCenterlineProjection.h,v $
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
// .NAME vtkvmtkPolyDataCenterlineProjection - Project all point data from a centerline onto a surface.
// .SECTION Description
// ...

#ifndef __vtkvmtkPolyDataCenterlineProjection_h
#define __vtkvmtkPolyDataCenterlineProjection_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineProjection : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataCenterlineProjection* New();
  vtkTypeMacro(vtkvmtkPolyDataCenterlineProjection,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);

protected:
  vtkvmtkPolyDataCenterlineProjection();
  ~vtkvmtkPolyDataCenterlineProjection();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* CenterlineRadiusArrayName;

  vtkPolyData* Centerlines;

  int UseRadiusInformation;

private:
  vtkvmtkPolyDataCenterlineProjection(const vtkvmtkPolyDataCenterlineProjection&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineProjection&);  // Not implemented.
};

#endif

