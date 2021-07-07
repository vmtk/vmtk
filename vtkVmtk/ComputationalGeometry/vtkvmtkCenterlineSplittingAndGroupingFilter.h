/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineSplittingAndGroupingFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.7 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineSplittingAndGroupingFilter - Multipurpose filter used to bundle and blank split tracts in all "branch extractor" like methods.
// .SECTION Description
// In order to extract branches of a bifurcation, centerlines are split into multiple tracts. This filter is responsible for groups tracts together into logical bundles (aka "branches"), and describing which tract is blanked at an n-furcation and which are not. 

#ifndef __vtkvmtkCenterlineSplittingAndGroupingFilter_h
#define __vtkvmtkCenterlineSplittingAndGroupingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSplittingAndGroupingFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSplittingAndGroupingFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetStringMacro(BlankingArrayName);
  vtkGetStringMacro(BlankingArrayName);

  vtkSetStringMacro(TractIdsArrayName);
  vtkGetStringMacro(TractIdsArrayName);

  vtkSetMacro(GroupingMode,int);
  vtkGetMacro(GroupingMode,int);
  void SetGroupingModeToFirstPoint()
  { this->SetGroupingMode(FIRSTPOINT); }
  void SetGroupingModeToLastPoint()
  { this->SetGroupingMode(LASTPOINT); }
  void SetGroupingModeToPointInTube()
  { this->SetGroupingMode(POINTINTUBE); }

//BTX
  enum {
    FIRSTPOINT,
    LASTPOINT,
    POINTINTUBE
  };
//ETX

  protected:
  vtkvmtkCenterlineSplittingAndGroupingFilter();
  ~vtkvmtkCenterlineSplittingAndGroupingFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) = 0;

  virtual void GroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts);
  
  void CoincidentExtremePointGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts, bool first = true);
  void PointInTubeGroupTracts(vtkPolyData* input, vtkPolyData* centerlineTracts);

  virtual void MergeTracts(vtkPolyData* centerlineTracts);

  void SplitCenterline(vtkPolyData* input, vtkIdType cellId, int numberOfSplittingPoints, const vtkIdType* subIds, const double* pcoords, const int* tractBlanking, vtkPolyData* splitCenterline);

  void MakeGroupIdsAdjacent(vtkPolyData* centerlineTracts);
  void MakeTractIdsAdjacent(vtkPolyData* centerlineTracts);

  char* RadiusArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;
  char* BlankingArrayName;
  char* TractIdsArrayName;

  int NumberOfSplittingPoints;
  vtkIdType* SubIds;
  double* PCoords;
  int* TractBlanking;
  int GroupingMode;

  private:
  vtkvmtkCenterlineSplittingAndGroupingFilter(const vtkvmtkCenterlineSplittingAndGroupingFilter&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSplittingAndGroupingFilter&);  // Not implemented.
};

#endif
