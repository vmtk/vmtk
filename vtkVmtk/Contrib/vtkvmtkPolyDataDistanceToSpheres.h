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

  Note: this class was contributed by 
	Tangui Morvan
	Kalkulo AS
	Simula Research Laboratory
=========================================================================*/
/**
 * @class   vtkvmtkPolyDataDistanceToSpheres
 * @brief   Compute, for each point of a surface, the minimum distance to
 *          the surface of a set of spheres.
 * @ingroup Contrib
 *
 * Spheres is a poly data whose points define sphere centers and whose
 * point scalars (if present) define the corresponding sphere radii
 * (radius 0 if no scalars are present, reducing spheres to points). For
 * each input point, vtkvmtkPolyDataDistanceToSpheres computes the distance
 * to the nearest sphere surface (Euclidean distance to the center minus
 * the radius, clamped to be non-negative), transforms it as
 * (DistanceOffset + DistanceScale * rawDistance) clamped between
 * MinDistance and MaxDistance (a non-positive MaxDistance, the default,
 * means unbounded), and writes the minimum such value over all spheres to
 * a point data array named DistanceToSpheresArrayName. If that array
 * already exists on the input, its values are combined with the newly
 * computed ones by taking the minimum, so the filter can be chained to
 * progressively refine a distance field from several sphere sets.
 */

#ifndef __vtkvmtkPolyDataDistanceToSpheres_h
#define __vtkvmtkPolyDataDistanceToSpheres_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataDistanceToSpheres : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataDistanceToSpheres* New();
  vtkTypeMacro(vtkvmtkPolyDataDistanceToSpheres,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the poly data whose points and (optional) point scalars define
   * the sphere centers and radii to compute the distance to.
   */
  vtkSetObjectMacro(Spheres,vtkPolyData);
  vtkGetObjectMacro(Spheres,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/get the constant added to the raw distance-to-sphere-surface value
   * before scaling and clamping. Default: 0.
   */
  vtkSetMacro(DistanceOffset,double);
  vtkGetMacro(DistanceOffset,double);
  ///@}

  ///@{
  /**
   * Set/get the factor the raw distance-to-sphere-surface value is
   * multiplied by (after adding DistanceOffset). Default: 1.
   */
  vtkSetMacro(DistanceScale,double);
  vtkGetMacro(DistanceScale,double);
  ///@}

  ///@{
  /**
   * Set/get the lower bound the (offset and scaled) distance is clamped to.
   * Default: 0.
   */
  vtkSetMacro(MinDistance,double);
  vtkGetMacro(MinDistance,double);
  ///@}

  ///@{
  /**
   * Set/get the upper bound the (offset and scaled) distance is clamped to.
   * A non-positive value (the default, -1) means unbounded.
   */
  vtkSetMacro(MaxDistance,double);
  vtkGetMacro(MaxDistance,double);
  ///@}

  ///@{
  /**
   * Set/get the name of the output point data array holding the computed
   * distance values. If an array by this name already exists on the input,
   * its values are combined with the newly computed ones by taking the
   * minimum.
   * Commonly named "DistanceToSpheres".
   */
  vtkSetStringMacro(DistanceToSpheresArrayName);
  vtkGetStringMacro(DistanceToSpheresArrayName);
  ///@}

protected:
  vtkvmtkPolyDataDistanceToSpheres();
  ~vtkvmtkPolyDataDistanceToSpheres();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* DistanceToSpheresArrayName;

  vtkPolyData* Spheres;

  double DistanceOffset;
  double DistanceScale;
  double MinDistance;
  double MaxDistance;


private:
  vtkvmtkPolyDataDistanceToSpheres(const vtkvmtkPolyDataDistanceToSpheres&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataDistanceToSpheres&);  // Not implemented.
};

#endif

