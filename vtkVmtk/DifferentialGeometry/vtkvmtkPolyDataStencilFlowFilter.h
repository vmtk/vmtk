/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkPolyDataStencilFlowFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:44 $
  Version:   $Revision: 1.2 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkPolyDataStencilFlowFilter - Displace points of a surface with an iterative algorithm based on stencil weighting. 
// .SECTION Description
// ..

#ifndef __vtkvmtkPolyDataStencilFlowFilter_h
#define __vtkvmtkPolyDataStencilFlowFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkConstants.h"
//#include "vtkvmtkDifferentialGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkvmtkStencils;

class VTK_VMTK_DIFFERENTIAL_GEOMETRY_EXPORT vtkvmtkPolyDataStencilFlowFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkvmtkPolyDataStencilFlowFilter* New();
  vtkTypeMacro(vtkvmtkPolyDataStencilFlowFilter,vtkPolyDataAlgorithm);

  vtkSetMacro(StencilType,int);
  vtkGetMacro(StencilType,int);
  void SetStencilTypeToUmbrellaStencil() 
    {this->SetStencilType(VTK_VMTK_UMBRELLA_STENCIL);};
  void SetStencilTypeToAreaWeightedUmbrellaStencil() 
    {this->SetStencilType(VTK_VMTK_AREA_WEIGHTED_UMBRELLA_STENCIL);};
  void SetStencilTypeToFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FE_LAPLACE_BELTRAMI_STENCIL);};
  void SetStencilTypeToFVFELaplaceBeltramiStencil() 
    {this->SetStencilType(VTK_VMTK_FVFE_LAPLACE_BELTRAMI_STENCIL);};

  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
 
  vtkSetMacro(RelaxationFactor,double);
  vtkGetMacro(RelaxationFactor,double);

  vtkSetMacro(ProcessBoundary,int);
  vtkGetMacro(ProcessBoundary,int);
  vtkBooleanMacro(ProcessBoundary,int);
 
  vtkSetMacro(ConstrainOnSurface,int);
  vtkGetMacro(ConstrainOnSurface,int);
  vtkBooleanMacro(ConstrainOnSurface,int);

  vtkSetMacro(MaximumDisplacement,double);
  vtkGetMacro(MaximumDisplacement,double);
  
protected:
  vtkvmtkPolyDataStencilFlowFilter();
  ~vtkvmtkPolyDataStencilFlowFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void ReleaseStencils();
  
  int StencilType;
  vtkvmtkStencils* Stencils;

  int NumberOfIterations;
  double RelaxationFactor;

  double MaximumDisplacement;

  int ProcessBoundary;
  int ConstrainOnSurface;
  
private:
  vtkvmtkPolyDataStencilFlowFilter(const vtkvmtkPolyDataStencilFlowFilter&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataStencilFlowFilter&);  // Not implemented.
};

#endif

