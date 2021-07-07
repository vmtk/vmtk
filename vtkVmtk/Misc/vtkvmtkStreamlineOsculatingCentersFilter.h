/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineOsculatingCentersFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkStreamlineOsculatingCentersFilter - Cluster streamlines based on Mahalanobis distance metric and K-Means clustering.
// .SECTION Description
// This class clusters streamlines.

#ifndef __vtkvmtkStreamlineOsculatingCentersFilter_h
#define __vtkvmtkStreamlineOsculatingCentersFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineOsculatingCentersFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineOsculatingCentersFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkStreamlineOsculatingCentersFilter *New();

  vtkSetObjectMacro(VoronoiDiagram,vtkPolyData);
  vtkGetObjectMacro(VoronoiDiagram,vtkPolyData);

  vtkSetStringMacro(VoronoiSheetIdsArrayName);
  vtkGetStringMacro(VoronoiSheetIdsArrayName);

  vtkGetObjectMacro(OsculatingCenters,vtkPolyData);

  protected:
  vtkvmtkStreamlineOsculatingCentersFilter();
  ~vtkvmtkStreamlineOsculatingCentersFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* VoronoiDiagram;
  char* VoronoiSheetIdsArrayName;
  
  vtkPolyData* OsculatingCenters;

  private:
  vtkvmtkStreamlineOsculatingCentersFilter(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
};

#endif
