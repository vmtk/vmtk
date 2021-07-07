/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineGeometry.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:52:56 $
Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineGeometry - Compute length, curvature, torsion, tortuosity, and frenet frames for individual centerlines. 
// .SECTION Description
// The length, curvature, torsion, and tortuosity metrics are scalar quantities which are identical for each point / tract that makes up a cell.  They are cell data which are attached to the centerline (which should only consist of one cell, as it has not been split or grouped). 
//
// The frenet reference frame is composed of three orthogonal vectors (tangent, normal, and binormal) unique to their parameterization along the centerline. Three vectors are stored as point data for each point making up the centerline. 
//
// A laplacian smoothing filter can be applied to the line if the computation appears to be unstable (as we are using second derivatives and such here). 


#ifndef __vtkvmtkCenterlineGeometry_h
#define __vtkvmtkCenterlineGeometry_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineGeometry : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineGeometry,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineGeometry* New();

  vtkSetStringMacro(LengthArrayName);
  vtkGetStringMacro(LengthArrayName);

  vtkSetStringMacro(CurvatureArrayName);
  vtkGetStringMacro(CurvatureArrayName);

  vtkSetStringMacro(TorsionArrayName);
  vtkGetStringMacro(TorsionArrayName);

  vtkSetStringMacro(TortuosityArrayName);
  vtkGetStringMacro(TortuosityArrayName);

  vtkSetStringMacro(FrenetTangentArrayName);
  vtkGetStringMacro(FrenetTangentArrayName);

  vtkSetStringMacro(FrenetNormalArrayName);
  vtkGetStringMacro(FrenetNormalArrayName);

  vtkSetStringMacro(FrenetBinormalArrayName);
  vtkGetStringMacro(FrenetBinormalArrayName);

  vtkSetMacro(SmoothingFactor,double);
  vtkGetMacro(SmoothingFactor,double);

  vtkSetMacro(NumberOfSmoothingIterations,int);
  vtkGetMacro(NumberOfSmoothingIterations,int);

  vtkSetMacro(LineSmoothing,int);
  vtkGetMacro(LineSmoothing,int);
  vtkBooleanMacro(LineSmoothing,int);

  vtkSetMacro(OutputSmoothedLines,int);
  vtkGetMacro(OutputSmoothedLines,int);
  vtkBooleanMacro(OutputSmoothedLines,int);

  static double ComputeLineCurvature(vtkPoints* linePoints, vtkDoubleArray* curvatureArray);
  static double ComputeLineTorsion(vtkPoints* linePoints, vtkDoubleArray* torsionArray);
  static void ComputeLineFrenetReferenceSystem(vtkPoints* linePoints, vtkDoubleArray* lineTangentArray, vtkDoubleArray* lineNormalArray, vtkDoubleArray* lineBinormalArray);

  protected:
  vtkvmtkCenterlineGeometry();
  ~vtkvmtkCenterlineGeometry();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* LengthArrayName;
  char* CurvatureArrayName;
  char* TorsionArrayName;
  char* TortuosityArrayName;
  char* FrenetTangentArrayName;
  char* FrenetNormalArrayName;
  char* FrenetBinormalArrayName;

  int LineSmoothing;
  int OutputSmoothedLines;
  double SmoothingFactor;
  int NumberOfSmoothingIterations;

  private:
  vtkvmtkCenterlineGeometry(const vtkvmtkCenterlineGeometry&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineGeometry&);  // Not implemented.
};

#endif
