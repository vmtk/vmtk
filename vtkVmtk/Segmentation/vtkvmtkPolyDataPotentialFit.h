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
 * @class   vtkvmtkPolyDataPotentialFit
 * @brief   Create an explicitly deformable model which evolves a surface to gradient magnitudes of an input image.
 * @ingroup Segmentation
 *
 * vtkvmtkPolyDataPotentialFit implements an explicit (mesh-based) deformable model ("snake") that
 * iteratively displaces the points of an input surface under three combined forces: a potential
 * force that pulls points towards local maxima of PotentialImage's gradient magnitude (typically
 * an edge/gradient-magnitude image derived from the volume being segmented), a stiffness
 * (Laplacian-smoothing) force that regularizes the surface by pulling each point towards the
 * centroid of its one-ring neighbors, and an inflation force that expands/contracts the surface
 * along its normal based on InflationImage. This is the filter behind the vmtkpotentialfit pype
 * script, used e.g. to snap a coarse initial surface onto image edges. At each of NumberOfIterations
 * outer iterations, the filter alternates NumberOfStiffnessSubIterations pure-stiffness relaxation
 * steps, NumberOfInflationSubIterations pure-inflation steps, and one combined
 * potential+stiffness+inflation step, adaptively choosing the time step (see AdaptiveTimeStep) so
 * that the largest point displacement does not exceed one voxel; iteration stops early once the
 * scaled maximum displacement falls below Convergence.
 */

