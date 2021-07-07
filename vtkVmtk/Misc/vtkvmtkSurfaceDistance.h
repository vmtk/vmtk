/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfaceDistance.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkSurfaceDistance - calculate the euclidian distance of one surface from another
// .SECTION Description
// .

#ifndef __vtkvmtkSurfaceDistance_h
#define __vtkvmtkSurfaceDistance_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_MISC_EXPORT vtkvmtkSurfaceDistance : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSurfaceDistance,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSurfaceDistance *New();

  // Description:
  // Set/Get the name of the array where the computed distance has to be stored.
  vtkSetStringMacro(DistanceArrayName);
  vtkGetStringMacro(DistanceArrayName);

  // Description:
  // Set/Get the name of the array where the computed distance has to be stored.
  vtkSetStringMacro(SignedDistanceArrayName);
  vtkGetStringMacro(SignedDistanceArrayName);

  // Description:
  // Set/Get the name of the array where the computed distance vectors have to be stored.
  vtkSetStringMacro(DistanceVectorsArrayName);
  vtkGetStringMacro(DistanceVectorsArrayName);

  // Description:
  // Set/Get the reference surface to compute distance from.
  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);

  protected:
  vtkvmtkSurfaceDistance();
  ~vtkvmtkSurfaceDistance();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char *DistanceArrayName;
  char *DistanceVectorsArrayName;
  char *SignedDistanceArrayName;
  vtkPolyData *ReferenceSurface;

  private:
  vtkvmtkSurfaceDistance(const vtkvmtkSurfaceDistance&);  // Not implemented.
  void operator=(const vtkvmtkSurfaceDistance&);  // Not implemented.
};

#endif
