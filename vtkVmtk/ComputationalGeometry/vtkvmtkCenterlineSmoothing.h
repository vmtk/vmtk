/*=========================================================================

Program:   VTK Blood Vessel Smoothing
Module:    $RCSfile: vtkvmtkCenterlineSmoothing.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineSmoothing - iteratively smooth a centerline with a laplacian kernel 
// .SECTION Description
// ...

#ifndef __vtkvmtkCenterlineSmoothing_h
#define __vtkvmtkCenterlineSmoothing_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalSmoothingWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineSmoothing : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineSmoothing,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineSmoothing* New();

  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);

  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);

  static void SmoothLine(vtkPoints* linePoints, vtkPoints* smoothLinePoints, int numberOfIterations = 10, double relaxation = 0.1);

  protected:
  vtkvmtkCenterlineSmoothing();
  ~vtkvmtkCenterlineSmoothing();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineSmoothing(const vtkvmtkCenterlineSmoothing&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineSmoothing&);  // Not implemented.
};

#endif