#ifndef __vtkvmtkPolyDataPotentialFit_h
#define __vtkvmtkPolyDataPotentialFit_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;
class vtkDoubleArray;
class vtkvmtkNeighborhoods;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkPolyDataPotentialFit : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataPotentialFit,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataPotentialFit *New();

  ///@{
  /**
   * Set/get the scalar image (typically a gradient-magnitude/edge image) whose spatial gradient
   * defines the potential force that attracts surface points towards image edges. Required; its
   * gradient is (re)computed internally at the start of RequestData.
   */
  virtual void SetPotentialImage(vtkImageData *);
  vtkGetObjectMacro(PotentialImage, vtkImageData);
  ///@}

  ///@{
  /**
   * Set/get the scalar image sampled (relative to InflationThreshold) to drive the inflation force;
   * if not set, EvaluateInflation returns a constant 1.0 everywhere (uniform inflation).
   */
  virtual void SetInflationImage(vtkImageData *);
  vtkGetObjectMacro(InflationImage, vtkImageData);
  ///@}

  ///@{
  /**
   * Set/get the value subtracted from InflationImage samples when evaluating the inflation force,
   * effectively the iso-value at which the inflation force changes sign. Default: 0.0.
   */
  vtkSetMacro(InflationThreshold, double);
  vtkGetMacro(InflationThreshold, double);
  ///@}

  ///@{
  /**
   * Set/get the maximum number of outer deformation iterations to perform (evolution may stop
   * earlier once TestConvergence succeeds). Default: VTK_VMTK_LARGE_INTEGER.
   */
  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);
  ///@}

  ///@{
  /**
   * Set/get the number of pure-stiffness (Laplacian smoothing) sub-iterations performed at the
   * start of each outer iteration, before the combined potential+stiffness+inflation step.
   * Default: 5.
   */
  vtkSetMacro(NumberOfStiffnessSubIterations, int);
  vtkGetMacro(NumberOfStiffnessSubIterations, int);
  ///@}

  ///@{
  /**
   * Set/get the number of pure-inflation sub-iterations performed at the start of each outer
   * iteration, before the combined potential+stiffness+inflation step. Default: 0.
   */
  vtkSetMacro(NumberOfInflationSubIterations, int);
  vtkGetMacro(NumberOfInflationSubIterations, int);
  ///@}

  ///@{
  /**
   * Set/get the relaxation factor applied to the total displacement at each sub-step (a fraction in
   * (0,1] used to damp the update). Default: 1.0.
   */
  vtkSetMacro(Relaxation, double);
  vtkGetMacro(Relaxation, double);
  ///@}

  ///@{
  /**
   * Set/get the weight of the potential (image-gradient-attraction) force in the combined
   * displacement. Default: 0.0.
   */
  vtkSetMacro(PotentialWeight, double);
  vtkGetMacro(PotentialWeight, double);
  ///@}

  ///@{
  /**
   * Set/get the weight of the stiffness (Laplacian smoothing) force in the combined displacement.
   * Default: 0.0.
   */
  vtkSetMacro(StiffnessWeight, double);
  vtkGetMacro(StiffnessWeight, double);
  ///@}

  ///@{
  /**
   * Set/get the weight of the inflation (normal expansion/contraction) force in the combined
   * displacement. Default: 0.0.
   */
  vtkSetMacro(InflationWeight, double);
  vtkGetMacro(InflationWeight, double);
  ///@}

  ///@{
  /**
   * Set/get the convergence threshold: iteration stops once TimeStep times the maximum point
   * displacement norm drops below this value. Default: 1E-1.
   */
  vtkSetMacro(Convergence, double);
  vtkGetMacro(Convergence, double);
  ///@}

  ///@{
  /**
   * Set/get the upper bound on the (possibly adaptive) time step, expressed in pixels per time
   * step. Default: 1.0.
   */
  vtkSetMacro(MaxTimeStep, double);
  vtkGetMacro(MaxTimeStep, double);
  ///@}

  ///@{
  /**
   * Set/get the time step used to scale point displacements at each sub-step. Overwritten every
   * sub-step when AdaptiveTimeStep is on; used as a fixed value otherwise. Default: 0.0.
   */
  vtkSetMacro(TimeStep, double);
  vtkGetMacro(TimeStep, double);
  ///@}

  ///@{
  /**
   * Toggle automatic time step selection: when on, TimeStep is recomputed at every sub-step as the
   * minimum potential image spacing divided by the maximum displacement norm (capped at
   * MaxTimeStep), keeping point motion below about one voxel per step. Default: on.
   */
  vtkSetMacro(AdaptiveTimeStep, int);
  vtkGetMacro(AdaptiveTimeStep, int);
  vtkBooleanMacro(AdaptiveTimeStep, int);
  ///@}

  ///@{
  /**
   * Toggle flipping of the computed surface normals used for the inflation force direction (passed
   * through to the internal vtkPolyDataNormals as FlipNormals). Default: off.
   */
  vtkSetMacro(FlipNormals, int);
  vtkGetMacro(FlipNormals, int);
  vtkBooleanMacro(FlipNormals, int);
  ///@}

  ///@{
  /**
   * Toggle whether the inflation force is additionally modulated by the local potential value
   * (EvaluatePotential), so that inflation slows down near strong image edges. Default: on.
   */
  vtkSetMacro(UsePotentialInInflation, int);
  vtkGetMacro(UsePotentialInInflation, int);
  vtkBooleanMacro(UsePotentialInInflation, int);
  ///@}

  ///@{
  /**
   * Set/get the spatial dimensionality (2 or 3) used when computing the gradient of PotentialImage.
   * Default: 3.
   */
  vtkSetMacro(Dimensionality, int);
  vtkGetMacro(Dimensionality, int);
  ///@}

  protected:
  vtkvmtkPolyDataPotentialFit();
  ~vtkvmtkPolyDataPotentialFit();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void EvaluateForce(double point[3], double force[3], bool normalize = true);
  double EvaluatePotential(double point[3]);
  double EvaluateInflation(double point[3]);
  void ComputeDisplacements(bool potential, bool stiffness, bool inflation);
  void ComputePotentialDisplacement(vtkIdType pointId, double potentialDisplacement[3]);
  void ComputeStiffnessDisplacement(vtkIdType pointId, double stiffnessDisplacement[3]);
  void ComputeInflationDisplacement(vtkIdType pointId, double inflationDisplacement[3]);
  void ComputeTimeStep();
  void ApplyDisplacements();

  int TestConvergence();

  static double ComputeMinSpacing(double spacing[3]);

  static bool IsInExtent(vtkIdType extent[6], int ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<=extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<=extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<=extent[5]-border) ? true : false;
    }

  static bool IsCellInExtent(int extent[6], int ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<extent[5]-border) ? true : false;
    }

  vtkImageData *PotentialImage;
  vtkImageData *InflationImage;
  vtkImageData *PotentialGradientImage;

  int NumberOfIterations;

  int NumberOfStiffnessSubIterations;
  int NumberOfInflationSubIterations;

  double TimeStep;
  double Relaxation;
  double MaxDisplacementNorm;
  double MinEdgeLength;
  double MinPotentialSpacing;
  double Convergence;

  double PotentialWeight;
  double StiffnessWeight;
  double InflationWeight;

  double InflationThreshold;
  double PotentialMaxNorm;
  int UsePotentialInInflation;
  
  double MaxTimeStep;
  int AdaptiveTimeStep;

  int FlipNormals;

  int Dimensionality;

  vtkDoubleArray *Displacements;

  vtkvmtkNeighborhoods *Neighborhoods;
  vtkDataArray *Normals;

  private:
  vtkvmtkPolyDataPotentialFit(const vtkvmtkPolyDataPotentialFit&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataPotentialFit&);  // Not implemented.
};

#endif
