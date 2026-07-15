/*=========================================================================

Program:   VMTK

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * @class   vtkvmtkBoundaryReferenceSystems
 * @brief   Creates a reference system consisting of a radius and normals on top of boundaries.
 * @ingroup ComputationalGeometry
 *
 * This class has methods to estimate the location of the boundary barycenter location, the boundary mean radius, and outward normals to the boundary.
 *
 * As a filter, it takes a surface with open boundaries (e.g. the inlet/outlet caps removed) as
 * input and outputs a vtkPolyData of vertex points, one per boundary, located at each boundary's
 * origin, with the per-boundary radius/normal/in-plane reference points stored as point data
 * arrays (see BoundaryRadiusArrayName, BoundaryNormalsArrayName, Point1ArrayName,
 * Point2ArrayName). This is the filter behind the vmtkboundaryreferencesystems pype script; a
 * typical use is specifying CFD boundary conditions at each inlet/outlet of a vascular model.
 *
 * @sa
 * vtkvmtkCapPolyData, vtkvmtkPolyDataBoundaryExtractor
 */

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
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkBoundaryReferenceSystems *New();

  ///@{
  /**
   * Set/Get the name of the point data array (one 3-component tuple per output vertex/boundary)
   * where the mean boundary radius is stored.
   * Commonly named "BoundaryRadius".
   */
  vtkSetStringMacro(BoundaryRadiusArrayName);
  vtkGetStringMacro(BoundaryRadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where the outward-pointing normal to each boundary
   * plane is stored.
   * Commonly named "BoundaryNormals".
   */
  vtkSetStringMacro(BoundaryNormalsArrayName);
  vtkGetStringMacro(BoundaryNormalsArrayName);
  ///@}

   ///@{
   /**
    * Set/Get the name of the point data array where the first in-plane reference point of each
    * boundary is stored (together with the origin, defines an arbitrary x axis on the boundary
    * plane; see ComputeReferenceSystemPoints).
    * Commonly named "Point1".
    */
  vtkSetStringMacro(Point1ArrayName);
  vtkGetStringMacro(Point1ArrayName);
   ///@}

  ///@{
  /**
   * Set/Get the name of the point data array where the second in-plane reference point of each
   * boundary is stored (together with the origin and Point1, fully defines the boundary plane's
   * local coordinate frame; see ComputeReferenceSystemPoints).
   * Commonly named "Point2".
   */
  vtkSetStringMacro(Point2ArrayName);
  vtkGetStringMacro(Point2ArrayName);
  ///@}

  /**
   * Compute the barycenter (mean position) of the given ordered boundary points.
   */
  static void ComputeBoundaryBarycenter(vtkPoints* points, double barycenter[3]);

  /**
   * Compute the mean distance of the given boundary points from their barycenter.
   */
  static double ComputeBoundaryMeanRadius(vtkPoints* points, double barycenter[3]);

  /**
   * Compute a (non-normalized) normal to the plane best fitting the given boundary points, about
   * their barycenter.
   */
  static void ComputeBoundaryNormal(vtkPoints* points, double barycenter[3], double normal[3]);

  /**
   * Flip normal if needed so that it points away from the body of surface, and store the result in
   * outwardNormal. boundaryCellId is the id, within boundaries, of the boundary polyline that normal
   * was computed for.
   */
  static void OrientBoundaryNormalOutwards(vtkPolyData* surface, vtkPolyData* boundaries, vtkIdType boundaryCellId, double normal[3], double outwardNormal[3]);

  /**
   * Given a boundary reference system (origin, normal, radius), compute two additional points,
   * point1 and point2, lying on the boundary plane at the given radius from origin, such that
   * (origin, point1) defines an arbitrary in-plane x axis and (origin, point1, point2) defines the
   * boundary plane -- used to fully pin down a local coordinate frame at each boundary (e.g. for
   * specifying CFD boundary conditions).
   */
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
