/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCenterlineReferenceSystemAttributesOffset.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkvmtkCenterlineReferenceSystemAttributesOffset - Move the abscissa metric such that its zero point is at a bifurcation center and rotate parallel transport normals such that they are normal to the bifurcation plane.
// .SECTION Description
// Abscissa to be set to 0 at the bifurcation. We twist the entire parallel transport reference framing (rigidly) around the centerline so that the parallel transport normals are normal to the bifurcation plane at the bifurcation. this is useful for analyses that may be conducted later on comparing vessel torsion to the parallel transport normal 

#ifndef __vtkvmtkCenterlineReferenceSystemAttributesOffset_h
#define __vtkvmtkCenterlineReferenceSystemAttributesOffset_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

#include "vtkPolyData.h"

class vtkDoubleArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCenterlineReferenceSystemAttributesOffset : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkCenterlineReferenceSystemAttributesOffset,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkCenterlineReferenceSystemAttributesOffset* New();

  vtkSetStringMacro(OffsetAbscissasArrayName);
  vtkGetStringMacro(OffsetAbscissasArrayName);

  vtkSetStringMacro(OffsetNormalsArrayName);
  vtkGetStringMacro(OffsetNormalsArrayName);

  vtkSetStringMacro(AbscissasArrayName);
  vtkGetStringMacro(AbscissasArrayName);

  vtkSetStringMacro(NormalsArrayName);
  vtkGetStringMacro(NormalsArrayName);

  vtkSetStringMacro(GroupIdsArrayName);
  vtkGetStringMacro(GroupIdsArrayName);

  vtkSetStringMacro(CenterlineIdsArrayName);
  vtkGetStringMacro(CenterlineIdsArrayName);

  vtkSetObjectMacro(ReferenceSystems,vtkPolyData);
  vtkGetObjectMacro(ReferenceSystems,vtkPolyData);

  vtkSetStringMacro(ReferenceSystemsNormalArrayName);
  vtkGetStringMacro(ReferenceSystemsNormalArrayName);

  vtkSetStringMacro(ReferenceSystemsGroupIdsArrayName);
  vtkGetStringMacro(ReferenceSystemsGroupIdsArrayName);

  vtkSetMacro(ReferenceGroupId,int);
  vtkGetMacro(ReferenceGroupId,int);

  protected:
  vtkvmtkCenterlineReferenceSystemAttributesOffset();
  ~vtkvmtkCenterlineReferenceSystemAttributesOffset();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* OffsetAbscissasArrayName;
  char* OffsetNormalsArrayName;

  char* AbscissasArrayName;
  char* NormalsArrayName;
  char* GroupIdsArrayName;
  char* CenterlineIdsArrayName;

  vtkPolyData* ReferenceSystems;

  char* ReferenceSystemsNormalArrayName;
  char* ReferenceSystemsGroupIdsArrayName;

  int ReferenceGroupId;

  private:
  vtkvmtkCenterlineReferenceSystemAttributesOffset(const vtkvmtkCenterlineReferenceSystemAttributesOffset&);  // Not implemented.
  void operator=(const vtkvmtkCenterlineReferenceSystemAttributesOffset&);  // Not implemented.
};

#endif
