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
 * @class   vtkvmtkPolyDataNetworkExtraction
 * @brief   Create an approximated network graph (preliminary to centerline) from an input surface with at least one hole in it.
 * @ingroup Misc
 *
 * Starting from an open boundary of the input surface, advances a series of inscribed spheres along
 * the (approximate) medial axis of the tubular structure, stepping each sphere outward by
 * AdvancementRatio times the local radius, splitting into new branches at bifurcations and stopping
 * at free ends or closed caps. The result is a coarse network graph (a fast, approximate
 * predecessor to full centerline extraction -- see vtkvmtkPolyDataCenterlines -- useful for
 * quickly identifying topology/seed points on very large or complex models). This is the filter
 * behind the vmtknetworkextraction pype script.
 *
 * @sa vtkvmtkPolyDataCenterlines
 */

#ifndef __vtkvmtkPolyDataNetworkExtraction_h
#define __vtkvmtkPolyDataNetworkExtraction_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkvmtkWin32Header.h"

class vtkPolyData;
class vtkPolyDataCollection;
class vtkIdTypeArray;
class vtkCollection;
class vtkPoints;

class VTK_VMTK_MISC_EXPORT vtkvmtkPolyDataNetworkExtraction : public vtkPolyDataAlgorithm
{
  public: 
  vtkTypeMacro(vtkvmtkPolyDataNetworkExtraction,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent) override;

  static vtkvmtkPolyDataNetworkExtraction *New();
 
