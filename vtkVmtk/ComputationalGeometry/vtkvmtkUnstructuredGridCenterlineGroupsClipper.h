/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkUnstructuredGridCenterlineGroupsClipper.h,v $
Language:  C++
Date:      Sun Feb 20 15:29:37 CET 2011
Version:   Revision: 1.0

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkUnstructuredGridCenterlineGroupsClipper - Split an unstructured grid into groups/branches corresponding to their proximity to the polyball line of a branched centerline. 
// .SECTION Description
// ..

#ifndef __vtkvmtkUnstructuredGridCenterlineGroupsClipper_h
#define __vtkvmtkUnstructuredGridCenterlineGroupsClipper_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkUnstructuredGridCenterlineGroupsClipper : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkUnstructuredGridCenterlineGroupsClipper,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkUnstructuredGridCenterlineGroupsClipper *New();
  
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetObjectMacro(CenterlineGroupIds,vtkIdList);
  vtkGetObjectMacro(CenterlineGroupIds,vtkIdList);

  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);

  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetMacro(ClipAllCenterlineGroupIds,int);
  vtkGetMacro(ClipAllCenterlineGroupIds,int);
  vtkBooleanMacro(ClipAllCenterlineGroupIds,int);

  vtkSetMacro(GenerateClippedOutput,int);
  vtkGetMacro(GenerateClippedOutput,int);
  vtkBooleanMacro(GenerateClippedOutput,int);

  vtkUnstructuredGrid* GetClippedOutput();
  
  vtkSetMacro(CutoffRadiusFactor,double);
  vtkGetMacro(CutoffRadiusFactor,double);

  vtkSetMacro(ClipValue,double);
  vtkGetMacro(ClipValue,double);

  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);

  protected:
  vtkvmtkUnstructuredGridCenterlineGroupsClipper();
  ~vtkvmtkUnstructuredGridCenterlineGroupsClipper();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* Centerlines;

  vtkIdList* CenterlineGroupIds;

  char* CenterlineGroupIdsArrayName;
  char* CenterlineRadiusArrayName;

  char* GroupIdsArrayName;
  char* BlankingArrayName;

  int ClipAllCenterlineGroupIds;
  double CutoffRadiusFactor;
  double ClipValue;

  int GenerateClippedOutput;

  int UseRadiusInformation;

  private:
  vtkvmtkUnstructuredGridCenterlineGroupsClipper(const vtkvmtkUnstructuredGridCenterlineGroupsClipper&);  // Not implemented.
  void operator=(const vtkvmtkUnstructuredGridCenterlineGroupsClipper&);  // Not implemented.
};

#endif
