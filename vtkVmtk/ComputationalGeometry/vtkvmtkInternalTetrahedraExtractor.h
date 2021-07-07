/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkInternalTetrahedraExtractor.h,v $
Language:  C++
Date:      $Date: 2006/04/06 16:46:43 $
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
// .NAME vtkvmtkInternalTetrahedraExtractor - Extract internal tetrahedra from a Delaunay tessellation of a surface.
// .SECTION Description
// This class takes in input the Delaunay tessellation of a point set and extracts internal tetrahedra based on outward oriented point normals (to be provided as input point data array). A tetrahedron \f$T_i\f$ is retained if \f[(x_j - c_i) \cdot n_j \geq 0  \qquad  \forall x_j \in T_i \f] where \f$x_i\f$ are the vertices of \f$T_i\f$, \f$c_i\f$ its circumcenter and \f$n_j\f$ the normals at the vertices. It is possible to properly handle capped regions (generated with vtkCapPolyData) by activating UseCaps and providing the ids of cap centers.
// .SECTION See Also
// vtkCapPolyData

#ifndef __vtkvmtkInternalTetrahedraExtractor_h
#define __vtkvmtkInternalTetrahedraExtractor_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkInternalTetrahedraExtractor : public vtkUnstructuredGridAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkInternalTetrahedraExtractor,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkInternalTetrahedraExtractor *New();
  
  // Description:
  // Set/Get the name of the array containing outward oriented point normals.
  vtkSetStringMacro(OutwardNormalsArrayName);
  vtkGetStringMacro(OutwardNormalsArrayName);

  // Description:
  // Turn on/off special handling of caps.
  vtkSetMacro(UseCaps,int);
  vtkGetMacro(UseCaps,int);
  vtkBooleanMacro(UseCaps,int);

  // Description:
  // Set/Get the ids of cap centers.
  vtkSetObjectMacro(CapCenterIds,vtkIdList);
  vtkGetObjectMacro(CapCenterIds,vtkIdList);

  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  // Description:
  // Turn on/off removal of surface slivers.
  vtkSetMacro(RemoveSubresolutionTetrahedra,int);
  vtkGetMacro(RemoveSubresolutionTetrahedra,int);
  vtkBooleanMacro(RemoveSubresolutionTetrahedra,int);

  vtkSetMacro(SubresolutionFactor,double);
  vtkGetMacro(SubresolutionFactor,double);

  vtkSetObjectMacro(Surface,vtkPolyData);
  vtkGetObjectMacro(Surface,vtkPolyData);

  protected:
  vtkvmtkInternalTetrahedraExtractor();
  ~vtkvmtkInternalTetrahedraExtractor();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int UseCaps;
  vtkIdList* CapCenterIds;
  char* OutwardNormalsArrayName;

  double Tolerance;

  int RemoveSubresolutionTetrahedra;
  vtkPolyData* Surface;
  double SubresolutionFactor;

  private:
  vtkvmtkInternalTetrahedraExtractor(const vtkvmtkInternalTetrahedraExtractor&);  // Not implemented.
  void operator=(const vtkvmtkInternalTetrahedraExtractor&);  // Not implemented.
};

#endif
