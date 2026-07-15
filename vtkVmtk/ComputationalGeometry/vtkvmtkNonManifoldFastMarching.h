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
 * @class   vtkvmtkNonManifoldFastMarching
 * @brief   Implements the Fast Marching Method on polygonal non-manifolds.
 * @ingroup ComputationalGeometry
 *
 * This class is used to solve the Eikonal equation \f[|\nabla T(\mathbf{x})| = F(\mathbf{x})\f] on non-manifolds made of convex polygons, using the Fast Marching Method, by J.A. Sethian (see below). The solution to the Eikonal equation represents the arrival times of a wave propagating on a domain with speed \f$F^{-1}(\mathbf{x})\f$ from given seed points (or regions). If F(x)=1, T(x) represents the geodesic distance field to the seed points (Note: F(x) can be equivalently interpreted as a cost function). The Fast Marching Method takes O(N logN). For more insight see J.A. Sethian, Level Set Methods and Fast Marching Methods, Cambridge University Press, 2nd Edition, 1999.
 *
 * The implementation given in this class is an extension of the Fast Marching Method for triangulated 2-manifolds proposed by Kimmel et al. (R. Kimmel and J.A. Sethian. Computing geodesic paths on manifolds. PNAS, 95(15): 8431-8435, Jul 1998.) to non-manifolds made of convex polygons, such as the Voronoi diagram.
 *
 * The propagation starts from a set of seeds. By default the seed points are given a propagation time of 0.0, but activating InitializeFromScalars and providing the point data array of name InitializationArrayName, it is possible to assign nonzero initialization times (this is useful when the zero propagation time point lies inside a cell). For the specification of F(x), the user must either provide a point data array of name CostFunctionArrayName, or activate UnitSpeed, which sets F(x)=1 everywhere, thus yielding a geodesic distance field. The propagation stops when all the points in the domain path-connected to the seeds have been visited. Alternatively it is possible to limit the propagation by setting StopTravelTime or StopNumberOfPoints. The solution is stored in a point data array of name SolutionArrayName (name provided by the user; see SolutionArrayName for typical values).
 *
 * The Regularization value adds a constant term to F(x), which acts as a regularization term for the minimal cost paths (see L.D. Cohen and R. Kimmel. Global minimum of active contour models: a minimal path approach. IJCV, 24(1): 57-78, Aug 1997).
 *
 *
 * @sa
 * vtkVoronoiDiagram3D vtkMinHeap
 */

#ifndef __vtkvmtkNonManifoldFastMarching_h
#define __vtkvmtkNonManifoldFastMarching_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"
#include "vtkPolyData.h"
#include "vtkvmtkMinHeap.h"
#include "vtkvmtkConstants.h"
//#include "vtkvmtkComputationalGeometryWin32Header.h"
#include "vtkvmtkWin32Header.h"

const char VTK_VMTK_ACCEPTED_STATUS = 0x01;
const char VTK_VMTK_CONSIDERED_STATUS = 0x02;
const char VTK_VMTK_FAR_STATUS = 0x04;

class vtkDoubleArray;
class vtkCharArray;

