/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBranchSections.h,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
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
// .NAME vtkvmtkPolyDataBranchSections - Extract a vessel (cross) section n-spheres distance from from the start point of a branch. 
// .SECTION Description
//  The set of vessel sections contain the profile as well as the the following information about the section:
//  - Branch Section Group Ids
//  - Branch Section Bifurcation Group Ids
//  - Branch Section Orientation
//  - Branch Section Distance Spheres
//  - Branch Section Point
//  - Branch Section Normal
//  - Branch Section Area
//  - Branch Section Min Size
//  - Branch Section Max Size
//  - Branch Section Shape
//  - Branch Section Closed

#ifndef __vtkvmtkPolyDataBranchSections_h
#define __vtkvmtkPolyDataBranchSections_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBranchSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBranchSections,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataBranchSections* New();

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);

  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);

  vtkSetStringMacro(CenterlineGroupIdsArrayName);
  vtkGetStringMacro(CenterlineGroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(CenterlineTractIdsArrayName);
  vtkGetStringMacro(CenterlineTractIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetStringMacro(BranchSectionAreaArrayName);
  vtkGetStringMacro(BranchSectionAreaArrayName);

  vtkSetStringMacro(BranchSectionMinSizeArrayName);
  vtkGetStringMacro(BranchSectionMinSizeArrayName);

  vtkSetStringMacro(BranchSectionMaxSizeArrayName);
  vtkGetStringMacro(BranchSectionMaxSizeArrayName);

  vtkSetStringMacro(BranchSectionShapeArrayName);
  vtkGetStringMacro(BranchSectionShapeArrayName);

  vtkSetStringMacro(BranchSectionGroupIdsArrayName);
  vtkGetStringMacro(BranchSectionGroupIdsArrayName);

  vtkSetStringMacro(BranchSectionClosedArrayName);
  vtkGetStringMacro(BranchSectionClosedArrayName);

  vtkSetStringMacro(BranchSectionDistanceSpheresArrayName);
  vtkGetStringMacro(BranchSectionDistanceSpheresArrayName);

  vtkSetMacro(NumberOfDistanceSpheres,int);
  vtkGetMacro(NumberOfDistanceSpheres,int);

  vtkSetMacro(ReverseDirection,int);
  vtkGetMacro(ReverseDirection,int);
  vtkBooleanMacro(ReverseDirection,int);

  static double ComputeBranchSectionArea(vtkPolyData* branchSection);
  static double ComputeBranchSectionShape(vtkPolyData* branchSection, double center[3], double sizeRange[2]);

  static void ExtractCylinderSection(vtkPolyData* cylinder, double origin[3], double normal[3], vtkPolyData* section, bool & closed);

  protected:
  vtkvmtkPolyDataBranchSections();
  ~vtkvmtkPolyDataBranchSections();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBranchSections(vtkPolyData* input, int groupId, vtkPolyData* output);

  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BranchSectionGroupIdsArrayName;
  char* BranchSectionAreaArrayName;
  char* BranchSectionMinSizeArrayName;
  char* BranchSectionMaxSizeArrayName;
  char* BranchSectionShapeArrayName;
  char* BranchSectionClosedArrayName;
  char* BranchSectionDistanceSpheresArrayName;

  int NumberOfDistanceSpheres;
  int ReverseDirection;

  private:
  vtkvmtkPolyDataBranchSections(const vtkvmtkPolyDataBranchSections&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBranchSections&);  // Not implemented.
};

#endif
