/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkVesselEnhancingDiffusionImageFilter.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:48:25 $
Version:   $Revision: 1.2 $

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

// .NAME vtkvmtkVesselEnhancingDiffusionImageFilter - Wrapper class around itk::VesselEnhancingDiffusionImageFilter
// .SECTION Description
// vtkvmtkVesselEnhancingDiffusionImageFilter


#ifndef __vtkvmtkVesselEnhancingDiffusionImageFilter_h
#define __vtkvmtkVesselEnhancingDiffusionImageFilter_h

#include "vtkSimpleImageToImageFilter.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_SEGMENTATION_EXPORT vtkvmtkVesselEnhancingDiffusionImageFilter : public vtkSimpleImageToImageFilter
{
 public:
  static vtkvmtkVesselEnhancingDiffusionImageFilter *New();
  vtkTypeMacro(vtkvmtkVesselEnhancingDiffusionImageFilter, vtkSimpleImageToImageFilter);

  vtkGetMacro(SigmaMin,double);
  vtkSetMacro(SigmaMin,double);

  vtkGetMacro(SigmaMax,double);
  vtkSetMacro(SigmaMax,double);

  vtkGetMacro(NumberOfSigmaSteps,int);
  vtkSetMacro(NumberOfSigmaSteps,int);

  vtkGetMacro(SigmaStepMethod,int);
  vtkSetMacro(SigmaStepMethod,int);

  void SetSigmaStepMethodToEquispaced()
  {
    this->SetSigmaStepMethod(EQUISPACED);
  }

  void SetSigmaStepMethodToLogarithmic()
  {
    this->SetSigmaStepMethod(LOGARITHMIC);
  }

  enum
  {
    EQUISPACED,
    LOGARITHMIC
  };

  vtkGetMacro(Alpha,double);
  vtkSetMacro(Alpha,double);

  vtkGetMacro(Beta,double);
  vtkSetMacro(Beta,double);

  vtkGetMacro(Gamma,double);
  vtkSetMacro(Gamma,double);

  vtkGetMacro(C,double);
  vtkSetMacro(C,double);

  vtkGetMacro(NumberOfIterations,int);
  vtkSetMacro(NumberOfIterations,int);

  vtkGetMacro(NumberOfDiffusionSubIterations,int);
  vtkSetMacro(NumberOfDiffusionSubIterations,int);

  vtkGetMacro(TimeStep,double);
  vtkSetMacro(TimeStep,double);

  vtkGetMacro(Epsilon,double);
  vtkSetMacro(Epsilon,double);

  vtkGetMacro(WStrength,double);
  vtkSetMacro(WStrength,double);

  vtkGetMacro(Sensitivity,double);
  vtkSetMacro(Sensitivity,double);

protected:

  vtkvmtkVesselEnhancingDiffusionImageFilter();
  ~vtkvmtkVesselEnhancingDiffusionImageFilter() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) override;

private:
  vtkvmtkVesselEnhancingDiffusionImageFilter(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkvmtkVesselEnhancingDiffusionImageFilter&);  // Not implemented.

  double SigmaMin;
  double SigmaMax;
  int NumberOfSigmaSteps;
  int SigmaStepMethod;
  int NumberOfIterations;
  int NumberOfDiffusionSubIterations;
  double TimeStep;
  double Epsilon;
  double WStrength;
  double Sensitivity;
  double Alpha;
  double Beta;
  double Gamma;
  double C;
};

#endif

