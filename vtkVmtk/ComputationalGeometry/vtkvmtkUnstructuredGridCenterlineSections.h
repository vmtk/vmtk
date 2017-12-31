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
class vtkTransform;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridCenterlineSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkUnstructuredGridCenterlineSections,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkUnstructuredGridCenterlineSections* New();

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetObjectMacro(SectionSource,vtkPolyData);
  vtkGetObjectMacro(SectionSource,vtkPolyData);

  vtkSetStringMacro(SectionUpNormalsArrayName);
  vtkGetStringMacro(SectionUpNormalsArrayName);

  vtkSetStringMacro(SectionNormalsArrayName);
  vtkGetStringMacro(SectionNormalsArrayName);

  vtkSetStringMacro(AdditionalNormalsArrayName);
  vtkGetStringMacro(AdditionalNormalsArrayName);

  vtkSetStringMacro(AdditionalScalarsArrayName);
  vtkGetStringMacro(AdditionalScalarsArrayName);

  vtkSetMacro(TransformSections,int);
  vtkGetMacro(TransformSections,int);
  vtkBooleanMacro(TransformSections,int);

  vtkSetMacro(UseSectionSource,int);
  vtkGetMacro(UseSectionSource,int);
  vtkBooleanMacro(UseSectionSource,int);

  vtkSetMacro(SourceScaling,int);
  vtkGetMacro(SourceScaling,int);
  vtkBooleanMacro(SourceScaling,int);

  vtkSetVectorMacro(OriginOffset,double,3);
  vtkGetVectorMacro(OriginOffset,double,3);
 
  vtkSetStringMacro(VectorsArrayName);
  vtkGetStringMacro(VectorsArrayName);
 
  vtkSetStringMacro(SectionIdsArrayName);
  vtkGetStringMacro(SectionIdsArrayName);
  
  vtkGetObjectMacro(SectionPointsPolyData,vtkPolyData);

  protected:
  vtkvmtkUnstructuredGridCenterlineSections();
  ~vtkvmtkUnstructuredGridCenterlineSections();  

  int FillInputPortInformation(int, vtkInformation *info) VTK_OVERRIDE;

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  double ComputeAngle(double vector0[3], double vector1[3]);
  void CreateTransform(vtkTransform* transform, double currentOrigin[3], double currentNormal[3], double currentUpNormal[3], double targetOrigin[3], double targetNormal[3], double targetUpNormal[3]);

  vtkPolyData* Centerlines;
  vtkPolyData* SectionSource;
  vtkPolyData* SectionPointsPolyData;

  char* SectionUpNormalsArrayName;
  char* SectionNormalsArrayName;
  char* AdditionalNormalsArrayName;
  char* AdditionalScalarsArrayName;
  char* SectionIdsArrayName;

  char* VectorsArrayName;

  int TransformSections;

  int UseSectionSource;
  int SourceScaling;

  double OriginOffset[3];

  private:
  vtkvmtkUnstructuredGridCenterlineSections(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridCenterlineSections&);  // Not implemented.
};

#endif
