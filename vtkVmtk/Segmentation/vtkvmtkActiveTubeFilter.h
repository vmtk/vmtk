/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

  Portions of this code are covered under the ITK copyright.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkActiveTubeFilter
 * @brief   Experimental: generates centerlines from an image.
 * @ingroup Segmentation
 *
 * vtkvmtkActiveTubeFilter takes as input a vtkPolyData composed of polyline cells (a centerline,
 * with a point data array giving the radius at each point) and evolves both the point coordinates
 * and the radii, cell by cell, as an active-contour ("snake") tube: at each iteration a cardinal
 * spline is fit through each cell's points and radii, the tube boundary is sampled at
 * NumberOfAngularEvaluations directions around the circumference, and the gradient of a supplied
 * PotentialImage is probed at each sampled point to compute an image force. The image force is
 * decomposed into an isotropic component (which changes the radius) and an anisotropic component
 * (which displaces the centerline), and combined with an internal stiffness force derived from the
 * spline's second derivatives to regularize the shape. The result is a centerline/radius pair that
 * has been pulled toward edges of the potential image while remaining smooth.
 *
 * This is the filter behind the vmtkactivetubes pype script, used experimentally to refine
 * centerlines (and their radius estimates) directly from an image, as an alternative to purely
 * distance-map-based centerline extraction.
 *
 * Developed with support from the EC FP7/2007-2013: ARCH, Project n. 224390
 */

#ifndef __vtkvmtkActiveTubeFilter_h
#define __vtkvmtkActiveTubeFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;
class vtkDoubleArray;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkActiveTubeFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkActiveTubeFilter,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkActiveTubeFilter *New();

  ///@{
  /**
   * Set/get the potential (cost) image whose negated gradient attracts the evolving tube surface,
   * e.g. an edge or vesselness feature image that is low near vessel boundaries. Required before
   * Update() is called; its gradient is recomputed internally at the start of RequestData.
   */
  virtual void SetPotentialImage(vtkImageData *);
  vtkGetObjectMacro(PotentialImage, vtkImageData);
  ///@}

  ///@{
  /**
   * Set/get the name of the point data array, on the input centerline poly data, holding the
   * per-point tube radius that is evolved together with the point coordinates.
   * Required before Update() is called. Commonly named "MaximumInscribedSphereRadius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/get the number of active-contour evolution iterations applied to every line cell. Default:
   * a very large integer (VTK_VMTK_LARGE_INTEGER) -- in practice the loop always runs exactly this
   * many iterations, since convergence is not currently checked (see Convergence).
   */
  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
  ///@}

  ///@{
  /**
   * Set/get the weight applied to the anisotropic image force (derived from the PotentialImage
   * gradient) that displaces the centerline, relative to StiffnessWeight. Default: 1.0.
   */
  vtkSetMacro(PotentialWeight,double);
  vtkGetMacro(PotentialWeight,double);
  ///@}

  ///@{
  /**
   * Set/get the weight applied to the internal stiffness force, derived from the second derivatives
   * of the fitted spline, that regularizes (smooths) the evolving tube. Default: 1.0.
   */
  vtkSetMacro(StiffnessWeight,double);
  vtkGetMacro(StiffnessWeight,double);
  ///@}

  ///@{
  /**
   * Set/get the intended convergence threshold for the iterative evolution. Currently unused: the
   * filter does not check this value to stop iteration early, and always runs NumberOfIterations
   * iterations. Default: 0.1.
   */
  vtkSetMacro(Convergence,double);
  vtkGetMacro(Convergence,double);
  ///@}

  ///@{
  /**
   * Set/get the fraction of the local minimum image spacing used to scale the per-iteration
   * displacement (a CFL-like time step controlling how far points/radii can move in one iteration).
   * Default: 0.1.
   */
  vtkSetMacro(CFLCoefficient,double);
  vtkGetMacro(CFLCoefficient,double);
  ///@}

  ///@{
  /**
   * Set/get the lower bound clamped onto the evolving tube radius at every point and iteration.
   * Default: 0.0.
   */
  vtkSetMacro(MinimumRadius,double);
  vtkGetMacro(MinimumRadius,double);
  ///@}

  ///@{
  /**
   * Toggle keeping the coordinates of each cell's first and last point fixed across iterations,
   * instead of letting them evolve like interior points. Default: off.
   */
  vtkSetMacro(FixedEndpointCoordinates,int);
  vtkGetMacro(FixedEndpointCoordinates,int);
  vtkBooleanMacro(FixedEndpointCoordinates,int);
  ///@}

  ///@{
  /**
   * Toggle keeping the radius of each cell's first and last point fixed across iterations, instead
   * of letting it evolve like interior points. Default: off.
   */
  vtkSetMacro(FixedEndpointRadius,int);
  vtkGetMacro(FixedEndpointRadius,int);
  vtkBooleanMacro(FixedEndpointRadius,int);
  ///@}

  ///@{
  /**
   * Set/get the number of directions sampled around the tube circumference, at each longitudinal
   * evaluation point, to estimate the isotropic (radius-changing) and anisotropic
   * (centerline-displacing) image forces. Default: 16.
   */
  vtkSetMacro(NumberOfAngularEvaluations,int);
  vtkGetMacro(NumberOfAngularEvaluations,int);
  ///@}

  ///@{
  /**
   * Toggle re-sampling each cell's points uniformly along arc length, using the spline fit through
   * the current points and radii, at every iteration. Default: on.
   */
  vtkSetMacro(SplineResamplingWhileIterating,int);
  vtkGetMacro(SplineResamplingWhileIterating,int);
  vtkBooleanMacro(SplineResamplingWhileIterating,int);
  ///@}

  ///@{
  /**
   * Toggle emission of a warning whenever a degenerate cross-section is encountered (the squared
   * tube norm derived from the spline derivatives is non-positive), instead of silently skipping the
   * longitudinal evaluation point. Default: off.
   */
  vtkSetMacro(NegativeNormWarnings,int);
  vtkGetMacro(NegativeNormWarnings,int);
  vtkBooleanMacro(NegativeNormWarnings,int);
  ///@}

  protected:
  vtkvmtkActiveTubeFilter();
  ~vtkvmtkActiveTubeFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void EvaluateForce(double point[3], double force[3], bool normalize);
  double EvaluatePotential(double point[3]);

  static bool IsInExtent(vtkIdType extent[6], int ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<=extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<=extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<=extent[5]-border) ? true : false;
    }

  static bool IsCellInExtent(int extent[6], int ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<extent[5]-border) ? true : false;
    }

  void EvolveCellSpline(vtkPolyData* lines, vtkIdType cellId);

  char* RadiusArrayName;

  vtkImageData *PotentialImage;
  vtkImageData *PotentialGradientImage;

  int NumberOfIterations;
  int NumberOfAngularEvaluations;

  double CFLCoefficient;
  double Convergence;

  double PotentialWeight;
  double StiffnessWeight;

  double MinimumRadius;

  double PotentialMaxNorm;

  int FixedEndpointCoordinates;
  int FixedEndpointRadius;

  int SplineResamplingWhileIterating;

  bool NegativeNormWarnings;

private:
  vtkvmtkActiveTubeFilter(const vtkvmtkActiveTubeFilter&);  // Not implemented.
  void operator=(const vtkvmtkActiveTubeFilter&);  // Not implemented.
};

#endif
