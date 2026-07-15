/*=========================================================================

Program:   VTK Blood Vessel Smoothing

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
 * @class   vtkvmtkCenterlineSmoothing
 * @brief   Iteratively smooth a centerline with a Laplacian kernel.
 * @ingroup ComputationalGeometry
 *
 * Applies Laplacian (moving-average) smoothing to the points of every line/polyline cell of the
 * input centerlines, independently for each cell: each point is repeatedly nudged towards the
 * midpoint of its two neighbors by a fraction (SmoothingFactor) of the difference, repeated
 * NumberOfSmoothingIterations times. This is the filter behind the vmtkcenterlinesmoothing pype
 * script; it's commonly used before differential-geometry computations (curvature, torsion) which
 * are sensitive to point-to-point noise.
 *
 * @sa
 * vtkvmtkCenterlineGeometry
 */

#ifndef __vtkvmtkCenterlineSmoothing_h
#define __vtkvmtkCenterlineSmoothing_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalSmoothingWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSmoothing : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSmoothing,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineSmoothing* New();

  ///@{
  /**
   * Set/Get the relaxation factor applied at each smoothing iteration: at each step, every point
   * moves this fraction of the way towards the midpoint of its two neighbors. Default: 0.01.
   */
  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);
  ///@}

  ///@{
  /**
   * Set/Get the number of Laplacian smoothing iterations applied to each line. Default: 100.
   */
  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);
  ///@}

  /**
   * Apply Laplacian smoothing to a single polyline, given as an ordered set of points, storing the
   * result in smoothLinePoints (resized/filled by this call). Static utility usable standalone,
   * also used internally by RequestData.
   */
  static void SmoothLine(vtkPoints* linePoints, vtkPoints* smoothLinePoints, int numberOfIterations = 10, double relaxation = 0.1);

  protected:
  vtkvmtkCenterlineSmoothing();
  ~vtkvmtkCenterlineSmoothing();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineSmoothing(const vtkvmtkCenterlineSmoothing&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSmoothing&);  // Not implemented.
};

#endif
