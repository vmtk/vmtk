/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataPatchingFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.6 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataPatchingFilter - “cut” a set of contiguous rectangular regions on the mesh that follow iso-contours in the StretchedMapping and AngularMetric arrays over which all the quantities of interest are averaged.
// .SECTION Description
// This script is designed to help map the surface of a population of vessels onto the same parametric space in order to enable statistical analyses of surface-based quantities.
//
// By means of the options PatchSize and CircularPatching, we impose the dimensions of the patches, in terms of “height” (in mm) of the patch along the longitudinal direction and number of angular cut over the interval (-π, +π) respectively; the result of this discretization can be seen visualizing the Slab and Sector arrays created by the script or the mesh new surface discretization.

#ifndef __vtkvmtkPolyDataPatchingFilter_h
#define __vtkvmtkPolyDataPatchingFilter_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
#include "vtkImageData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataPatchingFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataPatchingFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataPatchingFilter,vtkPolyDataAlgorithm);

  vtkSetStringMacro(LongitudinalMappingArrayName);
  vtkGetStringMacro(LongitudinalMappingArrayName);

  vtkSetStringMacro(CircularMappingArrayName);
  vtkGetStringMacro(CircularMappingArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(LongitudinalPatchNumberArrayName);
  vtkGetStringMacro(LongitudinalPatchNumberArrayName);

  vtkSetStringMacro(CircularPatchNumberArrayName);
  vtkGetStringMacro(CircularPatchNumberArrayName);

  vtkSetStringMacro(PatchAreaArrayName);
  vtkGetStringMacro(PatchAreaArrayName);

  vtkSetVector2Macro(PatchSize,double);
  vtkGetVectorMacro(PatchSize,double,2);

  vtkSetVector2Macro(PatchOffsets,double);
  vtkGetVectorMacro(PatchOffsets,double,2);

  vtkSetVector2Macro(LongitudinalPatchBounds,double);
  vtkGetVectorMacro(LongitudinalPatchBounds,double,2);

  vtkSetVector2Macro(CircularPatchBounds,double);
  vtkGetVectorMacro(CircularPatchBounds,double,2);

  vtkSetObjectMacro(PatchedData,vtkImageData);
  vtkGetObjectMacro(PatchedData,vtkImageData);

  vtkSetMacro(CircularPatching,int);
  vtkGetMacro(CircularPatching,int);
  vtkBooleanMacro(CircularPatching,int);

  vtkSetMacro(UseConnectivity,int);
  vtkGetMacro(UseConnectivity,int);
  vtkBooleanMacro(UseConnectivity,int);

protected:
  vtkvmtkPolyDataPatchingFilter();
  ~vtkvmtkPolyDataPatchingFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* LongitudinalMappingArrayName;
  char* CircularMappingArrayName;
  char* GroupIdsArrayName;

  char* LongitudinalPatchNumberArrayName;
  char* CircularPatchNumberArrayName;

  char* PatchAreaArrayName;

  double PatchSize[2];
  double PatchOffsets[2];
  double LongitudinalPatchBounds[2];
  double CircularPatchBounds[2];

  vtkImageData* PatchedData;

  int CircularPatching;
  int UseConnectivity;

private:
  vtkvmtkPolyDataPatchingFilter(const vtkvmtkPolyDataPatchingFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataPatchingFilter&);  // Not implemented.
};

#endif

