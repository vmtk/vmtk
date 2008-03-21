/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridCenterlineSections.h,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkUnstructuredGridCenterlineSections - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkUnstructuredGridCenterlineSections_h
#define __vtkvmtkUnstructuredGridCenterlineSections_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class vtkUnstructuredGrid;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridCenterlineSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeRevisionMacro(vtkvmtkUnstructuredGridCenterlineSections,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkUnstructuredGridCenterlineSections* New();

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetStringMacro(UpNormalsArrayName);
  vtkGetStringMacro(UpNormalsArrayName);

  vtkSetStringMacro(AdditionalNormalsArrayName);
  vtkGetStringMacro(AdditionalNormalsArrayName);

  vtkSetMacro(TransformSections,int);
  vtkGetMacro(TransformSections,int);
  vtkBooleanMacro(TransformSections,int);

  vtkSetMacro(OriginOffset,double);
  vtkGetMacro(OriginOffset,double);
 
  vtkSetStringMacro(VectorsArrayName);
  vtkGetStringMacro(VectorsArrayName);
  
  vtkGetObjectMacro(AdditionalNormalsPolyData,vtkPolyData);

  protected:
  vtkvmtkUnstructuredGridCenterlineSections();
  ~vtkvmtkUnstructuredGridCenterlineSections();  

  int FillInputPortInformation(int, vtkInformation *info);

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  double ComputeAngle(double vector0[3], double vector1[3]);

  vtkPolyData* Centerlines;
  vtkPolyData* AdditionalNormalsPolyData;

  char* UpNormalsArrayName;
  char* AdditionalNormalsArrayName;

  char* VectorsArrayName;

  int TransformSections;

  double OriginOffset;

  private:
  vtkvmtkUnstructuredGridCenterlineSections(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
};

#endif
