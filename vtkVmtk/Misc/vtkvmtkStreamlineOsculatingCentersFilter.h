/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineOsculatingCentersFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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
  vtkTypeRevisionMacro(vtkvmtkStreamlineOsculatingCentersFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkStreamlineOsculatingCentersFilter *New();
  
  protected:
  vtkvmtkStreamlineOsculatingCentersFilter();
  ~vtkvmtkStreamlineOsculatingCentersFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  private:
  vtkvmtkStreamlineOsculatingCentersFilter(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineOsculatingCentersFilter&);  // Not implemented.
};

#endif
