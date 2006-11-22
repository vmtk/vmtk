/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataPotentialFit.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

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

  // .NAME vtkvmtkPolyDataPotentialFit - ...
  // .SECTION Description
  // .

#ifndef __vtkvmtkPolyDataPotentialFit_h
#define __vtkvmtkPolyDataPotentialFit_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkvmtkWin32Header.h"

class vtkImageData;
class vtkDoubleArray;
class vtkvmtkNeighborhoods;

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkPolyDataPotentialFit : public vtkPolyDataToPolyDataFilter
{
  public: 
  vtkTypeRevisionMacro(vtkvmtkPolyDataPotentialFit,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent); 

  static vtkvmtkPolyDataPotentialFit *New();

  virtual void SetPotentialImage(vtkImageData *);
  vtkGetObjectMacro(PotentialImage, vtkImageData);

  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  vtkSetMacro(NumberOfStiffnessSubIterations, int);
  vtkGetMacro(NumberOfStiffnessSubIterations, int);

  vtkSetMacro(Relaxation, double);
  vtkGetMacro(Relaxation, double);

  vtkSetMacro(PotentialWeight, double);
  vtkGetMacro(PotentialWeight, double);

  vtkSetMacro(StiffnessWeight, double);
  vtkGetMacro(StiffnessWeight, double);

  vtkSetMacro(Convergence, double);
  vtkGetMacro(Convergence, double);

  vtkSetMacro(MaxTimeStep, double);
  vtkGetMacro(MaxTimeStep, double);

  vtkSetMacro(InplanePotential, int);
  vtkGetMacro(InplanePotential, int);
  vtkBooleanMacro(InplanePotential, int);

  vtkSetMacro(InplaneTolerance, double);
  vtkGetMacro(InplaneTolerance, double);

  protected:
  vtkvmtkPolyDataPotentialFit();
  ~vtkvmtkPolyDataPotentialFit();  

  void Execute();

  void EvaluateForce(double point[3], double force[3], bool normalize = true);
  void ComputeDisplacements(bool potential, bool stiffness);
  void ComputePotentialDisplacement(vtkIdType pointId, double potentialDisplacement[3]);
  void ComputeStiffnessDisplacement(vtkIdType pointId, double stiffnessDisplacement[3]);
  void ComputeTimeStep();
  void ApplyDisplacements();

  int TestConvergence();

  static double ComputeMinSpacing(double spacing[3]);

  static bool IsInExtent(vtkIdType extent[6], vtkIdType ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<=extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<=extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<=extent[5]-border) ? true : false;
    }

  static bool IsCellInExtent(vtkIdType extent[6], vtkIdType ijk[3], vtkIdType border)
    {
    return (ijk[0]>=extent[0]+border && ijk[0]<extent[1]-border) && (ijk[1]>=extent[2]+border && ijk[1]<extent[3]-border) && (ijk[2]>=extent[4]+border && ijk[2]<extent[5]-border) ? true : false;
    }

  vtkImageData *PotentialImage;

  int InplanePotential;
  double InplaneTolerance;

  int NumberOfIterations;

  int NumberOfStiffnessSubIterations;

  double TimeStep;
  double Relaxation;
  double MaxDisplacementNorm;
  double MinEdgeLength;
  double MinPotentialSpacing;
  double Convergence;

  double PotentialWeight;
  double StiffnessWeight;

  double PotentialMaxNorm;
  double MaxTimeStep;

  vtkDoubleArray *Displacements;

  vtkvmtkNeighborhoods *Neighborhoods;

  int VoxelOffsets[8][3];

  private:
  vtkvmtkPolyDataPotentialFit(const vtkvmtkPolyDataPotentialFit&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataPotentialFit&);  // Not implemented.
};

#endif
