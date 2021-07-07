/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkSurfaceProjectCellArray.h,v $
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
// .NAME vtkvmtkSurfaceProjectCellArray - Projects a cell array from a reference surface.
// .SECTION Description
// For each cell, the cell value chosen is the one of the cell on the reference surface which has the smallest minimum distance to the cell vertices. If the distance of any vertices in the cell to the reference surface is higher than DistanceTolerance, the cell array values are set to DefaultValue for each component. 

#ifndef __vtkvmtkSurfaceProjectCellArray_h
#define __vtkvmtkSurfaceProjectCellArray_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkSurfaceProjectCellArray : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkSurfaceProjectCellArray,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkSurfaceProjectCellArray *New();

  // Description:
  // Set/Get the reference surface to compute distance from.
  vtkSetObjectMacro(ReferenceSurface,vtkPolyData);
  vtkGetObjectMacro(ReferenceSurface,vtkPolyData);

  //Set/Get the name of the array to project
  vtkSetStringMacro(ProjectedArrayName);
  vtkGetStringMacro(ProjectedArrayName);

  //Set/Get the distance tolerance 
  vtkSetMacro(DistanceTolerance, double);
  vtkGetMacro(DistanceTolerance, double);

  //Set/Get the default value
  vtkSetMacro(DefaultValue, double);
  vtkGetMacro(DefaultValue, double);

  protected:
  vtkvmtkSurfaceProjectCellArray();
  ~vtkvmtkSurfaceProjectCellArray();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData *ReferenceSurface;
  
  char *ProjectedArrayName;
  
  double DistanceTolerance;
  double DefaultValue;

  private:
  vtkvmtkSurfaceProjectCellArray(const vtkvmtkSurfaceProjectCellArray&);  // Not implemented.
  void operator=(const vtkvmtkSurfaceProjectCellArray&);  // Not implemented.
};

#endif
