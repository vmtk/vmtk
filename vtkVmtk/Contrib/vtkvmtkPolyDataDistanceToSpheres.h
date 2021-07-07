/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataDistanceToSpheres.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
// .NAME vtkvmtkPolyDataDistanceToSpheres - ..
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataDistanceToSpheres_h
#define __vtkvmtkPolyDataDistanceToSpheres_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataDistanceToSpheres : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataDistanceToSpheres* New();
  vtkTypeMacro(vtkvmtkPolyDataDistanceToSpheres,vtkPolyDataAlgorithm);

  vtkSetObjectMacro(Spheres,vtkPolyData);
  vtkGetObjectMacro(Spheres,vtkPolyData);

  vtkSetMacro(DistanceOffset,double);
  vtkGetMacro(DistanceOffset,double);
  
  vtkSetMacro(DistanceScale,double);
  vtkGetMacro(DistanceScale,double);
  
  vtkSetMacro(MinDistance,double);
  vtkGetMacro(MinDistance,double);
  
  vtkSetMacro(MaxDistance,double);
  vtkGetMacro(MaxDistance,double);

  vtkSetStringMacro(DistanceToSpheresArrayName);
  vtkGetStringMacro(DistanceToSpheresArrayName);

protected:
  vtkvmtkPolyDataDistanceToSpheres();
  ~vtkvmtkPolyDataDistanceToSpheres();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* DistanceToSpheresArrayName;

  vtkPolyData* Spheres;

  double DistanceOffset;
  double DistanceScale;
  double MinDistance;
  double MaxDistance;


private:
  vtkvmtkPolyDataDistanceToSpheres(const vtkvmtkPolyDataDistanceToSpheres&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDistanceToSpheres&);  // Not implemented.
};

#endif

