/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkStreamlineToParticlesFilter.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkStreamlineToParticlesFilter - Cluster streamlines based on Mahalanobis distance metric and K-Means clustering.
// .SECTION Description
// This class clusters streamlines.

#ifndef __vtkvmtkStreamlineToParticlesFilter_h
#define __vtkvmtkStreamlineToParticlesFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_MISC_EXPORT vtkvmtkStreamlineToParticlesFilter : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkStreamlineToParticlesFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkStreamlineToParticlesFilter *New();
 
  vtkSetMacro(NumberOfInjections,int);
  vtkGetMacro(NumberOfInjections,int);
 
  vtkSetMacro(NumberOfParticlesPerInjection,int);
  vtkGetMacro(NumberOfParticlesPerInjection,int);

  vtkSetMacro(InjectionStart,double);
  vtkGetMacro(InjectionStart,double);
 
  vtkSetMacro(InjectionEnd,double);
  vtkGetMacro(InjectionEnd,double);
 
  vtkSetMacro(TracingEnd,double);
  vtkGetMacro(TracingEnd,double);
 
  vtkSetMacro(DeltaT,double);
  vtkGetMacro(DeltaT,double);
 
  vtkSetStringMacro(IntegrationTimeArrayName);
  vtkGetStringMacro(IntegrationTimeArrayName);
 
  vtkSetStringMacro(TimeArrayName);
  vtkGetStringMacro(TimeArrayName);
 
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
 
  protected:
  vtkvmtkStreamlineToParticlesFilter();
  ~vtkvmtkStreamlineToParticlesFilter();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int NumberOfInjections;
  int NumberOfParticlesPerInjection;

  double InjectionStart;
  double InjectionEnd;
  double TracingEnd;
  double DeltaT;

  char* IntegrationTimeArrayName;
  char* TimeArrayName;
  char* VelocityArrayName;

  private:
  vtkvmtkStreamlineToParticlesFilter(const vtkvmtkStreamlineToParticlesFilter&);  // Not implemented.
  void operator=(const vtkvmtkStreamlineToParticlesFilter&);  // Not implemented.
};

#endif
