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
 * @class   vtkvmtkPolyDataFlowExtensionsFilter
 * @brief   Extend open boundaries of a surface with straight (or centerline-following) tubes, tapering the cross-section to a target shape.
 * @ingroup ComputationalGeometry
 *
 * For each selected open boundary of the input surface, this filter appends a tubular extension that
 * grows outward from the boundary along a direction either estimated from the local centerline
 * tangent (ExtensionMode = USE_CENTERLINE_DIRECTION) or from the boundary's own normal (ExtensionMode
 * = USE_NORMAL_TO_BOUNDARY). The extension is swept from a target cross-section, which is a circle by
 * default, or the boundary's own outline when PreserveCrossSectionShape is on. The boundary's original
 * (possibly irregular and non-planar) shape is morphed into the target cross-section over the initial
 * TransitionRatio fraction of the extension, either by a thin-plate-spline warp or by fading out a
 * ramp (InterpolationMode). This is the filter behind the vmtkflowextensions pype script, which is typically
 * used to add inlet/outlet flow extensions to a vascular surface before CFD meshing, so that boundary
 * conditions can be applied away from geometrically complex regions and inlet/outlet flow profiles have
 * room to develop/relax.
 *
 * The extension is left open: its last ring of points is the new inlet/outlet boundary, to be closed
 * downstream (e.g. by vtkvmtkSimpleCapPolyData or vtkvmtkCapPolyData) if a closed surface is needed.
 *
 * @sa
 * vtkvmtkPolyDataBoundaryExtractor, vtkvmtkBoundaryReferenceSystems, vtkvmtkPolyBallLine
 */

