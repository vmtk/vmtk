/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBifurcationSections.h,v $
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
// .NAME vtkvmtkPolyDataBifurcationSections - Extract a set of vessel (cross) sections n-spheres away (upstream and downstream) from a bifurctation. 
// .SECTION Description
// The set of vessel sections contain the profile as well as the the following information about the section:
// - Bifurcation Section Group Ids
// - Bifurcation Section Bifurcation Group Ids
// - Bifurcation Section Orientation
// - Bifurcation Section Distance Spheres
// - Bifurcation Section Point
// - Bifurcation Section Normal
// - Bifurcation Section Area
// - Bifurcation Section Min Size
// - Bifurcation Section Max Size
// - Bifurcation Section Shape
// - Bifurcation Section Closed

#ifndef __vtkvmtkPolyDataBifurcationSections_h
#define __vtkvmtkPolyDataBifurcationSections_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBifurcationSections : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBifurcationSections,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataBifurcationSections* New();

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

  vtkSetStringMacro(BifurcationSectionPointArrayName);
  vtkGetStringMacro(BifurcationSectionPointArrayName);

  vtkSetStringMacro(BifurcationSectionNormalArrayName);
  vtkGetStringMacro(BifurcationSectionNormalArrayName);

  vtkSetStringMacro(BifurcationSectionAreaArrayName);
  vtkGetStringMacro(BifurcationSectionAreaArrayName);

  vtkSetStringMacro(BifurcationSectionMinSizeArrayName);
  vtkGetStringMacro(BifurcationSectionMinSizeArrayName);

  vtkSetStringMacro(BifurcationSectionMaxSizeArrayName);
  vtkGetStringMacro(BifurcationSectionMaxSizeArrayName);

  vtkSetStringMacro(BifurcationSectionShapeArrayName);
  vtkGetStringMacro(BifurcationSectionShapeArrayName);

  vtkSetStringMacro(BifurcationSectionGroupIdsArrayName);
  vtkGetStringMacro(BifurcationSectionGroupIdsArrayName);

  vtkSetStringMacro(BifurcationSectionBifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationSectionBifurcationGroupIdsArrayName);

  vtkSetStringMacro(BifurcationSectionOrientationArrayName);
  vtkGetStringMacro(BifurcationSectionOrientationArrayName);

  vtkSetStringMacro(BifurcationSectionDistanceSpheresArrayName);
  vtkGetStringMacro(BifurcationSectionDistanceSpheresArrayName);

  vtkSetStringMacro(BifurcationSectionClosedArrayName);
  vtkGetStringMacro(BifurcationSectionClosedArrayName);

  vtkSetMacro(NumberOfDistanceSpheres,int);
  vtkGetMacro(NumberOfDistanceSpheres,int);

  protected:
  vtkvmtkPolyDataBifurcationSections();
  ~vtkvmtkPolyDataBifurcationSections();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBifurcationSections(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output);
  
  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BifurcationSectionGroupIdsArrayName;
  char* BifurcationSectionBifurcationGroupIdsArrayName;
  char* BifurcationSectionOrientationArrayName;
  char* BifurcationSectionDistanceSpheresArrayName;
  char* BifurcationSectionPointArrayName;
  char* BifurcationSectionNormalArrayName;
  char* BifurcationSectionAreaArrayName;
  char* BifurcationSectionMinSizeArrayName;
  char* BifurcationSectionMaxSizeArrayName;
  char* BifurcationSectionShapeArrayName;
  char* BifurcationSectionClosedArrayName;

  int NumberOfDistanceSpheres;

  private:
  vtkvmtkPolyDataBifurcationSections(const vtkvmtkPolyDataBifurcationSections&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBifurcationSections&);  // Not implemented.
};

#endif
