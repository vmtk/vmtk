/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkCapPolyData.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
Version:   $Revision: 1.4 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
  // .NAME vtkvmtkCapPolyData - Add caps to boundaries.
  // .SECTION Description
  // This class closes the boundaries of a surface with a cap. Each cap is made of triangles sharing the boundary baricenter. Boundary baricenters are added to the dataset. It is possible to retrieve the ids of the added points with GetCapCenterIds. Boundary baricenters can be displaced along boundary normals through the Displacement parameter. Since this class is used as a preprocessing step for Delaunay tessellation, displacement is meant to avoid the occurence of degenerate tetrahedra on the caps.

#ifndef __vtkvmtkCapPolyData_h
#define __vtkvmtkCapPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkCapPolyData : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkCapPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static vtkvmtkCapPolyData *New();

  // Description:
  // Set/Get the ids of the boundaries to cap.
  vtkSetObjectMacro(BoundaryIds,vtkIdList);
  vtkGetObjectMacro(BoundaryIds,vtkIdList);

  vtkSetStringMacro(CellEntityIdsArrayName);
  vtkGetStringMacro(CellEntityIdsArrayName);

  vtkSetMacro(CellEntityIdOffset,int);
  vtkGetMacro(CellEntityIdOffset,int);

  // Description:
  // Set/Get the displacement of boundary baricenters along boundary normals relative to the radius.
  vtkSetMacro(Displacement,double);
  vtkGetMacro(Displacement,double);

  // Description:
  // Set/Get the displacement of boundary baricenters on the section plane relative to the radius.
  vtkSetMacro(InPlaneDisplacement,double);
  vtkGetMacro(InPlaneDisplacement,double);

  // Description:
  // Get the ids of the newly inserted boundary baricenters.
  vtkGetObjectMacro(CapCenterIds,vtkIdList);

  protected:
  vtkvmtkCapPolyData();
  ~vtkvmtkCapPolyData();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;

  vtkIdList* BoundaryIds;
  char* CellEntityIdsArrayName;
  int CellEntityIdOffset;

  double Displacement;
  double InPlaneDisplacement;
  vtkIdList* CapCenterIds;

  private:
  vtkvmtkCapPolyData(const vtkvmtkCapPolyData&);  // Not implemented.
  void operator=(const vtkvmtkCapPolyData&);  // Not implemented.
};

#endif
