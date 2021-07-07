/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineClusteringFilter.h,v $
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
// .NAME vtkvmtkStreamlineClusteringFilter - Cluster streamlines based on Mahalanobis distance metric and K-Means clustering.
// .SECTION Description
// This class clusters streamlines.

#ifndef __vtkvmtkStreamlineClusteringFilter_h
#define __vtkvmtkStreamlineClusteringFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineClusteringFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineClusteringFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkStreamlineClusteringFilter *New();
  
  vtkGetObjectMacro(ClusterCenters,vtkPolyData);

  protected:
  vtkvmtkStreamlineClusteringFilter();
  ~vtkvmtkStreamlineClusteringFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* ClusterCenters;

  private:
  vtkvmtkStreamlineClusteringFilter(const vtkvmtkStreamlineClusteringFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineClusteringFilter&);  // Not implemented.
};

#endif
