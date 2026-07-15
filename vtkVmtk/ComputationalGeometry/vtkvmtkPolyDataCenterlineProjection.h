/*=========================================================================

  Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkPolyDataCenterlineProjection
 * @brief   Project all point data from a centerline onto a surface.
 * @ingroup ComputationalGeometry
 *
 * For each point of the input surface, finds the nearest point on Centerlines and copies every
 * point data array present on the centerlines (e.g. radius, abscissa, Frenet frame) onto the
 * surface at that point. This is the filter behind the vmtksurfacecenterlineprojection pype
 * script; unlike vtkvmtkPolyDataCenterlineMetricFilter (which computes one specific derived
 * metric), this class does a blanket copy of all existing centerline arrays.
 *
 * @sa
 * vtkvmtkPolyDataCenterlineMetricFilter, vtkvmtkPolyDataDistanceToCenterlines
 */

#ifndef __vtkvmtkPolyDataCenterlineProjection_h
#define __vtkvmtkPolyDataCenterlineProjection_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataCenterlineProjection : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataCenterlineProjection* New();
  vtkTypeMacro(vtkvmtkPolyDataCenterlineProjection,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/Get the centerlines whose point data arrays are projected onto the input surface. Required
   * input.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Toggle using the maximum inscribed sphere radius (CenterlineRadiusArrayName) when locating the
   * nearest centerline point to each surface point, so that the search accounts for local vessel
   * size rather than pure Euclidean distance. Default: on.
   */
  vtkSetMacro(UseRadiusInformation,int);
  vtkGetMacro(UseRadiusInformation,int);
  vtkBooleanMacro(UseRadiusInformation,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of Centerlines holding the maximum inscribed sphere
   * radius at each point. Used when UseRadiusInformation is on.
   * Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(CenterlineRadiusArrayName);
  vtkGetStringMacro(CenterlineRadiusArrayName);
  ///@}

protected:
  vtkvmtkPolyDataCenterlineProjection();
  ~vtkvmtkPolyDataCenterlineProjection();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* CenterlineRadiusArrayName;

  vtkPolyData* Centerlines;

  int UseRadiusInformation;

private:
  vtkvmtkPolyDataCenterlineProjection(const vtkvmtkPolyDataCenterlineProjection&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataCenterlineProjection&);  // Not implemented.
};

#endif

