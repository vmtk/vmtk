/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataNetworkExtraction.h,v $
Language:  C++
Date:      $Date: 2006/07/17 09:53:14 $
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
// .NAME vtkvmtkPolyDataNetworkExtraction - create an approximated network graph (preliminary to centerline) from an input surface with atleast one hole in it. 
// .SECTION Description
// ...

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
  void PrintSelf(ostream& os, vtkIndent indent) override; 

  static vtkvmtkPolyDataNetworkExtraction *New();
 
  vtkSetStringMacro(MarksArrayName);
  vtkGetStringMacro(MarksArrayName);

  vtkSetStringMacro(RadiusArrayName);
  vtkGetStringMacro(RadiusArrayName);

  vtkSetStringMacro(TopologyArrayName);
  vtkGetStringMacro(TopologyArrayName);

  vtkSetMacro(AdvancementRatio,double);
  vtkGetMacro(AdvancementRatio,double);

  vtkGetMacro(TotalMarkedPoints,int);

  vtkGetMacro(MinimumStep,double);

  vtkGetObjectMacro(GraphLayout,vtkPolyData);

//BTX
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