class VTK_VMTK_COMPUTATIONAL_GEOMETRY_EXPORT vtkvmtkNonManifoldFastMarching : public vtkPolyDataAlgorithm
{
  public:
  vtkTypeMacro(vtkvmtkNonManifoldFastMarching,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkNonManifoldFastMarching *New();

  ///@{
  /**
   * Set/Get maximum travel time for travel time-based stop criterion.
   */
  vtkSetMacro(StopTravelTime,double);
  vtkGetMacro(StopTravelTime,double);
  ///@}

  ///@{
  /**
   * Set/Get maximum number of visited points for number of visited points-based stop criterion.
   */
  vtkSetMacro(StopNumberOfPoints,int);
  vtkGetMacro(StopNumberOfPoints,int);
  ///@}

  ///@{
  /**
   * Set/Get minimal cost path regularization amount.
   */
  vtkSetMacro(Regularization,double);
  vtkGetMacro(Regularization,double);
  ///@}

  ///@{
  /**
   * Toggle on/off boundary conditions expressed as a list of seeds.
   */
  vtkSetMacro(SeedsBoundaryConditions,int);
  vtkGetMacro(SeedsBoundaryConditions,int);
  vtkBooleanMacro(SeedsBoundaryConditions,int);
  ///@}

  ///@{
  /**
   * Toggle on/off boundary conditions expressed as poly data.
   */
  vtkSetMacro(PolyDataBoundaryConditions,int);
  vtkGetMacro(PolyDataBoundaryConditions,int);
  vtkBooleanMacro(PolyDataBoundaryConditions,int);
  ///@}

  ///@{
  /**
   * Set/Get seeds were boundary conditions are specified.
   */
  vtkSetObjectMacro(Seeds,vtkIdList);
  vtkGetObjectMacro(Seeds,vtkIdList);
  ///@}

  ///@{
  /**
   * (Set/Get) when set, the algorithm will terminate when the front has propagated from the (source)
   * "Seeds" to this (target) "Seed".
   */
  vtkSetObjectMacro(StopSeedId,vtkIdList);
  vtkGetObjectMacro(StopSeedId,vtkIdList);
  ///@}

  ///@{
  /**
   * Set/Get poly data were boundary conditions are specified.
   */
  vtkSetObjectMacro(BoundaryPolyData,vtkPolyData);
  vtkGetObjectMacro(BoundaryPolyData,vtkPolyData);
  ///@}

  ///@{
  /**
   * Set/Get the name of the 2-component point data array of boundary poly data where point ids of edges
   * of the input intersected by boundary poly data are stored.
   * Commonly named "EdgeArray".
   */
  vtkSetStringMacro(IntersectedEdgesArrayName);
  vtkGetStringMacro(IntersectedEdgesArrayName);
  ///@}

  ///@{
  /**
   * Toggle on/off using point data array of name InitializationArrayName for the solution value of seed
   * points. If off, a value of 0.0 is used for all seeds.
   */
  vtkSetMacro(InitializeFromScalars,int);
  vtkGetMacro(InitializeFromScalars,int);
  vtkBooleanMacro(InitializeFromScalars,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of point data InitializationArray
   */
  vtkSetStringMacro(InitializationArrayName);
  vtkGetStringMacro(InitializationArrayName);
  ///@}

  ///@{
  /**
   * Toggle on/off using a unit speed field over the whole domain (used to obtain geodesic distances).
   * If off, a CostFunctionArray must be provided.
   */
  vtkSetMacro(UnitSpeed,int);
  vtkGetMacro(UnitSpeed,int);
  vtkBooleanMacro(UnitSpeed,int);
  ///@}

  ///@{
  /**
   * Set/Get the name of point data CostFunctionArray
   * Commonly named "CostFunctionArray".
   */
  vtkSetStringMacro(CostFunctionArrayName);
  vtkGetStringMacro(CostFunctionArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of point data SolutionArray
   * Commonly named "EikonalSolution" or "VoronoiGeodesicDistance", depending on context.
   */
  vtkSetStringMacro(SolutionArrayName);
  vtkGetStringMacro(SolutionArrayName);
  ///@}

  protected:
  vtkvmtkNonManifoldFastMarching();
  ~vtkvmtkNonManifoldFastMarching();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  void InitPropagation(vtkPolyData* input);

  void SolveQuadratic(double a, double b, double c, char &nSol, double &x0, double &x1);

  void GetNeighbors(vtkPolyData* input, vtkIdType pointId, vtkIdList* neighborIds);
  double ComputeUpdateFromCellNeighbor(vtkPolyData* input, vtkIdType neighborId, vtkIdType* trianglePts);
  void UpdateNeighbor(vtkPolyData* input, vtkIdType neighborId);
  void UpdateNeighborhood(vtkPolyData* input, vtkIdType pointId);
  void Propagate(vtkPolyData* input);

  static double Max(double a, double b)    
    { return a-b > VTK_VMTK_DOUBLE_TOL ? a : b; }

  static double Min(double a, double b)    
    { return a-b < - VTK_VMTK_DOUBLE_TOL ? a : b; }

  vtkDoubleArray* TScalars;
  vtkCharArray* StatusScalars;
  vtkvmtkMinHeap* ConsideredMinHeap;

  vtkIdList* Seeds;
  vtkIdList* StopSeedId;
  vtkPolyData* BoundaryPolyData;

  double Regularization;
  double StopTravelTime;
  vtkIdType StopNumberOfPoints;
  int UnitSpeed;
  int InitializeFromScalars;
  char* IntersectedEdgesArrayName;
  char* InitializationArrayName;
  char* SolutionArrayName;
  char* CostFunctionArrayName;

  int SeedsBoundaryConditions;
  int PolyDataBoundaryConditions;

  vtkIdType NumberOfAcceptedPoints;

  int AllowLineUpdate;
  int UpdateFromConsidered;

  private:
  vtkvmtkNonManifoldFastMarching(const vtkvmtkNonManifoldFastMarching&);  // Not implemented.
  void operator=(const vtkvmtkNonManifoldFastMarching&);  // Not implemented.
};

#endif

