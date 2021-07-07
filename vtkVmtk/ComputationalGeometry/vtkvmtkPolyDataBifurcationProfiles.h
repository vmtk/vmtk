/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataBifurcationProfiles.h,v $
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
// .NAME vtkvmtkPolyDataBifurcationProfiles - compute bifurcation profiles (the bifurcation splitting lines) from an input surface & grouped and split centerlines.
// .SECTION Description
// Used to extract the bifurcation group, the orientation, and label the group as either coming from an upstream source or leading to a downstream centerline branch. 

#ifndef __vtkvmtkPolyDataBifurcationProfiles_h
#define __vtkvmtkPolyDataBifurcationProfiles_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataBifurcationProfiles : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataBifurcationProfiles,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataBifurcationProfiles* New();

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

  vtkSetStringMacro(BifurcationProfileGroupIdsArrayName);
  vtkGetStringMacro(BifurcationProfileGroupIdsArrayName);

  vtkSetStringMacro(BifurcationProfileBifurcationGroupIdsArrayName);
  vtkGetStringMacro(BifurcationProfileBifurcationGroupIdsArrayName);

  vtkSetStringMacro(BifurcationProfileOrientationArrayName);
  vtkGetStringMacro(BifurcationProfileOrientationArrayName);

  protected:
  vtkvmtkPolyDataBifurcationProfiles();
  ~vtkvmtkPolyDataBifurcationProfiles();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ComputeBifurcationProfiles(vtkPolyData* input, int bifurcationGroupId, vtkIdList* upStreamGroupIds, vtkIdList* downStreamGroupIds, vtkPolyData* output);
  
  vtkPolyData* Centerlines;

  char* GroupIdsArrayName;
  char* CenterlineRadiusArrayName;
  char* CenterlineGroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* CenterlineTractIdsArrayName;
  char* BlankingArrayName;

  char* BifurcationProfileGroupIdsArrayName;
  char* BifurcationProfileBifurcationGroupIdsArrayName;
  char* BifurcationProfileOrientationArrayName;

  private:
  vtkvmtkPolyDataBifurcationProfiles(const vtkvmtkPolyDataBifurcationProfiles&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataBifurcationProfiles&);  // Not implemented.
};

#endif
