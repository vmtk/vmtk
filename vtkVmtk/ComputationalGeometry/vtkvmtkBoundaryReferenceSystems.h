/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkBoundaryReferenceSystems.h,v $
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
// .NAME vtkvmtkBoundaryReferenceSystems - Creates a reference system consisting of a radius and normals on top of boundaries.
// .SECTION Description
// This class has methods to estimate the location of the boundary barycenter location, the boundary mean radius, and outward normals to the boundary.

#ifndef __vtkvmtkBoundaryReferenceSystems_h
#define __vtkvmtkBoundaryReferenceSystems_h

#include "vtkPolyDataAlgorithm.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkPoints;
class vtkPolyLine;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkBoundaryReferenceSystems : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkBoundaryReferenceSystems,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkBoundaryReferenceSystems *New();

  // Description:
  // Set/Get the name of the array where mean boundary radius has to be stored.
  vtkSetStringMacro(BoundaryRadiusArrayName);
  vtkGetStringMacro(BoundaryRadiusArrayName);

  // Description:
  // Set/Get the name of the array where normals to boundaries have to be stored.
  vtkSetStringMacro(BoundaryNormalsArrayName);
  vtkGetStringMacro(BoundaryNormalsArrayName);

   // Description:
  // Set/Get the name of the array where reference system points 1 have to be stored.
  vtkSetStringMacro(Point1ArrayName);
  vtkGetStringMacro(Point1ArrayName);

  // Description:
  // Set/Get the name of the array where reference system points 2 have to be stored.
  vtkSetStringMacro(Point2ArrayName);
  vtkGetStringMacro(Point2ArrayName);

  static void ComputeBoundaryBarycenter(vtkPoints* points, double barycenter[3]);
  static double ComputeBoundaryMeanRadius(vtkPoints* points, double barycenter[3]);
  static void ComputeBoundaryNormal(vtkPoints* points, double barycenter[3], double normal[3]);
  static void OrientBoundaryNormalOutwards(vtkPolyData* surface, vtkPolyData* boundaries, vtkIdType boundaryCellId, double normal[3], double outwardNormal[3]);
  static void ComputeReferenceSystemPoints(double origin[3], double normal[3], double radius, double point1[3], double point2[3]);

  protected:
  vtkvmtkBoundaryReferenceSystems();
  ~vtkvmtkBoundaryReferenceSystems();  

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* BoundaryRadiusArrayName;
  char* BoundaryNormalsArrayName;
  char* Point1ArrayName;
  char* Point2ArrayName;

  private:
  vtkvmtkBoundaryReferenceSystems(const vtkvmtkBoundaryReferenceSystems&);  // Not implemented.
  void operator=(const vtkvmtkBoundaryReferenceSystems&);  // Not implemented.
};

#endif
