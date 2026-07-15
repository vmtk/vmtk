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
 * @class   vtkvmtkPolyDataGeodesicRBFInterpolation
 * @brief   Interpolate a point-based array over a surface using a radial
 *          basis function of the mesh geodesic distance.
 * @ingroup Contrib
 *
 * vtkvmtkPolyDataGeodesicRBFInterpolation solves for a set of per-seed
 * coefficients such that a sum of radial basis functions, evaluated on the
 * Dijkstra graph-geodesic distance (rather than Euclidean distance) between
 * mesh points, exactly reproduces the seed values (SeedValues) at the seed
 * points (SeedIds). It then evaluates that sum at every point of the input
 * surface and writes the result to a point data array named
 * InterpolatedArrayName. At least two seed points are required.
 *
 * @sa vtkvmtkRBFInterpolation2
 */

#ifndef __vtkvmtkPolyDataGeodesicRBFInterpolation_h
#define __vtkvmtkPolyDataGeodesicRBFInterpolation_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"

class VTK_VMTK_CONTRIB_EXPORT vtkvmtkPolyDataGeodesicRBFInterpolation : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataGeodesicRBFInterpolation* New();
  vtkTypeMacro(vtkvmtkPolyDataGeodesicRBFInterpolation,vtkPolyDataAlgorithm);

  ///@{
  /**
   * Set/get the ids of the input surface points to use as interpolation
   * seeds; must have the same number of entries as SeedValues, at least 2.
   */
  vtkSetObjectMacro(SeedIds,vtkIdList);
  vtkGetObjectMacro(SeedIds,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/get the known values at the seed points (one value per entry of
   * SeedIds, in the same order) that the interpolant must reproduce.
   */
  vtkSetObjectMacro(SeedValues,vtkDataArray);
  vtkGetObjectMacro(SeedValues,vtkDataArray);
  ///@}

  ///@{
  /**
   * Set/get the name of the output point data array holding the
   * interpolated values.
   * Commonly named "InterpolatedRadius" or "ResolutionArray", depending on context.
   */
  vtkSetStringMacro(InterpolatedArrayName);
  vtkGetStringMacro(InterpolatedArrayName);
  ///@}

  ///@{
  /**
   * Set/get the type of radial basis function used, one of
   * THIN_PLATE_SPLINE (r^2 log r, the default), BIHARMONIC (r), or
   * TRIHARMONIC (r^3), where r is the mesh geodesic distance to a seed.
   */
  vtkSetMacro(RBFType,int);
  vtkGetMacro(RBFType,int);
  void SetRBFTypeToThinPlateSpline()
  { this->SetRBFType(THIN_PLATE_SPLINE); }
  void SetRBFTypeToBiharmonic()
  { this->SetRBFType(BIHARMONIC); }
  void SetRBFTypeToTriharmonic()
  { this->SetRBFType(TRIHARMONIC); }
  ///@}

//BTX
  enum 
  {
    THIN_PLATE_SPLINE,
    BIHARMONIC,
    TRIHARMONIC
  };
//ETX
    
protected:
  vtkvmtkPolyDataGeodesicRBFInterpolation();
  ~vtkvmtkPolyDataGeodesicRBFInterpolation();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double EvaluateRBF(double r);
  
  char* InterpolatedArrayName;

  vtkIdList* SeedIds;
  vtkDataArray* SeedValues;

  int RBFType;
  
private:
  vtkvmtkPolyDataGeodesicRBFInterpolation(const vtkvmtkPolyDataGeodesicRBFInterpolation&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataGeodesicRBFInterpolation&);  // Not implemented.
};

#endif