  ///@{
  /**
   * Set/Get the name of the point data array used internally to mark points of the input surface as
   * visited during network propagation (see the NON_VISITED/VISITED/GLOBAL enum). Default: "Marks".
   */
  vtkSetStringMacro(MarksArrayName);
  vtkGetStringMacro(MarksArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the point data array of the output network where the local inscribed sphere
   * radius is stored at each network point. Default: "Radius".
   */
  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the name of the cell data array of the output network where topology information
   * (segment connectivity) is stored. Default: "Topology".
   */
  vtkSetStringMacro(TopologyArrayName);
  vtkGetStringMacro(TopologyArrayName);
  ///@}

  ///@{
  /**
   * Set/Get the ratio between the propagation step size and the local maximum radius: at each step,
   * the next inscribed sphere is advanced by AdvancementRatio times the current radius. Values
   * closer to 1 produce a denser, more accurate network at higher computational cost. Default: not
   * set by the class itself; the vmtknetworkextraction pype script defaults it to 1.05.
   */
  vtkSetMacro(AdvancementRatio,double);
  vtkGetMacro(AdvancementRatio,double);
  ///@}

  /**
   * Get the total number of surface points visited (marked) during network propagation. Valid only
   * after Update() has been called.
   */
  vtkGetMacro(TotalMarkedPoints,int);

  /**
   * Get the smallest propagation step size encountered during network extraction. Valid only after
   * Update() has been called.
   */
  vtkGetMacro(MinimumStep,double);

  /**
   * Get a simplified graph-layout representation of the extracted network (nodes and edges only, no
   * geometric radius information), useful for visualization/inspection of the network topology.
   * Valid only after Update() has been called.
   */
  vtkGetObjectMacro(GraphLayout,vtkPolyData);

//BTX
  /**
   * Return values of the internal per-step propagation state machine (StepIteration): whether to
   * keep advancing, redefine the current step, or stop because a free end, bifurcation, or closed
   * cap was reached.
   */
  enum
  {
    STEP_ITERATION_PROCEED,
    STEP_ITERATION_REDEFINE,
    STEP_ITERATION_STOP_END,
    STEP_ITERATION_STOP_BIFURCATION,
    STEP_ITERATION_STOP_CLOSED
  };
//ETX

//BTX
  /**
   * Values stored in the MarksArrayName point data array, tracking each surface point's visitation
   * status during network propagation.
   */
  enum
  {
    NON_VISITED,
    VISITED,
    GLOBAL
  };
//ETX

protected:
  vtkvmtkPolyDataNetworkExtraction();
  ~vtkvmtkPolyDataNetworkExtraction();  

  void BoundaryExtractor (vtkPolyData* polyInput, vtkPolyData* boundary);
  void BoundarySeparator(vtkPolyData* appendedBoundaries, vtkPolyDataCollection* boundaries);
  void InsertInEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType pointId0, vtkIdType pointId1);
  bool InsertUniqueInEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType pointId0, vtkIdType pointId1);
  void GetFromEdgeTable(vtkIdTypeArray* edgeTable, vtkIdType position, vtkIdType edge[2]);
  void UpdateEdgeTableCollectionReal(vtkPolyData* model,vtkPolyDataCollection* profiles,vtkCollection* edgeTables);
  double Distance(double point1[3], double point2[3]);
  double GetFurthestDistance (vtkPolyDataCollection* polyDataCollection, double fromPoint[3]);
  void Barycenter (vtkPoints* points, double barycenter[3]);
  void ProfileBarycenter (vtkPoints* points, double barycenter[3]);
  void DefineVirtualSphere(vtkPolyDataCollection* baseProfiles, double center[3], double &radius, double ratio);
  vtkIdType CurrentPointId(vtkPolyData* model,vtkIdType currentEdge[2]);
  void InsertEdgeForNewProfiles(vtkPolyData* model, vtkIdType* edge, vtkIdTypeArray* edgeTable, vtkIdTypeArray* cellPairs, vtkIdList* pointIds);
  bool LookForNeighbors(vtkPolyData* model, vtkIdType pointId, vtkIdList* notVisitedIds, vtkIdTypeArray* edgeTableForIncludedGlobalProfiles);
  void PropagateFromBaseProfilePoint(vtkPolyData* model, vtkIdList* toVisitPointIds, double center[3], double radius, vtkIdTypeArray* edgeTableForNewProfiles, vtkIdTypeArray* cellPairsForNewProfiles, vtkIdList* pointIdsForNewProfiles, vtkPoints* markedPoints, vtkIdList* markedPointIds, vtkIdTypeArray* edgeTableForIncludedGlobalProfiles);
  void LocateVirtualPoint(vtkIdType edge[2], double center[3], double radius, vtkIdList* pointIdsForNewProfiles, vtkPoints* pointsForNewProfiles, vtkDoubleArray* pointDistancesForNewProfiles, double virtualPoint[3]);
  void ReconstructNewProfiles(vtkPoints* virtualPoints, vtkIdTypeArray* edgeTable, vtkIdTypeArray* cellPairs, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables);
  void GenerateNewProfiles(vtkPolyData* model, double center[3], double radius, vtkIdTypeArray* edgeTableForNewProfiles, vtkIdTypeArray* cellPairsForNewProfiles, vtkIdList* pointIdsForNewProfiles, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables);
  void UnmarkPoints(vtkPolyData* model, vtkIdList* markedPointIds);
  double ComputeStepRadius(vtkPoints* points, double point1[3], double point2[3]);
  double ComputeMeanRadius(vtkPoints* points, double point1[3]);
  void PointsForRadius(vtkPoints *markedPoints, vtkPolyDataCollection *baseProfiles, vtkPolyDataCollection *newProfiles, vtkPoints *pointsForRadius);
  void LookForIntersectingPoint(vtkPoints* segmentPoints, double center[3], double radius, vtkIdType &intersectingPointId);
  vtkIdType StepIteration(vtkPolyData* model, vtkPolyDataCollection* baseProfiles, vtkCollection* baseProfilesEdgeTables, vtkPolyDataCollection* globalProfiles, vtkCollection* globalProfilesEdgeTables, vtkPolyDataCollection* newProfiles, vtkCollection* newProfilesEdgeTables, vtkPoints* segmentPoints, vtkDoubleArray* segmentRadii, vtkPoints* bifurcationPoints, vtkDoubleArray* bifurcationRadii, double oldCenter[3], double &oldRadius, double advancementRatio);
  void MarkModelGlobalProfile(vtkPolyData* model, vtkIdTypeArray* newGlobalProfileEdgeTable);
  void SegmentTopology(vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, double firstSegmentPoint[3], double lastSegmentPoint[3], double firstPoint[3], double &firstRadius, double lastPoint[3], double &lastRadius, vtkIdType segmentTopology[2]);
  void BuildSegment(vtkPoints* segmentPoints, vtkDoubleArray* segmentRadii, vtkIdType segmentTopology[2], double firstPoint[3], double firstRadius, double lastPoint[3], double lastRadius, const double* centralPoint, vtkPolyData* segment);
  void InsertNewBifurcation(vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, vtkPoints* bifurcationPoints, vtkDoubleArray* bifurcationRadii, vtkPolyDataCollection* additionalSegments);
  void SegmentIteration(vtkPolyData* model, vtkPolyData* initialProfile, vtkIdTypeArray* initialProfileEdgeTable, vtkPolyDataCollection* globalProfiles, vtkCollection* globalProfilesEdgeTables, vtkCollection* bifurcations, vtkCollection* bifurcationsRadii, vtkPolyDataCollection* segments, double advancementRatio);
  void JoinSegments (vtkPolyData* segment0, vtkPolyData* segment1, bool first0, bool first1, vtkPolyData* segment);
  void RemoveDegenerateBifurcations(vtkPolyDataCollection* segments,vtkCollection* bifurcations);
  void GlobalIteration(vtkPolyData* model, vtkPolyDataCollection* globalProfiles, vtkPolyData* network, double advancementRatio);
  void MarkModelRealBoundary(vtkPolyData* model, vtkPolyData* modelBoundary);
  void Graph(vtkPolyData* network, vtkPolyData* graphLayout);

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkPolyData* GraphLayout;

  char* MarksArrayName;
  char* RadiusArrayName;
  char* TopologyArrayName;

  double MinimumStep;

  double AdvancementRatio;

  vtkIdType TotalMarkedPoints;

  private:
  vtkvmtkPolyDataNetworkExtraction(const vtkvmtkPolyDataNetworkExtraction&);  // Not implemented.
  void operator=(const vtkvmtkPolyDataNetworkExtraction&);  // Not implemented.
};

#endif
