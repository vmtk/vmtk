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
/*! \class vtkvmtkCenterlineGeometry
    \brief Compute length, curvature, torsion, tortuosity, and frenet frames for individual centerlines.
    \ingroup ComputationalGeometry

    vtkvmtkCenterlineGeometry takes a vtkPolyData composed of polyline (or line) cells representing
    centerlines -- typically the output of vtkvmtkCenterlineBranchExtractor / vtkvmtkCenterlineSplittingAndGroupingFilter
    after splitting and grouping, so that each cell is a single, non-branching tract -- and computes,
    for every cell, its arc length and Frenet-frame differential geometry (curvature, torsion,
    tortuosity, tangent/normal/binormal vectors). This is the filter behind the vmtkcenterlinegeometry
    pype script, and its output is commonly used to characterize vessel curvature/torsion (e.g. for
    tortuosity indices) or to build a local reference frame for stent/graft deployment analysis.

    The length, curvature, torsion, and tortuosity metrics are scalar quantities which are identical
    for each point / tract that makes up a cell. They are cell data which are attached to the
    centerline (which should only consist of one cell, as it has not been split or grouped).

    The frenet reference frame is composed of three orthogonal vectors (tangent, normal, and binormal)
    unique to their parameterization along the centerline. Three vectors are stored as point data for
    each point making up the centerline.

    A laplacian smoothing filter can be applied to the line if the computation appears to be unstable
    (as we are using second derivatives and such here).

    \sa vtkvmtkCenterlineBranchExtractor, vtkvmtkCenterlineSplittingAndGroupingFilter,
        vtkvmtkCenterlineSmoothing, vtkvmtkCenterlineAttributesFilter
*/

#ifndef __vtkvmtkCenterlineGeometry_h
#define __vtkvmtkCenterlineGeometry_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineGeometry : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkCenterlineGeometry,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkCenterlineGeometry* New();

  ///@{
  /*! Set/get the name of the cell data array (one value per centerline cell) where the arc length
      of each line is stored. Must be specified before Update() is called.
      * Commonly named "Length".
      */
  vtkSetStringMacro(LengthArrayName);
  vtkGetStringMacro(LengthArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the point data array (one value per centerline point) where the local
      curvature, estimated from finite differences of the discrete Frenet frame, is stored. Must be
      specified before Update() is called.
      * Commonly named "Curvature".
      */
  vtkSetStringMacro(CurvatureArrayName);
  vtkGetStringMacro(CurvatureArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the point data array (one value per centerline point) where the local
      torsion, estimated from finite differences of the discrete Frenet frame, is stored. Must be
      specified before Update() is called.
      * Commonly named "Torsion".
      */
  vtkSetStringMacro(TorsionArrayName);
  vtkGetStringMacro(TorsionArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the cell data array (one value per centerline cell) where the tortuosity
      of each line is stored, computed as (arc length / chord length) - 1, where the chord is the
      straight-line distance between the first and last point of the cell. Must be specified before
      Update() is called.
      * Commonly named "Tortuosity".
      */
  vtkSetStringMacro(TortuosityArrayName);
  vtkGetStringMacro(TortuosityArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the 3-component point data array where the Frenet tangent unit vector
      (direction of travel along the centerline) is stored for each point. Must be specified before
      Update() is called.
      * Commonly named "FrenetTangent".
      */
  vtkSetStringMacro(FrenetTangentArrayName);
  vtkGetStringMacro(FrenetTangentArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the 3-component point data array where the Frenet normal unit vector
      (points towards the local center of curvature) is stored for each point. Must be specified
      before Update() is called.
      * Commonly named "FrenetNormal".
      */
  vtkSetStringMacro(FrenetNormalArrayName);
  vtkGetStringMacro(FrenetNormalArrayName);
  ///@}

  ///@{
  /*! Set/get the name of the 3-component point data array where the Frenet binormal unit vector
      (tangent cross normal, completing the right-handed frame) is stored for each point. Must be
      specified before Update() is called.
      * Commonly named "FrenetBinormal".
      */
  vtkSetStringMacro(FrenetBinormalArrayName);
  vtkGetStringMacro(FrenetBinormalArrayName);
  ///@}

  ///@{
  /*! Set/get the relaxation factor used by the optional Laplacian smoothing pass applied to each
      line's points before curvature/torsion are computed (see LineSmoothing). Higher values smooth
      more aggressively per iteration. Default: 0.01. */
  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);
  ///@}

  ///@{
  /*! Set/get the number of Laplacian smoothing iterations applied to each line's points before
      curvature/torsion are computed (see LineSmoothing). Default: 100. */
  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);
  ///@}

  ///@{
  /*! Toggle Laplacian smoothing of each line's points prior to computing curvature, torsion, and
      the Frenet frame. Since these quantities involve second derivatives of the line geometry,
      smoothing can stabilize the computation on noisy or piecewise-linear centerlines. The output
      geometry arrays (points) are only replaced with the smoothed coordinates if OutputSmoothedLines
      is also on. Default: off. */
  vtkSetMacro(LineSmoothing,int);
  vtkGetMacro(LineSmoothing,int);
  vtkBooleanMacro(LineSmoothing,int);
  ///@}

  ///@{
  /*! Toggle whether the output centerline points are replaced by the smoothed points used internally
      for the differential geometry computation (only relevant when LineSmoothing is on). When off,
      the output retains the original input point coordinates and only the computed arrays change.
      Default: off. */
  vtkSetMacro(OutputSmoothedLines,int);
  vtkGetMacro(OutputSmoothedLines,int);
  vtkBooleanMacro(OutputSmoothedLines,int);
  ///@}

  /*! Compute the discrete curvature at every point of a single polyline, given as an ordered set of
      points, and store the per-point values in curvatureArray (resized/filled by this call). Returns
      the length-weighted average curvature over the line. Used internally by RequestData, but also
      usable standalone on an arbitrary vtkPoints polyline. */
  static double ComputeLineCurvature(vtkPoints* linePoints, vtkDoubleArray* curvatureArray);

  /*! Compute the discrete torsion at every point of a single polyline, given as an ordered set of
      points, and store the per-point values in torsionArray (resized/filled by this call). Returns
      the length-weighted average torsion over the line. Used internally by RequestData, but also
      usable standalone on an arbitrary vtkPoints polyline. */
  static double ComputeLineTorsion(vtkPoints* linePoints, vtkDoubleArray* torsionArray);

  /*! Compute the discrete Frenet reference frame (tangent, normal, binormal unit vectors) at every
      point of a single polyline, given as an ordered set of points, and store the per-point vectors
      in the three supplied 3-component arrays (resized/filled by this call). Used internally by
      RequestData, but also usable standalone on an arbitrary vtkPoints polyline. */
  static void ComputeLineFrenetReferenceSystem(vtkPoints* linePoints, vtkDoubleArray* lineTangentArray, vtkDoubleArray* lineNormalArray, vtkDoubleArray* lineBinormalArray);

  protected:
  vtkvmtkCenterlineGeometry();
  ~vtkvmtkCenterlineGeometry();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* LengthArrayName;
  char* CurvatureArrayName;
  char* TorsionArrayName;
  char* TortuosityArrayName;
  char* FrenetTangentArrayName;
  char* FrenetNormalArrayName;
  char* FrenetBinormalArrayName;

  int LineSmoothing;
  int OutputSmoothedLines;
  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineGeometry(const vtkvmtkCenterlineGeometry&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineGeometry&);  // Not implemented.
};

#endif
