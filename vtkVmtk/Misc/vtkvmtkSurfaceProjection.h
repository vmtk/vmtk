/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkSurfaceProjection.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:47:48 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkSurfaceProjection - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkSurfaceProjection_h
#define __vtkvmtkSurfaceProjection_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_MISC_EXPORT vtkvmtkSurfaceProjection : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSurfaceProjection,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkSurfaceProjection *New();

  // Description:
  // Set/Get the reference surface to compute distance from.
  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);

  protected:
  vtkvmtkSurfaceProjection();
  ~vtkvmtkSurfaceProjection();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  vtkPolyData *ReferenceSurface;

  private:
  vtkvmtkSurfaceProjection(const vtkvmtkSurfaceProjection&);  // Not implemented.
  void operator=(const vtkvmtkSurfaceProjection&);  // Not implemented.
};

#endif
