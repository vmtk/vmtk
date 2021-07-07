/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataPotentialFit.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

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
// .NAME vtkvmtkPolyDataPotentialFit - Create an explicitly deformable model which evolves a surface to gradient magnitudes of an input image.
// .SECTION Description
// ..

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataPotentialFit *New();

  virtual void SetPotentialImage(vtkImageData *);
  vtkGetObjectMacro(PotentialImage, vtkImageData);

  virtual void SetInflationImage(vtkImageData *);
  vtkGetObjectMacro(InflationImage, vtkImageData);

  vtkSetMacro(InflationThreshold, double);
  vtkGetMacro(InflationThreshold, double);

  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  vtkSetMacro(NumberOfStiffnessSubIterations, int);
  vtkGetMacro(NumberOfStiffnessSubIterations, int);

  vtkSetMacro(NumberOfInflationSubIterations, int);
  vtkGetMacro(NumberOfInflationSubIterations, int);

  vtkSetMacro(Relaxation, double);
  vtkGetMacro(Relaxation, double);

  vtkSetMacro(PotentialWeight, double);
  vtkGetMacro(PotentialWeight, double);

  vtkSetMacro(StiffnessWeight, double);
  vtkGetMacro(StiffnessWeight, double);

  vtkSetMacro(InflationWeight, double);
  vtkGetMacro(InflationWeight, double);

  vtkSetMacro(Convergence, double);
  vtkGetMacro(Convergence, double);

  vtkSetMacro(MaxTimeStep, double);
  vtkGetMacro(MaxTimeStep, double);

  vtkSetMacro(TimeStep, double);
  vtkGetMacro(TimeStep, double);
  
  vtkSetMacro(AdaptiveTimeStep, int);
  vtkGetMacro(AdaptiveTimeStep, int);
  vtkBooleanMacro(AdaptiveTimeStep, int);
  
  vtkSetMacro(FlipNormals, int);
  vtkGetMacro(FlipNormals, int);
  vtkBooleanMacro(FlipNormals, int);

  vtkSetMacro(UsePotentialInInflation, int);
  vtkGetMacro(UsePotentialInInflation, int);
  vtkBooleanMacro(UsePotentialInInflation, int);

  vtkSetMacro(Dimensionality, int);
  vtkGetMacro(Dimensionality, int);

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
