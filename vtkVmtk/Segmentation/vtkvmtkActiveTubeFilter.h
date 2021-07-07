/*=========================================================================

Program:   VMTK
Module:    $RCSfile: .h,v $
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
// .NAME vtkvmtkActiveTubeFilter - Experimental method for generating centerlines from an image.
// .SECTION Description
// Developed with support from the EC FP7/2007-2013: ARCH, Project n. 224390

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkActiveTubeFilter *New();

  virtual void SetPotentialImage(vtkImageData *);
  vtkGetObjectMacro(PotentialImage, vtkImageData);

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);

  vtkSetMacro(PotentialWeight,double);
  vtkGetMacro(PotentialWeight,double);

  vtkSetMacro(StiffnessWeight,double);
  vtkGetMacro(StiffnessWeight,double);

  vtkSetMacro(Convergence,double);
  vtkGetMacro(Convergence,double);

  vtkSetMacro(CFLCoefficient,double);
  vtkGetMacro(CFLCoefficient,double);
 
  vtkSetMacro(MinimumRadius,double);
  vtkGetMacro(MinimumRadius,double);
 
  vtkSetMacro(FixedEndpointCoordinates,int);
  vtkGetMacro(FixedEndpointCoordinates,int);
  vtkBooleanMacro(FixedEndpointCoordinates,int);
 
  vtkSetMacro(FixedEndpointRadius,int);
  vtkGetMacro(FixedEndpointRadius,int);
  vtkBooleanMacro(FixedEndpointRadius,int);
 
  vtkSetMacro(NumberOfAngularEvaluations,int);
  vtkGetMacro(NumberOfAngularEvaluations,int);
 
  vtkSetMacro(SplineResamplingWhileIterating,int);
  vtkGetMacro(SplineResamplingWhileIterating,int);
  vtkBooleanMacro(SplineResamplingWhileIterating,int);
 
  vtkSetMacro(NegativeNormWarnings,int);
  vtkGetMacro(NegativeNormWarnings,int);
  vtkBooleanMacro(NegativeNormWarnings,int);
 
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
