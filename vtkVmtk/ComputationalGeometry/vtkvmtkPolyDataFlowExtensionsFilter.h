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
 * @brief   Extend open boundaries of a surface with straight (or centerline-following) tubes, tapering the cross-section to a circle.
 * @ingroup ComputationalGeometry
 *
 * For each selected open boundary of the input surface, this filter appends a tubular extension that
 * grows outward from the boundary along a direction either estimated from the local centerline
 * tangent (ExtensionMode = USE_CENTERLINE_DIRECTION) or from the boundary's own normal (ExtensionMode
 * = USE_NORMAL_TO_BOUNDARY), and morphs the boundary's original (possibly irregular) cross-sectional
 * shape into a circular one over the initial TransitionRatio fraction of the extension, using either
 * linear or thin-plate-spline interpolation (InterpolationMode). This is the filter behind the
 * vmtkflowextensions pype script, which is typically used to add inlet/outlet flow extensions to a
 * vascular surface before CFD meshing, so that boundary conditions can be applied away from
 * geometrically complex regions and inlet/outlet flow profiles have room to develop/relax.
 *
 * @sa
 * vtkvmtkPolyDataBoundaryExtractor, vtkvmtkBoundaryReferenceSystems, vtkvmtkPolyBallLine
 */

#ifndef __vtkvmtkPolyDataFlowExtensionsFilter_h
#define __vtkvmtkPolyDataFlowExtensionsFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
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
   * Set/Get the absolute radius of the extension, used when AdaptiveExtensionRadius is off. Default: 1.0.
   */
  vtkSetMacro(ExtensionRadius,double);
  vtkGetMacro(ExtensionRadius,double);
  ///@}

  ///@{
  /**
   * Set/Get the fraction of the extension length, starting from the original boundary, over which the
   * cross-section is morphed from its original (possibly non-circular) shape into a circle. Beyond this
   * fraction, the extension is a straight, uniform-radius tube. Default: 0.5.
   */
  vtkSetMacro(TransitionRatio,double);
  vtkGetMacro(TransitionRatio,double);
  ///@}

  ///@{
  /**
   * Set/Get the stiffness parameter of the thin-plate-spline transform used to morph the boundary's
   * cross-section into a circle when InterpolationMode is USE_THIN_PLATE_SPLINE_INTERPOLATION. Larger
   * values produce a stiffer, less locally-deforming transform. Default: 1.0.
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
   * Set/Get the number of points used to discretize the (circular) cross-section of the extension,
   * used when AdaptiveNumberOfBoundaryPoints is off. Default: 50.
   */
  vtkSetMacro(NumberOfBoundaryPoints,int);
  vtkGetMacro(NumberOfBoundaryPoints,int);
  ///@}

  ///@{
  /**
   * Toggle whether the number of points used to discretize the extension's cross-section is taken
   * equal to the number of points on the original boundary (on), or from NumberOfBoundaryPoints (off).
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
   * into a circle over the TransitionRatio portion of the extension: USE_LINEAR_INTERPOLATION or
   * USE_THIN_PLATE_SPLINE_INTERPOLATION (default). Use the SetInterpolationModeToLinear() /
   * SetInterpolationModeToThinPlateSpline() convenience methods instead of setting the integer value
   * directly.
   */
  vtkSetMacro(InterpolationMode,int);
  vtkGetMacro(InterpolationMode,int);
  void SetInterpolationModeToLinear()
  { this->SetInterpolationMode(USE_LINEAR_INTERPOLATION); }
  void SetInterpolationModeToThinPlateSpline()
  { this->SetInterpolationMode(USE_THIN_PLATE_SPLINE_INTERPOLATION); }
  ///@}

//BTX
  enum {
    USE_NORMAL_TO_BOUNDARY = 0,
    USE_CENTERLINE_DIRECTION
  };

  enum {
    USE_LINEAR_INTERPOLATION = 0,
    USE_THIN_PLATE_SPLINE_INTERPOLATION
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

  double TransitionRatio;
  double Sigma;

  double CenterlineNormalEstimationDistanceRatio;

  int AdaptiveExtensionLength;
  int AdaptiveExtensionRadius;

  int NumberOfBoundaryPoints;
  int AdaptiveNumberOfBoundaryPoints;

  int ExtensionMode;
  int InterpolationMode;

  vtkIdList* BoundaryIds;

  private:
  vtkvmtkPolyDataFlowExtensionsFilter(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataFlowExtensionsFilter&);  // Not implemented.
};

#endif