#ifndef __vtkvmtkPolyDataFlowExtensionsFilter_h
#define __vtkvmtkPolyDataFlowExtensionsFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkPolyDataFlowExtensionsFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataFlowExtensionsFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataFlowExtensionsFilter *New();

  ///@{
  /**
   * Set/Get the centerlines used to estimate the extension direction when ExtensionMode is
   * USE_CENTERLINE_DIRECTION. Required in that mode; ignored when ExtensionMode is
   * USE_NORMAL_TO_BOUNDARY.
   */
  vtkSetObjectMacro(Centerlines,vtkPolyData);
  vtkGetObjectMacro(Centerlines,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the length of the extension, expressed as a multiple of the boundary's mean radius, used
   * when AdaptiveExtensionLength is on. Default: 1.0.
   */
  vtkSetMacro(ExtensionRatio,double);
  vtkGetMacro(ExtensionRatio,double);
  ///@}

  ///@{
  /**
   * Set/Get the absolute length of the extension, used when AdaptiveExtensionLength is off. Default: 0.0.
   */
  vtkSetMacro(ExtensionLength,double);
  vtkGetMacro(ExtensionLength,double);
  ///@}

  ///@{
  /**
   * Set/Get an optional per-boundary scale factor applied to the extension length, whether that
   * length comes from ExtensionRatio times the boundary's mean radius or from ExtensionLength.
   *
   * Entry i multiplies the length of the extension grown from boundary i, where i is the boundary's
   * id in the list of open boundaries extracted from the input (the same ids used in BoundaryIds).
   * A boundary whose id is beyond the end of the array, and every boundary when the array is not
   * set (default, NULL), get a factor of 1.0.
   */
  vtkSetObjectMacro(ExtensionLengthScaleFactors,vtkDoubleArray);
  vtkGetObjectMacro(ExtensionLengthScaleFactors,vtkDoubleArray);
  ///@}

  ///@{
  /**
   * Set/Get the absolute radius of the extension, used when AdaptiveExtensionRadius is off. Default: 1.0.
   * When PreserveCrossSectionShape is on, the boundary outline is uniformly scaled so that its mean
   * radius equals this value, rather than being replaced by a circle of this radius.
   */
  vtkSetMacro(ExtensionRadius,double);
  vtkGetMacro(ExtensionRadius,double);
  ///@}

  ///@{
  /**
   * Set/Get the fraction of the extension length, starting from the original boundary, over which the
   * cross-section is morphed from its original shape into the target cross-section. Beyond this
   * fraction, the extension is a straight, uniform tube. When PreserveCrossSectionShape is on, the
   * target cross-section is the boundary's own outline, so the transition only reconciles the extension
   * with the real, possibly oblique and non-planar rim. Default: 0.5.
   */
  vtkSetMacro(TransitionRatio,double);
  vtkGetMacro(TransitionRatio,double);
  ///@}

  ///@{
  /**
   * Set/Get the stiffness parameter of the thin-plate-spline transform used to morph the boundary's
   * cross-section into the target cross-section when InterpolationMode is
   * USE_THIN_PLATE_SPLINE_INTERPOLATION. Larger values produce a stiffer, less locally-deforming
   * transform. Ignored by the ramp interpolation modes. Default: 1.0.
   */
  vtkSetMacro(Sigma,double);
  vtkGetMacro(Sigma,double);
  ///@}

  ///@{
  /**
   * Set/Get the distance, expressed as a multiple of the boundary's mean radius, walked along the
   * centerline on either side of the boundary's projection when estimating the local centerline
   * tangent used as the extension direction (ExtensionMode = USE_CENTERLINE_DIRECTION). Default: 1.0.
   */
  vtkSetMacro(CenterlineNormalEstimationDistanceRatio,double);
  vtkGetMacro(CenterlineNormalEstimationDistanceRatio,double);
  ///@}

  ///@{
  /**
   * Toggle whether the extension length is computed automatically as ExtensionRatio times the
   * boundary's mean radius (on), or taken directly from ExtensionLength (off). Default: on.
   */
  vtkSetMacro(AdaptiveExtensionLength,int);
  vtkGetMacro(AdaptiveExtensionLength,int);
  vtkBooleanMacro(AdaptiveExtensionLength,int);
  ///@}

  ///@{
  /**
   * Toggle whether the extension radius is computed automatically from the boundary's own geometry
   * (on), or taken directly from ExtensionRadius (off). Default: on.
   */
  vtkSetMacro(AdaptiveExtensionRadius,int);
  vtkGetMacro(AdaptiveExtensionRadius,int);
  vtkBooleanMacro(AdaptiveExtensionRadius,int);
  ///@}

  ///@{
  /**
   * Toggle whether the extension keeps the cross-sectional shape of the boundary it grows from (on),
   * or morphs it into a circle (off, default). When on, the target cross-section is the boundary
   * outline projected onto the plane orthogonal to the extension direction, uniformly resampled by
   * arc length, so that the area distribution of a non-circular inlet/outlet is not altered. Note
   * that a strongly concave (non star-shaped) preserved outline may be difficult to cap downstream
   * with barycenter-fan cappers, just like the original boundary it reproduces. Default: off.
   */
  vtkSetMacro(PreserveCrossSectionShape,int);
  vtkGetMacro(PreserveCrossSectionShape,int);
  vtkBooleanMacro(PreserveCrossSectionShape,int);
  ///@}

  ///@{
  /**
   * Set/Get the number of points used to discretize the target cross-section of the extension,
   * used when AdaptiveNumberOfBoundaryPoints is off. Default: 50.
   *
   * This count also sets how finely the transition is resolved, because the extension is built one
   * layer of points at a time and the layers are spaced by the distance between two neighbouring
   * points of the target cross-section: 2 * sin(pi/NumberOfBoundaryPoints) * radius for a circular
   * target cross-section, perimeter/NumberOfBoundaryPoints for a preserved one. The number of
   * layers the transition is made of therefore grows in proportion to NumberOfBoundaryPoints, and
   * the first layer, which is one layer thickness away from the boundary, gets that much closer to
   * it. Raising this is what makes the junction between the surface and the extension smooth on a
   * strongly non-circular boundary, where the default 50 leaves the transition a handful of coarse
   * layers. The cost is quadratic: NumberOfBoundaryPoints points per layer, over a number of layers
   * proportional to NumberOfBoundaryPoints.
   */
  vtkSetMacro(NumberOfBoundaryPoints,int);
  vtkGetMacro(NumberOfBoundaryPoints,int);
  ///@}

  ///@{
  /**
   * Toggle whether the number of points used to discretize the extension's target cross-section is
   * taken equal to the number of points on the original boundary (on), or from NumberOfBoundaryPoints (off).
   * Turning this on keeps the extension at the mesh density of the surface it grows from, which on
   * a finely meshed surface also gives the transition the thin layers it needs to be smooth,
   * without having to pick a count; see NumberOfBoundaryPoints for why the count matters.
   * Default: off.
   */
  vtkSetMacro(AdaptiveNumberOfBoundaryPoints,int);
  vtkGetMacro(AdaptiveNumberOfBoundaryPoints,int);
  vtkBooleanMacro(AdaptiveNumberOfBoundaryPoints,int);
  ///@}

  ///@{
  /**
   * Set/Get the ids (into the list of open boundaries extracted from the input, in the order returned
   * by vtkvmtkPolyDataBoundaryExtractor) of the boundaries to extend. If not set (default, NULL),
   * every open boundary of the input surface is extended.
   */
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);
  ///@}

  /**
   * Get where each open boundary of the input ended up in the output: entry i is the id of the
   * boundary of the output surface that replaced boundary i of the input, in the list of open
   * boundaries extracted from the output -- so that a downstream filter that indexes boundaries the
   * way this one does (vtkvmtkCapPolyData and the other cappers, through their own BoundaryIds) can
   * still be told which boundary is which.
   *
   * Extending a boundary replaces it: the new open boundary at the tip of the extension is made of
   * new points, several radii from the original, and the boundary extraction order of the output is
   * not the input's. A boundary that was not extended still appears here, mapped to wherever it now
   * falls in that order. Entry i is -1 if boundary i has no counterpart in the output.
   *
   * Indexed consistently with BoundaryIds, like the input boundary ids everywhere else in this
   * filter. Valid only after Update() has been called.
   */
  vtkGetObjectMacro(OutputBoundaryIds,vtkIdList);

  ///@{
  /**
   * Set/Get the method used to compute the direction along which each boundary is extended: either
   * USE_NORMAL_TO_BOUNDARY (the boundary's own outward normal) or USE_CENTERLINE_DIRECTION (the local
   * tangent of Centerlines, default). Use the SetExtensionModeToUseNormalToBoundary() /
   * SetExtensionModeToUseCenterlineDirection() convenience methods instead of setting the integer
   * value directly.
   */
  vtkSetMacro(ExtensionMode,int);
  vtkGetMacro(ExtensionMode,int);
  void SetExtensionModeToUseNormalToBoundary()
  { this->SetExtensionMode(USE_NORMAL_TO_BOUNDARY); }
  void SetExtensionModeToUseCenterlineDirection()
  { this->SetExtensionMode(USE_CENTERLINE_DIRECTION); }
  ///@}

  ///@{
  /**
   * Set/Get the method used to morph the extension's cross-section from the original boundary shape
   * into the target cross-section over the TransitionRatio portion of the extension.
   *
   * USE_THIN_PLATE_SPLINE_INTERPOLATION warps the transition portion of the extension with a
   * thin-plate spline pinned to the original boundary at one end and to the undeformed extension at
   * the other. It is the default, and is kept as such for backward compatibility. A thin-plate
   * spline reproduces the boundary exactly only where it is pinned, and loses the finer features of
   * the cross-section faster than the coarse ones as it moves away, so on a strongly non-circular
   * (say, flat) boundary most of the transition happens within the first layers of the extension
   * and lengthening TransitionRatio does not spread it out.
   *
   * USE_LINEAR_INTERPOLATION and USE_RAMP_INTERPOLATION instead pair each point of the target
   * cross-section with the point of the boundary it grows from, and fade the displacement between
   * the two out over the transition: linearly for the former, and with a smoothstep, flat at both
   * ends, for the latter. Both make the extension start on the real cross-section and reach the
   * target one exactly at the end of the transition, however non-circular the boundary is, so the
   * transition is both smoother and as long as TransitionRatio asks for. USE_RAMP_INTERPOLATION is
   * the one to prefer: being flat at both ends, it leaves no crease where the extension meets the
   * boundary or where it becomes a uniform tube. How closely the extension actually starts on the
   * boundary is still limited by the thickness of its first layer, so a strongly non-circular
   * boundary also wants a NumberOfBoundaryPoints high enough to resolve the transition into thin
   * layers.
   *
   * Use the SetInterpolationModeToLinear() / SetInterpolationModeToThinPlateSpline() /
   * SetInterpolationModeToRamp() convenience methods instead of setting the integer value directly.
   */
  vtkSetMacro(InterpolationMode,int);
  vtkGetMacro(InterpolationMode,int);
  void SetInterpolationModeToLinear()
  { this->SetInterpolationMode(USE_LINEAR_INTERPOLATION); }
  void SetInterpolationModeToThinPlateSpline()
  { this->SetInterpolationMode(USE_THIN_PLATE_SPLINE_INTERPOLATION); }
  void SetInterpolationModeToRamp()
  { this->SetInterpolationMode(USE_RAMP_INTERPOLATION); }
  ///@}

//BTX
  enum {
    USE_NORMAL_TO_BOUNDARY = 0,
    USE_CENTERLINE_DIRECTION
  };

  enum {
    USE_LINEAR_INTERPOLATION = 0,
    USE_THIN_PLATE_SPLINE_INTERPOLATION,
    USE_RAMP_INTERPOLATION
  };
//ETX

  protected:
  vtkvmtkPolyDataFlowExtensionsFilter();
  ~vtkvmtkPolyDataFlowExtensionsFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* Centerlines;

  double ExtensionRatio;
  double ExtensionLength;
  double ExtensionRadius;

  vtkDoubleArray* ExtensionLengthScaleFactors;

  double TransitionRatio;
  double Sigma;

  double CenterlineNormalEstimationDistanceRatio;

  int AdaptiveExtensionLength;
  int AdaptiveExtensionRadius;

  int PreserveCrossSectionShape;

  int NumberOfBoundaryPoints;
  int AdaptiveNumberOfBoundaryPoints;

  int ExtensionMode;
  int InterpolationMode;

  vtkIdList* BoundaryIds;

  vtkIdList* OutputBoundaryIds;

  private:
  vtkvmtkPolyDataFlowExtensionsFilter(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
};

#endif
