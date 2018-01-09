/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineEndpointExtractor.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.5 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCenterlineEndpointExtractor - ...
  // .SECTION Description
  // ...

#ifndef __vtkvmtkCenterlineEndpointExtractor_h
#define __vtkvmtkCenterlineEndpointExtractor_h

#include "vtkvmtkCenterlineSplittingAndGroupingFilter.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"
  
class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineEndpointExtractor : public vtkvmtkCenterlineSplittingAndGroupingFilter
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineEndpointExtractor,vtkvmtkCenterlineSplittingAndGroupingFilter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE; 

  static vtkvmtkCenterlineEndpointExtractor *New();

  vtkSetMacro(NumberOfEndpointSpheres,int);
  vtkGetMacro(NumberOfEndpointSpheres,int);

  vtkSetMacro(NumberOfGapSpheres,int);
  vtkGetMacro(NumberOfGapSpheres,int);

  vtkSetMacro(ExtractionMode,int);
  vtkGetMacro(ExtractionMode,int);
  void SetExtractionModeToFirstEndpoint() 
    { this->SetExtractionMode(VTK_VMTK_FIRST_ENDPOINT); }
  void SetExtractionModeToLastEndpoint() 
    { this->SetExtractionMode(VTK_VMTK_LAST_ENDPOINT); }
  void SetExtractionModeToBothEndpoints() 
    { this->SetExtractionMode(VTK_VMTK_BOTH_ENDPOINTS); }
  
  protected:
  vtkvmtkCenterlineEndpointExtractor();
  ~vtkvmtkCenterlineEndpointExtractor();  

  virtual void ComputeCenterlineSplitting(vtkPolyData* input, vtkIdType cellId) VTK_OVERRIDE;

  int NumberOfEndpointSpheres;
  int NumberOfGapSpheres;
  int ExtractionMode;

  //BTX
  enum
    {
      VTK_VMTK_FIRST_ENDPOINT,
      VTK_VMTK_LAST_ENDPOINT,
      VTK_VMTK_BOTH_ENDPOINTS
    };

  //ETX

  private:
  vtkvmtkCenterlineEndpointExtractor(const vtkvmtkCenterlineEndpointExtractor&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineEndpointExtractor&);  // Not implemented.
};

#endif
